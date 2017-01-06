/* Ninetjer - _Truly_ Free (non-GPL) Unix .NET Runtime
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

#include "app/simple.hpp"

#include "enc/encode/hex.hpp"
#include "enc/hash/md5.hpp"

#include "ios/helpers.hpp"
#include "net/http/urlencode.hpp"

int Main(const app::Options &options) {
    ext::Buffer magic("$1$");
    ext::Buffer salt(options.GetArguments()[0]);
    ext::Buffer password(options.GetArguments()[1]);

    _S<ios::Buffer> mixin;

    {
        _S<enc::Md5Hasher> md5(&mixin);
        md5.WriteFully(password.Begin(), password.GetSize());
        md5.WriteFully(salt.Begin(), salt.GetSize());
        md5.WriteFully(password.Begin(), password.GetSize());
    }

    _S<ios::Buffer> final;

    {
        _S<enc::Md5Hasher> md5(&final);
        md5.WriteFully(password.Begin(), password.GetSize());
        md5.WriteFully(magic.Begin(), magic.GetSize());
        md5.WriteFully(salt.Begin(), salt.GetSize());
        _forall (size_t, i, 0, password.GetSize())
            ios::Put(md5, mixin[i % 16]);
        for (size_t i(password.GetSize()); i != 0; i >>= 1)
            ios::Put(md5, (i & 1) == 0 ? password[0] : '\0');
    }

    _S<ios::Buffer> temp;
    temp.Reserve(final.GetSize());

    _forall (unsigned, i, 0, 1000) {
        ext::Swap(final, temp);
        final.Clear();

        _S<enc::Md5Hasher> md5(&final);
        _S<ios::FormatWriter> writer(md5);

        if ((i & 1) == 0)
            writer << temp;
        else
            writer << password;

        if ((i % 3) != 0)
            writer << salt;
        if ((i % 7) != 0)
            writer << password;

        if ((i & 1) == 0)
            writer << password;
        else
            writer << temp;
    }

    // XXX: if this is Base64 encoding I'm going to be seriously pissed off

    static const char chars[] = "./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    ext::Buffer crypt;
    crypt.Reserve(22);

    _forall (unsigned, i, 0, 5) {
        uint32_t byte(final[i] << 16 | final[6 + i] << 8 | final[i == 4 ? 5 : 12 + i]);
        _forall (unsigned, i, 0, 4) {
            crypt.InsertLast(chars[byte & 0x3f]);
            byte >>= 6;
        }
    }

    uint32_t byte(final[11]);
    crypt.InsertLast(chars[byte & 0x3f]);
    crypt.InsertLast(chars[byte >> 6 & 0x3f]);

    api::Cout << magic << salt << "$" << crypt << ios::NewLine;

    return 0;
}
