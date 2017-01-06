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

#ifndef MENES_ENC_ENCRYPT_RC2_HPP
#define MENES_ENC_ENCRYPT_RC2_HPP

#include "cxx/platform.hpp"
#include "enc/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "ios/block.hpp"

namespace enc {

class Rc2Crypter {
  private:
    union {
        uint8_t key8_[128];
        uint16_t key16_[64];
    };

    void MixRound_(unsigned &used, uint16_t text[4]) const;
    void MashRound_(unsigned &used, uint16_t text[4]) const;

    void ReverseMixRound_(unsigned &used, uint16_t text[4]) const;
    void ReverseMashRound_(unsigned &used, uint16_t text[4]) const;

  public:
    Rc2Crypter(const ext::Buffer &key, unsigned bits = ~unsigned());

    void Encrypt(byte_t text[4]) const;
    void Decrypt(byte_t text[4]) const;
};

class Rc2Encrypter :
    public ios::BlockWriter<8>
{
  private:
    _R<ios::Writer> writer_;
    Rc2Crypter crypter_;

  protected:
    virtual void Write_(const byte_t *data);
    virtual void Close_(const byte_t *data, size_t length);

  public:
    Rc2Encrypter(const _R<ios::Writer> &writer, const ext::Buffer &key, unsigned bits = ~unsigned());

    virtual void Flush() {
        writer_->Flush();
    }
};

class Rc2Decrypter :
    public ios::BlockWriter<8>
{
  private:
    _R<ios::Writer> writer_;
    Rc2Crypter crypter_;

  protected:
    virtual void Write_(const byte_t *data);
    virtual void Close_(const byte_t *data, size_t length);

  public:
    Rc2Decrypter(const _R<ios::Writer> &writer, const ext::Buffer &key, unsigned bits = ~unsigned());

    virtual void Flush() {
        writer_->Flush();
    }
};

}

#endif//MENES_ENC_ENCRYPT_RC2_HPP
