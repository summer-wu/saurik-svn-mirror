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

#ifndef MENES_ENC_ENCRYPT_3DES_HPP
#define MENES_ENC_ENCRYPT_3DES_HPP

#include "cxx/platform.hpp"
#include "enc/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "enc/encrypt/des.hpp"

namespace enc {

class TripleDesCrypter {
  private:
    DesCrypter crypt0_;
    DesCrypter crypt1_;
    DesCrypter crypt2_;

  public:
    TripleDesCrypter(const ext::Buffer &key0, const ext::Buffer &key1, const ext::Buffer &key2) :
        crypt0_(key0),
        crypt1_(key1),
        crypt2_(key2)
    {
    }

    void Encrypt(byte_t data[8]) {
        crypt0_.Encrypt(data);
        crypt1_.Decrypt(data);
        crypt2_.Encrypt(data);
    }

    void Decrypt(byte_t data[8]) {
        crypt2_.Decrypt(data);
        crypt1_.Encrypt(data);
        crypt0_.Decrypt(data);
    }
};

class TripleDesEncrypter :
    public ios::BlockWriter<8>
{
  private:
    _R<ios::Writer> writer_;
    TripleDesCrypter crypter_;

  protected:
    virtual void Write_(const byte_t *data) {
        byte_t result[8];
        ext::CopyAssign(result, data, 8);
        crypter_.Encrypt(result);
        writer_->WriteFully(result, 8);
    }

    virtual void Close_(const byte_t *data, size_t length) {
        _assert(length == 0);
    }

  public:
    TripleDesEncrypter(const _R<ios::Writer> &writer, const ext::Buffer &key0, const ext::Buffer &key1, const ext::Buffer &key2) :
        writer_(writer),
        crypter_(key0, key1, key2)
    {
    }

    virtual void Flush() {
        writer_->Flush();
    }
};

class TripleDesDecrypter :
    public ios::BlockWriter<8>
{
  private:
    _R<ios::Writer> writer_;
    TripleDesCrypter crypter_;

  protected:
    virtual void Write_(const byte_t *data) {
        byte_t result[8];
        ext::CopyAssign(result, data, 8);
        crypter_.Decrypt(result);
        writer_->WriteFully(result, 8);
    }

    virtual void Close_(const byte_t *data, size_t length) {
        _assert(length == 0);
    }

  public:
    TripleDesDecrypter(const _R<ios::Writer> &writer, const ext::Buffer &key0, const ext::Buffer &key1, const ext::Buffer &key2) :
        writer_(writer),
        crypter_(key0, key1, key2)
    {
    }

    virtual void Flush() {
        writer_->Flush();
    }
};

}

#endif//MENES_ENC_ENCRYPT_3DES_HPP
