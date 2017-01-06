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

#include "cxx/standard.hh"
#pragma hdrstop("build/cxx/standard.pch")

#include "xml/concisexml.hpp"

namespace xml {

namespace {
    const cse::String cxs_(_B("http://www.saurik.com/menes/xml/cxs/0.9"));
    const xml::Name argument_(_B("argument"), cxs_);
    const xml::Name child_(_B("children"), cxs_);
}

void ConciseXmlDecoder::EOutputText_(const cse::String &text) {
    out_->OutputText(text);
}

void ConciseXmlDecoder::EOutputComment_(const cse::String &comment) {
    out_->OutputComment(comment);
}

void ConciseXmlDecoder::EOpenElement_(const Name &name, const Arguments &arguments, const Attributes &attributes) {
    out_->OpenElement(name);

    _foreach (const Attributes, attribute, attributes)
        if (Text *text = dynamic_cast<Text *>(attribute->Second().GetValue())) {
            out_->OpenAttribute(attribute->First());
            text->WriteTo(*out_);
            out_->CloseAttribute();
        }

    out_->FlushElement();

    _foreach (const Arguments, argument, arguments) {
        out_->OpenElement(argument_);
        (*argument)->WriteTo(*this);
        out_->CloseElement();
    }

    _foreach (const Attributes, attribute, attributes)
        if (dynamic_cast<Text *>(attribute->Second().GetValue()) == NULL) {
            out_->OpenElement(attribute->First());
            attribute->Second()->WriteTo(*this);
            out_->CloseElement();
        }

    out_->OpenElement(child_);
}

void ConciseXmlDecoder::ECloseElement_(const Name &name) {
    out_->CloseElement();
    out_->CloseElement();
}

ConciseXmlDecoder::ConciseXmlDecoder(const _R<Writer> &out) :
    out_(out)
{
    //out_->MapPrefix("cxs", "http://www.saurik.com/menes/xml/cxs/0.9");
}

ConciseXmlDecoder::~ConciseXmlDecoder() {
}

}
