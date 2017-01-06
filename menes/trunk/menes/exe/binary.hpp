/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2007-2009  Jay Freeman (saurik)
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

#ifndef MENES_EXE_BINARY_HPP
#define MENES_EXE_BINARY_HPP

#include "cxx/platform.hpp"
#include "exe/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "api/memorymap.hpp"
#include "cse/string.hpp"

namespace exe {

class Binary :
    public api::MemoryMap
{
  private:
    typedef api::MemoryMap Base_;

  protected:
    bool swapped_;

    virtual void *GetPointer_(uint32_t address, const char *segment_name = NULL) = 0;
    virtual void *GetOffset_(uint32_t offset) = 0;

  public:
    template <typename Type_>
    Type_ Swap(Type_ value) const {
        return swapped_ ? ext::SwapBytes(value) : value;
    }

    Binary(const cse::String &path) :
        Base_(path),
        swapped_(false)
    {
    }

    template <typename Target_>
    Target_ *GetPointer(uint32_t address, const char *segment_name = NULL) {
        return GetPointer<void>(address, segment_name);
    }

    template <typename Target_>
    Target_ *GetOffset(uint32_t offset) {
        return GetOffset<void>(offset);
    }
};

}

#endif//MENES_EXE_BINARY_HPP
