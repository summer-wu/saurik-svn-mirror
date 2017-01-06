/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2005  Jay Freeman (saurik)
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

#ifndef MENES_EXT_HELPERS_HPP
#define MENES_EXT_HELPERS_HPP

#include "cxx/platform.hpp"
#include "ext/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "ext/vector.hpp"

namespace ext {

template <typename Element_>
class VectorHelper :
    public Vector<Element_>
{
  private:
    typedef ext::Vector<Element_> Base_;

  public:
    VectorHelper() {
    }

    template <typename Arg0_>
    VectorHelper(const Arg0_ &arg0) {
        Base_::Reserve(1);
        InsertLast(arg0);
    }

    template <typename Arg0_, typename Arg1_>
    VectorHelper(const Arg0_ &arg0, const Arg1_ &arg1) {
        Base_::Reserve(2);
        InsertLast(arg0);
        InsertLast(arg1);
    }

    template <typename Arg0_, typename Arg1_, typename Arg2_>
    VectorHelper(const Arg0_ &arg0, const Arg1_ &arg1, const Arg2_ &arg2) {
        Base_::Reserve(3);
        InsertLast(arg0);
        InsertLast(arg1);
        InsertLast(arg2);
    }

    template <typename Arg0_, typename Arg1_, typename Arg2_, typename Arg3_>
    VectorHelper(const Arg0_ &arg0, const Arg1_ &arg1, const Arg2_ &arg2, const Arg3_ &arg3) {
        Base_::Reserve(4);
        InsertLast(arg0);
        InsertLast(arg1);
        InsertLast(arg2);
        InsertLast(arg3);
    }

    template <typename Arg0_, typename Arg1_, typename Arg2_, typename Arg3_, typename Arg4_>
    VectorHelper(const Arg0_ &arg0, const Arg1_ &arg1, const Arg2_ &arg2, const Arg3_ &arg3, const Arg4_ &arg4) {
        Base_::Reserve(5);
        InsertLast(arg0);
        InsertLast(arg1);
        InsertLast(arg2);
        InsertLast(arg3);
        InsertLast(arg4);
    }

    template <typename Arg0_, typename Arg1_, typename Arg2_, typename Arg3_, typename Arg4_, typename Arg5_>
    VectorHelper(const Arg0_ &arg0, const Arg1_ &arg1, const Arg2_ &arg2, const Arg3_ &arg3, const Arg4_ &arg4, const Arg5_ &arg5) {
        Base_::Reserve(6);
        InsertLast(arg0);
        InsertLast(arg1);
        InsertLast(arg2);
        InsertLast(arg3);
        InsertLast(arg4);
        InsertLast(arg5);
    }

    template <typename Arg0_, typename Arg1_, typename Arg2_, typename Arg3_, typename Arg4_, typename Arg5_, typename Arg6_>
    VectorHelper(const Arg0_ &arg0, const Arg1_ &arg1, const Arg2_ &arg2, const Arg3_ &arg3, const Arg4_ &arg4, const Arg5_ &arg5, const Arg6_ &arg6) {
        Base_::Reserve(7);
        InsertLast(arg0);
        InsertLast(arg1);
        InsertLast(arg2);
        InsertLast(arg3);
        InsertLast(arg4);
        InsertLast(arg5);
        InsertLast(arg6);
    }

    template <typename Arg0_, typename Arg1_, typename Arg2_, typename Arg3_, typename Arg4_, typename Arg5_, typename Arg6_, typename Arg7_>
    VectorHelper(const Arg0_ &arg0, const Arg1_ &arg1, const Arg2_ &arg2, const Arg3_ &arg3, const Arg4_ &arg4, const Arg5_ &arg5, const Arg6_ &arg6, const Arg7_ &arg7) {
        Base_::Reserve(8);
        InsertLast(arg0);
        InsertLast(arg1);
        InsertLast(arg2);
        InsertLast(arg3);
        InsertLast(arg4);
        InsertLast(arg5);
        InsertLast(arg6);
        InsertLast(arg7);
    }
};

}

#define _V ext::VectorHelper

#endif//MENES_EXT_HELPERS_HPP
