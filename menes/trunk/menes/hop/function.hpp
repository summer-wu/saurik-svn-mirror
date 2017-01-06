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

#ifndef MENES_ETL_FUNCTION_HPP
#define MENES_ETL_FUNCTION_HPP

#include "cxx/platform.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "etl/list.hpp"
#include "etl/manip.hpp"
#include "etl/vtable.hpp"

#include "ext/refcount.hpp"

namespace hop {

// XXX: this is a horrible hack
// XXX: make this work on PARISC
#ifdef _MSC_VER
#pragma pack(push, etl_structalign)
#pragma pack(4)
#endif

template <typename Cell_>
class ArgStruct {
  private:
    typedef ArgStruct<Cell_> This_;

  public:
    typedef typename Cell_::Head Head;
    typedef ArgStruct<typename Cell_::Tail> Tail;

    Head head_;
    Tail tail_;

    ArgStruct() {
    }

    // forces broken aggregate abi on Unix
    ArgStruct(const This_ &rhs) :
        head_(rhs.head_),
        tail_(rhs.tail_)
    {
    }

    ArgStruct(typename etl::ReferenceTo<const Head>::Result head, const Tail &tail) :
        head_(head),
        tail_(tail)
    {
    }

    ArgStruct(
        const ArgStruct<typename etl::Clip<Cell_, 1>::Result> &head,
        typename etl::ReferenceTo<
            const typename etl::At<
                Cell_,
                etl::Length<Cell_>::value - 1
            >::Result
        >::Result tail
    ) :
        head_(head.head_),
        tail_(head.tail_, tail)
    {
    }
};

template <typename Type_>
class ArgStruct< etl::Cons<Type_> > {
  private:
    typedef ArgStruct< etl::Cons<Type_> > This_;

  public:
    typedef Type_ Head;
    typedef ArgStruct<etl::Null> Tail;

    Type_ head_;

    ArgStruct() {
    }

    // forces broken aggregate abi on Unix
    ArgStruct(const ArgStruct< etl::Cons<Type_> > &rhs) :
        head_(rhs.head_)
    {
    }

    ArgStruct(typename etl::ReferenceTo<const Head>::Result rhs) :
        head_(rhs)
    {
    }

    ArgStruct(typename etl::ReferenceTo<const Head>::Result head, const Tail &tail) :
        head_(head)
    {
    }
};

template <>
class ArgStruct<etl::Null> {
  private:
    typedef ArgStruct<etl::Null> This_;

  public:
    ArgStruct() {
    }

    // forces broken aggregate abi on Unix
    ArgStruct(const ArgStruct<etl::Null> &rhs) {
    }
};

#ifdef _MSC_VER
#pragma pack(pop, etl_structalign)
#endif

template <typename Type_>
struct Function;

template <typename Type_>
struct FunctionTraits {
    typedef typename Type_::Traits::Type Type;

    typedef typename Type_::Traits::Return Return;
    typedef typename Type_::Traits::Args Args;

    typedef typename Type_::Traits::Host Host;
    typedef typename Type_::Traits::Member Member;

    typedef typename Type_::Traits::Pointer Pointer;
    typedef typename Type_::Traits::Prototype Prototype;
};

template <typename Prototype_>
class FunctionCall;

// XXX: move somewhere else
namespace be {
class Empty {
}; }

template <typename Derived_, typename Prototype_, typename Base_ = be::Empty>
class FunctionInvoke;

namespace be {
    template <typename Type_>
    struct IsPointerToClass_ {
        static const bool value = false;
    };

    template <typename Type_>
    struct IsPointerToClass_<Type_ *> {
        static const bool value = etl::IsClass<Type_>::value;
    };

    template <typename Return_, typename Args_, bool IsHost_ = IsPointerToClass_<typename Args_::Head>::value>
    struct FunctionMember_;

