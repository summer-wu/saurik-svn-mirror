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

#if MENES_ETL_ARG_ARITY == 0
#define MENES_ETL_ARG_COMMA
#else
#define MENES_ETL_ARG_COMMA ,
#endif

#define MENES_ETL_ARG_LIST etl::Cons<Return_, MENES_ETL_ARG_CONS >

namespace be {
template <typename Return_, class Host_ MENES_ETL_ARG_COMMA MENES_ETL_ARG_TYPENAME>
struct FunctionMember_<Return_, etl::Cons<Host_ *, MENES_ETL_ARG_CONS >, true> {
    typedef Host_ Host;
    typedef Return_ (Host_::*Member)(MENES_ETL_ARG_TYPES);
}; }

template <typename Return_ MENES_ETL_ARG_COMMA MENES_ETL_ARG_TYPENAME>
struct FunctionTraits< MENES_ETL_ARG_LIST > {
    typedef MENES_ETL_ARG_LIST Type;

    typedef Return_ Return;
    typedef MENES_ETL_ARG_CONS Args;

    typedef Return_ (*Pointer)(MENES_ETL_ARG_TYPES);
    typedef Return_ (Prototype)(MENES_ETL_ARG_TYPES);

    typedef typename be::FunctionMember_<Return, Args>::Host Host;
    typedef typename be::FunctionMember_<Return, Args>::Member Member;
};

template <typename Return_ MENES_ETL_ARG_COMMA MENES_ETL_ARG_TYPENAME>
struct FunctionTraits< Return_ (MENES_ETL_ARG_TYPES) > {
    typedef MENES_ETL_ARG_LIST Type;

    typedef Return_ Return;
    typedef MENES_ETL_ARG_CONS Args;

    typedef Return_ (*Pointer)(MENES_ETL_ARG_TYPES);
    typedef Return_ (Prototype)(MENES_ETL_ARG_TYPES);

    typedef typename be::FunctionMember_<Return, Args>::Host Host;
    typedef typename be::FunctionMember_<Return, Args>::Member Member;
};

template <typename Return_ MENES_ETL_ARG_COMMA MENES_ETL_ARG_TYPENAME>
struct FunctionTraits< Return_ (*)(MENES_ETL_ARG_TYPES)> {
    typedef MENES_ETL_ARG_LIST Type;

    typedef Return_ Return;
    typedef MENES_ETL_ARG_CONS Args;

    typedef Return_ (*Pointer)(MENES_ETL_ARG_TYPES);
    typedef Return_ (Prototype)(MENES_ETL_ARG_TYPES);

    typedef typename be::FunctionMember_<Return, Args>::Host Host;
    typedef typename be::FunctionMember_<Return, Args>::Member Member;
};

template <typename Return_, class Host_ MENES_ETL_ARG_COMMA MENES_ETL_ARG_TYPENAME>
struct FunctionTraits<Return_ (Host_::*)(MENES_ETL_ARG_TYPES)> {
    typedef etl::Cons<Return_, etl::Cons<Host_ *, MENES_ETL_ARG_CONS > > Type;

    typedef Return_ Return;
    typedef etl::Cons<Host_ *, MENES_ETL_ARG_CONS > Args;

    typedef Return_ (*Pointer)(Host_ * MENES_ETL_ARG_COMMA MENES_ETL_ARG_TYPES);
    typedef Return_ (Prototype)(Host_ * MENES_ETL_ARG_COMMA MENES_ETL_ARG_TYPES);

    typedef Host_ Host;
    typedef Return_ (Host_::*Member)(MENES_ETL_ARG_TYPES);
};

template <typename Return_ MENES_ETL_ARG_COMMA MENES_ETL_ARG_TYPENAME>
class FunctionCall<Return_ (MENES_ETL_ARG_TYPES)> {
  private:
    typedef FunctionTraits<Return_ (MENES_ETL_ARG_TYPES)> Traits_;

  public:
    template <typename Type_, typename Cell_>
    static typename Traits_::Return InvokeDirect(Type_ type, const ArgStruct<Cell_> &args) {
        return type(MENES_ETL_ARG_CALL0);
    }

    template <typename Type_, typename Cell_>
    static typename Traits_::Return InvokeIndirect(Type_ type, const ArgStruct<Cell_> &args) {
        return (*type)(MENES_ETL_ARG_CALL0);
    }

#if MENES_ETL_ARG_ARITY != 0
    template <typename Type_, typename Host_, typename Cell_>
    static typename Traits_::Return InvokeMember(Type_ type, Host_ *host, const ArgStruct<Cell_> &args) {
        return (host->*type)(MENES_ETL_ARG_CALL1);
    }
#endif
};

