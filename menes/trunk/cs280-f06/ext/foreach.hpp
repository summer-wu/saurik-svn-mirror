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

#ifndef MENES_EXT_FOREACH_HPP
#define MENES_EXT_FOREACH_HPP

#include "cxx/platform.hpp"
#include "ext/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "cxx/for.hpp"
#include "etl/template.hpp"

namespace ext {
namespace be {
template <typename Type_>
struct ScopeReference_ {
    Type_ &value_;

    ScopeReference_(Type_ &value) :
        value_(value)
    {
    }

    _finline operator bool() const {
        return false;
    }

    Type_ *operator ->() const {
        return &value_;
    }
}; } }

namespace ext {

enum Capitalization {
    UnderscoredLowerCase = 0,
    UpperCamelCase = 1
};

namespace be {
    template <typename List_>
    typename etl::IfValidType< typename List_::const_iterator, etl::Case<UnderscoredLowerCase> >::Result CheckCapitalization_(List_ *);

    template <typename List_>
    typename etl::IfValidType< typename List_::ConstIterator, etl::Case<UpperCamelCase> >::Result CheckCapitalization_(List_ *);
}

template <typename List_>
struct ListCapitalization {
    static const Capitalization value = static_cast<Capitalization>(sizeof(be::CheckCapitalization_(reinterpret_cast<List_ *>(NULL)).value) - 1);
};

template <typename List_, Capitalization Capitalization_>
struct StrictIterator;

template <typename List_>
struct StrictIterator<List_, UnderscoredLowerCase> {
    typedef typename List_::iterator Result;
};

template <typename List_>
struct StrictIterator<const List_, UnderscoredLowerCase> {
    typedef typename List_::const_iterator Result;
};

template <typename List_>
struct StrictIterator<List_, UpperCamelCase> {
    typedef typename List_::Iterator Result;
};

template <typename List_>
struct StrictIterator<const List_, UpperCamelCase> {
    typedef typename List_::ConstIterator Result;
};

template <typename List_, Capitalization Capitalization_ = ListCapitalization<List_>::value>
struct ListTraits;

template <typename List_>
struct ListTraits<List_, UnderscoredLowerCase> {
    typedef typename StrictIterator<List_, UnderscoredLowerCase>::Result Iterator;

    static _finline Iterator Begin(List_ &arg) {
        return arg.begin();
    }

    static _finline Iterator End(List_ &arg) {
        return arg.end();
    }
};

template <typename List_>
struct ListTraits<List_, UpperCamelCase> {
    typedef typename StrictIterator<List_, UpperCamelCase>::Result Iterator;

    static _finline Iterator Begin(List_ &arg) {
        return arg.Begin();
    }

    static _finline Iterator End(List_ &arg) {
        return arg.End();
    }
};

}

#define _foreach_(type, item, set, forall, _typename) \
    for (bool _stop(true); _stop; ) \
        for (type &_set = set; _stop; _stop = false) \
            forall (_typename ext::ListTraits< type >::Iterator, item, ext::ListTraits< type >::Begin(_set), ext::ListTraits< type >::End(_set))

#define _foreach(type, item, set) \
    _foreach_(type, item, set, _forall, )

#define _rforeach(type, item, set) \
    _foreach_(type, item, set, _rforall, )

#define _tforeach(type, item, set) \
    _foreach_(type, item, set, _forall, typename)

#define _rtforeach(type, item, set) \
    _foreach_(type, item, set, _rforall, typename)

#endif//MENES_EXT_FOREACH_HPP