    template <typename Return_, typename Args_>
    struct FunctionMember_<Return_, Args_, false> {
        typedef void Host;
        typedef void **Member;
    };
}

template <typename Prototype_>
struct FunctionValue {
    typedef FunctionTraits<Prototype_> Traits;
    typedef typename Traits::Return (*Invoker)(const Function<Prototype_> *, ArgStruct<typename Traits::Args>);

    Invoker invoker;

    union {
        void *data;
        typename Traits::Member member;
    };

    FunctionValue() :
        invoker(NULL),
        data(NULL)
    {
    }

    FunctionValue(Invoker invoker, void *data) :
        invoker(invoker),
        data(data)
    {
    }

    FunctionValue(Invoker invoker, typename Traits::Member member) :
        invoker(invoker),
        member(member)
    {
    }
};

template <typename Prototype_>
class FunctionManager {
  private:
    typedef FunctionManager<Prototype_> This_;

  public:
    typedef FunctionTraits<Prototype_> Traits;
    typedef FunctionValue<Prototype_> Value;

  private:
    const etl::TypeVTable *table_;

  public:
    FunctionManager() :
        table_(NULL)
    {
    }

    FunctionManager(const etl::TypeVTable *table) :
        table_(table)
    {
    }

    This_ &operator =(const etl::TypeVTable *table) {
        table_ = table;
        return *this;
    }

    bool operator ==(const This_ &rhs) const {
        return table_ == rhs.table_;
    }

    bool operator !=(const This_ &rhs) const {
        return !operator ==(rhs);
    }

    bool operator <(const This_ &rhs) const {
        return table_ < rhs.table_;
    }

    inline void Clear(Value &value) const {
        value.invoker = NULL;
        value.data = NULL;
        value.member = NULL;
    }

    inline bool IsEmpty(const Value &value) const {
        return value.invoker == NULL;
    }

    // XXX: *sigh*, this is still somewhat bogus... hehehe
    bool IsEqual(const Value &lhs, const Value &rhs) const {
        if (lhs.invoker != rhs.invoker)
            return false;
        if (lhs.invoker == NULL)
            return true;
        return table_ == NULL ? lhs.data == rhs.data : table_->Equality(lhs.data, rhs.data);
    }

    // XXX: *sigh*, this is still somewhat bogus... hehehe
    bool IsLess(const Value &lhs, const Value &rhs) const {
        if (lhs.invoker != rhs.invoker)
            return lhs.invoker < rhs.invoker;
        if (lhs.invoker == NULL)
            return false;
        return table_ == NULL ? lhs.data < rhs.data : table_->Less(lhs.data, rhs.data);
    }

    void Take(Value &lhs, const Value &rhs) const {
        if (table_ == NULL)
            lhs = rhs;
        else {
            lhs.invoker = rhs.invoker;
            lhs.data = table_->CopyConstructor(rhs.data);
        }
    }

