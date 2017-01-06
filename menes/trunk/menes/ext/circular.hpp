/* Menes - C++ High-Level Utility Library
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

#ifndef MENES_EXT_CIRCULAR_HPP
#define MENES_EXT_CIRCULAR_HPP

#include "cxx/platform.hpp"
#include "ext/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

namespace ext {

template <typename Element_>
class Circular {
    template <typename Element2_>
    friend void ext::Swap(Circular<Element2_> &lhs, Circular<Element2_> &rhs);

  private:
    typedef Circular<Element_> This_;
    typedef Buffer<Element_> Buffer_;

    /* // XXX: fix :(
    template <typename DataIterator_>
    class Iterator_ {
      private:
        typedef Iterator_<DataIterator_> This_;

        Buffer_ &data;
        DataIterator_ i_;

      public:
        Iterator_(const DataIterator_ &i) :
            i_(i)
        {
        }

        template <typename DataIterator_>
        Iterator_(const Iterator_<DataIterator2_> &rhs) :
            i_(rhs.i_)
        {
        }

        This_ &operator ++() {
            if (++i_ == data.End())
                i_ = data.Begin();
            return *this;
        }

        This_ &operator --() {
            if (i_-- == data.Begin())
                i_ = data.End() - 1;
            return *this;
        }

        This_ operator ++(int) {
            This_ temp(*this);
            operator ++();
            return temp;
        }

        This_ operator --(int) {
            This_ temp(*this);
            operator --();
            return temp;
        }

        Element_ &operator *() const {
            return *data;
        }
    };*/

  private:
    Buffer_ data_;
    size_t size_;

    Buffer_::Iterator head_;

    inline bool IsReversed_() const {
        return data_.End() - head_ > size_;
    }

    inline Buffer_::Iterator Tail_() const {
        return IsReversed_() ?
            size_ - (data_.End() - head_) + head_ :
            head_ + size_;
    }

  public:
    Circular() :
        size_(0),
        head_(data_.Begin())
    {
    }

    Circular(const This_ &rhs) :
        size_(0),
        head_(data_.Begin())
    {
        Assign(rhs);
    }

    template <typename Element2_>
    Circular(const Circular<Element2_> &rhs) :
        size_(0),
        head_(data_.Begin())
    {
        Assign(rhs);
    }

    ~Circular() {
        Clear();
    }

    size_t GetCapacity() const {
        return data_.GetSize();
    }

    void Reserve(size_t capacity) {
        if (capacity <= data_.GetSize())
            return;

        Buffer_ data(capacity);
        if (IsReversed_())
            MoveConstruct<Element_>(
                MoveConstruct<Element_>(data.Begin(), head_, data_.End()),
                data.Begin(),
                Tail_()
            );
        else
            MoveConstruct<Element_>(data.Begin(), head_, Tail_());

        ext::Swap(data_, data);
        head_ = data.Begin();
    }

    template <typename Iterator_>
    void InsertLast(Iterator_ begin, Iterator_ end) {
        while (begin != end)
            InsertLast(*(begin++));
    }

    template <typename Element2_>
    void Assign(const Circular<Element2_> &rhs) {
        Clear();
        Reserve(rhs.GetSize());

        if (rhs.IsReversed_())
            CopyConstruct<Element_>(
                CopyConstruct<Element_>(data_.Begin(), rhs.head_, rhs.data_.End()),
                rhs.data_.Begin(),
                rhs.Tail_()
            );
        else
            CopyConstruct<Element_>(data_.Begin(), rhs.head_, rhs.Tail_());

        size_ = rhs.GetSize();
    }

    template <typename Iterator_>
    void Assign(Iterator_ begin, Iterator_ end) {
        Clear();
        InsertLast(begin, end);
    }

    size_t GetSize() const {
        return size_;
    }

    bool IsEmpty() const {
        return GetSize() == 0;
    }

    bool IsFull() const {
        return GetSize() == data_.GetSize();
    }

    template <typename Type_>
    void InsertFirst() {
        Reserve(GetSize() + 1);

        if (head_ == data_.Begin())
            head_ = data_.End();

        new (--head_) Element_();
        ++size_;
    }

    template <typename Type_>
    void InsertFirst(const Type_ &value) {
        Reserve(GetSize() + 1);

        if (head_ == data_.Begin())
            head_ = data_.End();

        new (--head_) Element_(value);
        ++size_;
    }

    template <typename Type_>
    void InsertLast() {
        Reserve(GetSize() + 1);

        new (Tail_()) Element_();
        ++size_;
    }

    template <typename Type_>
    void InsertLast(const Type_ &value) {
        Reserve(GetSize() + 1);

        new (Tail_()) Element_(value);
        ++size_;
    }

    void RemoveFirst() {
        _assert(!IsEmpty());

        head_->~Element();
        --size_;

        if (++head_ == data_.End())
            head_ = data_.Begin();
    }

    void RemoveLast() {
        _assert(!IsEmpty());

        --size_;
        Tail_()->~Element();
    }

    void Clear() {
        if (IsReversed_()) {
            Deconstruct<Element_>(head_, data_.End());
            Deconstruct<Element_>(data_.Begin(), Tail_());
        } else
            Deconstruct<Element_>(head_, Tail_());

        head_ = data_.Begin();
        size_ = 0;
    }

    void Delete() {
        Clear();
        data_.Delete();
    }

    const Type_ &operator [](size_t index) const {
        _assert(index < GetSize());
        index += head_ - data_.Begin();
        if (index >= data_.GetSize())
            index -= data_.GetSize();
        return data_[index];
    }

    Type_ &operator [](size_t index) {
        _assert(index < GetSize());
        index += head_ - data_.Begin();
        if (index >= data_.GetSize())
            index -= data_.GetSize();
        return data_[index];
    }
};

template <typename Element_>
void Swap(Circular<Element_> &lhs, Circular<Element_> &rhs) {
    ext::Swap(lhs.data_, rhs.data_);
    ext::Swap(lhs.size_, rhs.size_);
    ext::Swap(lhs.head_, rhs.head_);
}

}

#endif//MENES_EXT_CIRCULAR_HPP
