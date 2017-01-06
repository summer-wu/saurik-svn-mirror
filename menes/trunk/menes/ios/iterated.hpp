/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2004  Jay Freeman (saurik)
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

#ifndef MENES_IOS_ITERATED_HPP
#define MENES_IOS_ITERATED_HPP

#include "cxx/platform.hpp"
#include "ios/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "ext/iterator.hpp"
#include "ext/traits.hpp"

#include "ios/streams.hpp"

namespace ios {

template <typename Iterator_>
class IteratedStream :
    public Reader
{
  protected:
    Iterator_ current_;

  public:
    IteratedStream() :
        current_()
    {
    }

    IteratedStream(const Iterator_ &current) :
        current_(current)
    {
    }

    virtual size_t Read(byte_t *data, size_t length) {
        // XXX: fucking-a
        byte_t *end(ext::CopyConstruct<byte_t>(data, _I(current_, length)));
        size_t size(end - data);
        current_ += size;
        return size;
    }

    operator const Iterator_ &() const {
        return current_;
    }

    operator Iterator_ &() {
        return current_;
    }
};

template <typename Iterator_>
class BoundedIteratedStream :
    public IteratedStream<Iterator_>
{
  private:
    typedef IteratedStream<Iterator_> Base_;

  protected:
    Iterator_ end_;

  public:
    BoundedIteratedStream() :
        end_()
    {
    }

    BoundedIteratedStream(const Iterator_ &current, const Iterator_ &end) :
        IteratedStream<Iterator_>(current),
        end_(end)
    {
    }

    virtual size_t Read(byte_t *data, size_t length) {
        size_t left(length);
        for (; left != 0 && Base_::current_ != end_; --left)
            *(data++) = *(Base_::current_++);
        return length - left;
    }

    bool AtEnd() const {
        return Base_::current_ == end_;
    }

    Iterator_ &operator ++() {
        if (Base_::current_ == end_)
            throw ext::EosException();
        return ++Base_::current_;
    }

    Iterator_ operator ++(int) {
        if (Base_::current_ == end_)
            throw ext::EosException();
        return Base_::current_++;
    }
};

template <typename Iterator_>
class ResettableIteratedStream :
    public BoundedIteratedStream<Iterator_>
{
  private:
    typedef BoundedIteratedStream<Iterator_> Base_;

    Iterator_ begin_;

  public:
    ResettableIteratedStream(const Iterator_ &begin, const Iterator_ &end) :
        BoundedIteratedStream<Iterator_>(begin, end),
        begin_(begin)
    {
    }

    void Reset() {
        Base_::current_ = begin_;
    }
};

typedef IteratedStream<const byte_t *> MemoryReader;
typedef BoundedIteratedStream<const byte_t *> MemoryRangeReader;

}

#endif//MENES_IOS_ITERATED_HPP