    bool Drop(const Value &value) const {
        if (table_ != NULL)
            table_->Delete(value.data);
        return true;
    }
};

#define MENES_ETL_ARG_ARITY 0
#define MENES_ETL_ARG_ARGS0
#define MENES_ETL_ARG_ARGS1
#define MENES_ETL_ARG_CALL0
#define MENES_ETL_ARG_CALL1
#define MENES_ETL_ARG_CONS etl::Null
#define MENES_ETL_ARG_PARAMS
#define MENES_ETL_ARG_TYPENAME
#define MENES_ETL_ARG_TYPES
#include "hop/function_.hpp"
#undef MENES_ETL_ARG_TYPES
#undef MENES_ETL_ARG_TYPENAME
#undef MENES_ETL_ARG_PARAMS
#undef MENES_ETL_ARG_CONS
#undef MENES_ETL_ARG_CALL1
#undef MENES_ETL_ARG_CALL0
#undef MENES_ETL_ARG_ARITY
#undef MENES_ETL_ARG_ARGS1
#undef MENES_ETL_ARG_ARGS0
#undef MENES_ETL_ARG_ARITY

#define MENES_ETL_ARG_ARITY 1
#define MENES_ETL_ARG_ARGS0 arg0
#define MENES_ETL_ARG_ARGS1
#define MENES_ETL_ARG_CALL0 args.head_
#define MENES_ETL_ARG_CALL1
#define MENES_ETL_ARG_CONS etl::Cons<Arg0_>
#define MENES_ETL_ARG_PARAMS Arg0_ arg0
#define MENES_ETL_ARG_TYPENAME typename Arg0_
#define MENES_ETL_ARG_TYPES Arg0_
#include "hop/function_.hpp"
#undef MENES_ETL_ARG_TYPES
#undef MENES_ETL_ARG_TYPENAME
#undef MENES_ETL_ARG_PARAMS
#undef MENES_ETL_ARG_CONS
#undef MENES_ETL_ARG_CALL1
#undef MENES_ETL_ARG_CALL0
#undef MENES_ETL_ARG_ARGS1
#undef MENES_ETL_ARG_ARGS0
#undef MENES_ETL_ARG_ARITY

#define MENES_ETL_ARG_ARITY 2
#define MENES_ETL_ARG_ARGS0 arg0, arg1
#define MENES_ETL_ARG_ARGS1 arg1
#define MENES_ETL_ARG_CALL0 args.head_, args.tail_.head_
#define MENES_ETL_ARG_CALL1 args.head_
#define MENES_ETL_ARG_CONS etl::Cons<Arg0_, etl::Cons<Arg1_> >
#define MENES_ETL_ARG_PARAMS Arg0_ arg0, Arg1_ arg1
#define MENES_ETL_ARG_TYPENAME typename Arg0_, typename Arg1_
#define MENES_ETL_ARG_TYPES Arg0_, Arg1_
#include "hop/function_.hpp"
#undef MENES_ETL_ARG_TYPES
#undef MENES_ETL_ARG_TYPENAME
#undef MENES_ETL_ARG_PARAMS
#undef MENES_ETL_ARG_CONS
#undef MENES_ETL_ARG_CALL1
#undef MENES_ETL_ARG_CALL0
#undef MENES_ETL_ARG_ARGS1
#undef MENES_ETL_ARG_ARGS0
#undef MENES_ETL_ARG_ARITY

#define MENES_ETL_ARG_ARITY 3
#define MENES_ETL_ARG_ARGS0 arg0, arg1, arg2
#define MENES_ETL_ARG_ARGS1 arg1, arg2
#define MENES_ETL_ARG_CALL0 args.head_, args.tail_.head_, args.tail_.tail_.head_
#define MENES_ETL_ARG_CALL1 args.head_, args.tail_.head_
#define MENES_ETL_ARG_CONS etl::Cons<Arg0_, etl::Cons<Arg1_, etl::Cons<Arg2_> > >
#define MENES_ETL_ARG_PARAMS Arg0_ arg0, Arg1_ arg1, Arg2_ arg2
#define MENES_ETL_ARG_TYPENAME typename Arg0_, typename Arg1_, typename Arg2_
#define MENES_ETL_ARG_TYPES Arg0_, Arg1_, Arg2_
#include "hop/function_.hpp"
#undef MENES_ETL_ARG_TYPES
#undef MENES_ETL_ARG_TYPENAME
#undef MENES_ETL_ARG_PARAMS
#undef MENES_ETL_ARG_CONS
#undef MENES_ETL_ARG_CALL1
#undef MENES_ETL_ARG_CALL0
#undef MENES_ETL_ARG_ARGS1
#undef MENES_ETL_ARG_ARGS0
#undef MENES_ETL_ARG_ARITY

#define MENES_ETL_ARG_ARITY 4
#define MENES_ETL_ARG_ARGS0 arg0, arg1, arg2, arg3
#define MENES_ETL_ARG_ARGS1 arg1, arg2, arg3
#define MENES_ETL_ARG_CALL0 args.head_, args.tail_.head_, args.tail_.tail_.head_, args.tail_.tail_.tail_.head_
#define MENES_ETL_ARG_CALL1 args.head_, args.tail_.head_, args.tail_.tail_.head_
#define MENES_ETL_ARG_CONS etl::Cons<Arg0_, etl::Cons<Arg1_, etl::Cons<Arg2_, etl::Cons<Arg3_> > > >
#define MENES_ETL_ARG_PARAMS Arg0_ arg0, Arg1_ arg1, Arg2_ arg2, Arg3_ arg3
#define MENES_ETL_ARG_TYPENAME typename Arg0_, typename Arg1_, typename Arg2_, typename Arg3_
#define MENES_ETL_ARG_TYPES Arg0_, Arg1_, Arg2_, Arg3_
#include "hop/function_.hpp"
#undef MENES_ETL_ARG_TYPES
#undef MENES_ETL_ARG_TYPENAME
#undef MENES_ETL_ARG_PARAMS
#undef MENES_ETL_ARG_CONS
#undef MENES_ETL_ARG_CALL1
#undef MENES_ETL_ARG_CALL0
#undef MENES_ETL_ARG_ARGS1
#undef MENES_ETL_ARG_ARGS0
#undef MENES_ETL_ARG_ARITY

#define MENES_ETL_ARG_ARITY 5
#define MENES_ETL_ARG_ARGS0 arg0, arg1, arg2, arg3, arg4
#define MENES_ETL_ARG_ARGS1 arg1, arg2, arg3, arg4
#define MENES_ETL_ARG_CALL0 args.head_, args.tail_.head_, args.tail_.tail_.head_, args.tail_.tail_.tail_.head_, args.tail_.tail_.tail_.tail_.head_
#define MENES_ETL_ARG_CALL1 args.head_, args.tail_.head_, args.tail_.tail_.head_, args.tail_.tail_.tail_.head_
#define MENES_ETL_ARG_CONS etl::Cons<Arg0_, etl::Cons<Arg1_, etl::Cons<Arg2_, etl::Cons<Arg3_, etl::Cons<Arg4_> > > > >
#define MENES_ETL_ARG_PARAMS Arg0_ arg0, Arg1_ arg1, Arg2_ arg2, Arg3_ arg3, Arg4_ arg4
#define MENES_ETL_ARG_TYPENAME typename Arg0_, typename Arg1_, typename Arg2_, typename Arg3_, typename Arg4_
#define MENES_ETL_ARG_TYPES Arg0_, Arg1_, Arg2_, Arg3_, Arg4_
#include "hop/function_.hpp"
#undef MENES_ETL_ARG_TYPES
#undef MENES_ETL_ARG_TYPENAME
#undef MENES_ETL_ARG_PARAMS
#undef MENES_ETL_ARG_CONS
#undef MENES_ETL_ARG_CALL1
#undef MENES_ETL_ARG_CALL0
#undef MENES_ETL_ARG_ARGS1
#undef MENES_ETL_ARG_ARGS0
#undef MENES_ETL_ARG_ARITY

#define MENES_ETL_ARG_ARITY 6
#define MENES_ETL_ARG_ARGS0 arg0, arg1, arg2, arg3, arg4, arg5
#define MENES_ETL_ARG_ARGS1 arg1, arg2, arg3, arg4, arg5
#define MENES_ETL_ARG_CALL0 args.head_, args.tail_.head_, args.tail_.tail_.head_, args.tail_.tail_.tail_.head_, args.tail_.tail_.tail_.tail_.head_, args.tail_.tail_.tail_.tail_.tail_.head_
#define MENES_ETL_ARG_CALL1 args.head_, args.tail_.head_, args.tail_.tail_.head_, args.tail_.tail_.tail_.head_, args.tail_.tail_.tail_.tail_.head_
#define MENES_ETL_ARG_CONS etl::Cons<Arg0_, etl::Cons<Arg1_, etl::Cons<Arg2_, etl::Cons<Arg3_, etl::Cons<Arg4_, etl::Cons<Arg5_> > > > > >
#define MENES_ETL_ARG_PARAMS Arg0_ arg0, Arg1_ arg1, Arg2_ arg2, Arg3_ arg3, Arg4_ arg4, Arg5_ arg5
#define MENES_ETL_ARG_TYPENAME typename Arg0_, typename Arg1_, typename Arg2_, typename Arg3_, typename Arg4_, typename Arg5_
#define MENES_ETL_ARG_TYPES Arg0_, Arg1_, Arg2_, Arg3_, Arg4_, Arg5_
#include "hop/function_.hpp"
#undef MENES_ETL_ARG_TYPES
#undef MENES_ETL_ARG_TYPENAME
#undef MENES_ETL_ARG_PARAMS
#undef MENES_ETL_ARG_CONS
#undef MENES_ETL_ARG_CALL1
#undef MENES_ETL_ARG_CALL0
#undef MENES_ETL_ARG_ARGS1
#undef MENES_ETL_ARG_ARGS0
#undef MENES_ETL_ARG_ARITY

#define MENES_ETL_ARG_ARITY 7
#define MENES_ETL_ARG_ARGS0 arg0, arg1, arg2, arg3, arg4, arg5, arg6
#define MENES_ETL_ARG_ARGS1 arg1, arg2, arg3, arg4, arg5, arg6
#define MENES_ETL_ARG_CALL0 args.head_, args.tail_.head_, args.tail_.tail_.head_, args.tail_.tail_.tail_.head_, args.tail_.tail_.tail_.tail_.head_, args.tail_.tail_.tail_.tail_.tail_.head_, args.tail_.tail_.tail_.tail_.tail_.tail_.head_
#define MENES_ETL_ARG_CALL1 args.head_, args.tail_.head_, args.tail_.tail_.head_, args.tail_.tail_.tail_.head_, args.tail_.tail_.tail_.tail_.head_, args.tail_.tail_.tail_.tail_.tail_.head_
#define MENES_ETL_ARG_CONS etl::Cons<Arg0_, etl::Cons<Arg1_, etl::Cons<Arg2_, etl::Cons<Arg3_, etl::Cons<Arg4_, etl::Cons<Arg5_, etl::Cons<Arg6_> > > > > > >
#define MENES_ETL_ARG_PARAMS Arg0_ arg0, Arg1_ arg1, Arg2_ arg2, Arg3_ arg3, Arg4_ arg4, Arg5_ arg5, Arg6_ arg6
#define MENES_ETL_ARG_TYPENAME typename Arg0_, typename Arg1_, typename Arg2_, typename Arg3_, typename Arg4_, typename Arg5_, typename Arg6_
#define MENES_ETL_ARG_TYPES Arg0_, Arg1_, Arg2_, Arg3_, Arg4_, Arg5_, Arg6_
#include "hop/function_.hpp"
#undef MENES_ETL_ARG_TYPES
#undef MENES_ETL_ARG_TYPENAME
#undef MENES_ETL_ARG_PARAMS
#undef MENES_ETL_ARG_CONS
#undef MENES_ETL_ARG_CALL1
#undef MENES_ETL_ARG_CALL0
#undef MENES_ETL_ARG_ARGS1
#undef MENES_ETL_ARG_ARGS0
#undef MENES_ETL_ARG_ARITY

#define MENES_ETL_ARG_ARITY 8
#define MENES_ETL_ARG_ARGS0 arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7
#define MENES_ETL_ARG_ARGS1 arg1, arg2, arg3, arg4, arg5, arg6, arg7
#define MENES_ETL_ARG_CALL0 args.head_, args.tail_.head_, args.tail_.tail_.head_, args.tail_.tail_.tail_.head_, args.tail_.tail_.tail_.tail_.head_, args.tail_.tail_.tail_.tail_.tail_.head_, args.tail_.tail_.tail_.tail_.tail_.tail_.head_, args.tail_.tail_.tail_.tail_.tail_.tail_.tail_.head_
#define MENES_ETL_ARG_CALL1 args.head_, args.tail_.head_, args.tail_.tail_.head_, args.tail_.tail_.tail_.head_, args.tail_.tail_.tail_.tail_.head_, args.tail_.tail_.tail_.tail_.tail_.head_, args.tail_.tail_.tail_.tail_.tail_.tail_.head_
#define MENES_ETL_ARG_CONS etl::Cons<Arg0_, etl::Cons<Arg1_, etl::Cons<Arg2_, etl::Cons<Arg3_, etl::Cons<Arg4_, etl::Cons<Arg5_, etl::Cons<Arg6_, etl::Cons<Arg7_> > > > > > > >
#define MENES_ETL_ARG_PARAMS Arg0_ arg0, Arg1_ arg1, Arg2_ arg2, Arg3_ arg3, Arg4_ arg4, Arg5_ arg5, Arg6_ arg6, Arg7_ arg7
#define MENES_ETL_ARG_TYPENAME typename Arg0_, typename Arg1_, typename Arg2_, typename Arg3_, typename Arg4_, typename Arg5_, typename Arg6_, typename Arg7_
#define MENES_ETL_ARG_TYPES Arg0_, Arg1_, Arg2_, Arg3_, Arg4_, Arg5_, Arg6_, Arg7_
#include "hop/function_.hpp"
#undef MENES_ETL_ARG_TYPES
#undef MENES_ETL_ARG_TYPENAME
#undef MENES_ETL_ARG_PARAMS
#undef MENES_ETL_ARG_CONS
#undef MENES_ETL_ARG_CALL1
#undef MENES_ETL_ARG_CALL0
#undef MENES_ETL_ARG_ARGS1
#undef MENES_ETL_ARG_ARGS0
#undef MENES_ETL_ARG_ARITY

template <typename Prototype_>
struct CallThrough {
    typedef FunctionTraits<Prototype_> Traits;

