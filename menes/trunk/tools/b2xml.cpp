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

#include "api/console.hpp"
#include "api/files.hpp"

#include "app/command.hpp"

#include "ext/refcount.hpp"
#include "ext/shared.hpp"
#include "ext/stackref.hpp"
#include "cse/string.hpp"
#include "ext/uuid.hpp"

#include "opt/memory.hpp"
#include "opt/profile.hpp"
#include "opt/reports.hpp"

#include "xml/domwriter.hpp"
#include "xml/element.hpp"
#include "xml/name.hpp"
#include "xml/parse.hpp"
#include "xml/textwriter.hpp"

app::SimpleCommand<> main_;

namespace {

void bDecode_(char type, ios::Reader &in, xml::TextWriter &writer);

void bDecode_(ios::Reader &in, xml::TextWriter &writer) {
    bDecode_(ios::Get(in), in, writer);
}

cse::String bDecodeString_(char first, ios::Reader &in) {
    cse::String length;
    length.InsertLast(first);

    for (;;) {
        char next(ios::Get(in));
        if (next == ':')
            break;
        length.InsertLast(next);
    }

    ext::Vector<char> string(lexical_cast<size_t>(length));
    in.ReadFully(string.Begin(), string.GetSize());

    return cse::String(string.Begin(), string.GetSize());
}

void bDecodeDictionary_(ios::Reader &in, xml::TextWriter &writer) {
    _forever {
        char type(ios::Get(in));
        if (type == 'e')
            break;
        _assert(type >= '0' && type <= '9');
        writer.OpenElement(bDecodeString_(type, in));
        bDecode_(in, writer);
        writer.CloseElement();
    }
}

void bDecodeList_(ios::Reader &in, xml::TextWriter &writer) {
    writer.OpenElement("list");
    _forever {
        char type(ios::Get(in));
        if (type == 'e')
            break;
        writer.OpenElement("element");
        bDecode_(type, in, writer);
        writer.CloseElement();
    }
    writer.CloseElement();
}

void bDecode_(char type, ios::Reader &in, xml::TextWriter &writer) {
    if (type >= '0' && type <= '9')
        writer.OutputText(bDecodeString_(type, in));
    else switch (type) {
        case 'i': {
            cse::String integer;
            _forever {
                char next(ios::Get(in));
                if (next == 'e')
                    break;
                // XXX: I hate pedantic C++
                writer.OutputText(cse::String(static_cast<size_t>(1), cse::CodePoint(next)));
            }
       } break;

        case 'd':
            bDecodeDictionary_(in, writer);
        break;

        case 'l':
            bDecodeList_(in, writer);
        break;

        default:
            throw;
    }
}

}

namespace app {
template <>
int SimpleCommand<>::Execute(const app::Options &options) {
    _S<xml::TextWriter> writer(api::Cout.GetStream());;

    cse::String file(*options[0]);
    _R<ios::Reader> in;

    if (file == "-")
        in = &api::Cin;
    else {
        in = new _H<api::FileReader>(file);
    }

    writer.OpenElement("bencoded");

    _forever {
        char type;
        if (!in->Get(type))
            break;
        bDecode_(type, *in, writer);
    }

    writer.CloseElement();

    return 0;
} }
