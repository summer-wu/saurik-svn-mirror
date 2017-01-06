/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2002-2005  Jay Freeman (saurik)
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

#ifndef MENES_EXT_BITSET_HPP
#define MENES_EXT_BITSET_HPP

#include "cxx/platform.hpp"
#include "ext/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "api/random.hpp"
#include "ext/array.hpp"

namespace ext {

class PrintWriter;

template <size_t Size_>
class BitArray {
  protected:
    class Proxy_ {
      private:
        BitArray<Size_> &array_;
        size_t index_;

      public:
        Proxy_(BitArray<Size_> &array, size_t index) :
            array_(array),
            index_(index)
        {
        }

        _finline Proxy_ &operator =(bool value) {
            array_.Set(index_, value);
            return *this;
        }

        _finline operator bool() const {
            return array_.At(index_);
        }
    };

  public:
    class ConstIterator {
      private:
        const BitArray<Size_> &array_;
        size_t index_;

      public:
        _finline ConstIterator(const BitArray<Size_> &array, size_t index) :
            array_(array),
            index_(index)
        {
        }

        _finline ConstIterator &operator ++() {
            ++index_;
            return *this;
        }

        _finline bool operator *() const {
            return array_[index_];
        }

        _finline bool operator ==(const ConstIterator &rhs) {
            return index_ == rhs.index_;
        }

        _finline bool operator !=(const ConstIterator &rhs) {
            return !operator ==(rhs);
        }
    };

  private:
    static const size_t width_ = ((Size_ - 1) >> 5) + 1;
    // XXX: VS.NET 2003 required this to not just be width_
    ext::Array<((Size_ - 1) >> 5) + 1, uint32_t> data_;

    uint32_t Group_(size_t index) const {
        _assert(index >= 0 && index < Size_);
        return data_[index >> 5];
    }

    uint32_t &Group_(size_t index) {
        _assert(index >= 0 && index < Size_);
        return data_[index >> 5];
    }

    uint32_t Mask_(size_t index) const {
        return 0x1 << (index & 0x1f);
    }

  public:
    BitArray() {
    }

    BitArray(bool rhs) {
        rhs ? Set() : Reset();
    }

    BitArray(const BitArray<Size_> &rhs) {
        operator =(rhs);
    }

    BitArray<Size_> &operator =(const BitArray<Size_> &rhs) {
        _forall (size_t, i, 0, width_)
            data_[i] = rhs.data_[i];
        return *this;
    }

    BitArray<Size_> &operator &=(const BitArray<Size_> &rhs) {
        _forall (size_t, i, 0, width_)
            data_[i] &= rhs.data_[i];
        return *this;
    }

    BitArray<Size_> &operator |=(const BitArray<Size_> &rhs) {
        _forall (size_t, i, 0, width_)
            data_[i] |= rhs.data_[i];
        return *this;
    }

    BitArray<Size_> &operator ^=(const BitArray<Size_> &rhs) {
        _forall (size_t, i, 0, width_)
            data_[i] ^= rhs.data_[i];
        return *this;
    }

    BitArray<Size_> &AndNotEqual(const BitArray<Size_> &rhs) {
        _forall (size_t, i, 0, width_)
            data_[i] &= ~rhs.data_[i];
        return *this;
    }

    ConstIterator Begin() const {
        return ConstIterator(*this, 0);
    }

    ConstIterator End() const {
        return ConstIterator(*this, Size_);
    }

    bool operator ==(const BitArray<Size_> &rhs) const {
        _forall (size_t, i, 0, width_ - 1)
            if (data_[i] != rhs.data_[i])
                return false;
        return (((data_[width_ - 1] ^ rhs.data_[width_ - 1]) & (~uint32_t() >> ((width_ << 5) - Size_)))) == 0;
    }

    _finline bool operator !=(const BitArray<Size_> &rhs) const {
        return !operator ==(rhs);
    }

    _finline bool operator [](size_t index) const {
        return At(index);
    }

    _finline Proxy_ operator [](size_t index) {
        return Proxy_(*this, index);
    }

    size_t Count() const {
        // XXX: this can be made _so_ faster
        size_t count(0);
        _forall (size_t, i, 0, Size_)
            if (At(i))
                ++count;
        return count;
    }

    void Randomize() {
        ios::ReadFully(api::WeakRandom, reinterpret_cast<char *>(data_.Begin()), width_ * sizeof(uint32_t));
    }

    void Flip() {
        _forall (size_t, i, 0, width_)
            data_[i] ^= ~uint32_t();
        // XXX: data_[i] = ~data_[i] ?
    }

    _finline void Flip(size_t index) {
        Group_(index) ^= Mask_(index);
    }

    _finline bool At(size_t index) const {
        return (Group_(index) & Mask_(index)) != 0;
    }

    void Remove(size_t index) {
        {
            uint32_t lmask(~uint32_t() << ((index & 0x1f) + 1));
            uint32_t &group(Group_(index));
            group = (group & ~lmask & ~Mask_(index)) | ((group & lmask) >> 1);
        }

        _forall (size_t, i, (index >> 5) + 1, width_) {
            data_[i-1] |= ((data_[i] & 0x1) << 31);
            data_[i] >>= 1;
        }
    }

    void InsertBefore(size_t index, bool value) {
        for (size_t i(width_ - 1); i != (index >> 5); --i)
            data_[i] = (data_[i] << 1) | ((data_[i-1] & (0x1 << 31)) >> 31);

        {
            uint32_t lmask(~uint32_t() << (index & 0x1f));
            uint32_t &group(Group_(index));
            group = (group & ~lmask) | (value ? Mask_(index) : 0x0) | ((group & lmask) << 1);
        }
    }

    uint32_t Range(size_t index, size_t length) const {
        _assert(index >= 0 && index + length <= Size_);
        _assert(length >= 0 && length <= 32);

        size_t offset(index & 0x1f);
        if (offset + length > 32) {
            size_t group(index >> 5);
            return (data_[group] >> offset) | ((data_[group + 1] & ~(~uint32_t() << (length + index - 32))) << (32 - index));
        } else if (length == 32) {
            return data_[index >> 5];
        } else {
            return (data_[index >> 5] >> offset) & ~(~uint32_t() << length);
        }
    }

    void Reset() {
        _forall (size_t, i, 0, width_)
            data_[i] = 0x0;
    }

    _finline void Reset(size_t index) {
        Group_(index) &= ~Mask_(index);
    }

    void Set() {
        _forall (size_t, i, 0, width_)
            data_[i] = ~uint32_t();
    }

    _finline void Set(size_t index) {
        Group_(index) |= Mask_(index);
    }

    _finline void Set(size_t index, bool value) {
        value ? Set(index) : Reset(index);
    }

    _finline bool Any() const {
        _forall (size_t, i, 0, width_ - 1)
            if (data_[i] != 0x0)
                return true;
        return (data_[width_ - 1] & (~uint32_t() >> ((width_ << 5) - Size_))) != 0;
    }

    _finline bool None() const {
        return !Any();
    }

    _finline size_t GetSize() const {
        return Size_;
    }
};

template <size_t Size_>
inline ios::PrintWriter &operator <<(ios::PrintWriter &lhs, const BitArray<Size_> &rhs) {
    _forall (size_t, i, 0, Size_)
        lhs << (rhs[i] ? "1" : "0");
    return lhs;
}

}

#endif//MENES_EXT_BITSET_HPP
