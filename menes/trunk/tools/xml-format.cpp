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

#include "app/simple.hpp"

#include "ext/refcount.hpp"
#include "ext/shared.hpp"
#include "ext/stackref.hpp"
#include "cse/string.hpp"
#include "ext/uuid.hpp"

#include "opt/memory.hpp"
#include "opt/profile.hpp"
#include "opt/reports.hpp"

#include "xml/concisexml.hpp"
#include "xml/domwriter.hpp"
#include "xml/element.hpp"
#include "xml/name.hpp"
#include "xml/parse.hpp"
#include "xml/resolver.hpp"
#include "xml/textwriter.hpp"

class StripWriter :
    public xml::Writer
{
  private:
    _R<xml::Writer> writer_;

    bool texting_;
    cse::String text_;

  public:
    StripWriter(const _R<xml::Writer> &writer) :
        writer_(writer),
        texting_(true)
    {
    }

    virtual void OutputPi(const cse::String &target, const cse::String &value) {
        text_.Clear();
        writer_->OutputPi(target, value);
    }

    virtual void MapPrefix(const cse::String &prefix, const cse::String &uri) {
         writer_->MapPrefix(prefix, uri);
    }

    virtual void UnmapPrefix(const cse::String &prefix) {
        writer_->UnmapPrefix(prefix);
    }

    virtual void OpenElement(const xml::Name &name) {
        texting_ = true;
        text_.Clear();
        writer_->OpenElement(name);
    }

    virtual void CloseElement() {
        if (!text_.IsEmpty()) {
            writer_->OutputText(text_);
            text_.Clear();
        }

        texting_ = false;
        writer_->CloseElement();
    }

    virtual void FlushElement() {
        writer_->FlushElement();
    }

    virtual void OpenAttribute(const xml::Name &name) {
        writer_->OpenAttribute(name);
    }

    virtual void CloseAttribute() {
        writer_->CloseAttribute();
    }

    virtual void OutputComment(const cse::String &comment) {
        text_.Clear();
        writer_->OutputComment(comment);
    }

    virtual void OutputText(const cse::String &text) {
        if (texting_)
            text_.InsertAllLast(text);
    }
};

int Main(const app::Options &options) {
    bool prettyprint(true);

    if (options[_B("no-pretty-print")] != NULL)
        prettyprint = false;

    _R<xml::Writer> writer;

    writer = new _H<xml::TextWriter>(api::Cout.GetStream(), prettyprint);

    if (options[_B("decode-concise-xml")] != NULL)
        writer = new _H<xml::ConciseXmlDecoder>(writer);

    writer = new _H<xml::Resolver>(writer);

    if (options[_B("strip")] != NULL)
        writer = new _H<StripWriter>(writer);

    _R<ios::Reader> in;

    if (options[0] != NULL && *options[0] != _B("-"))
        in = new _H<api::FileReader>(*options[0]);
    else
        in = &api::Cin;

    xml::Settings settings;
    xml::Parse(*in, *writer, settings);
    return 0;
}
