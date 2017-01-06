/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2004-2005  Jay Freeman (saurik)
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

#ifndef MENES_IOS_GETPUT_HPP
#define MENES_IOS_GETPUT_HPP

#include "cxx/platform.hpp"
#include "ios/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "ext/byteorder.hpp"
#include "ext/traits.hpp"

namespace ios {

template <typename Element_>
_finline const Element_ &Get(const Element_ *&reader) {
    return *(reader++);
}

// ERR: error: invalid initialization of reference of type ‘const byte_t*&’ from expression of type ‘byte_t*’
template <typename Element_>
_finline const Element_ &Get(Element_ *&reader) {
    return *(reader++);
}

template <typename Element_, typename Value_>
_finline void Put(Element_ *&writer, const Value_ &value) {
    *(writer++) = value;
}

template <typename Element_>
_finline void ReadFully(const Element_ *&reader, Element_ *data, size_t length) {
    ext::CopyAssign(data, reader, length);
    reader += length;
}

template <typename Element_>
_finline void WriteFully(Element_ *&reader, const Element_ *data, size_t length) {
    reader = ext::CopyAssign(reader, data, length);
}

// XXX: these are of _very_ limited usefulness
// XXX: add ReadFully<>() ios:: helper

template <typename Type_, typename Reader_>
Type_ GetLittle(Reader_ &reader) {
    Type_ value;
    ReadFully(reader, reinterpret_cast<byte_t *>(&value), sizeof(value));
    return ext::LittleEndianOrder::Decode(value);
}

template <typename Type_, typename Reader_>
Type_ GetBig(Reader_ &reader) {
    Type_ value;
    ReadFully(reader, reinterpret_cast<byte_t *>(&value), sizeof(value));
    return ext::BigEndianOrder::Decode(value);
}

template <typename Type_, typename Reader_>
Type_ GetBig(Reader_ &reader, size_t bytes) {
    Type_ value(0);
    _forall (size_t, i, 0, bytes)
        value = (value << 8) | Get(reader);
    return value;
}

template <typename Type_, typename Writer_>
void PutBig(Writer_ &writer, Type_ value) {
    value = ext::BigEndianOrder::Encode(value);
    WriteFully(writer, reinterpret_cast<byte_t *>(&value), sizeof(value));
}

template <typename Type_, typename Writer_>
void PutBig(Writer_ &writer, Type_ value, size_t bytes) {
    _rforall (size_t, i, 0, bytes)
        Put(writer, value >> bytes * 8);
}

}

#endif//MENES_IOS_GETPUT_HPP
