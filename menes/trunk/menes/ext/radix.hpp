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

#ifndef MENES_EXT_RADIX_HPP
#define MENES_EXT_RADIX_HPP

#include "cxx/platform.hpp"
#include "ext/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "hop/function.hpp"
#include "ext/pointer.hpp"

namespace ext {

template <typename Pred_>
struct FloatingRadixAdapter :
    public Pred_
{
    typedef hop::FunctionTraits<uint32_t (typename Pred_::Traits::Args::Head)> Traits;

    FloatingRadixAdapter(Pred_ pred = Pred_()) :
        Pred_(pred)
    {
    }

    template <typename Type_>
    _finline uint32_t Execute(Type_ value) const {
        union {
            float fl;
            uint32_t fi;
        } vl = {Pred_::Execute(value)};
        return vl.fi ^ (-static_cast<int32_t>(vl.fi >> 31) | 0x80000000);
    }
};

namespace be {
    template <size_t Bits_, size_t Offset_, typename Type_>
    _finline Type_ RadixOffset_(Type_ x) {
        return (x >> Offset_ * Bits_) & ~(~Type_() << Bits_);
    }

    template <typename Value_>
    struct RadixItem_ {
        typedef RadixItem_<Value_> This_;

        size_t index_;
        Value_ value_;

        _finline RadixItem_() {
        }

        _finline RadixItem_(size_t index, Value_ value) :
            index_(index), value_(value)
        {
        }

        _finline void operator =(const This_ &rhs) {
            index_ = rhs.index_;
            value_ = rhs.value_;
        }
    };

    template <size_t Bits_, typename Value_, size_t Passes_, size_t Pass_>
    struct RadixHistogram_ {
        _finline static void Execute(size_t * const b, Value_ const fi) {
            (b + (Passes_ - Pass_) * (1 << Bits_))[(be::RadixOffset_<Bits_, Passes_ - Pass_>(fi) + 1) & ~(-1 << Bits_)]++;
            RadixHistogram_<Bits_, Value_, Passes_, Pass_ - 1>::Execute(b, fi);
        }
    };

    template <size_t Bits_, typename Value_, size_t Passes_>
    struct RadixHistogram_<Bits_, Value_, Passes_, 0> {
        _finline static void Execute(size_t * const b, Value_ const fi) {
        }
    };

    template <size_t Bits_, size_t Passes_, size_t Pass_>
    struct RadixSum_ :
        public RadixSum_<Bits_, Passes_, Pass_ - 1>
    {
        size_t sum_;

        _finline RadixSum_(size_t * const b) :
            RadixSum_<Bits_, Passes_, Pass_ - 1>(b),
            sum_(~size_t())
        {
            (b + (Pass_ - 1) * (1 << Bits_))[0] = ~size_t();
        }

        _finline void Pass1(size_t * const b, size_t i) {
            RadixSum_<Bits_, Passes_, Pass_ - 1>::Pass1(b, i);
            sum_ += (b + (Pass_ - 1) * (1 << Bits_))[i];
        }

        _finline void Pass2(size_t * const b, size_t i) {
            RadixSum_<Bits_, Passes_, Pass_ - 1>::Pass2(b, i);
            (b + (Pass_ - 1) * (1 << Bits_))[i] = sum_;
        }
    };

    template <size_t Bits_, size_t Passes_>
    struct RadixSum_<Bits_, Passes_, 0> {
        _finline RadixSum_(size_t * const b) {
        }

        _finline void Pass1(size_t * const b, size_t i) {
        }

        _finline void Pass2(size_t * const b, size_t i) {
        }
    };

    template <size_t Bits_, typename Value_, size_t Pass_>
    struct RadixPass_ {
        _finline RadixPass_(size_t * const b, RadixItem_<Value_> *swap1, RadixItem_<Value_> *swap2, size_t size) {
            RadixPass_<Bits_, Value_, Pass_ - 1>(b, swap1, swap2, size);
            for (size_t i(0); i != size; ++i) {
                RadixItem_<Value_> &item((Pass_ % 2 ? swap2 : swap1)[i]);
                (Pass_ % 2 ? swap1 : swap2)[++(b + Pass_ * (1 << Bits_))[be::RadixOffset_<Bits_, Pass_>(item.value_)]] = item;
            }
        }
    };

    template <size_t Bits_, typename Value_>
    struct RadixPass_<Bits_, Value_, 0> {
        _finline RadixPass_(size_t * const b, RadixItem_<Value_> *swap1, RadixItem_<Value_> *swap2, size_t size) {
        }
    };
}

template <size_t Bits_, size_t Passes_, typename In_, typename Out_, typename Pred_>
void CustomRadixSort(In_ begin, In_ end, Out_ dst, Pred_ pred = Pred_()) {
    static const size_t kHist(1 << Bits_);

    size_t b[Passes_ * (1 << Bits_)] = {0}, i, size(end - begin);

    typedef typename Pred_::Traits::Return Value_;

    ext::AutoPointer<be::RadixItem_<Value_>, ext::NewArrayAllocator> space(size * 2);

    be::RadixItem_<Value_> * const swap1(space);
    be::RadixItem_<Value_> * const swap2(swap1 + size);

    for (In_ curr(begin); curr != end; ++curr) {
        Value_ fi(pred.Execute(*curr));
        be::RadixHistogram_<Bits_, Value_, Passes_, Passes_>::Execute(b, fi);
#ifdef MENES_EXT_RADIX_PREFLIP
        swap1[curr - begin] = be::RadixItem_<Value_>(curr - begin, fi);
#endif
    }

    be::RadixSum_<Bits_, Passes_, Passes_> sum(b);
    for (i = 1; i != (1 << Bits_); ++i) {
        sum.Pass1(b, i);
        sum.Pass2(b, i);
    }

#ifdef MENES_EXT_RADIX_PREFLIP
    for (i = 0; i != size; ++i) {
        be::RadixItem_<Value_> &item(swap1[i]);
        swap2[++(b + 0 * kHist)[be::RadixOffset_<Bits_, 0>(item.value_)]] = item;
    }
#else
    for (In_ curr(begin); curr != end; ++curr) {
        Value_ fi(pred.Execute(*curr));
        swap2[++(b + 0 * kHist)[be::RadixOffset_<Bits_, 0>(fi)]] = be::RadixItem_<Value_>(curr - begin, fi);
    }
#endif

    be::RadixPass_<Bits_, Value_, Passes_ - 2>(b, swap1, swap2, size);

    for (i = 0; i != size; ++i) {
        be::RadixItem_<Value_> &item((Passes_ % 2 ? swap1 : swap2)[i]);
        dst[++(b + (Passes_ - 1) * kHist)[be::RadixOffset_<Bits_, Passes_ - 1>(item.value_)]] = begin[item.index_];
    }
}

template <typename In_, typename Out_, typename Pred_>
void RadixSort(In_ begin, In_ end, Out_ dst, Pred_ pred = Pred_()) {
    CustomRadixSort<11, 3>(begin, end, dst, pred);
}

template <typename In_, typename Out_, typename Pred_>
void FloatingRadixSort(In_ begin, In_ end, Out_ dst, Pred_ pred) {
    RadixSort(begin, end, dst, FloatingRadixAdapter<Pred_>(pred));
}

}

#endif//MENES_EXT_RADIX_HPP
