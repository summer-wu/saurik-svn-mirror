/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2005  Jay Freeman (saurik)
*/

/*
 *        Redistribution and use in source and binary
 * forms, with or without modification, are permitted
 * provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the
 *    above copyright notice, this list of conditions
 *    and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the
 *    above copyright notice, this list of conditions
 *    and the following disclaimer in the documentation
 *    and/or other materials provided with the
 *    distribution.
 * 3. The name of the author may not be used to endorse
 *    or promote products derived from this software
 *    without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "cxx/standard.hh"
#pragma hdrstop("build/cxx/standard.pch")

#include "enc/compress/zlib.hpp"
#include "enc/hash/adler32.hpp"

#include "ext/suballoc.hpp"

#include "ios/bits.hpp"
#include "ios/fork.hpp"
#include "ios/getput.hpp"

namespace enc {

namespace {

struct Code_ {
    unsigned extra_;
    unsigned offset_;
};

// XXX: technically all the offsets are calculable from the extra fields

Code_ LengthCodes_[29] = {
    {0,   3}, {0,   4}, {0,   5}, {0,   6},
    {0,   7}, {0,   8}, {0,   9}, {0,  10},
    {1,  11}, {1,  13}, {1,  15}, {1,  17},
    {2,  19}, {2,  23}, {2,  27}, {2,  31},
    {3,  35}, {3,  43}, {3,  51}, {3,  59},
    {4,  67}, {4,  83}, {4,  99}, {4, 115},
    {5, 131}, {5, 163}, {5, 195}, {5, 227},
    {0, 258}
};

Code_ DistanceCodes_[30] = {
    {0,      1}, {0,      2}, {0,     3}, {0,      4},
    {1,      5}, {1,      7}, {2,     9}, {2,     13},
    {3,     17}, {3,     25}, {4,    33}, {4,     49},
    {5,     65}, {5,     97}, {6,   129}, {6,    193},
    {7,    257}, {7,    385}, {8,   513}, {8,    769},
    {9,   1025}, {9,   1537}, {10, 2049}, {10,  3073},
    {11,  4097}, {11,  6145}, {12, 8193}, {12, 12289},
    {13, 16385}, {13, 24577}
};

unsigned ClaOrder_[19] = {
    16, 17, 18,  0,  8,  7, 9,
     6, 10,  5, 11,  4, 12, 3,
    13,  2, 14,  1, 15
};

struct HuffmanNode_ {
    bool leaf_;

    union {
        HuffmanNode_ *children_[2];
        unsigned value_;
    };

    HuffmanNode_() :
        leaf_(false)
    {
        children_[0] = NULL;
        children_[1] = NULL;
    }

    ~HuffmanNode_() {
        if (!leaf_) {
            delete children_[0];
            delete children_[1];
        }
    }
};

_S< ext::SubAllocator<> > allocator_;

class HuffmanTable_ {
  private:
    ios::BitReader &bits_;
    HuffmanNode_ *tree_;

  public:
    HuffmanTable_(ios::BitReader &bits, byte_t table[], size_t size) :
        bits_(bits)
    {
        OPT_TIMER("enc::HuffmanTable_::HuffmanTable_()")

        byte_t longest(0);

        _forall (size_t, i, 0, size) {
            //api::Cout << "L: " << table[i] << ios::NewLine;
            if (longest < table[i])
                longest = table[i];
        }

        _assert(longest <= 32);

        ext::Block<byte_t> counts(longest + 1);

        _forall (size_t, i, 0, longest + 1)
            counts[i] = 0;

        _forall (size_t, i, 0, size)
            ++counts[table[i]];

        counts[0] = 0;

        ext::Block<uint32_t> smallest(longest);

        uint32_t code(0);
        _forall (size_t, i, 0, longest) {
            code = (code + counts[i]) << 1;
            smallest[i] = code;
            //api::Cout << "S: " << smallest[i] << ios::NewLine;
        }

        tree_ = new (allocator_) HuffmanNode_();

        _forall (size_t, i, 0, size) {
            byte_t length(table[i]);
            if (length == 0)
                continue;

            uint32_t code(smallest[length - 1]++);

            /*api::Cout << "C: ";
            _forall (byte_t, j, 0, length)
                api::Cout << ((code >> (length - 1 - j)) & 0x1);
            api::Cout << ios::NewLine;*/

            HuffmanNode_ *node(tree_);
            while (length-- != 0) {
                _assert(node != NULL);
                _assert(!node->leaf_);

                // ERR: error C2061: syntax error : identifier 'node'
                HuffmanNode_ * &next = node->children_[(code >> length) & 0x1];
                if (next == NULL)
                    next = new (allocator_) HuffmanNode_();
                node = next;
            }

            node->leaf_ = true;
            node->value_ = i;
        }
    }

    ~HuffmanTable_() {
        OPT_TIMER("enc::HuffmanTable_::~HuffmanTable_()")
        delete tree_;
    }

    unsigned Get() {
        //OPT_TIMER("enc::HuffmanTable_::Get()")

        HuffmanNode_ *node(tree_);
        while (!node->leaf_)
            node = node->children_[bits_.Get() ? 1 : 0];

        return node->value_;
    }

    _finline ios::BitReader &GetBits() {
        return bits_;
    }
};

