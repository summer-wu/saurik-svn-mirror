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

#ifndef MENES_EXT_VECTOR_HPP
#define MENES_EXT_VECTOR_HPP

#include "cxx/platform.hpp"
#include "ext/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "ext/algorithm.hpp"
#include "ext/block.hpp"
#include "ext/iterator.hpp"
#include "ext/shared.hpp"
#include "ext/traits.hpp"
#include "ext/workarounds.hpp"

#include "opt/profile.hpp"

#include <typeinfo> //OK

namespace ext {

namespace be {
    OPT_DECLAREPERFORMANCE(EXT_DECLARE extern, removeat_)

    template <bool Sharable_>
    struct ShareBlock_ {
        template <typename Block_>
        static void Execute(Block_ &lhs, const Block_ &rhs) {
            lhs = rhs;
        }
    };

    template <>
    struct ShareBlock_<false> {
        template <typename Block_>
        static void Execute(Block_ &lhs, const Block_ &rhs) {
            // buffer cannot be shared
            _assert(false);
        }
    };

    // this class exists to make debugging vectors somewhat bareable
    // XXX: this should arguably be templated over Vector_ instead
    template <typename Element_, typename Block_>
    struct VectorPerformance_ {
        OPT_DECLAREPERFORMANCE(static, insertlast_)
        OPT_DECLAREPERFORMANCE(static, reserve_)
        OPT_DECLAREPERFORMANCE(static, touch_)
    };
}

template <typename Element_, typename Block_>
class Vector {
  private:
    typedef Vector<Element_, Block_> This_;
    typedef be::VectorPerformance_<Element_, Block_> Performance_;

  public:
    typedef typename Block_::ConstIterator ConstIterator;
    typedef typename Block_::Iterator Iterator;

    // XXX: maybe Block_::Value or something?
    typedef Element_ Value;

    typedef typename Block_::Allocator Allocator;

  private:
    Block_ data_;
    size_t size_;

    _finline size_t SizeUp_(size_t size) const {
        return size * 2;
    }

  public:
    inline Vector(const Allocator &allocator = Allocator()) :
        data_(allocator),
        size_(0)
    {
    }

    inline Vector(size_t size, const Allocator &allocator = Allocator()) :
        data_(allocator),
        size_(0)
    {
        SetSize(size);
    }

    template <typename Type_>
    inline Vector(size_t size, const Type_ &value, const Allocator &allocator = Allocator()) :
        data_(allocator),
        size_(0)
    {
        SetSize(size, value);
    }

    inline Vector(const Element_ *begin, ShareMode mode = Copy, size_t capacity = 0, const Allocator &allocator = Allocator()) :
        data_(allocator),
        size_(0)
    {
        Assign(begin, mode, capacity);
    }

    inline Vector(const Element_ *begin, size_t size, ShareMode mode = Copy, size_t capacity = 0, const Allocator &allocator = Allocator()) :
        data_(allocator),
        size_(0)
    {
        Assign(begin, size, mode, capacity);
    }

    template <typename Iterator_>
    inline Vector(const Iterator_ &begin, size_t size, const Allocator &allocator = Allocator()) :
        data_(allocator),
        size_(0)
    {
        Assign(begin, size);
    }

    template <typename Iterator_>
    inline Vector(const Iterator_ &begin, Iterator_ end, const Allocator &allocator = Allocator()) :
        data_(allocator),
        size_(0)
    {
        Assign(begin, end);
    }

    // XXX: this is a failure on a few different levels
    inline Vector(const This_ &rhs) :
        data_(rhs.GetAllocator()),
        size_(0)
    {
        operator =(rhs);
    }

    inline ~Vector() {
        Clear();
    }

    // XXX: this should assign allocator
    inline This_ &operator =(const This_ &rhs) {
        if (Block_::IsSharable()) {
            be::ShareBlock_<Block_::Sharable>::Execute(data_, rhs.data_);
            size_ = rhs.size_;
        } else {
            Assign(rhs.Begin(), rhs.GetSize());
        }
        return *this;
    }

    inline ConstIterator Begin() const {
        return data_.Begin();
    }

    inline Iterator Begin() {
        return data_.Begin();
    }

    inline ConstIterator End() const {
        return data_.Begin() + size_;
    }

    inline Iterator End() {
        return data_.Begin() + size_;
    }

    inline const Element_ &First() const {
        return *Begin();
    }