    _finline static typename Traits::Return FreeCall(typename Traits::Pointer pointer, const ArgStruct<typename Traits::Args> &args) {
        return FunctionCall<Prototype_>::InvokeDirect(pointer, args);
    }

    _finline static typename Traits::Return MemberCall(typename Traits::Member member, const ArgStruct<typename Traits::Args> &args) {
        return FunctionCall<Prototype_>::InvokeMember(member, args.head_, args.tail_);
    }

    _finline static typename Traits::Return MemberCall(typename Traits::Member member, typename Traits::Host *host, const ArgStruct<typename Traits::Args::Tail> &args) {
        return FunctionCall<Prototype_>::InvokeMember(member, host, args);
    }
};

template <typename Return_, typename Host_>
struct CallThrough<Return_ (Host_ *)> {
    typedef FunctionTraits<Return_ (Host_ *)> Traits;

    _finline static typename Traits::Return FreeCall(typename Traits::Pointer pointer, const ArgStruct<typename Traits::Args> &args) {
        return pointer(args.head_);
    }

    _finline static typename Traits::Return MemberCall(typename Traits::Member member, const ArgStruct<typename Traits::Args> &args) {
        return (args.head_->*member)();
    }

    _finline static typename Traits::Return MemberCall(typename Traits::Member member, Host_ *host, const ArgStruct<typename Traits::Args::Tail> &args) {
        return (host->*member)();
    }
};

template <typename Return_>
struct CallThrough<Return_ ()> {
    typedef FunctionTraits<Return_ ()> Traits;

