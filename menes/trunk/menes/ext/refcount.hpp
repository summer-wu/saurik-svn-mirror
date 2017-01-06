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

#ifndef MENES_EXT_REFCOUNT_HPP
#define MENES_EXT_REFCOUNT_HPP

#include "cxx/platform.hpp"
#include "ext/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "api/atomic.hpp"
#include "etl/bind.hpp"

#include "ext/types.hpp"
#include "ext/utility.hpp"

#include "opt/profile.hpp"

namespace ext {

// XXX: this is somewhat unsafe... ?...
class IndirectCount {
  private:
    api::Atomic *count_;

    void Attach(const IndirectCount &rhs) {
        count_ = rhs.count_;
        if (count_ != NULL)
            ++*count_;
    }

    void Detach() {
        if (count_ != NULL) {
            if (Alone())
                delete count_;
            else
                --*count_;
        }
    }

    static api::Atomic *NewAtomic() {
        OPT_TIMER("ext::IndirectCount::NewAtomic()")
        return new api::Atomic(0);
    }

  public:
    explicit IndirectCount(bool counted = true) :
        count_(counted ? NewAtomic() : NULL)
    {
    }

    IndirectCount(const IndirectCount &rhs) {
        Attach(rhs);
    }

    ~IndirectCount() {
        Detach();
    }

    IndirectCount &operator =(const IndirectCount &rhs) {
        Detach();
        Attach(rhs);
        return *this;
    }

    bool Alone() const {
        return Count() == 0;
    }

    uint32_t Count() const {
        return count_ == NULL ? 1 : count_->GetValue();
    }

    void Reset(bool counted = true) {
        if (!Alone()) {
            Detach();
            count_ = counted ? NewAtomic() : NULL;
        }
    }

    bool operator !() {
        return Alone();
    }
};

template <typename Counted_>
struct ValueReferenceHelper {
    typedef Counted_ Value;

    inline static bool IsEqual(const Value &lhs, const Value &rhs) {
        return lhs == rhs;
    }

    inline static bool IsLess(const Value &lhs, const Value &rhs) {
        return lhs < rhs;
    }

    _finline void Mend(Value &lhs, const Value &rhs) {
        lhs = rhs;
    }

    _finline bool operator ==(const ValueReferenceHelper<Counted_> &) const {
        return true;
    }

    _finline bool operator !=(const ValueReferenceHelper<Counted_> &) const {
        return false;
    }

    _finline bool operator <(const ValueReferenceHelper<Counted_> &) const {
        return false;
    }
};

#ifdef __GNUC__
template <typename Counted_>
#else
template <typename Counted_, Counted_ Null_ = 0>
#endif
struct NullReferenceHelper :
    public ValueReferenceHelper<Counted_>
{
    typedef ValueReferenceHelper<Counted_> Base_;

#ifdef __GNUC__
    static const Counted_ Null_;
#endif

    _finline static void Clear(typename Base_::Value &value) {
        value = Null_;
    }

    _finline static bool IsEmpty(const typename Base_::Value &value) {
        return value == Null_;
    }
};

#ifdef __GNUC__
template <typename Counted_>
const Counted_ NullReferenceHelper<Counted_>::Null_(0);
#endif

template <typename Type_>
struct ReferenceAllocator;

template <typename Counted_, typename Allocator_>
class ReferenceManager;

enum Allocation {
    HeapAllocated,
    StackAllocated
};

class ReferenceCounted {
    template <typename Type_>
    friend struct ReferenceAllocator;

    template <typename Counted_, typename Allocator_>
    friend class ReferenceManager;

    template <typename Counted_>
    friend class HeapReference;

    template <typename Counted_>
    friend class StackReference;

  private:
    mutable api::Atomic count_;

    void Take() const {
        ++count_;
    }

    bool Drop() const {
        return !(--count_);
    }

    virtual Allocation GetAllocation_() = 0;

  protected:
    ReferenceCounted() :
        count_(1)
    {
    }

