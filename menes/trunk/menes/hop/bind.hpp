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

#ifndef MENES_ETL_FNBIND_HPP
#define MENES_ETL_FNBIND_HPP

#include "cxx/platform.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "hop/function.hpp"

namespace hop {

namespace be {

template <unsigned Index_, typename Prototype_, typename Arg_>
struct BindBase_ {
    typedef typename Function<Prototype_>::Traits::Type InType;
    typedef typename etl::Erase<InType, Index_ + 1>::Result OutType;

    typedef Arg_ Arg;
    typedef Function<typename FunctionTraits<OutType>::Prototype> Result;
    typedef typename FunctionManager<typename Result::Traits::Prototype>::Value Value;

    typedef typename Function<Prototype_>::Traits::Args ArgType;
    typedef typename etl::Head<ArgType, Index_>::Result PreArgs;
    typedef typename etl::At<ArgType, Index_>::Result BoundArg;
    typedef typename etl::Skip<ArgType, Index_ + 1>::Result PostArgs;

    struct Binder {
        Function<Prototype_> function;
        Arg arg;

        // XXX: this used to be typename etl::ReferenceTo<const Arg>::Result arg
        Binder(const Function<Prototype_> &function, const Arg &arg) :
            function(function),
            arg(arg)
        {
        }

        bool operator ==(const Binder &rhs) const {
            return function == rhs.function && arg == rhs.arg;
        }

