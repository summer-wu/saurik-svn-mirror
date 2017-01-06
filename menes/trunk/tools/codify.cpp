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

#include "enc/encode/base64.hpp"
#include "enc/encode/hex.hpp"
#include "enc/encode/quotedprintable.hpp"
#include "enc/encode/trigraph.hpp"

#include "enc/encrypt/3des.hpp"
#include "enc/encrypt/des.hpp"
#include "enc/encrypt/rc2.hpp"
#include "enc/encrypt/rc4.hpp"

#include "enc/hash/md5.hpp"
#include "enc/hash/sha1.hpp"

#include "ios/helpers.hpp"
#include "net/http/urlencode.hpp"

#include "xml/xmlencode.hpp"
#include "waf/escape.hpp"

int Main(const app::Options &options) {
    _R<ios::Writer> writer(&api::Cout);

    _foreach (const app::ArgumentList, argument, options.GetArguments())
        if (*argument == _B("3des-decrypt")) {
            cse::String key0(*++argument);
            cse::String key1(*++argument);
            cse::String key2(*++argument);
            writer = new _H<enc::TripleDesDecrypter>(writer, key0, key1, key2);
        } else if (*argument == _B("3des-encrypt")) {
            cse::String key0(*++argument);
            cse::String key1(*++argument);
            cse::String key2(*++argument);
            writer = new _H<enc::TripleDesEncrypter>(writer, key0, key1, key2);
        } else if (*argument == _B("base64-decode"))
            writer = new _H<enc::Base64Decoder>(writer);
        else if (*argument == _B("base64-encode"))
            writer = new _H<enc::Base64Encoder>(writer);
        else if (*argument == _B("des-decrypt")) {
            cse::String key(*++argument);
            writer = new _H<enc::DesDecrypter>(writer, key);
        } else if (*argument == _B("des-encrypt")) {
            cse::String key(*++argument);
            writer = new _H<enc::DesEncrypter>(writer, key);
        } else if (*argument == _B("hex-decode"))
            writer = new _H<enc::HexDecoder>(writer);
        else if (*argument == _B("hex-encode"))
            writer = new _H<enc::HexEncoder>(writer);
        else if (*argument == _B("md5-hash"))
            writer = new _H<enc::Md5Hasher>(writer);
        else if (*argument == _B("quoted-printable-decode"))
            writer = new _H<enc::QuotedPrintableDecoder>(writer);
        else if (*argument == _B("rc2-decrypt")) {
            cse::String key(*++argument);
            // XXX: lexical_cast<>()
            unsigned bits(lexical_cast<unsigned>(cse::String(*--argument)));
            writer = new _H<enc::Rc2Decrypter>(writer, key, bits);
        } else if (*argument == _B("rc2-encrypt")) {
            cse::String key(*++argument);
            // XXX: lexical_cast<>()
            unsigned bits(lexical_cast<unsigned>(cse::String(*--argument)));
            writer = new _H<enc::Rc2Encrypter>(writer, key, bits);
        } else if (*argument == _B("rc4-crypt")) {
            cse::String key(*++argument);
            writer = new _H<enc::Rc4Writer>(writer, key);
        } else if (*argument == _B("sha1-hash"))
            writer = new _H<enc::Sha1Hasher>(writer);
        else if (*argument == _B("trigraph-decode"))
            writer = new _H<enc::TrigraphDecoder>(writer);
        else if (*argument == _B("url-decode"))
            writer = new _H<net::Http::UrlDecoder>(writer);
        else if (*argument == _B("url-encode")) {
            ext::Buffer reserved(*++argument);
            writer = new _H<net::Http::UrlEncoder>(writer, reserved);
        } else if (*argument == _B("xml-decode"))
            writer = new _H<xml::XmlDecoder>(writer);
        else if (*argument == _B("xml-encode"))
            writer = new _H<xml::XmlEncoder>(writer);
        else if (*argument == _B("waf-escape"))
            writer = new _H<waf::Escape>(writer);
        else {
            api::Cerr << "unknown coder: " << *argument << ios::NewLine;
            return -1;
        }

    ios::ReadToWrite(api::Cin, *writer);
    return 0;
}