    _finline static typename Traits::Return FreeCall(typename Traits::Pointer pointer, const ArgStruct<typename Traits::Args> &args) {
        return pointer();
    }

    _finline static typename Traits::Return MemberCall(typename Traits::Member member, const ArgStruct<typename Traits::Args> &args) {
        _assume(false);
        return typename Traits::Return();
    }
};

template <typename Prototype_>
struct Function :
    public FunctionInvoke<
        Function<Prototype_>,
        Prototype_,
        _R<
            FunctionValue<Prototype_>,
            FunctionManager<Prototype_>
        >
    >
{
  private:
    typedef Function<Prototype_> This_;

    typedef FunctionInvoke<
        Function<Prototype_>,
        Prototype_,
        _R<
            FunctionValue<Prototype_>,
            FunctionManager<Prototype_>
        >
    > Base_;

  public:
    typedef FunctionManager<Prototype_> Manager;
    typedef FunctionTraits<Prototype_> Traits;
    typedef FunctionValue<Prototype_> Value;

  private:
    static typename Traits::Return InvokeFree_(const This_ *this_, ArgStruct<typename Traits::Args> args) {
        return CallThrough<Prototype_>::FreeCall(lowlevel_cast<typename Traits::Pointer>(this_->GetValue().data), args);
    }

    static typename Traits::Return InvokeMember_(const This_ *this_, ArgStruct<typename Traits::Args> args) {
        return CallThrough<Prototype_>::MemberCall(this_->GetValue().member, args);
    }

    template <typename Type_>
    static typename Traits::Return InvokeAdapter_(const This_ *this_, ArgStruct<typename Traits::Args> args) {
        return FunctionCall<Prototype_>::InvokeIndirect(reinterpret_cast<Type_ *>(this_->GetValue().data), args);
    }

  public:
    _finline Function() {
    }

    _finline Function(const This_ &rhs) :
        Base_(rhs)
    {
    }

    _finline Function(typename Traits::Pointer rhs) {
        operator =(rhs);
    }

    // XXX: this used to be _finline
    inline Function(typename Traits::Member rhs) {
        operator =(rhs);
    }

    // XXX: this is fishy as it calls through to the old broken function
    // XXX: rewrite using etl::IfValid or something
    template <typename Type_>
    _finline Function(const Type_ &rhs) {
        operator =(rhs);
    }

    _finline Function(const Value &value, const Manager &manager) {
        Assign(value, manager);
    }

    using Base_::operator !;
    using Base_::operator ();

    using Base_::Assign;
    using Base_::GetValue;
    using Base_::IsEmpty;

    using Base_::operator ==;
    using Base_::operator !=;
    using Base_::operator <;

    This_ &operator =(const This_ &rhs) {
        Base_::operator =(rhs);
        return *this;
    }

    This_ &operator =(typename Traits::Pointer rhs) {
        Base_::Clear();
        Value &value(Base_::GetValue_());
        value.invoker = &InvokeFree_;
        value.data = lowlevel_cast<void *>(rhs);
        Base_::GetManager_() = NULL;
        return *this;
    }

    This_ &operator =(typename Traits::Member rhs) {
        Base_::Clear();
        Value &value(Base_::GetValue_());
        value.invoker = &InvokeMember_;
        value.member = rhs;
        Base_::GetManager_() = NULL;
        return *this;
    }

    template <typename Type_>
    typename etl::IfValidType<hop::FunctionTraits<Type_>, This_ &>::Result
    operator =(const Type_ &rhs) {
        Base_::Clear();

        Value value;
        value.invoker = &InvokeAdapter_<Type_>;
        value.data = const_cast<void *>(reinterpret_cast<const void *>(&rhs));

        static const etl::TypeVTableImpl<Type_> table;
        Assign(value, &table);
        return *this;
    }

    inline typename Traits::Return InvokeReference(const ArgStruct<typename Traits::Args> &args) const {
        typedef typename Traits::Return (*Target)(const This_ *, ArgStruct<typename Traits::Args>);
        //_assert(!operator !());
        return reinterpret_cast<Target>(GetValue().invoker)(this, args);
    }

    inline typename Traits::Return InvokeValue(ArgStruct<typename Traits::Args> args) const {
        return InvokeReference(args);
    }
};

}

#endif//MENES_ETL_FUNCTION_HPP