class RingBuffer_ :
    public ios::Writer
{
  private:
    ext::Block<byte_t> block_;
    size_t tail_;

    void Recover_(size_t length, size_t distance, ios::Writer &writer, size_t tail) {
        //api::Cout << ios::NewLine << length << " " << distance << ios::NewLine;

        size_t size(block_.GetSize());
        _assert(distance <= size);

        size_t start((tail + size - distance) % size);

        if (start + length <= size)
            writer.WriteFully(block_.Begin() + start, length);
        else {
            size_t rest(size - start);
            writer.WriteFully(block_.Begin() + start, rest);
            writer.WriteFully(block_.Begin(), length - rest);
        }
    }

  public:
    RingBuffer_(size_t size) :
        block_(size),
        tail_(0)
    {
    }

    virtual size_t Write(const byte_t *data, size_t length) {
        OPT_TIMER("enc::RingBuffer_::Write()")

        size_t size(block_.GetSize());

        if (length >= size) {
            ext::CopyAssign(block_.Begin(), data + length - size, size);
            tail_ = 0;
        } else if (tail_ + length > size) {
            size_t rest(size - tail_);
            ext::CopyAssign(block_.Begin() + tail_, data, rest);

            tail_ = length - rest;
            ext::CopyAssign(block_.Begin(), data + rest, tail_);
        } else {
            ext::CopyAssign(block_.Begin() + tail_, data, length);
            tail_ = (tail_ + length) % size;
            /*api::Cout.WriteFully("\n", 1);
            api::Cout.WriteFully(block_.Begin(), tail_);
            api::Cout.WriteFully("\n", 1);*/
        }

        return length;
    }

    virtual void Flush() {
    }

    void Recover(size_t length, size_t distance, ios::Writer &writer) {
        OPT_TIMER("enc::RingBuffer_::Recover()")

        size_t tail(tail_);

        while (distance < length) {
            Recover_(distance, distance, writer, tail);
            length -= distance;
        }

        Recover_(length, distance, writer, tail);
    }
};

void DecodeTable_(HuffmanTable_ &cl, byte_t table[], size_t count, size_t size) {
    OPT_TIMER("enc::DecodeTable_()")

    for (unsigned i(0); i != count; ) {
        switch (unsigned next = cl.Get()) {
            case 16: {
                unsigned count(cl.GetBits().Get(2) + 3);
                _forall (unsigned, j, 0, count)
                    table[i + j] = table[i - 1];
                i += count;
            } break;

            case 17: {
                unsigned count(cl.GetBits().Get(3) + 3);
                _forall (unsigned, j, 0, count)
                    table[i + j] = 0;
                i += count;
            } break;

            case 18: {
                unsigned count(cl.GetBits().Get(7) + 11);
                _forall (unsigned, j, 0, count)
                    table[i + j] = 0;
                i += count;
            } break;

            default:
                table[i++] = next;
            break;
        }
    }

    _forall (unsigned, i, count, size)
        table[i] = 0;
}

}

