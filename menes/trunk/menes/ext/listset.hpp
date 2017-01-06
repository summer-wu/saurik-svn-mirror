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

#ifndef MENES_EXT_LISTSET_HPP
#define MENES_EXT_LISTSET_HPP

#include "cxx/platform.hpp"
#include "ext/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "ext/algorithm.hpp"
#include "ext/list.hpp"
#include "ext/set.hpp"
#include "ext/vector.hpp"

namespace ext {

/*namespace be {

template <typename Storage_>
struct DuplicateFilter_ {
    template <typename Type_>
    _finline static bool Execute(const Storage_ &storage, const Type_ &value) {
        return FindFirst(storage, value) != storage.End();
    }
};

template <typename Storage_>
struct NoFilter_ {
    template <typename Type_>
    _finline static bool Execute(const Storage_ &storage, const Type_ &value) {
        return false;
    }
};

}*/

template <
    typename Element_,
    typename Key_,
    typename KeyOf_,
    typename List_
> class ListSet :
    public Set<Element_, Key_>
{
    template <typename ListF_>
    friend void Swap(ListSet<ListF_> &lhs, ListSet<ListF_> &rhs);

  public:
    typedef typename List_::ConstIterator ConstIterator;
    typedef typename List_::Iterator Iterator;

  private:
    typedef ListSet<Element_, Element_, KeyOf_> This_;

    List_ list_;

  public:
    ListSet() {
    }

    size_t GetSize() const {
        return list_.GetSize();
    }

    bool IsEmpty() const {
        return list_.IsEmpty();
    }

    operator bool() const {
        return !IsEmpty();
    }

    ConstIterator Begin() const {
        return list_.Begin();
    }

    Iterator Begin() {
        return list_.Begin();
    }

    ConstIterator End() const {
        return list_.End();
    }

    Iterator End() {
        return list_.End();
    }

    template <typename ElementF_>
    etl::Pair<Iterator, bool> Insert(const ElementF_ &value) {
        Iterator i(Find(KeyOf_::Execute(value)));
        if (i != list_.End())
            return etl::Pair<Iterator, bool>(i, false);
        list_.InsertLast(value);
        return etl::Pair<Iterator, bool>(list_.End() - 1, true);
    }

    virtual etl::Pair<Iterator, bool> Insert(const Element_ &value) {
        return Insert<Element_>(value);
    }

    // XXX: this assumes a fast swap and a slow copy constructor
    template <typename KeyF_>
    bool Remove(const KeyF_ &key) {
        typename List_::Iterator i(Find(key));
        if (i == list_.End())
            return false;
        if (i != list_.End() - 1)
            ext::Swap(*i, *(list_.End() - 1));
        list_.RemoveLast();
        return true;
    }

    virtual bool Remove(const Key_ &key) {
        return Remove<Key_>(key);
    }

    virtual void Clear() {
        list_.Clear();
    }

    template <typename KeyF_>
    ConstIterator Find(const KeyF_ &key) const {
        OPT_TIMER_GROUP("ext::ListSet::operator []() const", typeid(Element_).name())
        return FindFirstWith< KeyEqualTo<KeyOf_> >(list_, key);
    }

    virtual ConstIterator Find(const Key_ &key) const {
        return Find<Key_>(key);
    }

    template <typename KeyF_>
    Iterator Find(const KeyF_ &key) {
        OPT_TIMER_GROUP("ext::ListSet::Find()", typeid(Element_).name())
        return FindFirstWith< KeyEqualTo<KeyOf_> >(list_, key);
    }

    virtual Iterator Find(const Key_ &key) {
        return Find<Key_>(key);
    }

    template <typename KeyF_>
    bool Contains(const Key_ &key) const {
        return Find(key) != End();
    }

    virtual bool Contains(const Key_ &key) const {
        return Contains<Key_>(key);
    }
};

template <typename List_>
void Swap(ListSet<List_> &lhs, ListSet<List_> &rhs) {
    ext::Swap(lhs.list_, rhs.list_);
}

}

#endif//MENES_EXT_LISTSET_HPP
