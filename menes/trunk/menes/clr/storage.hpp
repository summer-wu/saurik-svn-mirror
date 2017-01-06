/* Ninetjer - _Truly_ Free (non-GPL) Unix .NET Runtime
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

#ifndef NINETJER_STORAGE_HPP
#define NINETJER_STORAGE_HPP

#include "cxx/platform.hpp"
#include "clr/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "ext/vector.hpp"

#include "clr/element.hpp"

namespace clr {

struct StackSlot {
    enum Type {
        V, I4, I8, I, F, O, U
    } typ;

    union {
        int32_t  i32;
        int64_t  i64;
        int      nat;
        double   flt;
        int      obj;
        unsigned ptr;
    };

    StackSlot() {
    }

    StackSlot(Type type) :
        typ(type)
    {
    }
};

struct ValueSlot {
    _S<Element> type_;

    union ValueUnion {
        int8_t   int8_;
        uint8_t  uint8_;
        bool     bool_;
        int16_t  int16_;
        uint16_t uint16_;
        wchar_t  char_;
        int32_t  int32_;
        uint32_t uint32_;
        int64_t  int64_;
        uint64_t uint64_;
        int      int_;
        unsigned uint_;
        float    float32_;
        double   float64_;
        int      object_;
    } value_;

    ValueSlot();
    ValueSlot(const Element &type);

    ValueSlot &operator = (const StackSlot &slot);
    operator StackSlot() const;

    static void StoreValue(StackSlot slot, const Element &type, void *value);
    static StackSlot LoadValue(const Element &type, const void *value);
};

typedef ext::Vector<ValueSlot> ValueList;

}

#endif//NINETJER_STORAGE_HPP