ENC_DECLARE void ZLibInflate(ios::Reader &reader, ios::Writer &writer) {
    OPT_TIMER("enc::ZLibInflate()")

    byte_t cmf(ios::Get(reader));
    byte_t flg(ios::Get(reader));
    _assert((cmf * 256 + flg) % 31 == 0);

    unsigned cm(cmf & 0xf);
    _assert(cm == 8);

    unsigned cinfo(cmf >> 4);

    bool fdict((flg & 0x20) != 0);
    _assert(!fdict);

    unsigned flevel(flg >> 6);

    _S<Adler32Hasher> hasher;

    {
        _S<RingBuffer_> ring(32 * 1024);

        _S<ios::ForkedWriter> fork;
        fork.Insert(&hasher);
        fork.Insert(&writer);
        fork.Insert(&ring);

        _S<ios::BitReader> bits(&reader);

        _forever {
            bool bfinal(bits.Get());
            byte_t btype(bits.Get(2));

            switch (btype) {
                case 0: {
                    bits.Align();

                    uint16_t len(ios::GetBig<uint16_t>(reader));
                    uint16_t nlen(ios::GetBig<uint16_t>(reader));

                    // ERR: warning: comparison of promoted ~unsigned with unsigned
                    byte_t nnlen(~nlen);
                    _assert(len == nnlen);

                    ios::ReadToWriteFully(reader, fork, len);
                } break;

                case 1: {
                    _assert(false);
                } break;

                case 2: {
                    size_t hlit(bits.Get(5) + 257);
                    size_t hdist(bits.Get(5) + 1);
                    size_t hclen(bits.Get(4) + 4);

                    byte_t cla[19];

                    _forall (size_t, i, 0, hclen)
                        cla[ClaOrder_[i]] = bits.Get(3);
                    _forall (size_t, i, hclen, 19)
                        cla[ClaOrder_[i]] = 0;

                    byte_t lla[286];
                    byte_t da[32];

                    {
                        HuffmanTable_ cl(bits, cla, 19);
                        DecodeTable_(cl, lla, hlit, 286);
                        DecodeTable_(cl, da, hdist, 32);
                    }

                    HuffmanTable_ ll(bits, lla, 286);
                    HuffmanTable_ d(bits, da, 32);

                    OPT_TIMER("enc::ZLibInflate()[Inner]")

                    byte_t buffer[1024];
                    size_t count(0);

                    _forever {
                        unsigned llc(ll.Get());

                        if (llc == 256)
                            break;
                        else if (llc < 256) {
                            if (count == 1024) {
                                fork.WriteFully(buffer, count);
                                count = 0;
                            }

                            buffer[count++] = llc;
                        } else {
                            _assert(llc < 286);

                            if (count != 0) {
                                fork.WriteFully(buffer, count);
                                count = 0;
                            }

                            Code_ &lle(LengthCodes_[llc - 257]);
                            size_t length(bits.Get(lle.extra_) + lle.offset_);

                            unsigned dc(d.Get());
                            _assert(dc < 30);

                            Code_ &de(DistanceCodes_[dc]);
                            size_t distance(bits.Get(de.extra_) + de.offset_);

                            ring.Recover(length, distance, fork);
                        }
                    }

                    if (count != 0) {
                        fork.WriteFully(buffer, count);
                        count = 0;
                    }
                } break;

                case 3:
                    _assert(false);
                break;
            }

            if (bfinal)
                break;
        }

        bits.Align();
    }

    uint32_t adler32(ios::GetBig<uint32_t>(reader));
    _assert(adler32 == hasher.GetHash());
}

}