/* // as I now consider this evil, delete it?
template <MENES_ETL_ARG_TYPENAME>
class FunctionCall<void (MENES_ETL_ARG_TYPES)> {
  private:
    typedef FunctionTraits<void (MENES_ETL_ARG_TYPES)> Traits_;

  public:
    template <typename Type_, typename Cell_>
    static void InvokeDirect(Type_ type, const ArgStruct<Cell_> &args) {
        type(MENES_ETL_ARG_CALL0);
    }

    template <typename Type_, typename Cell_>
    static void InvokeIndirect(Type_ type, const ArgStruct<Cell_> &args) {
        (*type)(MENES_ETL_ARG_CALL0);
    }

#if MENES_ETL_ARG_ARITY != 0
    template <typename Type_, typename Host_, typename Cell_>
    static void InvokeMember(Type_ type, Host_ *host, const ArgStruct<Cell_> &args) {
        (host->*type)(MENES_ETL_ARG_CALL1);
    }
#endif
};*/

template <typename Derived_, typename Return_ MENES_ETL_ARG_COMMA MENES_ETL_ARG_TYPENAME, typename Base_>
class FunctionInvoke<Derived_, Return_ (MENES_ETL_ARG_TYPES), Base_> :
    public Base_
{
  private:
    typedef FunctionTraits<Return_ (MENES_ETL_ARG_TYPES)> Traits_;

#if MENES_ETL_ARG_ARITY != 0 && defined(CFG_AGGREGATE_PASS_BY_REFERENCE)
    Return_ BrokenAggregateAbi_(int first, MENES_ETL_ARG_PARAMS) const {
        typedef Return_ (Derived_::*Target)(void *) const;
        return (static_cast<const Derived_ *>(this)->*reinterpret_cast<Target>(&Derived_::InvokeValue))(&first + 1);
    }
#endif

  public:
    // XXX: look into inline assembling this sucker
    Return_ operator ()(MENES_ETL_ARG_PARAMS) const {
#if MENES_ETL_ARG_ARITY == 0
        return static_cast<const Derived_ *>(this)->InvokeValue(ArgStruct<typename Traits_::Args>());
#elif defined(CFG_AGGREGATE_PASS_BY_REFERENCE)
        return BrokenAggregateAbi_(0, MENES_ETL_ARG_ARGS0);
#else
        typedef Return_ (Derived_::*Target)(MENES_ETL_ARG_TYPES) const;
        return (static_cast<const Derived_ *>(this)->*reinterpret_cast<Target>(&Derived_::InvokeValue))(MENES_ETL_ARG_ARGS0);
#endif
    }
};

/* // as I now consider this evil, delete it?
template <typename Derived_ MENES_ETL_ARG_COMMA MENES_ETL_ARG_TYPENAME, typename Base_>
class FunctionInvoke<Derived_, void (MENES_ETL_ARG_TYPES), Base_> :
    public Base_
{
  private:
    typedef FunctionTraits<void (MENES_ETL_ARG_TYPES)> Traits_;

#if MENES_ETL_ARG_ARITY != 0 && defined(CFG_AGGREGATE_PASS_BY_REFERENCE)
    void BrokenAggregateAbi_(int first, MENES_ETL_ARG_PARAMS) const {
        typedef void (Derived_::*Target)(void *) const;
        (static_cast<const Derived_ *>(this)->*reinterpret_cast<Target>(&Derived_::InvokeValue))(&first + 1);
    }
#endif

  public:
    // XXX: look into inline assembling this sucker
    void operator ()(MENES_ETL_ARG_PARAMS) const {
#if MENES_ETL_ARG_ARITY == 0
        static_cast<const Derived_ *>(this)->InvokeValue(ArgStruct<typename Traits_::Args>());
#elif defined(CFG_AGGREGATE_PASS_BY_REFERENCE)
        BrokenAggregateAbi_(0, MENES_ETL_ARG_ARGS0);
#else
        typedef void (Derived_::*Target)(MENES_ETL_ARG_TYPES) const;
        (static_cast<const Derived_ *>(this)->*reinterpret_cast<Target>(&Derived_::InvokeValue))(MENES_ETL_ARG_ARGS0);
#endif
    }
};*/

#undef MENES_ETL_ARG_LIST
#undef MENES_ETL_ARG_COMMA
