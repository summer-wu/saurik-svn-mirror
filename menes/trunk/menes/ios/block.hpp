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

#ifndef MENES_IOS_BLOCK_HPP
#define MENES_IOS_BLOCK_HPP

#include "cxx/platform.hpp"
#include "ios/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "ext/vector.hpp"
#include "ios/streams.hpp"

namespace ios {

template <size_t Block_>
class BlockWriter :
    public ios::Writer
{
  public:
    static const size_t Block = Block_;

  protected:
    byte_t block_[Block];
    unsigned count_;

    virtual void _finalize() {
        if (!std::uncaught_exception())
            Close_(block_, count_);
        return ios::Writer::_finalize();
    }

    virtual void Write_(const byte_t *data) = 0;
    virtual void Close_(const byte_t *data, size_t length) = 0;

  public:
    BlockWriter() :
        count_(0)
    {
    }

    virtual size_t Write(const byte_t *data, size_t length) {
        size_t left(Block - count_);

        if (length < left)
            left = length;
        else {
            ext::CopyAssign(block_ + count_, data, left);
            Write_(block_);
            count_ = 0;

            data += left;
            left = length - left;

            while (left >= Block) {
                Write_(data);
                data += Block;
                left -= Block;
            }
        }

        ext::CopyAssign(block_ + count_, data, left);
        count_ += left;
        return length;
    }
};

}

#endif//MENES_IOS_BLOCK_HPP
