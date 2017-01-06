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

#include "cxx/standard.hh"
#pragma hdrstop("build/cxx/standard.pch")

#include "enc/encrypt/rc4.hpp"

namespace enc {

Rc4Writer::Rc4Writer(const _R<ios::Writer> &writer, const ext::Buffer &key) :
    ios::WriterWriter(writer),
    i_(0),
    j_(0)
{
    _forall (unsigned, i, 0, 256)
        state_[i] = i;

    unsigned j(0);
    _forall (unsigned, i, 0, 256) {
        j = (j + state_[i] + key[i % key.GetSize()]) % 256;
        ext::Swap(state_[i], state_[j]);
    }
}

size_t Rc4Writer::Write(const byte_t *data, size_t length) {
    _forall (size_t, i, 0, length) {
        i_ = (i_ + 1) % 256;
        j_ = (j_ + state_[i_]) % 256;

        ext::Swap(state_[i_], state_[j_]);
        unsigned sum((state_[i_] + state_[j_]) % 256);
        ios::Put(*writer_, state_[sum] ^ *(data++));
    }

    return length;
}

}
