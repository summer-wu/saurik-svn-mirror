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

#ifndef MENES_CSE_STRING_HPP
#define MENES_CSE_STRING_HPP

#include "cxx/platform.hpp"
#include "cse/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "alg/logbase2.hpp"
#include "etl/traits.hpp"

#include "cse/character.hpp"
#include "cse/charset.hpp"
#include "cse/types.hpp"
#include "cse/utf8.hpp"

#include "ext/algorithm.hpp"
#include "ext/foreach.hpp"
#include "ext/shared.hpp"
#include "ext/vector.hpp"

#if defined(MENES_STL_COMPATIBLE) || defined(MENES_STL_CONVERTABLE)
#include <string>
#endif

#ifdef MENES_STL_ITERATORS
namespace std {
    struct bidirectional_iterator_tag;
}
#endif

namespace cse {

namespace be {
template <typename Encoding_, typename DataIterator_>
class StringIterator_ {
  public:
    typedef cse::CodePoint Indirect;

#ifdef MENES_STL_ITERATORS
    typedef std::bidirectional_iterator_tag iterator_category;
    typedef cse::CodePoint value_type;
    typedef size_t difference_type;
    typedef cse::CodePoint *pointer;
    typedef cse::CodePoint reference;
#endif

  private:
    typedef StringIterator_<Encoding_, DataIterator_> This_;

    DataIterator_ i_;

  public:
    StringIterator_(const DataIterator_ &i) :
        i_(i)
    {
    }

    template <typename DataIteratorF_>
    StringIterator_(const StringIterator_<Encoding_, DataIteratorF_> &rhs) :
        i_(rhs.GetDataIterator())
    {
    }

    DataIterator_ GetDataIterator() const {
        return i_;
    }

    This_ &operator ++() {
        Encoding_::Skip(i_);
        return *this;
    }

    This_ operator ++(int) {
        This_ temp(*this);
        ++*this;
        return temp;
    }

    This_ &operator --() {
        Encoding_::Retreat(i_);
        return *this;
    }

    This_ operator --(int) {
        This_ temp(*this);
        --*this;
        return temp;
    }

    This_ operator +(int distance) const {
        This_ temp(*this);
        bool negative(distance < 0);
        if (negative)
            distance = -distance;
        while (distance-- != 0)
            negative ? --temp : ++temp;
        return temp;
    }

    This_ operator -(int distance) const {
        return operator +(-distance);
    }

    int operator -(const This_ &rhs) const {
        if (GetDataIterator() < rhs.GetDataIterator())
            return -(rhs - *this);

        size_t distance(0);
        _forall (DataIterator_, i, rhs.GetDataIterator(), GetDataIterator())
            ++distance;
        return distance;
    }

    cse::CodePoint operator *() const {
        DataIterator_ i(i_);
        return Encoding_::Advance(i);
    }

    bool operator ==(const This_ &rhs) const {
        return GetDataIterator() == rhs.GetDataIterator();
    }

    bool operator !=(const This_ &rhs) const {
        return !operator ==(rhs);
    }

    bool operator <(const This_ &rhs) const {
        return GetDataIterator() < rhs.GetDataIterator();
    }
}; }

template <typename Encoding_, typename Block_>
class UnicodeString {
  private:
    typedef UnicodeString<Encoding_, Block_> This_;
    typedef ext::Vector<This_> ThisList_;

  public:
    typedef typename Encoding_::CodeUnit CodeUnit;
    typedef ext::Vector< CodeUnit, typename Block_::template Template<CodeUnit>::Result > Data;

    typedef be::StringIterator_<Encoding_, typename Data::Iterator> Iterator;
    typedef be::StringIterator_<Encoding_, typename Data::ConstIterator> ConstIterator;

  private:
    Data data_;
    size_t size_;

    void Assign_(const CodeUnit *begin, size_t size, ext::ShareMode mode) {
        OPT_TIMER("cse::String::Assign_()")
        data_.Assign(begin, size, mode);
    }

  public:
    inline UnicodeString() :
        size_(0)
    {
    }

    inline UnicodeString(const CodeUnit *rhs, size_t size, ext::ShareMode mode = ext::Copy) :
        size_(0)
    {
        Assign(rhs, size, mode);
    }

    inline UnicodeString(const CodeUnit *rhs, ext::ShareMode mode = ext::Copy) :
        size_(0)
    {
        Assign(rhs, mode);
    }

    template <typename Iterator_>
    inline UnicodeString(const Iterator_ &begin, size_t size) :
        size_(0)
    {
        Assign(begin, size);
    }

