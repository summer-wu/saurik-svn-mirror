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

#ifndef MENES_CSE_ENCODING_HPP
#define MENES_CSE_ENCODING_HPP

#include "cxx/platform.hpp"
#include "cse/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "opt/profile.hpp"

namespace cse {

template <typename Encoding_>
class Encoding {
  private:
    OPT_DECLAREPERFORMANCE(CSE_DECLARE static, lengthp_)
    OPT_DECLAREPERFORMANCE(CSE_DECLARE static, lengthe_)
    OPT_DECLAREPERFORMANCE(CSE_DECLARE static, lengths_)

  public:
    template <typename Iterator_>
    static size_t Length(Iterator_ begin, size_t *size = NULL) {
        OPT_SCOPETIMER(lengthp_)

        size_t length(0);
        Iterator_ end(begin);
        while (Encoding_::Advance(end))
            ++length;
        if (size != NULL)
            *size = end - begin - 1;
        return length;
    }

    template <typename Iterator_>
    static size_t Length(Iterator_ begin, Iterator_ end) {
        OPT_SCOPETIMER(lengthe_)

        size_t length(0);
        // XXX: this isn't exactly safe as I might overshoot
        while (begin != end && Encoding_::Advance(begin))
            ++length;
        return length;
    }

    template <typename Iterator_>
    static size_t Length(Iterator_ begin, size_t size) {
        OPT_SCOPETIMER(lengths_)

        size_t length(0);
        while (size != 0 && Encoding_::Advance(begin, &size))
            ++length;
        return length;
    }
};

}

#endif//MENES_CSE_ENCODING_HPP
