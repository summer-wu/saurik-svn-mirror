/* Menes Optimization - C++ Profiling and Verification
 * Copyright (C) 2003-2005  Jay Freeman (saurik)
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

#ifndef MENES_SZF_SAVELOAD_EXT_HPP
#define MENES_SZF_SAVELOAD_EXT_HPP

#include "cxx/platform.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "etl/template.hpp"
#include "etl/traits.hpp"

#include "ext/array.hpp"
#include "ext/map.hpp"
#include "ext/redblackset.hpp"
#include "ext/types.hpp"
#include "ext/vector.hpp"

#include "szf/linkage.hpp"
#include "szf/method.hpp"

namespace szf {

SZF_DECLARE void Save(Method *method, const cse::String &value, const char *name);
SZF_DECLARE void Load(Method *method, cse::String &value, const char *name);

SZF_DECLARE void Save(Method *method, const ext::Uuid &value, const char *name);
SZF_DECLARE void Load(Method *method, ext::Uuid &value, const char *name);

template <size_t Size_, typename Elem_>
void Save(Method *method, const ext::Array<Size_, Elem_> &value, const char *name) {
    method->OpenScope(name);

    typedef ext::Array<Size_, Elem_> Type_;
    _tforeach (Type_, i, value)
        Save(method, *i, "element");

    method->CloseScope();
}

template <size_t Size_, typename Elem_>
void Load(Method *method, ext::Array<Size_, Elem_> &value, const char *name) {
    method->OpenScope(name);

    typedef ext::Array<Size_, Elem_> Type_;
    _tforeach (Type_, i, value)
        Load(method, *i, "element");

    method->CloseScope();
}

template <typename Elem_, typename Buffer_>
void Save(Method *method, const ext::Vector<Elem_, Buffer_> &value, const char *name) {
    method->OpenScope(name);
    Save(method, value.GetSize(), "@size");

    typedef ext::Vector<Elem_, Buffer_> Type_;
    _tforeach (const Type_, i, value)
        Save(method, *i, "element");

    method->CloseScope();
}

template <typename Elem_, typename Buffer_>
void Load(Method *method, ext::Vector<Elem_, Buffer_> &value, const char *name) {
    method->OpenScope(name);
    value.SetSize(method->template Load<size_t>("@size"));

    typedef ext::Vector<Elem_, Buffer_> Type_;
    _tforeach (Type_, i, value)
        Load(method, *i, "element");

    method->CloseScope();
}

template <typename Set_, typename Key_, typename Value_>
void Save(Method *method, const ext::Map<Set_, Key_, Value_> &value, const char *name) {
    method->OpenScope(name);
    Save(method, value.GetSize(), "@size");

    typedef ext::Map<Set_, Key_, Value_> Type_;
    _tforeach (Type_, i, value)
        Save(method, *i, "element");

    method->CloseScope();
}

template <typename Set_, typename Key_, typename Value_>
void Load(Method *method, ext::Map<Set_, Key_, Value_> &value, const char *name) {
    method->OpenScope(name);
    size_t size(method->template Load<size_t>("@size"));

    _forall (size_t, i, 0, size) {
        etl::Pair<Key_, Value_> element;
        Load(method, element, "element");
        value.Insert(element.First(), element.Second());
    }

    method->CloseScope();
}

}

#endif//MENES_SZF_SAVELOAD_EXT_HPP