    template <typename Iterator_>
    inline UnicodeString(const Iterator_ &begin, const Iterator_ &end, ext::ShareMode mode = ext::Copy) :
        size_(0)
    {
        Assign(begin, end, mode);
    }

    inline UnicodeString(size_t size, const CodePoint &value) :
        size_(0)
    {
        Assign(size, value);
    }

    inline UnicodeString(const Data &data) :
        data_(data)
    {
        size_ = Encoding_::Length(GetData().Begin(), GetData().GetSize());
    }

    template <typename EncodingR_, typename BlockR_>
    inline UnicodeString(const UnicodeString<EncodingR_, BlockR_> &rhs) :
        size_(0)
    {
        Assign(rhs.Begin(), rhs.End());
    }

    inline ConstIterator Begin() const {
        return ConstIterator(GetData().Begin());
    }

    inline Iterator Begin() {
        return Iterator(data_.Begin());
    }

    inline ConstIterator End() const {
        return ConstIterator(GetData().End());
    }

    inline Iterator End() {
        return Iterator(data_.End());
    }

    inline CodePoint First() const {
        return *Begin();
    }

    inline CodePoint Last() const {
        return *(End() - 1);
    }

    inline const Data &GetData() const {
        return data_;
    }

    inline size_t GetSize() const {
        return size_;
    }

    inline void Reserve(size_t size) {
        data_.Reserve(size);
    }

    /* // XXX: not supported
    void SetSize(size_t size) {
        if (size > size_)
            throw ext::InvalidArgumentException();
        // XXX: implement
        _assert(false);
    } */

    inline bool IsEmpty() const {
        return GetData().IsEmpty();
    }

    inline size_t GetCapacity() const {
        return GetData().GetCapacity();
    }

    void Clear() {
        data_.Clear();
        size_ = 0;
    }

    void Delete() {
        data_.Delete();
        size_ = 0;
    }

    This_ Or(const This_ &ifempty) const {
        return IsEmpty() ? ifempty : *this;
    }

    // XXX: some kind of optimization for empty strings would be awesome
    const CodeUnit *NullTerminate() const {
        return data_.NullTerminate();
    }

    void InsertLast(cse::CodePoint value) {
        CodeUnit units[Encoding_::MaxUnits];
        CodeUnit *begin(units);
        data_.InsertAllLast(units, Encoding_::Encode(value, begin));
        ++size_;
    }

    void InsertAllLast(const This_ &rhs) {
        size_ += rhs.GetSize();
        data_.InsertAllLast(rhs.GetData());
    }

    void InsertAllLast(const CodeUnit *begin, size_t size) {
        size_ += Encoding_::Length(begin, begin + size);
        data_.InsertAllLast(begin, begin + size);
    }

    void InsertAllLast(const CodeUnit *begin) {
        size_t size;
        size_ += Encoding_::Length(begin, &size);
        data_.InsertAllLast(begin, begin + size);
    }

    void InsertAllLast(const ConstIterator &begin, const ConstIterator &end) {
        InsertAllLast(begin.GetDataIterator(), end.GetDataIterator() - begin.GetDataIterator());
    }

    // XXX: make this faster
    void Assign(size_t size, const CodePoint &value) {
        Clear();

        _forall (size_t, i, 0, size)
            InsertLast(value);
    }

    void Assign(const CodeUnit *begin, size_t size, ext::ShareMode mode = ext::Copy) {
        if (size == 0)
            Clear();
        else {
            OPT_TIMER("cse::String::Assign(CodeUnit *, size_t)")
            size_ = Encoding_::Length(begin, begin + size);
            Assign_(begin, size, mode);
        }
    }

    void Assign(const CodeUnit *begin, ext::ShareMode mode = ext::Copy) {
        if (begin == NULL)
            Clear();
        else {
            OPT_TIMER("cse::String::Assign(CodeUnit *)")
            // XXX: this was probably a stupid idea, hehe
            //_assert(mode != ext::Copy);
            size_t size;
            size_ = Encoding_::Length(begin, &size);
            Assign_(begin, size, mode);
        }
    }

    template <typename Iterator_>
    void Assign(const Iterator_ &begin, const Iterator_ &end, ext::ShareMode mode = ext::Copy) {
        // XXX: can only share with extensive information of data type
        _assert(mode == ext::Copy);

        _forall (Iterator_, i, begin, end)
            InsertLast(*i);
    }

    void Assign(const ConstIterator &begin, const ConstIterator &end, ext::ShareMode mode = ext::Copy) {
        data_.Assign(begin.GetDataIterator(), end.GetDataIterator());
        size_ = Encoding_::Length(GetData().Begin(), GetData().GetSize());
    }