    inline Element_ &First() {
        return *Begin();
    }

    inline const Element_ &Last() const {
        return *(End() - 1);
    }

    inline Element_ &Last() {
        return *(End() - 1);
    }

    const Allocator &GetAllocator() const {
        return data_.GetAllocator();
    }

    inline size_t GetSize() const {
        return size_;
    }

    void SetSize(size_t size) {
        if (size == GetSize())
            return;

        Reserve(size);

        if (!data_.IsWritable()) {
            Block_ data(SizeUp_(size), data_.GetAllocator());
            ext::Swap(data_, data);

            if (size < GetSize())
                Assign(data.Begin(), size);
            else {
                Assign(data.Begin(), data.GetSize());
                DefaultConstruct<Element_>(End(), Begin() + size);
            }
        } else if (size < GetSize()) {
            Deconstruct<Element_>(_I(Begin() + size, End()));
        } else {
            DefaultConstruct<Element_>(End(), Begin() + size);
        }

        size_ = size;
    }

    template <typename Type_>
    void SetSize(size_t size, const Type_ &value) {
        if (size == GetSize())
            return;

        Reserve(size);
        Touch();

        if (size < GetSize())
            Deconstruct<Element_>(_I(Begin() + size, End()));
        else
            FillConstruct<Element_>(End(), Begin() + size, value);

        size_ = size;
    }

    _finline bool IsEmpty() const {
        return GetSize() == 0;
    }

    inline size_t GetCapacity() const {
        return data_.GetSize();
    }

    void Reserve(size_t capacity) {
        if (capacity <= data_.GetSize())
            return;

        OPT_SCOPETIMER(Performance_::reserve_)
        Block_ data(SizeUp_(capacity), data_.GetAllocator());

        if (data_.IsWritable())
            MoveConstruct<Element_>(data.Begin(), _I(Begin(), End()));
        else
            CopyConstruct<Element_>(data.Begin(), _I(Begin(), End()));

        ext::Swap(data_, data);
    }

    const Element_ *NullTerminate() const {
        This_ *this_(const_cast<This_ *>(this));
        this_->Reserve(data_.GetSize() + 1);
        // this may be paranoid, but the entire concept is insane...
        (&*(this_->End() - 1))[1] = 0;
        return Begin();
    }

    template <typename Type_>
    void InsertBefore(const Type_ &value, Iterator before) {
        InsertBefore(value, before - Begin());
    }

    template <typename Type_>
    void InsertBefore(const Type_ &value, size_t before) {
        // XXX: this is a stupid implementation
        InsertAllBefore(&value, 1, before);
    }

    template <typename Iterator_>
    void InsertAllBefore(Iterator_ begin, size_t size, Iterator before) {
        InsertAllBefore(begin, size, before - Begin());
    }

    template <typename Iterator_>
    void InsertAllBefore(Iterator_ begin, size_t size, size_t before) {
        if (GetCapacity() < GetSize() + size) {
            Block_ data(SizeUp_(GetSize() + size), data_.GetAllocator());
            ext::MoveConstruct<Element_>(
                CopyConstruct<Element_>(
                    MoveConstruct<Element_>(data.Begin(), _I(Begin(), Begin() + before)),
                    _I(begin, size)
                ), _I(Begin() + before, End())
            );
            ext::Swap(data, data_);
        } else {
            Touch();

            ext::CopyConstruct<Element_>(End(), _I(End() - size, End()));
            ext::ReverseCopyAssign(End(), Begin() + before, End() - size);
            ext::CopyAssign(Begin() + before, begin, begin + size);
        }

        size_ += size;
    }

    template <typename ElementR_, typename BlockR_>
    void InsertAllBefore(const Vector<ElementR_, BlockR_> &value, Iterator before) {
        InsertAllBefore(value, before - Begin());
    }

    template <typename ElementR_, typename BlockR_>
    void InsertAllBefore(const Vector<ElementR_, BlockR_> &value, size_t before) {
        InsertAllBefore(value.Begin(), value.GetSize(), before);
    }

    template <typename Type_>
    void InsertFirst(const Type_ &value) {
        InsertBefore(value, Begin());
    }

    Iterator InsertLast() {
        Reserve(GetSize() + 1);
        Touch();
        new (&*End()) Element_();
        ++size_;
        return End() - 1;
    }

