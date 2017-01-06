/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2002-2004  Jay Freeman (saurik)
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

#ifndef MENES_EXT_BYTEORDER_HPP
#define MENES_EXT_BYTEORDER_HPP

#include "cxx/platform.hpp"
#include "ext/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "cxx/casts.hpp"

namespace ext {

_finline uint8_t SwapBytes(uint8_t value) {
    return value;
}

_finline int8_t SwapBytes(int8_t value) {
    return SwapBytes(uint8_t(value));
}

_finline uint16_t SwapBytes(uint16_t value) {
    /* __asm xor eax, eax
    __asm mov ax, value
    __asm bswap eax
    __asm shr eax, 16 */
    return ((value >>  8) & 0x00ff) |
           ((value <<  8) & 0xff00);
}

_finline int16_t SwapBytes(int16_t value) {
    return SwapBytes(uint16_t(value));
}

_finline uint32_t SwapBytes(uint32_t value) {
    /* __asm xor eax, eax
    __asm mov eax, value
    __asm bswap eax */
    value = ((value >>  8) & 0x00ff00ff) |
            ((value <<  8) & 0xff00ff00);
    value = ((value >> 16) & 0x0000ffff) |
            ((value << 16) & 0xffff0000);
    return value;
}

_finline int32_t SwapBytes(int32_t value) {
    return SwapBytes(uint32_t(value));
}

_finline uint64_t SwapBytes(uint64_t value) {
    value = ((value >>  8) & UINT64_C(0x00ff00ff00ff00ff)) |
            ((value <<  8) & UINT64_C(0xff00ff00ff00ff00));
    value = ((value >> 16) & UINT64_C(0x0000ffff0000ffff)) |
            ((value << 16) & UINT64_C(0xffff0000ffff0000));
    value = ((value >> 32) & UINT64_C(0x00000000ffffffff)) |
            ((value << 32) & UINT64_C(0xffffffff00000000));
    return value;
}

_finline int64_t SwapBytes(int64_t value) {
    return SwapBytes(uint64_t(value));
}

_finline float SwapBytes(float value) {
    return lowlevel_cast<float>(SwapBytes(lowlevel_cast<uint32_t>(value)));
}

_finline double SwapBytes(double value) {
    return lowlevel_cast<double>(SwapBytes(lowlevel_cast<uint64_t>(value)));
}

struct NativeOrder {
    template <typename Type_>
    static _finline Type_ Encode(Type_ value) {
        return value;
    }

    template <typename Type_>
    static _finline Type_ Decode(Type_ value) {
        return value;
    }
};

struct ByteSwappedOrder {
    template <typename Type_>
    static _finline Type_ Encode(Type_ value) {
        return SwapBytes(value);
    }

    template <typename Type_>
    static _finline Type_ Decode(Type_ value) {
        return SwapBytes(value);
    }
};

#ifdef CFG_BIG_ENDIAN
typedef ByteSwappedOrder LittleEndianOrder;
typedef NativeOrder BigEndianOrder;
#else
typedef ByteSwappedOrder BigEndianOrder;
typedef NativeOrder LittleEndianOrder;
#endif

typedef BigEndianOrder NetworkOrder;
typedef NativeOrder HostOrder;

template <typename Encoding_, typename External_, typename Internal_ = External_>
class Ordered {
  private:
    Internal_ value_;

  public:
    _finline Ordered<Encoding_, External_, Internal_> &operator =(const External_ &value) {
        value_ = Encoding_::Encode(static_cast<Internal_>(value));
        return *this;
    }

    _finline operator External_() const {
        return static_cast<External_>(Encoding_::Decode(value_));
    }
};

}

typedef int8_t lint8_t;
typedef uint8_t luint8_t;
typedef ext::Ordered<ext::LittleEndianOrder, int16_t> lint16_t;
typedef ext::Ordered<ext::LittleEndianOrder, uint16_t> luint16_t;
typedef ext::Ordered<ext::LittleEndianOrder, int32_t> lint32_t;
typedef ext::Ordered<ext::LittleEndianOrder, uint32_t> luint32_t;
typedef ext::Ordered<ext::LittleEndianOrder, int64_t> lint64_t;
typedef ext::Ordered<ext::LittleEndianOrder, uint64_t> luint64_t;

typedef int8_t bint8_t;
typedef uint8_t buint8_t;
typedef ext::Ordered<ext::BigEndianOrder, int16_t> bint16_t;
typedef ext::Ordered<ext::BigEndianOrder, uint16_t> buint16_t;
typedef ext::Ordered<ext::BigEndianOrder, int32_t> bint32_t;
typedef ext::Ordered<ext::BigEndianOrder, uint32_t> buint32_t;
typedef ext::Ordered<ext::BigEndianOrder, int64_t> bint64_t;
typedef ext::Ordered<ext::BigEndianOrder, uint64_t> buint64_t;

#endif//MENES_EXT_BYTEORDER_HPP
