/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2001-2005  Jay Freeman (saurik)
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

#ifndef MENES_EXT_QUEUES_HPP
#define MENES_EXT_QUEUES_HPP

#include "cxx/platform.hpp"
#include "ext/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "ext/vector.hpp"

#include <algorithm>

namespace ext {

template <typename Element_>
class Queue :
    public Vector<Element_>
{
  private:
    typedef Vector<Element_> Base_;

  public:
    template <typename Type_>
    _finline void Push(const Type_ &value) {
        Base_::InsertLast(value);
    }

    _finline void Pop() {
        Base_::RemoveFirst();
    }

    template <typename Type_>
    _finline bool Pop(Type_ &value) {
        if (Base_::IsEmpty())
            return false;

        value = Front();
        Pop();
        return true;
    }

    _finline const Element_ &Front() const {
        return Base_::First();
    }

    _finline Element_ &Front() {
        return Base_::First();
    }
};

/*template <
    typename Elem_,
    typename Cont_ = ext::Vector<Elem_>,
    typename Pred_ = std::less<Elem_>
>
class PriorityQueue {
    friend class iterator;

  protected:
    typedef typename Cont_::Iterator itertype_;

  public:
    typedef typename Cont_::Value Value;
    typedef Cont_ cont_type;

  protected:
    cont_type cont_;
    Pred_ pred_;
    itertype_ end_;

  public:
    explicit PriorityQueue(const Pred_ &pred = Pred_()) :
        pred_(pred)
    {
    }

    template <typename In_>
    PriorityQueue(In_ first, In_ last, const Pred_ &pred = Pred_()) :
        pred_(pred)
    {
        for (In_ i(first); i != last; ++i)
            push(*i);
    }

    bool empty() const {
        return cont_.IsEmpty();
    }

    size_t size() const {
        return cont_.size();
    }

    const Pred_ &GetPredicate() const {
        return pred_;
    }

    const Elem_ &top() const {
        return cont_.First();
    }

    Value &top() {
        return end_ == cont_.End() ? cont_.First() : *end_;
    }

    void push(const Value &elem) {
        cont_.InsertLast(elem);
        std::push_heap(cont_.Begin(), end_ = cont_.End(), pred_);
    }

    void pop() {
        if (end_ == cont_.End()) {
            std::pop_heap(cont_.Begin(), cont_.End(), pred_);
            cont_.RemoveLast();
            end_ = cont_.End();
        } else {
            cont_.RemoveAt(end_);
        }
    }

    bool next() {
        if (cont_.begin() == end_)
            return false;
        std::pop_heap(cont_.begin(), end_--, pred_);
        return true;
    }

    void reheap() {
        std::make_heap(cont_.Begin(), end_ = cont_.End(), pred_);
    }

    cont_type &cont() {
        return cont_;
    }
};*/

}

#endif//MENES_EXT_QUEUES_HPP
