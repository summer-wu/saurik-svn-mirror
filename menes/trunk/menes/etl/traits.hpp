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

#ifndef MENES_ETL_TYPETRAITS_HPP
#define MENES_ETL_TYPETRAITS_HPP

#include "cxx/platform.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "etl/template.hpp"
#include "etl/manip.hpp"

namespace etl {

template <typename Type_>
struct IsArray;

namespace be {

struct ImplicitFromAny {
    template <typename Type_>
    ImplicitFromAny(const Type_ &);
};

template <typename Type_>
struct ImplicitFrom {
    ImplicitFrom(Type_);
};

}

namespace be {
    template <typename Left_, typename Right_>
    static Yes CheckImplicit_(Right_, Left_);

    template <typename Left_, typename Right_>
    static No CheckImplicit_(be::ImplicitFromAny, be::ImplicitFromAny);
}

template <typename Left_, typename Right_>
struct IsImplicit {
  public:
    void gcc32();

  private:
    static Right_ right_;

  public:
    static const bool value = (sizeof(be::CheckImplicit_<Left_, Right_>(right_, right_).value) == sizeof(Yes::value));
};

template <typename Left_, typename Right_>
struct IsSame {
    static const bool value = false;
};

template <typename Type_>
struct IsSame<Type_, Type_> {
    static const bool value = true;
};

namespace be {
    template <typename Checked_>
    static Yes CheckClass_(void (Checked_::*)());

    template <typename Checked_>
    static No CheckClass_(...);
}

template <typename Type_>
struct IsClass {
    void gcc32();

    static const bool value = (sizeof(be::CheckClass_<Type_>(0).value) == sizeof(Yes::value));
};

namespace be {
    struct Empty_ {
    };

    struct NotEmpty_ {
        No no_;
    };

    template <typename Checked_>
    struct Subclass_ :
        Checked_
    {
        Subclass_();
        No no_;
    };

    template <typename Checked_>
    static Subclass_<Checked_> CheckEmpty_(
        typename IfTrue<
            IsClass<Checked_>::value,
            Subclass_<Checked_> *
        >::Result);

    template <typename Checked_>
    static Subclass_<NotEmpty_> CheckEmpty_(...);
}

template <typename Type_>
struct IsEmpty {
    static const bool value = (sizeof(be::CheckEmpty_<Type_>(0)) == sizeof(be::Subclass_<be::Empty_>));
};

template <typename Type_>
struct IsFunction {
  public:
    void gcc32();

  private:
    template <typename Checked_>
    static Yes Check_(...);

    template <typename Checked_>
    static No Check_(const Checked_ *);

