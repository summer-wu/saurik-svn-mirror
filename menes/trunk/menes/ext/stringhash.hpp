/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2004  Jay Freeman (saurik)
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

#ifndef MENES_EXT_STRINGHASH_HPP
#define MENES_EXT_STRINGHASH_HPP

#include "cxx/platform.hpp"
#include "ext/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "ext/buffer.hpp"
#include "ext/foreach.hpp"
#include "ext/shift.hpp"

namespace ext {

uint32_t CrcHash(const ext::Buffer &value) {
    uint32_t hash(0);
    _foreach (ext::Buffer, c, value)
        hash = LeftCircleShift(hash, 5) ^ *c;
    return hash;
}

uint32_t PjwHash(const ext::Buffer &value) {
    uint32_t hash(0);

    _foreach (ext::Buffer, c, value) {
        hash = (hash << 4) + *c;
        if (uint32_t bits = hash & 0xf << 24) {
            hash ^= bits >> 24;
            hash ^= bits;
        }
    }

    return hash;
}

template <typename Type_>
struct Fnv1Parameters;

template <>
struct Fnv1Parameters<uint32_t> {
    static const uint32_t Prime = 16777619;
    static const uint32_t Basis = 2166136261;
};

template <>
struct Fnv1Parameters<uint64_t> {
    static const uint64_t Prime = 1099511628211;
    static const uint64_t Basis = 14695981039346656037;
};

/*
template <>
struct Fnv1Parameters<uint128_t> {
    static const uint128_t Prime = 309485009821345068724781401;
    static const uint128_t Basis = 275519064689413815358837431229664493455;
};

template <>
struct Fnv1Parameters<uint256_t> {
    static const uint256_t Prime = 374144419156711147060143317175368453031918731002211;
    static const uint256_t Basis = 100029257958052580907070968620625704837092796014241193945225284501741471925557;
};
*/

//uint32_t BuzHash(const ext::Buffer &value);

template <uint32_t Initial_, uint32_t Multiplier_>
uint32_t MultiplyAddHash(const ext::Buffer &value) {
    uint32_t hash(Initial_);
    _foreach (ext::Buffer, c, value)
        hash = hash * Multiplier_ + *c;
    return hash;
}

template <uint32_t Initial_, uint32_t Multiplier_>
uint32_t MultiplyXorHash(const ext::Buffer &value) {
    uint32_t hash(Initial_);
    _foreach (ext::Buffer, c, value)
        hash = (hash * Multiplier_) ^ *c;
    return hash;
}

template <uint32_t Initial_, uint32_t Multiplier_>
uint32_t XOrMultiplyHash(const ext::Buffer &value) {
    uint32_t hash(Initial_);
    _foreach (ext::Buffer, c, value)
        hash = (hash ^ *c) * Multiplier_;
    return hash;
}

template <typename Type_>
Type_ Fnv1Hash(const ext::Buffer &value) {
    typedef Fnv1Parameters<Type_> Parameters;
    return MultiplyXorHash<Parameters::Basis, Parameters::Prime/* XXX: , Type_*/>(value);
}

template <typename Type_>
Type_ Fnv1aHash(const ext::Buffer &value) {
    typedef Fnv1Parameters<Type_> Parameters;
    return XorMultiplyHash<Parameters::Basis, Parameters::Prime/* XXX: , Type_*/>(value);
}

uint32_t Djb2Hash(const ext::Buffer &value) {
    return MultiplyAddHash<5381, 33>(value); // XXX: should this be MultiplyXorHash()?
}

uint32_t SdbmHash(const ext::Buffer &value) {
    return MultiplyAddHash<0, 65599>(value);
}

uint32_t LoseLoseHash(const ext::Buffer &value) {
    return MultiplyAddHash<0, 1>(value);
}

uint32_t Hash(const ext::Buffer &value) {
    return SdbmHash(value);
}

}

#endif//MENES_EXT_STRINGHASH_HPP