    template <typename Iterator_>
    void Assign(const Iterator_ &begin, size_t size) {
        if (size == 0)
            Clear();
        else {
            OPT_TIMER("cse::String::Assign(Iterator, size_t)")
            data_.Assign(begin, size);
            size_ = Encoding_::Length(GetData().Begin(), GetData().GetSize());
        }
    }

    bool Equals(const CodeUnit *rhs) const {
        _tforeach (const Data, i, data_)
            if (*rhs == 0 || *i != *(rhs++))
                return false;
        return *rhs == 0;
    }

    bool Equals(const Data &rhs) const {
        return GetData() == rhs;
    }

    bool Equals(const CodePoint &rhs) const {
        return GetSize() == 1 && First() == rhs;
    }

    template <typename BlockR_>
    bool Equals(const cse::UnicodeString<Encoding_, BlockR_> &rhs) const {
        return GetData() == rhs.GetData();
    }

    template <typename BlockR_>
    int Compare(const cse::UnicodeString<Encoding_, BlockR_> &rhs) const {
        if (GetSize() != rhs.GetSize())
            return GetSize() < rhs.GetSize() ? -1 : 1;
        return ext::Compare(Begin(), rhs.Begin(), GetSize());
    }

    bool StartsWith(const cse::CodePoint &value) const {
        return IsEmpty() ? false : First() == value;
    }

    template <typename BlockR_>
    bool StartsWithAll(const cse::UnicodeString<Encoding_, BlockR_> &rhs) const {
        return GetData().StartsWithAll(rhs.GetData());
    }

    bool StartsWithAll(const CodeUnit *begin, size_t size) const {
        return GetData().StartsWithAll(begin, size);
    }

    bool StartsWithAll(const CodeUnit *begin) const {
        size_t size;
        Encoding_::Length(begin, &size);
        return StartsWithAll(begin, size);
    }

    template <typename BlockR_>
    bool EndsWithAll(const cse::UnicodeString<Encoding_, BlockR_> &rhs) const {
        return GetData().EndsWithAll(rhs.GetData());
    }

    bool EndsWithAll(const CodeUnit *begin, size_t size) const {
        return GetData().EndsWithAll(begin, size);
    }

    bool EndsWithAll(const CodeUnit *begin) const {
        size_t size;
        Encoding_::Length(begin, &size);
        return EndsWithAll(begin, size);
    }

    template <typename Predicate_>
    This_ TrimLeft() const {
        for (ConstIterator start(Begin()); ; ++start)
            if (start == End())
                return This_();
            else if (!Predicate_::Execute(*start))
                return This_(start, End());
    }

    template <typename Predicate_>
    This_ TrimRight() const {
        ConstIterator finish(End());
        if (finish == Begin())
            return This_();
        _forever {
            if (--finish == Begin())
                return This_();
            if (!Predicate_::Execute(*finish))
                return This_(Begin(), finish + 1);
        }
    }

    template <typename Predicate_>
    This_ Trim() const {
        // XXX: this implementation sucks
        return TrimLeft<Predicate_>().TrimRight<Predicate_>();
    }

    This_ &operator =(const CodeUnit *rhs) {
        Assign(rhs);
        return *this;
    }

    cse::CodePoint operator [](size_t index) const {
        _assert(!IsEmpty());

        ConstIterator c(Begin());
        _forall (size_t, i, 0, index)
            if (++c == End())
                _assert(false);

        return *c;
    }

    operator const Data &() const {
        return GetData();
    }

    This_ Substring(size_t start, size_t end = ~size_t()) const {
        if (end == ~size_t())
            end = GetSize();

        _assert(start <= end);
        _assert(end <= GetSize());

        ConstIterator c(Begin() + start);

        This_ value;
        for (size_t i(start); i != end; ++i, ++c)
            value.InsertLast(*c);

        return value;
    }

#ifdef MENES_STL_CONVERTABLE
    UnicodeString(const std::string &rhs) :
        size_(0)
    {
        Assign(&*rhs.begin(), rhs.size());
    }

    This_ &operator =(const std::string &rhs) {
        Assign(&*rhs.begin(), rhs.size());
        return *this;
    }

    operator std::string() const {
        return IsEmpty() ? std::string() : std::string(data_.Begin(), data_.GetSize());
    }
#endif

#ifdef MENES_STL_COMPATIBLE
    typedef ConstIterator const_iterator;
    typedef Iterator iterator;

