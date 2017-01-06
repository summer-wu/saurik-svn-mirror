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

#ifndef MENES_EXT_LIST_HPP
#define MENES_EXT_LIST_HPP

#include "cxx/platform.hpp"
#include "ext/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "ext/foreach.hpp"
#include "ext/predicate.hpp"

namespace ext {

template <typename ListL_, typename ListR_>
typename ListTraits<ListL_>::Iterator FindFirstAll(ListL_ &string, const ListR_ &substring) {
    _tforeach (ListL_, l, string) {
        typename ListTraits<ListL_>::Iterator i(l);
        _tforeach (const ListR_, r, substring)
            if (i == string.End() || *(i++) != *r)
                goto next;
        return l;
    next:; }

    return string.End();
}

template <typename ListL_, typename ListR_>
typename ListTraits<ListL_>::Iterator FindLastAll(ListL_ &string, const ListR_ &substring) {
    _rtforeach (ListL_, l, string) {
        typename ListTraits<ListL_>::Iterator i(l);
        _tforeach (const ListR_, r, substring)
            if (i == string.End() || *(i++) != *r)
                goto next;
        return l;
    next:; }

    return string.End();
}

template <typename ListL_, typename ListR_>
bool ContainsAll(ListL_ &string, const ListR_ &substring) {
    return FindFirstAll(string, substring) != string.End();
}

template <typename Predicate_, typename ListL_, typename ValueR_>
typename ListTraits<ListL_>::Iterator FindFirstWith(ListL_ &string, const ValueR_ &value) {
    _tforeach (ListL_, l, string)
        if (Predicate_::Execute(*l, value))
            return l;
    return string.End();
}

template <typename Predicate_, typename ListL_, typename ValueR_>
typename ListTraits<ListL_>::Iterator FindLastWith(ListL_ &string, const ValueR_ &value) {
    _rtforeach (ListL_, l, string)
        if (Predicate_::Execute(*l, value))
            return l;
    return string.End();
}

template <typename ListL_, typename ValueR_>
_finline typename ListTraits<ListL_>::Iterator FindFirst(ListL_ &string, const ValueR_ &value) {
    return FindFirstWith<ext::EqualTo>(string, value);
}

template <typename ListL_, typename ValueR_>
_finline typename ListTraits<ListL_>::Iterator FindLast(ListL_ &string, const ValueR_ &value) {
    return FindLastWith<ext::EqualTo>(string, value);
}

template <typename ListL_, typename ValueR_>
_finline bool Contains(ListL_ &string, const ValueR_ &value) {
    return FindFirst(string, value) != string.End();
}

template <typename List_, typename ListL_, typename ListR_>
List_ JoinAll(const ListL_ &list, const ListR_ &separator) {
    List_ result;

    _tforeach (const ListL_, value, list) {
        if (_index != 0)
            result.InsertAllLast(separator);
        result.InsertAllLast(*value);
    }

    return result;
}

template <typename ListL_, typename ListR_>
_L<ListL_> SplitAll(const ListL_ &list, const ListR_ &separator) {
    _L<ListL_> result;

    size_t size(separator.End() - separator.Begin());
    typename ListL_::ConstIterator begin(list.Begin());

  find:
    typename ListL_::ConstIterator end(FindFirstAll(_I(begin, list.End()), separator));
    result.InsertLast(ListL_(begin, end));

    if (end != list.End()) {
        begin = end + size;
        goto find;
    }

    return result;
}

}

#endif//MENES_EXT_LIST_HPP