    template <typename Type_>
    void InsertLast(const Type_ &value) {
        OPT_SCOPETIMER(Performance_::insertlast_)

        Reserve(GetSize() + 1);
        Touch();
        new (&*End()) Element_(value);
        ++size_;
    }

    template <typename ElementR_, typename BlockR_>
    void InsertAllLast(const Vector<ElementR_, BlockR_> &value) {
        InsertAllLast(value.Begin(), value.GetSize());
    }

    void InsertAllLast(const ConstIterator &begin, const ConstIterator &end) {
        InsertAllLast(begin, end - begin);
    }

    template <typename List_>
    void InsertAllLast(const List_ &rhs) {
        _tforeach (const List_, i, rhs)
            InsertLast(*i);
    }

    template <typename Iterator_>
    void InsertAllLast(Iterator_ begin, size_t size) {
        if (size == 0)
            return;

        OPT_TIMER_GROUP("ext::Vector::InsertAllLast()", typeid(Element_).name())

        Reserve(GetSize() + size);
        Touch();
        CopyConstruct<Element_>(End(), _I(begin, begin + size));
        size_ += size;
    }

    template <typename Iterator_>
    void Assign(Iterator_ begin, Iterator_ end) {
        Clear();
        InsertAllLast(_I(begin, end));
    }

    template <typename Type_>
    void Set(const Type_ &value) {
        Touch();
        ext::FillAssign(Begin(), End(), value);
    }

    void Touch() {
        OPT_SCOPETIMER(Performance_::touch_)

        if (data_.IsWritable())
            return;

        Block_ data(data_.GetSize(), data_.GetAllocator());
        ext::CopyConstruct<Element_>(data.Begin(), _I(Begin(), End()));
        ext::Swap(data, data_);

        _assert(data_.IsWritable());
    }

    void Assign(const Element_ *begin, ShareMode mode, size_t capacity = 0) {
        _assert((mode & Protection) == ReadOnly);
        Assign(const_cast<Element_ *>(begin), mode, capacity);
    }

    void Assign(const Element_ *begin, size_t size, ShareMode mode, size_t capacity = 0) {
        _assert((mode & Protection) == ReadOnly);
        Assign(const_cast<Element_ *>(begin), size, mode, capacity);
    }

    void Assign(Element_ *begin, ShareMode mode, size_t capacity = 0) {
        return Assign(begin, ext::FindFirstZero(begin) - begin, mode, capacity);
    }

    void Assign(Element_ *begin, size_t size, ShareMode mode, size_t capacity = 0) {
        if (!Block_::SupportsMode(mode) || (mode & Copy) != 0)
            return Assign(begin, size);
        if (capacity == 0)
            capacity = size;

        data_.Assign(begin, capacity, mode);
        size_ = size;
    }

    template <typename Iterator_>
    void Assign(Iterator_ begin) {
        OPT_TIMER_GROUP("ext::Vector::Assign(Iterator_)", typeid(Element_).name())

        for (Clear(); *begin; ++begin)
            InsertLast(*begin);

        // XXX: only here as workaround for ios::Reader::GetIterator()
        ++begin;
    }

    template <typename Iterator_>
    void Assign(Iterator_ begin, size_t size) {
        OPT_TIMER_GROUP("ext::Vector::Assign(Iterator_, size_t)", typeid(Element_).name())

        if (!data_.IsWritable() || GetCapacity() < size) {
            Block_ data(SizeUp_(size), data_.GetAllocator());
            ext::CopyConstruct<Element_>(data.Begin(), _I(begin, size));
            ext::Swap(data, data_);
        } else if (size < GetSize()) {
            Touch();
            ext::Deconstruct<Element_>(_I(
                ext::CopyAssign(Begin(), begin, size).First(),
                End()
            ));
        } else {
            Touch();
            ext::CopyConstruct<Element_>(
                End(),
                _I(
                    ext::CopyAssign(Begin(), begin, GetSize()).Second(),
                    size - GetSize()
                )
            );
        }

        size_ = size;
    }

    template <typename Type_>
    void Assign(const Type_ *begin, const Type_ *end) {
        return Assign(begin, end - begin);
    }

    template <typename Type_>
    void Set(const Type_ &value, size_t size) {
        if (GetCapacity() < size) {
            Block_ data(SizeUp_(size), data_.GetAllocator());
            ext::FillConstruct<Element_>(data.Begin(), data.Begin() + size, value);
            ext::Swap(data, data_);
        } else if (size < GetSize()) {
            Touch();
            ext::FillAssign(Begin(), Begin() + size, value);
            ext::Deconstruct<Element_>(_I(Begin() + size, End()));
        } else {
            Touch();
            ext::FillAssign(Begin(), End(), value);
            ext::FillConstruct<Element_>(End(), Begin() + size, value);
        }

        size_ = size;
    }

