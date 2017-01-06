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

#ifndef MENES_EXT_ITERATOR_HPP
#define MENES_EXT_ITERATOR_HPP

#include "cxx/platform.hpp"
#include "ext/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "ext/workarounds.hpp"

namespace ext {

template <typename Iterator_>
struct TypeOfIndirect {
    typedef typename Iterator_::Indirect Result;
};

template <typename Type_>
struct TypeOfIndirect<Type_ *> {
    typedef Type_ &Result;
};

template <typename Iterator_>
class IteratorPair {
  public:
    typedef Iterator_ ConstIterator;
    typedef Iterator_ Iterator;

  private:
    Iterator_ begin_;
    Iterator_ end_;

  public:
    _finline IteratorPair(const Iterator_ &begin, const Iterator_ &end) :
        begin_(begin),
        end_(end)
    {
    }

    _finline const ConstIterator &Begin() const {
        return begin_;
    }

    _finline const Iterator &Begin() {
        return begin_;
    }

    _finline const ConstIterator &End() const {
        return end_;
    }

    _finline const Iterator &End() {
        return end_;
    }
};

template <typename Iterator_>
class BoundedIterator {
  private:
    typedef BoundedIterator<Iterator_> This_;

    Iterator_ begin_;
    size_t size_;

  public:
    _finline BoundedIterator(const Iterator_ &begin, size_t size) :
        begin_(begin),
        size_(size)
    {
    }

    const Iterator_ &GetIterator() const {
        return begin_;
    }

    Iterator_ &GetIterator() {
        return begin_;
    }

    typename TypeOfIndirect<Iterator_>::Result operator *() const {
        return *begin_;
    }

    This_ &operator ++() {
        _assert(size_ != 0);
        ++begin_;
        --size_;
        return *this;
    }

    // XXX: the semantics of this are really off
    bool operator ==(const This_ &rhs) const {
        return size_ == rhs.size_;
    }

    bool operator !=(const This_ &rhs) const {
        return !operator ==(rhs);
    }

    size_t operator -(const This_ &rhs) const {
        return rhs.size_ - size_;
    }
};

namespace be {

template <typename Iterator_>
struct UnderlyingIterator_ {
    typedef Iterator_ Result;

    _finline static const Iterator_ &Get(const Iterator_ &iterator) {
        return iterator;
    }

    _finline static Iterator_ &Get(Iterator_ &iterator) {
        return iterator;
    }
};

template <typename Iterator_>
struct UnderlyingIterator_< BoundedIterator<Iterator_> > {
    typedef Iterator_ Result;

    _finline static const Iterator_ &Get(const BoundedIterator<Iterator_> &iterator) {
        return iterator.GetIterator();
    }

    _finline static Iterator_ &Get(BoundedIterator<Iterator_> &iterator) {
        return iterator.GetIterator();
    }
};

}

template <typename Iterator_>
class IteratorSize {
  public:
    typedef BoundedIterator<Iterator_> ConstIterator;
    typedef BoundedIterator<Iterator_> Iterator;

  private:
    Iterator_ begin_;
    size_t size_;

  public:
    _finline IteratorSize(const Iterator_ &begin, size_t size) :
        begin_(begin),
        size_(size)
    {
    }

    _finline ConstIterator Begin() const {
        return ConstIterator(begin_, size_);
    }

    _finline Iterator Begin() {
        return Iterator(begin_, size_);
    }

    _finline ConstIterator End() const {
        return ConstIterator(begin_, 0);
    }

    _finline Iterator End() {
        return Iterator(begin_, 0);
    }
};

namespace be {

template <typename Iterator_>
_finline ext::IteratorPair<Iterator_> IteratorToList_(const Iterator_ &begin, const Iterator_ &end) {
    return ext::IteratorPair<Iterator_>(begin, end);
}

template <typename Iterator_>
_finline ext::IteratorSize<Iterator_> IteratorToList_(const Iterator_ &begin, size_t size) {
    return ext::IteratorSize<Iterator_>(begin, size);
}

}

}

#define _I(lhs, rhs) _unconst(ext::be::IteratorToList_(lhs, rhs))

#endif//MENES_EXT_ITERATOR_HPP