    ReferenceCounted(const ReferenceCounted &rhs) :
        count_(1)
    {
    }

    ReferenceCounted &operator =(const ReferenceCounted &rhs) {
        // the reference count needs to be maintained
        return *this;
    }

    virtual ~ReferenceCounted() {
        _assert(count_ == 1);
        /*      ^^^^^^^^^^^
              If you are seeing this, you tried to delete a ReferenceCounted
            object. That operation is not supported... don't do that. Someone
            else still has a reference to this object, and if that reference
            were cleared the object would just immediately dissapear anyway.

              This object _may_ have been allocated using ext::StackReference<>,
            in which case it is alright (and required) to delete it, but only
            when all of the outstanding references to the object are gone. This
            deletion didn't meet that criteria :).
        */
    }

    virtual void _initialize() {
    }

    virtual void _finalize() {
    }

  public:
    api::Atomic::Type GetReferenceCount() const {
        return count_;
    }
};

template <typename Counted_>
_finline const ReferenceCounted *ToRefCounted_(const Counted_ *counted) {
    return counted;
}

template <typename Type_>
struct ReferenceAllocator {
    _finline ReferenceAllocator() {
    }

    template <typename Foreign_>
    _finline ReferenceAllocator(const ReferenceAllocator<Foreign_> &rhs) {
    }

