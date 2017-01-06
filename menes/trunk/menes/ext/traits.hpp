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

#ifndef MENES_EXT_TRAITS_HPP
#define MENES_EXT_TRAITS_HPP

#include "cxx/platform.hpp"
#include "ext/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "etl/composite.hpp"
#include "etl/template.hpp"
#include "etl/traits.hpp"

#include "ext/foreach.hpp"
#include "ext/iterator.hpp"
#include "ext/predicate.hpp"

#include "opt/profile.hpp"

#include <cstring>
#include <new> //OK
#include <typeinfo> //OK

namespace ext {

namespace be {

template <template <typename TypeF_> class IsAble_, typename Type_, typename Out_, typename In_>
struct IsNotMemAble_ {
  private:
    typedef typename UnderlyingIterator_<In_>::Result Under_;

  public:
    static const bool value =
        !IsAble_<Type_>::value ||
        !etl::IsSame<Type_ *, Out_>::value ||
        !etl::IsSame<const Type_ *, Under_>::value && !etl::IsSame<Type_ *, Under_>::value;
};

}

template <typename Type_, typename In_>
inline typename In_::Iterator Deconstruct(const In_ &list) {
    typename In_::Iterator i(list.Begin());
    while (i != list.End())
        i++->~Type_();
    return i;
}

template <typename Iterator_>
Iterator_ Delete(Iterator_ begin, Iterator_ end) {
    while (begin != end)
        delete *(begin++);
    return begin;
}

template <typename Iterator_>
Iterator_ FindFirstZero(Iterator_ begin) {
    for (;; ++begin)
        if (*begin == 0)
            return begin;
}

template <typename Iterator_>
bool CompareEqual(Iterator_ lhs, Iterator_ rhs, size_t length) {
    OPT_TIMER_GROUP("ext::CompareEqual()", typeid(Iterator_).name())

    for (size_t i(0); i != length; ++i, ++lhs, ++rhs)
        if (*lhs != *rhs)
            return false;
    return true;
}

template <typename Iterator_>
int Compare(Iterator_ lhs, Iterator_ rhs, size_t length) {
    OPT_TIMER_GROUP("ext::Compare()", typeid(Iterator_).name())

    for (size_t i(0); i != length; ++i, ++lhs, ++rhs)
        if (*lhs != *rhs)
            return *lhs < *rhs ? -1 : 1;
    return 0;
}

template <typename IteratorL_, typename IteratorR_>
bool StartsWith(IteratorL_ lbegin, IteratorL_ lend, IteratorR_ rbegin, IteratorR_ rend) {
    _forever {
        if (rbegin == rend)
            return true;
        if (lbegin == lend)
            return false;
        if (*(lbegin++) != *(rbegin++))
            return false;
    }
}

template <typename IteratorL_, typename IteratorR_>
bool EndsWith(IteratorL_ lbegin, IteratorL_ lend, IteratorR_ rbegin, IteratorR_ rend) {
    _forever {
        if (rbegin == rend)
            return true;
        if (lbegin == lend)
            return false;
        if (*(--lend) != *(--rend))
            return false;
    }
}

template <typename Out_, typename In_>
Out_ ReverseCopyAssign(Out_ dst, In_ begin, In_ end) {
    while (end != begin)
        *(--dst) = *(--end);
    return dst;
}

// XXX: this should support MemCopyable
template <typename Out_, typename In_>
Out_ CopyAssign(Out_ dst, In_ begin, In_ end) {
    while (begin != end)
        *(dst++) = *(begin++);
    return dst;
}

// XXX: this should support MemCopyable
template <typename Out_, typename In_>
etl::Pair<Out_, In_> CopyAssign(Out_ dst, In_ begin, size_t size) {
    for (; size-- != 0; ++dst, ++begin)
        *dst = *begin;
    return etl::Pair<Out_, In_>(dst, begin);
}

template <typename Type_, typename In_>
typename etl::IfTrue< etl::LogicalNot<bool, be::IsNotMemAble_<etl::IsMemCopyable, Type_, Type_ *, In_>::value>::value, etl::Pair<Type_ *, In_> >::Result
CopyAssign(Type_ *dst, In_ begin, size_t size) {
    ::memcpy(dst, be::UnderlyingIterator_<In_>::Get(begin), size * sizeof(*dst));
    return etl::Pair<Type_ *, In_>(dst + size * sizeof(*dst), begin + size);
}

template <typename Type_, typename Iterator_>
Iterator_ DefaultConstruct(Iterator_ begin, Iterator_ end) {
    while (begin != end)
        new (&*(begin++)) Type_();
    return begin;
}

template <typename Type1_, typename Iterator_, typename Type2_>
Iterator_ FillConstruct(Iterator_ begin, Iterator_ end, const Type2_ &value) {
    while (begin != end)
        new (&*(begin++)) Type1_(value);
    return begin;
}

template <typename Iterator_, typename Type2_>
Iterator_ FillAssign(Iterator_ begin, Iterator_ end, const Type2_ &value) {
    while (begin != end)
        *(begin++) = value;
    return begin;
}

template <typename Iterator_, typename Type2_>
Iterator_ FillAssign(Iterator_ begin, size_t size, const Type2_ &value) {
    while (size-- != 0)
        *(begin++) = value;
    return begin;
}

template <typename Type_, typename Out_, typename In_>
typename etl::IfTrue<be::IsNotMemAble_<etl::IsMemCopyable, Type_, Out_, typename In_::ConstIterator>::value, Out_>::Result
CopyConstruct(Out_ out, const In_ &in) {
    _tforeach (const In_, i, in)
        new (&*(out++)) Type_(*i);
    return out;
}

template <typename Type_, typename In_>
typename etl::IfTrue<etl::LogicalNot<bool, be::IsNotMemAble_<etl::IsMemCopyable, Type_, Type_ *, typename In_::ConstIterator>::value>::value, Type_ *>::Result
CopyConstruct(Type_ *out, const In_ &in) {
    ::memcpy(out, be::UnderlyingIterator_<typename In_::ConstIterator>::Get(in.Begin()), (in.End() - in.Begin()) * sizeof(Type_));
    return out + (in.End() - in.Begin());
}

template <typename Type_, typename Out_, typename In_>
typename etl::IfTrue<be::IsNotMemAble_<etl::IsMemMovable, Type_, Out_, typename In_::Iterator>::value, Out_>::Result
MoveConstruct(Out_ out, In_ &in) {
    _tforeach (In_, i, in) {
        new (&*(out++)) Type_(*i);
        i->~Type_();
    }

    return out;
}

template <typename Type_, typename In_>
typename etl::IfTrue<etl::LogicalNot<bool, be::IsNotMemAble_<etl::IsMemMovable, Type_, Type_ *, typename In_::Iterator>::value>::value, Type_ *>::Result
MoveConstruct(Type_ *out, const In_ &in) {
    ::memcpy(out, in.Begin(), reinterpret_cast<char *>(in.End()) - reinterpret_cast<char *>(in.Begin()));
    return out + (in.End() - in.Begin());
}

/*template<>
class type_traits<char> :
    public basic_typetraits<char>
{
  public:
    static const char *find(const char *data, size_t length, const char &value) {
        return static_cast<const char *>(::memchr(data, value, length));
    }

#if defined(HAVE_MEMRCHR)
    static const char *rfind(const char *data, size_t length, const char &value) {
        return static_cast<const char *>(::memrchr(data, value, length));
    }
#endif

    static size_t length(const char *data) {
        return ::strlen(data);
    }

    static int Compare(const char *lhs, const char *rhs, size_t length) {
        return ::memcmp(lhs, rhs, length);
    }

    static void copy(char *dst, const char *src, size_t length) {
        ::memcpy(dst, src, length);
    }

    static void CopyConstruct(char *dst, const char *src, size_t length) {
        ::memcpy(dst, src, length);
    }

    static void move(char *dst, const char *src, size_t length) {
        ::memmove(dst, src, length);
    }

    static void fill(char *dst, const char &value, size_t length) {
        ::memset(dst, value, length);
    }

    static void fillctor(char *dst, const char &value, size_t length) {
        ::memset(dst, value, length);
    }
};

template<>
class type_traits<wchar_t> :
    public basic_typetraits<wchar_t>
{
  public:
    static const wchar_t *find(const wchar_t *data, size_t length,  const wchar_t &value) {
        return static_cast<const wchar_t *>(::wmemchr(data, value, length));
    }

    static size_t length(const wchar_t *data) {
        return ::wcslen(data);
    }

    static int Compare(const wchar_t *lhs, const wchar_t *rhs, size_t length) {
        return ::wmemcmp(lhs, rhs, length);
    }

    static void copy(wchar_t *dst, const wchar_t *src, size_t length) {
        ::wmemcpy(dst, src, length);
    }

    static void CopyConstruct(wchar_t *dst, const wchar_t *src, size_t length) {
        ::wmemcpy(dst, src, length);
    }

    static void move(wchar_t *dst, const wchar_t *src, size_t length) {
        ::wmemmove(dst, src, length);
    }

    static void CopyConstruct(wchar_t *dst, const wchar_t &value, size_t length) {
        ::wmemset(dst, value, length);
    }

    static void fillctor(wchar_t *dst, const wchar_t &value, size_t length) {
        ::wmemset(dst, value, length);
    }
};*/

}

#endif//MENES_EXT_TRAITS_HPP
