/* Skiazo - Menes-Compatible OpenGL Graphics Engine
 * Copyright (C) 2006  Jay Freeman (saurik)
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

#ifndef MENES_ALG_EXPONENTS_HPP
#define MENES_ALG_EXPONENTS_HPP

#include "cxx/platform.hpp"
#include "alg/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

/* XXX: this is cheating */
#include <cmath>

namespace alg {

namespace be {

template <typename Type_, unsigned Exponent_>
struct Power_ {
    static _finline Type_ Execute(const Type_ &value) {
        return value * Power_<Type_, Exponent_ - 1>::Execute(value);
    }
};

template <typename Type_>
struct Power_<Type_, 1> {
    static _finline Type_ Execute(const Type_ &value) {
        return value;
    }
};

}

template <unsigned Exponent_, typename Type_>
_finline Type_ Power(const Type_ &value) {
    return be::Power_<Type_, Exponent_>::Execute(value);
}

template <typename Type_>
_finline Type_ Square(const Type_ &value) {
    return Power<2>(value);
}

template <typename Type_>
_finline Type_ SquareRoot(const Type_ &value) {
    /* XXX: this is cheating */
    return ::sqrt(value);
}

}

#endif/*MENES_ALG_EXPONENTS_HPP*/