        bool operator <(const Binder &rhs) const {
            if (!(function == rhs.function))
                return function < rhs.function;
            if (!(arg == rhs.arg))
                return arg < rhs.arg;
            return false;
        }
    };
};

template <unsigned Index_, typename Prototype_>
struct BindLength {
    static const unsigned value = etl::Length<typename Function<Prototype_>::Traits::Args>::value - Index_ - 1;
};

template <unsigned Index_, typename Prototype_, typename Arg_, unsigned Length_>
struct Bind :
    public BindBase_<Index_, Prototype_, Arg_>
{
    typedef BindBase_<Index_, Prototype_, Arg_> Base_;
    static typename Function<Prototype_>::Traits::Return InvokeBind(const typename Base_::Result *this_, ArgStruct<typename Base_::PreArgs> pre, ArgStruct<typename Base_::PostArgs> post) {
        typename Base_::Binder &binder(*reinterpret_cast<typename Base_::Binder *>(this_->GetValue().data));
#ifdef CFG_AGGREGATE_PASS_BY_REFERENCE
        typename Base_::BoundArg arg(binder.arg);
        ArgStruct<typename Base_::ArgType> args(pre, arg, post);
        typedef typename Function<Prototype_>::Traits::Return (Function<Prototype_>::*Target)(ArgStruct<typename Base_::ArgType> *);
        return (binder.function.*reinterpret_cast<Target>(&Function<Prototype_>::InvokeValue))(&args);
#else
        typedef typename Function<Prototype_>::Traits::Return (Function<Prototype_>::*Target)(ArgStruct<typename Base_::PreArgs>, typename Base_::BoundArg, ArgStruct<typename Base_::PostArgs>);
        return (binder.function.*reinterpret_cast<Target>(&Function<Prototype_>::InvokeValue))(pre, binder.arg, post);
#endif
    }
};

template <typename Prototype_, typename Arg_, unsigned Length_>
struct Bind<0, Prototype_, Arg_, Length_> :
    public BindBase_<0, Prototype_, Arg_>
{
    typedef BindBase_<0, Prototype_, Arg_> Base_;
    static typename Function<Prototype_>::Traits::Return InvokeBind(const typename Base_::Result *this_, ArgStruct<typename Base_::PostArgs> post) {
        typename Base_::Binder &binder(*reinterpret_cast<typename Base_::Binder *>(this_->GetValue().data));
#ifdef CFG_AGGREGATE_PASS_BY_REFERENCE
        typename Base_::BoundArg arg(binder.arg);
        ArgStruct<typename Base_::ArgType> args(arg, post);
        typedef typename Function<Prototype_>::Traits::Return (Function<Prototype_>::*Target)(ArgStruct<typename Base_::ArgType> *);
        return (binder.function.*reinterpret_cast<Target>(&Function<Prototype_>::InvokeValue))(&args);
#else
        typedef typename Function<Prototype_>::Traits::Return (Function<Prototype_>::*Target)(typename Base_::BoundArg, ArgStruct<typename Base_::PostArgs>);
        return (binder.function.*reinterpret_cast<Target>(&Function<Prototype_>::InvokeValue))(binder.arg, post);
#endif
    }
};

template <unsigned Index_, typename Prototype_, typename Arg_>
struct Bind<Index_, Prototype_, Arg_, 0> :
    public BindBase_<Index_, Prototype_, Arg_>
{
    typedef BindBase_<Index_, Prototype_, Arg_> Base_;
    static typename Function<Prototype_>::Traits::Return InvokeBind(const typename Base_::Result *this_, ArgStruct<typename Base_::PreArgs> pre) {
        typename Base_::Binder &binder(*reinterpret_cast<typename Base_::Binder *>(this_->GetValue().data));
#ifdef CFG_AGGREGATE_PASS_BY_REFERENCE
        typename Base_::BoundArg arg(binder.arg);
        ArgStruct<typename Base_::ArgType> args(pre, arg);
        typedef typename Function<Prototype_>::Traits::Return (Function<Prototype_>::*Target)(ArgStruct<typename Base_::ArgType> *);
        return (binder.function.*reinterpret_cast<Target>(&Function<Prototype_>::InvokeValue))(&args);
#else
        typedef typename Function<Prototype_>::Traits::Return (Function<Prototype_>::*Target)(ArgStruct<typename Base_::PreArgs>, typename Base_::BoundArg);
        return (binder.function.*reinterpret_cast<Target>(&Function<Prototype_>::InvokeValue))(pre, binder.arg);
#endif
    }
};

template <typename Prototype_, typename Arg_>
struct Bind<0, Prototype_, Arg_, 0> :
    public BindBase_<0, Prototype_, Arg_>
{
    typedef BindBase_<0, Prototype_, Arg_> Base_;
    static typename Function<Prototype_>::Traits::Return InvokeBind(const typename Base_::Result *this_, ArgStruct<etl::Null>) {
        typename Base_::Binder &binder(*reinterpret_cast<typename Base_::Binder *>(this_->GetValue().data));
#ifdef CFG_AGGREGATE_PASS_BY_REFERENCE
        typename Base_::BoundArg arg(binder.arg);
        ArgStruct<typename Base_::ArgType> args(arg);
        typedef typename Function<Prototype_>::Traits::Return (Function<Prototype_>::*Target)(ArgStruct<typename Base_::ArgType> *);
        return (binder.function.*reinterpret_cast<Target>(&Function<Prototype_>::InvokeValue))(&args);
#else
        typedef typename Function<Prototype_>::Traits::Return (Function<Prototype_>::*Target)(typename Base_::BoundArg);
        return (binder.function.*reinterpret_cast<Target>(&Function<Prototype_>::InvokeValue))(binder.arg);
#endif
    }
};

template <unsigned Index_, typename Arg_, typename Prototype_>
typename be::Bind<Index_, Prototype_, Arg_, be::BindLength<Index_, Prototype_>::value>::Result Bind_(const Function<Prototype_> &functor, Arg_ arg) {
    typedef be::Bind<Index_, Prototype_, Arg_, be::BindLength<Index_, Prototype_>::value> Bind;

    typedef typename Bind::Result::Value Value;
    typedef typename Bind::Result::Value::Invoker Invoker;
    typedef typename Bind::Binder Binder;

    static const etl::TypeVTableImpl<typename Bind::Binder> table;
    Binder binder(functor, arg);
    return typename Bind::Result(Value(reinterpret_cast<Invoker>(&Bind::InvokeBind), &binder), &table);
}

}

template <unsigned Index_, typename Arg_, typename Functor_>
typename be::Bind<Index_, typename FunctionTraits<Functor_>::Prototype, Arg_, be::BindLength<Index_, typename FunctionTraits<Functor_>::Prototype>::value>::Result Bind(const Functor_ &functor, Arg_ arg) {
    return be::Bind_<Index_>(Function<typename FunctionTraits<Functor_>::Prototype>(functor), arg);
}

template <typename Functor_>
Function<typename FunctionTraits<Functor_>::Return ()>
BindAll(const Functor_ &functor) {
    return functor;
}

template <typename Arg0_, typename Functor_>
Function<typename FunctionTraits<Functor_>::Return ()>
BindAll(const Functor_ &functor, Arg0_ arg0) {
    return Bind<0, Arg0_>(functor, arg0);
}

template <typename Arg0_, typename Arg1_, typename Functor_>
Function<typename FunctionTraits<Functor_>::Return ()>
BindAll(const Functor_ &functor, Arg0_ arg0, Arg1_ arg1) {
    return Bind<0, Arg1_>(Bind<0, Arg0_>(functor, arg0), arg1);
}

template <typename Arg0_, typename Arg1_, typename Arg2_, typename Functor_>
Function<typename FunctionTraits<Functor_>::Return ()>
BindAll(const Functor_ &functor, Arg0_ arg0, Arg1_ arg1, Arg2_ arg2) {
    return Bind<0, Arg2_>(Bind<0, Arg1_>(Bind<0, Arg0_>(functor, arg0), arg1), arg2);
}

template <typename Arg0_, typename Arg1_, typename Arg2_, typename Arg3_, typename Functor_>
Function<typename FunctionTraits<Functor_>::Return ()>
BindAll(const Functor_ &functor, Arg0_ arg0, Arg1_ arg1, Arg2_ arg2, Arg3_ arg3) {
    return Bind<0, Arg3_>(Bind<0, Arg2_>(Bind<0, Arg1_>(Bind<0, Arg0_>(functor, arg0), arg1), arg2), arg3);
}

template <typename Arg0_, typename Arg1_, typename Arg2_, typename Arg3_, typename Arg4_, typename Functor_>
Function<typename FunctionTraits<Functor_>::Return ()>
BindAll(const Functor_ &functor, Arg0_ arg0, Arg1_ arg1, Arg2_ arg2, Arg3_ arg3, Arg4_ arg4) {
    return Bind<0, Arg4_>(Bind<0, Arg3_>(Bind<0, Arg2_>(Bind<0, Arg1_>(Bind<0, Arg0_>(functor, arg0), arg1), arg2), arg3), arg4);
}

template <typename Arg0_, typename Arg1_, typename Arg2_, typename Arg3_, typename Arg4_, typename Arg5_, typename Functor_>
Function<typename FunctionTraits<Functor_>::Return ()>
BindAll(const Functor_ &functor, Arg0_ arg0, Arg1_ arg1, Arg2_ arg2, Arg3_ arg3, Arg4_ arg4, Arg5_ arg5) {
    return Bind<0, Arg5_>(Bind<0, Arg4_>(Bind<0, Arg3_>(Bind<0, Arg2_>(Bind<0, Arg1_>(Bind<0, Arg0_>(functor, arg0), arg1), arg2), arg3), arg4), arg5);
}

template <typename Arg0_, typename Arg1_, typename Arg2_, typename Arg3_, typename Arg4_, typename Arg5_, typename Arg6_, typename Functor_>
Function<typename FunctionTraits<Functor_>::Return ()>
BindAll(const Functor_ &functor, Arg0_ arg0, Arg1_ arg1, Arg2_ arg2, Arg3_ arg3, Arg4_ arg4, Arg5_ arg5, Arg6_ arg6) {
    return Bind<0, Arg6_>(Bind<0, Arg5_>(Bind<0, Arg4_>(Bind<0, Arg3_>(Bind<0, Arg2_>(Bind<0, Arg1_>(Bind<0, Arg0_>(functor, arg0), arg1), arg2), arg3), arg4), arg5), arg6);
}

namespace be {
template <typename Type_>
class ConstReference {
  private:
    typedef ConstReference<Type_> This_;

  private:
    const Type_ *value_;

  public:
    _finline ConstReference(const Type_ &value) :
        value_(&value)
    {
    }

    _finline operator const Type_ &() const {
        return *value_;
    }

    bool operator ==(const This_ &rhs) const {
        return value_ == rhs.value_;
    }

    bool operator <(const This_ &rhs) const {
        return value_ < rhs.value_;
    }
}; }

template <typename Type_>
be::ConstReference<Type_> ConstReference(const Type_ &value) {
    return be::ConstReference<Type_>(value);
}

}

namespace ext {
template <typename Direct_, typename Indirect_, typename Derived_, typename Right_>
hop::Function<
    typename hop::FunctionTraits<
        typename etl::Erase<
            typename hop::FunctionTraits<Right_>::Type,
            1
        >::Result
    >::Prototype
> operator ->*(const ext::SmartPointer<Direct_, Indirect_, Derived_> &pointer, Right_ rhs) {
    return hop::Bind<0>(rhs, static_cast<Indirect_>(pointer));
} }

#endif//MENES_ETL_FNBIND_HPP
