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

#ifndef MENES_API_COM_HANDLE_HPP
#define MENES_API_COM_HANDLE_HPP

#include "cxx/platform.hpp"
#include "api/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "api/com/error.hpp"

struct IErrorInfo;

namespace api {
namespace Com {

template <typename Counted_>
class ReferenceManager :
    public ext::NullReferenceHelper<Counted_ *>
{
    template <typename Counted2_>
    friend class ReferenceManager;

  private:
    typedef ReferenceManager<Counted_> This_;
    typedef ext::NullReferenceHelper<Counted_ *> Base_;

  public:
    static void Take(typename Base_::Value &lhs, const typename Base_::Value &rhs) {
        lhs = rhs;
        if (lhs != NULL)
            lhs->AddRef();
    }

    static bool Drop(const typename Base_::Value &value) {
        return value->Release() == 0;
    }
};

template <typename Type_>
struct Handle {
    typedef ext::Handle<
        Type_,
        ReferenceManager<Type_>
    > Result;
};

} }

#endif//MENES_API_COM_HANDLE_HPP
