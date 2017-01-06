/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2004-2005  Jay Freeman (saurik)
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

#ifndef MENES_DBI_STATEMENT_HPP
#define MENES_DBI_STATEMENT_HPP

#include "cxx/platform.hpp"
#include "dbi/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "cse/types.hpp"

#include "etl/limits.hpp"
#include "etl/template.hpp"

#include "ext/refcount.hpp"
#include "ext/types.hpp"

namespace dbi {

EXT_HANDLESTUB(class Blob)
class ResultSet;

class Null {
};

class Statement :
    public ext::ReferenceCounted
{
  public:
    virtual void Set(unsigned index, const ext::Buffer &value) = 0;
    virtual void Set(unsigned index, int64_t value) = 0;
    virtual void Set(unsigned index, bool value) = 0;
    virtual void Set(unsigned index, const cse::String &value) = 0;
    virtual void Set(unsigned index, const ext::Uuid &value) = 0;
    virtual void Set(unsigned index, const Blob &value) = 0;
    virtual void Set(unsigned index, const Null &value) = 0;

    template <typename Type_>
    void Set(unsigned index, const Type_ *value) {
        if (value == NULL)
            Set(index, dbi::Null());
        else
            Set(index, *value);
    }

    template <typename Type_>
    typename etl::IfTrue<etl::IsIntegral<Type_>::value, void>::Result
    Set(unsigned index, Type_ value) {
        Set(index, static_cast<int64_t>(value));
    }

    virtual _R<ResultSet> Execute() const = 0;

    template <typename Arg0_>
    _R<ResultSet> Execute(const Arg0_ &arg0) {
        Set(0, arg0);
        return Execute();
    }

    template <typename Arg0_, typename Arg1_>
    _R<ResultSet> Execute(const Arg0_ &arg0, const Arg1_ &arg1) {
        Set(0, arg0);
        Set(1, arg1);
        return Execute();
    }

    template <typename Arg0_, typename Arg1_, typename Arg2_>
    _R<ResultSet> Execute(const Arg0_ &arg0, const Arg1_ &arg1, const Arg2_ &arg2) {
        Set(0, arg0);
        Set(1, arg1);
        Set(2, arg2);
        return Execute();
    }

    template <typename Arg0_, typename Arg1_, typename Arg2_, typename Arg3_>
    _R<ResultSet> Execute(const Arg0_ &arg0, const Arg1_ &arg1, const Arg2_ &arg2, const Arg3_ &arg3) {
        Set(0, arg0);
        Set(1, arg1);
        Set(2, arg2);
        Set(3, arg3);
        return Execute();
    }

    template <typename Arg0_, typename Arg1_, typename Arg2_, typename Arg3_, typename Arg4_>
    _R<ResultSet> Execute(const Arg0_ &arg0, const Arg1_ &arg1, const Arg2_ &arg2, const Arg3_ &arg3, const Arg4_ &arg4) {
        Set(0, arg0);
        Set(1, arg1);
        Set(2, arg2);
        Set(3, arg3);
        Set(4, arg4);
        return Execute();
    }

    template <typename Arg0_, typename Arg1_, typename Arg2_, typename Arg3_, typename Arg4_, typename Arg5_>
    _R<ResultSet> Execute(const Arg0_ &arg0, const Arg1_ &arg1, const Arg2_ &arg2, const Arg3_ &arg3, const Arg4_ &arg4, const Arg5_ &arg5) {
        Set(0, arg0);
        Set(1, arg1);
        Set(2, arg2);
        Set(3, arg3);
        Set(4, arg4);
        Set(5, arg5);
        return Execute();
    }

    template <typename Arg0_, typename Arg1_, typename Arg2_, typename Arg3_, typename Arg4_, typename Arg5_, typename Arg6_>
    _R<ResultSet> Execute(const Arg0_ &arg0, const Arg1_ &arg1, const Arg2_ &arg2, const Arg3_ &arg3, const Arg4_ &arg4, const Arg5_ &arg5, const Arg6_ &arg6) {
        Set(0, arg0);
        Set(1, arg1);
        Set(2, arg2);
        Set(3, arg3);
        Set(4, arg4);
        Set(5, arg5);
        Set(6, arg6);
        return Execute();
    }
};

}

#endif//MENES_DBI_STATEMENT_HPP
