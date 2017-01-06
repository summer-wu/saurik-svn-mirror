/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2004  Jay Freeman (saurik)
*/

/*
 *        Redistribution and use in source and binary
 * forms, with or without modification, are permitted
 * provided that the following conditions are met:
 *·
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
 *·
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

#ifndef MENES_ETL_GROUP_HPP
#define MENES_ETL_GROUP_HPP

#include "cxx/platform.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "etl/composite.hpp"

namespace etl {

_finline etl::Tuple<>::Result
Group() {
    return etl::Tuple<>::Result();
}

template <typename Arg0_>
_finline typename etl::Tuple<Arg0_>::Result
Group(const Arg0_ &arg0) {
    typename etl::Tuple<Arg0_>::Result value(Group());
    value.Set<0>(arg0);
    return value;
}

template <typename Arg0_, typename Arg1_>
_finline typename etl::Tuple<Arg0_, Arg1_>::Result
Group(const Arg0_ &arg0, const Arg1_ &arg1) {
    typename etl::Tuple<Arg0_, Arg1_>::Result value(Group(arg0));
    value.Set<1>(arg1);
    return value;
}

template <typename Arg0_, typename Arg1_, typename Arg2_>
_finline typename etl::Tuple<Arg0_, Arg1_, Arg2_>::Result
Group(const Arg0_ &arg0, const Arg1_ &arg1, const Arg2_ &arg2) {
    typename etl::Tuple<Arg0_, Arg1_, Arg2_>::Result value(Group(arg0, arg1));
    value.Set<2>(arg2);
    return value;
}

template <typename Arg0_, typename Arg1_, typename Arg2_, typename Arg3_>
_finline typename etl::Tuple<Arg0_, Arg1_, Arg2_, Arg3_>::Result
Group(const Arg0_ &arg0, const Arg1_ &arg1, const Arg2_ &arg2, const Arg3_ &arg3) {
    typename etl::Tuple<Arg0_, Arg1_, Arg2_, Arg3_>::Result value(Group(arg0, arg1, arg2));
    value.Set<3>(arg3);
    return value;
}

template <typename Arg0_, typename Arg1_, typename Arg2_, typename Arg3_, typename Arg4_>
_finline typename etl::Tuple<Arg0_, Arg1_, Arg2_, Arg3_, Arg4_>::Result
Group(const Arg0_ &arg0, const Arg1_ &arg1, const Arg2_ &arg2, const Arg3_ &arg3, const Arg4_ &arg4) {
    typename etl::Tuple<Arg0_, Arg1_, Arg2_, Arg3_, Arg4_>::Result value(Group(arg0, arg1, arg2, arg3));
    value.Set<4>(arg4);
    return value;
}

template <typename Arg0_, typename Arg1_, typename Arg2_, typename Arg3_, typename Arg4_, typename Arg5_>
_finline typename etl::Tuple<Arg0_, Arg1_, Arg2_, Arg3_, Arg4_, Arg5_>::Result
Group(const Arg0_ &arg0, const Arg1_ &arg1, const Arg2_ &arg2, const Arg3_ &arg3, const Arg4_ &arg4, const Arg5_ &arg5) {
    typename etl::Tuple<Arg0_, Arg1_, Arg2_, Arg3_, Arg4_, Arg5_>::Result value(Group(arg0, arg1, arg2, arg3, arg4));
    value.Set<5>(arg5);
    return value;
}

template <typename Arg0_, typename Arg1_, typename Arg2_, typename Arg3_, typename Arg4_, typename Arg5_, typename Arg6_>
_finline typename etl::Tuple<Arg0_, Arg1_, Arg2_, Arg3_, Arg4_, Arg5_, Arg6_>::Result
Group(const Arg0_ &arg0, const Arg1_ &arg1, const Arg2_ &arg2, const Arg3_ &arg3, const Arg4_ &arg4, const Arg5_ &arg5, const Arg6_ &arg6) {
    typename etl::Tuple<Arg0_, Arg1_, Arg2_, Arg3_, Arg4_, Arg5_, Arg6_>::Result value(Group(arg0, arg1, arg2, arg3, arg4, arg5));
    value.Set<6>(arg6);
    return value;
}

}

#endif//MENES_ETL_GROUP_HPP