    inline void Clear() {
        if (size_ == 0)
            return;

        if (data_.IsWritable())
            Deconstruct<Element_>(_I(Begin(), End()));
        else
            data_.Clear();

        size_ = 0;
    }

    inline void Delete() {
        Clear();
        data_.Delete();
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

    template <typename ElementR_, typename BlockR_>
    void ReplaceAllAt(const Vector<ElementR_, BlockR_> &value, Iterator at) {
        ReplaceAllAt(value, at - Begin());
    }

    template <typename ElementR_, typename BlockR_>
    void ReplaceAllAt(const Vector<ElementR_, BlockR_> &value, size_t at) {
        // XXX: better implementation please
        RemoveAt(at);
        InsertAllBefore(value, at);
    }

    inline void RemoveAllAt(Iterator begin, size_t size) {
        _assert(begin >= Begin());
        _assert(begin <= End());
        _assert(begin + size <= End());

        Touch();

        ext::Deconstruct<Element_>(_I(
            ext::CopyAssign(begin, begin + size, End()),
            End()
        ));

        size_ -= size;
    }

    inline void RemoveAt(Iterator element) {
        RemoveAt(element - Begin());
    }

    void RemoveAt(size_t index) {
        OPT_SCOPETIMER(be::removeat_)

        _assert(index < GetSize());

        Touch();
        ext::CopyAssign(Begin() + index, Begin() + index + 1, End());
        ext::Deconstruct<Element_>(_I(End() - 1, End()));
        --size_;
    }

    template <typename Type_>
    inline bool Remove(const Type_ &value) {
        return RemoveFirst(value);
    }

    template <typename Type_>
    bool RemoveFirst(const Type_ &value) {
        Iterator element(FindFirst(*this, value));
        if (element == End())
            return false;
        RemoveAt(element);
        return true;
    }

    void RemoveFirst() {
        _assert(!IsEmpty());
        Touch();
        First().~Element_();
        MoveConstruct<Element_>(Begin(), _I(Begin() + 1, End()));
        --size_;
    }

    void RemoveLast() {
        _assert(!IsEmpty());
        Touch();
        Last().~Element_();
        --size_;
    }

    template <typename ElementR_, typename BlockR_>
    bool StartsWithAll(const Vector<ElementR_, BlockR_> &value) const {
        return StartsWithAll(value.Begin(), value.GetSize());
    }

    template <typename Iterator_>
    bool StartsWithAll(Iterator_ begin, size_t size) const {
        return size > GetSize() ? false : ext::StartsWith(Begin(), End(), begin, begin + size);
    }

    template <typename ElementR_, typename BlockR_>
    bool EndsWithAll(const Vector<ElementR_, BlockR_> &value) const {
        return EndsWithAll(value.Begin(), value.GetSize());
    }

    template <typename Iterator_>
    bool EndsWithAll(Iterator_ begin, size_t size) const {
        return size > GetSize() ? false : ext::EndsWith(Begin(), End(), begin, begin + size);
    }

    _finline const Element_ &At(int index) const {
        return operator [](index < 0 ? GetSize() + index : index);
    }

    _finline Element_ &At(int index) {
        return operator [](index < 0 ? GetSize() + index : index);
    }

    _finline const Element_ &operator [](size_t index) const {
        _assert(index < size_);
        return data_[index];
    }

    _finline Element_ &operator [](size_t index) {
        _assert(index < size_);
        return data_[index];
    }

    _finline const Element_ &operator ()(size_t index) const {
        _assert(index < size_);
        return data_[index];
    }

    _finline Element_ &operator ()(size_t index) {
        _assert(index < size_);
        return data_[index];
    }

#ifdef MENES_STL_COMPATIBLE
    typedef ConstIterator const_iterator;
    typedef Iterator iterator;

    typedef size_t size_type;
    typedef Element_ value_type;

    _deprecated size_type size() const {
        return GetSize();
    }

    _deprecated void clear() {
        Clear();
    }

    _deprecated bool empty() const {
        return IsEmpty();
    }

    _deprecated void push_back(const value_type &value) {
        InsertLast(value);
    }

    _deprecated void pop_back() {
        RemoveLast();
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

    _deprecated const value_type &front() const {
        return First();
    }

    _deprecated value_type &front() {
        return First();
    }

    _deprecated const value_type &back() const {
        return Last();
    }

    _deprecated value_type &back() {
        return Last();
    }

    _deprecated const value_type &at(size_type index) const {
        return operator [](index);
    }

    _deprecated value_type &at(size_type index) {
        return operator [](index);
    }

    _deprecated void erase(iterator index) {
        RemoveAt(index);
    }

    _deprecated void resize(size_type size) {
        SetSize(size);
    }

    _deprecated void reserve(size_type capacity) {
        Reserve(capacity);
    }

    _deprecated void insert(iterator before, const value_type &value) {
        InsertBefore(value, before);
    }
#endif
};

namespace be {
    OPT_TEMPLATEPERFORMANCE_GROUP(typename Element_ COMMA typename Block_, VectorPerformance_<Element_ COMMA Block_>::insertlast_, "ext::Vector::InsertLast()", typeid(Element_).name())
    OPT_TEMPLATEPERFORMANCE_GROUP(typename Element_ COMMA typename Block_, VectorPerformance_<Element_ COMMA Block_>::reserve_, "ext::Vector::Reserve()", typeid(Element_).name())
    OPT_TEMPLATEPERFORMANCE_GROUP(typename Element_ COMMA typename Block_, VectorPerformance_<Element_ COMMA Block_>::touch_, "ext::Vector::Touch()", typeid(Element_).name())
}

template <typename LeftElement_, typename LeftBlock_, typename RightElement_, typename RightBlock_>
bool operator ==(const Vector<LeftElement_, LeftBlock_> &lhs, const Vector<RightElement_, RightBlock_> &rhs) {
    return lhs.GetSize() == rhs.GetSize() && CompareEqual(lhs.Begin(), rhs.Begin(), lhs.GetSize());
}

template <typename LeftElement_, typename LeftBlock_, typename RightElement_, typename RightBlock_>
bool operator !=(const Vector<LeftElement_, LeftBlock_> &lhs, const Vector<RightElement_, RightBlock_> &rhs) {
    return lhs.GetSize() != rhs.GetSize() || !CompareEqual(lhs.Begin(), rhs.Begin(), lhs.GetSize());
}

template <typename LeftElement_, typename LeftBlock_, typename RightElement_, typename RightBlock_>
bool operator <(const Vector<LeftElement_, LeftBlock_> &lhs, const Vector<RightElement_, RightBlock_> &rhs) {
    return lhs.GetSize() == rhs.GetSize() && Compare(lhs.Begin(), rhs.Begin(), lhs.GetSize()) < 0 || lhs.GetSize() < rhs.GetSize();
}

template <typename LeftElement_, typename LeftBlock_, typename RightElement_, typename RightBlock_>
bool operator <=(const Vector<LeftElement_, LeftBlock_> &lhs, const Vector<RightElement_, RightBlock_> &rhs) {
    return lhs.GetSize() == rhs.GetSize() && Compare(lhs.Begin(), rhs.Begin(), lhs.GetSize()) <= 0 || lhs.GetSize() < rhs.GetSize();
}

template <typename LeftElement_, typename LeftBlock_, typename RightElement_, typename RightBlock_>
bool operator >(const Vector<LeftElement_, LeftBlock_> &lhs, const Vector<RightElement_, RightBlock_> &rhs) {
    return lhs.GetSize() == rhs.GetSize() && Compare(lhs.Begin(), rhs.Begin(), lhs.GetSize()) > 0 || lhs.GetSize() > rhs.GetSize();
}

template <typename LeftElement_, typename LeftBlock_, typename RightElement_, typename RightBlock_>
bool operator >=(const Vector<LeftElement_, LeftBlock_> &lhs, const Vector<RightElement_, RightBlock_> &rhs) {
    return lhs.GetSize() == rhs.GetSize() && Compare(lhs.Begin(), rhs.Begin(), lhs.GetSize()) >= 0 || lhs.GetSize() > rhs.GetSize();
}

}

namespace etl {
template <typename Element_, typename Block_>
struct IsMemMovable< ext::Vector<Element_, Block_> > {
    static const bool value = true;
}; }

#define _L ext::Vector

#endif//MENES_EXT_VECTOR_HPP
