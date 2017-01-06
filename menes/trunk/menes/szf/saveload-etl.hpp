/* Menes Optimization - C++ Profiling and Verification
 * Copyright (C) 2003-2004  Jay Freeman (saurik)
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

#ifndef MENES_SZF_SAVELOAD_ETL_HPP
#define MENES_SZF_SAVELOAD_ETL_HPP

#include "cxx/platform.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "szf/method.hpp"

#include "etl/template.hpp"
#include "etl/traits.hpp"

namespace szf {

// XXX: this should be generalized to etl::Composite<>
template <typename First_, typename Second_>
void Save(Method *method, const etl::Pair<First_, Second_> &value, const char *name) {
    method->OpenScope(name);
    Save(method, value.First(), "first");
    Save(method, value.Second(), "second");
    method->CloseScope();
}

template <typename First_, typename Second_>
void Load(Method *method, etl::Pair<First_, Second_> &value, const char *name) {
    method->OpenScope(name);
    Load(method, value.First(), "first");
    Load(method, value.Second(), "second");
    method->CloseScope();
}

template <typename Cell_>
void Save(Method *method, const etl::Struct<Cell_> &value, const char *name) {
    method->OpenScope(name);
    Save(method, value.head, "head");
    Save(method, value.tail, "tail");
    method->CloseScope();
}

template <typename Type_>
void Save(Method *method, const etl::Struct< etl::Cons<Type_> > &value, const char *name) {
    method->OpenScope(name);
    Save(method, value.head, "head");
    method->CloseScope();
}

inline void Save(Method *method, const etl::Struct<etl::Null> &value, const char *name) {
    method->OpenScope(name);
    method->CloseScope();
}

template <typename Cell_>
void Load(Method *method, etl::Struct<Cell_> &value, const char *name) {
    method->OpenScope(name);
    Load(method, value.head, "head");
    Load(method, value.tail, "tail");
    method->CloseScope();
}

template <typename Type_>
void Load(Method *method, etl::Struct< etl::Cons<Type_> > &value, const char *name) {
    method->OpenScope(name);
    Load(method, value.head, "head");
    method->CloseScope();
}

inline void Load(Method *method, etl::Struct<etl::Null> &value, const char *name) {
    method->OpenScope(name);
    method->CloseScope();
}

template <typename Cell_>
void Save(Method *method, const hop::ArgStruct<Cell_> &value, const char *name) {
    method->OpenScope(name);
    Save(method, value.head, "head");
    Save(method, value.tail, "tail");
    method->CloseScope();
}

template <typename Type_>
void Save(Method *method, const hop::ArgStruct< etl::Cons<Type_> > &value, const char *name) {
    method->OpenScope(name);
    Save(method, value.head, "head");
    method->CloseScope();
}

inline void Save(Method *method, const hop::ArgStruct<etl::Null> &value, const char *name) {
    method->OpenScope(name);
    method->CloseScope();
}

template <typename Cell_>
void Load(Method *method, hop::ArgStruct<Cell_> &value, const char *name) {
    method->OpenScope(name);
    Load(method, value.head, "head");
    Load(method, value.tail, "tail");
    method->CloseScope();
}

template <typename Type_>
void Load(Method *method, hop::ArgStruct< etl::Cons<Type_> > &value, const char *name) {
    method->OpenScope(name);
    Load(method, value.head, "head");
    method->CloseScope();
}

inline void Load(Method *method, hop::ArgStruct<etl::Null> &value, const char *name) {
    method->OpenScope(name);
    method->CloseScope();
}

}

#endif//MENES_SZF_SAVELOAD_ETL_HPP
