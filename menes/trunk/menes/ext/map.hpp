/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2003-2004  Jay Freeman (saurik)
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

#ifndef MENES_EXT_MAP_HPP
#define MENES_EXT_MAP_HPP

#include "cxx/platform.hpp"
#include "ext/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "etl/composite.hpp"

#include "ext/algorithm.hpp"
#include "ext/exception.hpp"
#include "ext/listset.hpp"
#include "ext/vector.hpp"

namespace ext {

template <typename Set_, typename Key_, typename Value_>
class Map {
    template <typename SetF_, typename KeyF_, typename ValueF_>
    friend void Swap(Map<SetF_, KeyF_, ValueF_> &lhs, Map<SetF_, KeyF_, ValueF_> &rhs);

  private:
    typedef etl::Pair<Key_, Value_> Element_;

  public:
    typedef typename Set_::ConstIterator ConstIterator;
    typedef typename Set_::Iterator Iterator;

  private:
    typedef Map<Set_, Key_, Value_> This_;

    Set_ set_;

  public:
    virtual ~Map() {
    }

    const Set_ &GetSet() const {
        return set_;
    }

    size_t GetSize() const {
        return set_.GetSize();
    }

    bool IsEmpty() const {
        return set_.IsEmpty();
    }

    ConstIterator Begin() const {
        return set_.Begin();
    }

    Iterator Begin() {
        return set_.Begin();
    }

    ConstIterator End() const {
        return set_.End();
    }

    Iterator End() {
        return set_.End();
    }

    template <typename KeyF_, typename ValueF_>
    etl::Pair<Iterator, bool> Insert(const KeyF_ &key, const ValueF_ &value) {
        /* // XXX: this serves no purpose
        if (Find(key) != set_.End())
            return false; */
        // XXX: this will get fixed when I port to Atum
        // the issue is that I need to construct a fucking
        // temporary Pair because I can't pass multiple
        // constructor arguments through InsertLast into
        // the new on the other end :(
        // set_.Insert(Element_(key, value));
        // XXX: in the mean time, we will use hack #2
        /* // XXX: and _this_ doesn't work because of the KeyOf_() call to remove duplicates
        etl::Pair<Iterator, bool> result(set_.Insert(key));
        if (result.tail != false)
            result.head->tail = value;
        return result; */
        return set_.Insert(Element_(key, value));
    }

    virtual etl::Pair<Iterator, bool> Insert(const Key_ &key, const Value_ &value) {
        return Insert<Key_, Value_>(key, value);
    }

    template <typename KeyF_>
    bool Remove(const KeyF_ &key) {
        return set_.Remove(key);
    }

    virtual bool Remove(const Key_ &key) {
        return Remove<Key_>(key);
    }

    virtual void Clear() {
        set_.Clear();
    }

    template <typename KeyF_>
    ConstIterator Find(const KeyF_ &key) const {
        return set_.Find(key);
    }

    virtual ConstIterator Find(const Key_ &key) const {
        return Find<Key_>(key);
    }

    template <typename KeyF_>
    Iterator Find(const KeyF_ &key) {
        return set_.Find(key);
    }

    virtual Iterator Find(const Key_ &key) {
        return Find<Key_>(key);
    }

    template <typename KeyF_>
    bool Contains(const Key_ &key) const {
        return set_.Contains(key);
    }

    virtual bool Contains(const Key_ &key) const {
        return Contains<Key_>(key);
    }

    // XXX: we really are missing Get(), GetAs<>(), and GetAsOr<>()
    virtual const Value_ &GetOr(const Key_ &key, const Value_ &value) const {
        ConstIterator i(Find(key));
        return i == End() ? value : i->Second();
    }

    virtual const Value_ &operator [](const Key_ &key) const {
        ConstIterator i(Find(key));
        if (i == End())
            throw ext::NotFoundException();
        return i->Second();
    }

    virtual Value_ &operator [](const Key_ &key) {
        // XXX: this is super slow and pretty stupid but is really, really general
        // note that there are some (many?) places that require the Value_() construction
        Iterator i(Find(key));
        return i == End() ? Insert(key, Value_()).First()->Second() : i->Second();
    }

    bool operator ==(const This_ &rhs) const {
        if (GetSize() != rhs.GetSize())
            return false;

        ConstIterator li(Begin());
        ConstIterator ri(rhs.Begin());

        _repeat (GetSize()) {
            if (!(*li == *ri))
                return false;

            ++li;
            ++ri;
        }

        return true;
    }

    bool operator <(const This_ &rhs) const {
        if (GetSize() != rhs.GetSize())
            return GetSize() < rhs.GetSize();

        ConstIterator li(Begin());
        ConstIterator ri(rhs.Begin());

        _repeat (GetSize()) {
            if (!(*li == *ri))
                return *li < *ri;

            ++li;
            ++ri;
        }

        return false;
    }
};

template <typename SetF_, typename KeyF_, typename ValueF_>
void Swap(Map<SetF_, KeyF_, ValueF_> &lhs, Map<SetF_, KeyF_, ValueF_> &rhs) {
    ext::Swap(lhs.set_, rhs.set_);
}

}

#endif//MENES_EXT_MAP_HPP