    inline static void DeallocateEx(Type_ *address) {
        delete ToRefCounted_(address);
    }
};

template <typename Counted_, typename Allocator_>
class ReferenceManager :
    public NullReferenceHelper<Counted_ *>
{
    template <typename Counted2_, typename Allocator2_>
    friend class ReferenceManager;

  private:
    typedef ReferenceManager<Counted_, Allocator_> This_;
    typedef NullReferenceHelper<Counted_ *> Base_;

  public:
    typedef typename Allocator_::template Template<Counted_>::Result Allocator;

  private:
    Allocator allocator_;

  public:
    ReferenceManager(const Allocator &allocator = Allocator()) :
        allocator_(allocator)
    {
    }

    template <typename CountedF_, typename AllocatorF_>
    ReferenceManager(const ReferenceManager<CountedF_, AllocatorF_> &rhs) :
        allocator_(rhs.allocator_)
    {
    }

    static void Take(typename Base_::Value &lhs, const typename Base_::Value &rhs) {
        ToRefCounted_(lhs = rhs)->Take();
    }

    bool Drop(const typename Base_::Value &value) {
        const ReferenceCounted *counted(ToRefCounted_(value));

        if (!counted->Drop())
            return false;
        else {
            allocator_.DeallocateEx(value);
            return true;
        }
    }
};

template <typename Counted_, typename Manager_>
class Handle :
    public SmartPointer< Counted_, typename Manager_::Value, Handle<Counted_, Manager_> >
{
    friend class SmartPointer< Counted_, typename Manager_::Value, Handle<Counted_, Manager_> >;

  public:
    typedef Manager_ Manager;
    typedef typename Manager_::Value Value;

  private:
    typedef Handle<Counted_, Manager_> This_;

  protected:
    EmptyBase<Manager, Value> manager_;

  private:
    inline Value GetSmartPointer_() const {
        return GetValue();
    }

    _finline void Initialize_(const Value &rhs) {
        if (manager_.IsEmpty(rhs))
            manager_.Clear(GetValue_());
        else
            manager_.Take(GetValue_(), rhs);
    }

  protected:
    Manager &GetManager_() {
        return manager_;
    }

    Value &GetValue_() {
        return manager_.member_;
    }

  public:
    Handle(const Manager &manager = Manager()) :
        manager_(manager)
    {
        manager_.Clear(GetValue_());
    }

    Handle(Value rhs, bool mended = false, const Manager &manager = Manager()) :
        manager_(manager)
    {
        if (mended)
            manager_.Mend(GetValue_(), rhs);
        else
            Initialize_(rhs);
    }

    Handle(const This_ &rhs) :
        manager_(rhs.GetManager())
    {
        Initialize_(rhs.GetValue());
    }

    // XXX: maybe I should check if the other guy is empty with his manager?
    template <typename Counted2_, typename Manager2_>
    Handle(const Handle<Counted2_, Manager2_> &rhs) :
        manager_(rhs.GetManager())
    {
        Initialize_(rhs.GetValue());
    }

    inline ~Handle() {
        Clear();
    }

    bool Clear() {
        if (manager_.IsEmpty(GetValue()))
            return true;

        bool released(manager_.Drop(GetValue()));
        manager_.Clear(GetValue_());
        return released;
    }

    void Assign(const Value &rhs) {
        if (GetValue() == rhs)
            return;
        if (manager_.IsEmpty(rhs)) {
            if (!manager_.IsEmpty(GetValue()))
                manager_.Drop(GetValue_());
            manager_.Clear(GetValue_());
        } else {
            Value old(GetValue());
            manager_.Take(GetValue_(), rhs);
            if (!manager_.IsEmpty(old))
                manager_.Drop(old);
        }
    }

    void Assign(const Value &rhs, const Manager_ &manager) {
        if (manager.IsEmpty(rhs)) {
            if (!manager_.IsEmpty(GetValue()))
                manager_.Drop(GetValue_());
            manager_.Clear(GetValue_());
        } else {
            Value old(GetValue());
            manager.Take(GetValue_(), rhs);
            if (!manager_.IsEmpty(old))
                manager_.Drop(old);
        }

        GetManager_() = manager;
    }

    This_ &operator =(const Value &rhs) {
        Assign(rhs);
        return *this;
    }

    This_ &operator =(const This_ &rhs) {
        Assign(rhs.GetValue(), rhs.GetManager());
        return *this;
    }

    template <typename Counted2_, typename Manager2_>
    This_ &operator =(const Handle<Counted2_, Manager2_> &rhs) {
        Assign(rhs.GetValue(), rhs.GetManager());
        return *this;
    }

    const Manager &GetManager() const {
        return manager_;
    }

    inline const Value &GetValue() const {
        return manager_.member_;
    }

    inline operator const Value &() const {
        return GetValue();
    }

    bool IsEmpty() const {
        return manager_.IsEmpty(GetValue());
    }

    _deprecated Value Tear() {
        Value torn;
        if (IsEmpty())
            manager_.Clear(torn);
        else
            manager_.Take(torn, GetValue());
        return torn;
    }

    Value Release() {
        Value old(GetValue());
        manager_.Clear(GetValue_());
        return old;
    }

    // XXX: this doesn't call manager_::Mend()
    _deprecated Value &Mend() {
        Clear();
        return GetValue_();
    }

    void Mend(Value rhs) {
        Clear();
        manager_.Mend(GetValue_(), rhs);
    }

    bool operator !() const {
        return IsEmpty();
    }

    inline bool operator ==(const Value &rhs) const {
        return manager_.IsEqual(GetValue(), rhs);
    }

    bool operator ==(const This_ &rhs) const {
        return GetManager() == rhs.GetManager() && operator ==(rhs.GetValue());
    }

    inline bool operator !=(const Value &rhs) const {
        return !manager_.IsEqual(GetValue(), rhs);
    }

    bool operator !=(const This_ &rhs) const {
        return GetManager() != rhs.GetManager() || operator !=(rhs.GetValue());
    }

    inline bool operator <(const Value &rhs) const {
        return manager_.IsLess(GetValue(), rhs);
    }

    bool operator <(const This_ &rhs) const {
        return GetManager() == rhs.GetManager() ? operator <(rhs.GetValue()) : GetManager() < rhs.GetManager();
    }
};

}

#define EXT_HANDLESTUB(Type) \
const ext::ReferenceCounted *ToRefCounted_(const Type *counted);

#define EXT_HANDLESHIM(Type) \
const ext::ReferenceCounted *ToRefCounted_(const Type *counted) { \
    return counted; \
}

#define _R ext::Handle

#endif//MENES_EXT_REFCOUNT_HPP