    typedef size_t size_type;
    typedef cse::CodePoint value_type;

    _deprecated const CodeUnit *c_str() const {
        return NullTerminate();
    }

    _deprecated size_type size() const {
        return GetSize();
    }

    _deprecated void clear() {
        Clear();
    }

    _deprecated bool empty() const {
        return IsEmpty();
    }

    _deprecated const_iterator begin() const {
        return Begin();
    }

    _deprecated iterator begin() {
        return Begin();
    }

    _deprecated const_iterator end() const {
        return End();
    }

    _deprecated iterator end() {
        return End();
    }

    _deprecated This_ substr(size_type begin, size_type size = ~size_t()) const {
        return Substring(begin, size == ~size_t() ? GetSize() : begin + size);
    }

    _deprecated size_t find(cse::CodePoint value) const {
        return FindFirst(*this, value) - Begin();
    }

    _deprecated size_type length() const {
        return GetSize();
    }
#endif
};

#ifndef _MSC_VER
// XXX: this will work, but requires more trickery
extern template CSE_DECLARE class UnicodeString<Utf8Encoding>;
#endif

CSE_DECLARE extern String EmptyString;

template <typename EncodingL_, typename BlockL_, typename EncodingR_, typename BlockR_>
inline bool operator <(const UnicodeString<EncodingL_, BlockL_> &lhs, const UnicodeString<EncodingR_, BlockR_> &rhs) {
    return lhs.GetData() < rhs.GetData();
}

template <typename EncodingR_, typename BlockR_>
inline bool operator ==(const typename EncodingR_::CodeUnit *lhs, const UnicodeString<EncodingR_, BlockR_> &rhs) {
    return rhs.Equals(lhs);
}

template <typename EncodingL_, typename BlockL_, typename Right_>
inline bool operator ==(const UnicodeString<EncodingL_, BlockL_> &lhs, const Right_ &rhs) {
    return lhs.Equals(rhs);
}

template <typename EncodingL_, typename BlockL_, typename Right_>
inline bool operator !=(const UnicodeString<EncodingL_, BlockL_> &lhs, const Right_ &rhs) {
    return !lhs.Equals(rhs);
}

template <typename EncodingL_, typename BlockL_, typename EncodingR_, typename BlockR_>
inline bool operator <=(const UnicodeString<EncodingL_, BlockL_> &lhs, const UnicodeString<EncodingR_, BlockR_> &rhs) {
    return lhs.GetData() <= rhs.GetData();
}

template <typename EncodingL_, typename BlockL_, typename EncodingR_, typename BlockR_>
inline bool operator >(const UnicodeString<EncodingL_, BlockL_> &lhs, const UnicodeString<EncodingR_, BlockR_> &rhs) {
    return lhs.GetData() > rhs.GetData();
}

template <typename EncodingL_, typename BlockL_, typename EncodingR_, typename BlockR_>
inline bool operator >=(const UnicodeString<EncodingL_, BlockL_> &lhs, const UnicodeString<EncodingR_, BlockR_> &rhs) {
    return lhs.GetData() >= rhs.GetData();
}

#ifdef MENES_STL_CONVERTABLE
template <typename EncodingL_, typename BlockL_>
inline UnicodeString<EncodingL_, BlockL_> operator +(const UnicodeString<EncodingL_, BlockL_> &lhs, const std::string &rhs) {
    UnicodeString<EncodingL_, BlockL_> temp(lhs);
    temp.InsertAllLast(rhs);
    return temp;
}
#endif

template <typename EncodingL_, typename BlockL_, typename Right_>
inline UnicodeString<EncodingL_, BlockL_> &operator <<(UnicodeString<EncodingL_, BlockL_> &lhs, const Right_ &rhs) {
    lhs.InsertAllLast(rhs);
    return lhs;
}

}

namespace etl {
template <typename Encoding_, typename DataIterator_>
struct IsMemMovable< cse::be::StringIterator_<Encoding_, DataIterator_> > {
    static const bool value = IsMemMovable<DataIterator_>::value;
}; }

namespace etl {
template <typename Encoding_, typename Block_>
struct IsMemMovable< cse::UnicodeString<Encoding_, Block_> > {
    static const bool value = IsMemMovable<Block_>::value;
}; }

// XXX: add a "\0\0\0\0" to the beginning of the string and support ReadOnlyInternal usage
#define _B(s) ext::Buffer(reinterpret_cast<const byte_t *>(s), sizeof(s) - 1, ext::ReadOnlyExternal)
#define _Vs _V<cse::String>

#endif//MENES_CSE_STRING_HPP
