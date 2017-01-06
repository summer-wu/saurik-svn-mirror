/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2002-2006  Jay Freeman (saurik)
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

#ifndef MENES_API_APR_ARRAY_HPP
#define MENES_API_APR_ARRAY_HPP

#include "cxx/platform.hpp"
#include "api/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "api/apr/pool.hpp"

#include <apr_tables.h>

namespace api {
namespace Apr {

template <typename Element_>
class Array {
  private:
    Pool pool_;
    ::apr_array_header_t *array_;

  public:
    inline Array(const Array &rhs) :
        array_(::apr_array_copy(pool_, rhs))
    {
    }

    inline Array(int size = 0) :
        array_(::apr_array_make(pool_, size, sizeof (Element_)))
    {
        _repeat (size)
            ::apr_array_push(array_);
    }

    template <typename Type_>
    inline Array(int size, const Type_ &value) :
        array_(::apr_array_make(pool_, size, sizeof (Element_)))
    {
        _repeat (size)
            Push() = value;
    }

    template <typename Iterator_>
    inline Array(Iterator_ begin, Iterator_ end) :
        array_(::apr_array_make(pool_, 0, sizeof (Element_)))
    {
        _forall (Iterator_, value, begin, end)
            Push() = value;
    }

    inline bool IsEmpty() const
    {
        return ::apr_is_empty_array(array_);
    }

    Element_ &Pop()
    {
        return *reinterpret_cast<Element_ *>(::apr_array_pop(array_));
    }

    Element_ &Push()
    {
        return *reinterpret_cast<Element_ *>(::apr_array_push(array_));
    }

    operator ::apr_array_header_t *()
    {
        return array_;
    }
};

} }

#endif//MENES_API_APR_ARRAY_HPP
