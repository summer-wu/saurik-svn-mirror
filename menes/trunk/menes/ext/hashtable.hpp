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

#ifndef MENES_EXT_HASHTABLE_HPP
#define MENES_EXT_HASHTABLE_HPP

#include "cxx/platform.hpp"
#include "ext/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "ext/block.hpp"
#include "ext/listmap.hpp"
#include "ext/utility.hpp"
#include "ext/vector.hpp"

#include "opt/profile.hpp"

namespace ext {

template <typename Type_>
struct ShimHasher {
    inline uint32_t operator ()(const Type_ &value) const {
        return Hash(value);
    }
};

template <typename Type_>
struct Identity {
    inline const Type_ &operator ()(const Type_ &value) const {
        return value;
    }
};

template < typename Value_, typename Keyer_ = Identity<Value_>, typename Hasher_ = ShimHasher<Value_> >
class HashSet {
    template <typename ElementF_, typename HasherF_>
    friend void ext::Swap(HashSet<ElementF_, HasherF_> &lhs, HashSet<ElementF_, HasherF_> &rhs);

  private:
    typedef HashSet<Value_, Keyer_, Hasher_> &This_;

    typedef uint32_t Hash_;

    EmptyBase<Keyer_, size_t> keyer_;
    EmptyBase<Hasher_, size_t> hasher_;

    typedef Pair<Hash_, Value_> Entry_;
    typedef ListMap<Key_, Entry_> Bucket_;
    typedef Pointer< Bucket_, NewArrayAllocator<Bucket_> > Buckets_;

    Buckets_ buckets_;

    size_t &GetBits_() {
        return key_.member_;
    }

    size_t &GetSize_() {
        return hasher_.member_;
    }

    inline Bucket_ &GetBucketByHash_(const Hash_ &hash) {
        return buckets_[hash & ((1 << bits_) - 1)];
    }

    Bucket_ &GetBucketByValue_(const Value_ &key) {
        return GetBucketByHash_(hasher_(key));
    }

    void Increase_() {
        OPT_TIMER("HashSet::Increase()")

        size_t buckets(1 << bits_);
        Buckets_ old(new Bucket_[1 << ++bits_]);
        ext::Swap(old, buckets_);

        GetSize_() = 0;

        for (size_t b(0); b != buckets; ++b)
            for (typename Bucket_::ConstIterator i(old[b].Begin()); i != old[b].End(); ++i)
                Insert(i->head, i->tail);
    }

  public:
    HashSet(size_t bits = 2, const Hasher_ &hasher = Hasher_()) :
        hasher_(hasher, 0),
        bits_(bits),
        buckets_(new Bucket_[1 << bits_])
    {
    }

    size_t GetSize() const {
        return hasher_.member_;
    }

    bool Insert(const Value_ &key) {
        OPT_TIMER("HashSet::Insert(key)")
        return Insert(key, hasher_(key));
    }

    bool Insert(const Value_ &key, const Hash_ &hash) {
        if (GetSize() > (1 << bits_) / 2)
            Increase_();

        OPT_TIMER("HashSet::Insert(key, hash)")
        if (!GetBucketByHash_(hash).Insert(key, hash))
            return false;

        ++GetSize_();
        return true;
    }

    bool Remove(const Value_ &key) {
        if (!GetBucketByValue_(key).Remove(key))
            return false;
        --GetSize_();
        return true;
    }
};

template <typename Value_, typename HasherF_>
void Swap(HashSet<Value_, HasherF_> &lhs, HashSet<Value_, HasherF_> &rhs) {
    ext::Swap(lhs.hasher_, rhs.hasher_);
    ext::Swap(lhs.buckets_, rhs.buckets_);
    ext::Swap(lhs.bits, rhs.bits);
}

template <typename Type_>
typename etl::IfTrue<etl::Limits<Type_>::IsIntegral, uint32_t>::Result
Hash(const Type_ &value) {
    return static_cast<uint32_t>(value);
}

}

#endif//MENES_EXT_HASHTABLE_HPP
