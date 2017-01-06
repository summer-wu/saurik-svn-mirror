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
 * BUT NOT PUSHBACKED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT PUSHBACKED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef MENES_IOS_PUSHBACK_HPP
#define MENES_IOS_PUSHBACK_HPP

#include "cxx/platform.hpp"
#include "ios/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "ios/streams.hpp"

namespace ios {

class PushBackReader :
    public ios::ReaderReader
{
  private:
    bool valid_;
    byte_t last_;

  public:
    PushBackReader(const _R<Reader> &reader) :
        ReaderReader(reader),
        valid_(false)
    {
    }

    ~PushBackReader() {
        if (!std::uncaught_exception())
            _assert(!valid_);
    }

    virtual size_t Read(byte_t *data, size_t length) {
        if (length == 0)
            return 0;
        else if (!valid_)
            return ReaderReader::Read(data, length);
        else {
            valid_ = false;
            data[0] = last_;
            return 1 + ReaderReader::Read(data + 1, length - 1);
        }
    }

    void PushBack(byte_t last) {
        _assert(!valid_);
        valid_ = true;
        last_ = last;
    }
};

}

#endif//MENES_IOS_PUSHBACK_HPP