  public:
    static const bool value = (sizeof(Check_<Type_>(0).value) == sizeof(Yes::value));
};

#define ETL_TRAIT_DECLARE_1(Name, Param0, Value) \
template <Param0> \
struct Name { \
    static const bool value = Value; \
};

#define ETL_TRAIT_DECLARE_2(Name, Param0, Param1, Value) \
template <Param0, Param1> \
struct Name { \
    static const bool value = Value; \
};


#define ETL_TRAIT_SPECIAL_0_1(Name, Arg0, Value) \
template <> \
struct Name<Arg0> { \
    static const bool value = Value; \
};

#define ETL_TRAIT_SPECIAL_1_1(Name, Param0, Arg0, Value) \
template <Param0> \
struct Name<Arg0> { \
    static const bool value = Value; \
};

#define ETL_TRAIT_SPECIAL_1_2(Name, Param0, Arg0, Arg1, Value) \
template <Param0> \
struct Name<Arg0, Arg1> { \
    static const bool value = Value; \
};


#define ETL_TRAIT_SPECIAL_2_1(Name, Param0, Param1, Arg0, Value) \
template <Param0, Param1> \
struct Name<Arg0> { \
    static const bool value = Value; \
};

ETL_TRAIT_DECLARE_1(IsVoid, typename Type_, false)
ETL_TRAIT_SPECIAL_0_1(IsVoid, void, true)

ETL_TRAIT_DECLARE_1(IsSigned, typename Type_, false)
ETL_TRAIT_SPECIAL_0_1(IsSigned, signed char, true)
ETL_TRAIT_SPECIAL_0_1(IsSigned, signed short, true)
ETL_TRAIT_SPECIAL_0_1(IsSigned, signed int, true)
ETL_TRAIT_SPECIAL_0_1(IsSigned, signed long, true)
ETL_TRAIT_SPECIAL_0_1(IsSigned, signed long long, true)

ETL_TRAIT_DECLARE_1(IsUnsigned, typename Type_, false)
ETL_TRAIT_SPECIAL_0_1(IsUnsigned, unsigned char, true)
ETL_TRAIT_SPECIAL_0_1(IsUnsigned, unsigned short, true)
ETL_TRAIT_SPECIAL_0_1(IsUnsigned, unsigned int, true)
ETL_TRAIT_SPECIAL_0_1(IsUnsigned, unsigned long, true)
ETL_TRAIT_SPECIAL_0_1(IsUnsigned, unsigned long long, true)

ETL_TRAIT_DECLARE_1(IsNotSigned, typename Type_, IsUnsigned<Type_>::value)
ETL_TRAIT_SPECIAL_0_1(IsNotSigned, byte_t, true)

ETL_TRAIT_DECLARE_1(IsNotUnsigned, typename Type_, IsSigned<Type_>::value)
ETL_TRAIT_SPECIAL_0_1(IsNotUnsigned, byte_t, true)

ETL_TRAIT_DECLARE_1(IsIntegral, typename Type_, IsSigned<Type_>::value || IsUnsigned<Type_>::value)
ETL_TRAIT_SPECIAL_0_1(IsIntegral, byte_t, true)

ETL_TRAIT_DECLARE_1(IsFloating, typename Type_, false)
ETL_TRAIT_SPECIAL_0_1(IsFloating, float, true)
ETL_TRAIT_SPECIAL_0_1(IsFloating, double, true)
ETL_TRAIT_SPECIAL_0_1(IsFloating, long double, true)

ETL_TRAIT_DECLARE_1(IsPointer, typename Type_, false)
ETL_TRAIT_SPECIAL_1_1(IsPointer, typename Type_, Type_ *, true)
ETL_TRAIT_SPECIAL_1_1(IsPointer, typename Type_, const Type_ *, true)

ETL_TRAIT_DECLARE_1(IsReference, typename Type_, false)
ETL_TRAIT_SPECIAL_1_1(IsReference, typename Type_, Type_ &, true)

ETL_TRAIT_DECLARE_1(IsMemberPointer, typename Type_, false)
ETL_TRAIT_SPECIAL_2_1(IsMemberPointer, typename Member_, typename Host_, Member_ Host_::*, true)

ETL_TRAIT_DECLARE_1(IsFieldPointer, typename Type_, IsMemberPointer<Type_>::value && !IsFunction<Type_>::value)
ETL_TRAIT_DECLARE_1(IsMethodPointer, typename Type_, IsMemberPointer<Type_>::value && IsFunction<Type_>::value)

ETL_TRAIT_DECLARE_1(IsArray, typename Type_, false)
ETL_TRAIT_SPECIAL_2_1(IsArray, typename Type_, size_t Size_, Type_[Size_], true)

ETL_TRAIT_DECLARE_1(IsConst, typename Type_, false)
ETL_TRAIT_SPECIAL_1_1(IsConst, typename Type_, const Type_, true)

ETL_TRAIT_DECLARE_1(IsVolatile, typename Type_, false)
ETL_TRAIT_SPECIAL_1_1(IsVolatile, typename Type_, volatile Type_, true)

ETL_TRAIT_DECLARE_1(IsArithmetic, typename Type_, IsIntegral<Type_>::value || IsFloating<Type_>::value)
ETL_TRAIT_DECLARE_1(IsFundamental, typename Type_, IsArithmetic<Type_>::value || IsVoid<Type_>::value)

ETL_TRAIT_DECLARE_2(IsEnum, typename Type_, bool Warning_ = IsArithmetic<Type_>::value || IsClass<Type_>::value || IsReference<Type_>::value, false)
ETL_TRAIT_SPECIAL_1_2(IsEnum, typename Type_, Type_, false, (IsImplicit<be::ImplicitFrom<int>, Type_>::value))

ETL_TRAIT_DECLARE_1(IsObject, typename Type_, !(IsFunction<Type_>::value || IsReference<Type_>::value || IsVoid<Type_>::value))

ETL_TRAIT_DECLARE_1(IsScalar, typename Type_,
    IsArithmetic<Type_>::value ||
    IsEnum<Type_>::value ||
    IsPointer<Type_>::value ||
    IsMemberPointer<Type_>::value
)

ETL_TRAIT_DECLARE_1(IsCompound, typename Type_, !IsFundamental<Type_>::value)

#if 1
// XXX: this was super unfortunate; I hate the world sometimes ;(

template <typename Type_>
struct IsUnionable;

namespace be {

template <typename Type_, bool IsArray_ = IsArray<Type_>::value>
struct IsArrayOfUnionable {
    static const bool value = false;
};

template <typename Type_>
struct IsArrayOfUnionable<Type_, true> {
    static const bool value = IsUnionable<typename StripArray<Type_>::Result>::value;;
};

}

ETL_TRAIT_DECLARE_1(IsUnionable, typename Type_,
    IsEmpty<Type_>::value ||
    IsScalar<Type_>::value ||
    be::IsArrayOfUnionable<Type_>::value
)
#else
template <typename Type_>
struct IsUnionable {
  private:
    template <typename Checked_>
    static Yes Check_(int, UnionOf<Checked_> u = UnionOf<Checked_>());

    template <typename Checked_>
    static No Check_(be::ImplicitFrom<int>);

  public:
    static const bool value = (sizeof(Check_<Type_>(0).value) == sizeof(Yes::value));
};
#endif

ETL_TRAIT_DECLARE_1(IsPod, typename Type_, IsUnionable<Type_>::value)
ETL_TRAIT_DECLARE_1(IsMemCopyable, typename Type_, IsPod<Type_>::value)
ETL_TRAIT_DECLARE_1(IsMemMovable, typename Type_, IsMemCopyable<Type_>::value)

#undef ETL_TRAIT_CV_SPEC_2_1
#undef ETL_TRAIT_CV_SPEC_1_1

#undef ETL_TRAIT_SPECIAL_2_1
#undef ETL_TRAIT_SPECIAL_1_1

#undef ETL_TRAIT_DECLARE_1

}

#define ETL_(Trait, Type) \
namespace etl { \
template <> \
struct Trait<Type> { \
    static const bool value = true; \
}; }

#define ETL_MEMCOPYABLE(Type) ETL_(IsMemCopyable, Type)
#define ETL_MEMMOVABLE(Type) ETL_(IsMemMovable, Type)

#endif//MENES_ETL_TYPETRAITS_HPP
