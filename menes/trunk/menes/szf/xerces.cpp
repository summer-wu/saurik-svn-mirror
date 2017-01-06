/* Menes Optimization - C++ Profiling and Verification
 * Copyright (C) 2003-2005  Jay Freeman (saurik)
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

#include "cse/lexical.hpp"
#include "ext/uuid.hpp"

#include "ios/string.hpp"
#include "szf/xerces.hpp"

#include "xml/name.hpp"
#include "xml/textwriter.hpp"

namespace szf {

XmlMethod::XmlMethod() {
}

XmlMethod::~XmlMethod() {
}

void XmlMethod::OpenScope(const char *name) {
    writer_->OpenElement(xml::Name("http://www.saurik.com/menes/xml/0"));
}

void XmlMethod::CloseScope() {
    writer_->CloseElement();
}

void XmlMethod::SaveBoolean(bool value, const char *name) {
    SaveString(lexical_cast<cse::String>(value), name);
}

void XmlMethod::LoadBoolean(bool &value, const char *name) {
    _assert(false);
    // XXX: implement
}

void XmlMethod::SaveSigned(int64_t value, const char *name) {
    SaveString(lexical_cast<cse::String>(value), name);
}

void XmlMethod::LoadSigned(int64_t &value, const char *name) {
    _assert(false);
    // XXX: implement
}

void XmlMethod::SaveUnsigned(uint64_t value, const char *name) {
    SaveString(lexical_cast<cse::String>(value), name);
}

void XmlMethod::LoadUnsigned(uint64_t &value, const char *name) {
    _assert(false);
    // XXX: implement
}

void XmlMethod::SaveFloating(double value, const char *name) {
    SaveString(lexical_cast<cse::String>(value), name);
}

void XmlMethod::LoadFloating(double &value, const char *name) {
    _assert(false);
    // XXX: implement
}

void XmlMethod::SaveUuid(const ext::Uuid &value, const char *name) {
    SaveString(lexical_cast<cse::String>(value), name);
}

void XmlMethod::LoadUuid(ext::Uuid &value, const char *name) {
    _assert(false);
    // XXX: implement
}

void XmlMethod::SaveString(const cse::String &value, const char *name) {
    if (*name == '@')
        writer_->SetAttribute(name + 1, value);
    else {
        OpenScope(name);
        SaveString(value, "@value");
        CloseScope();
    }
}

void XmlMethod::LoadString(cse::String &value, const char *name) {
    _assert(false);
    // XXX: implement
}

void XmlMethod::SaveData(const char *data, size_t length, const char *name) {
    _assert(false);
    // XXX: implement
}

void XmlMethod::LoadData(char *data, size_t length, const char *name) {
    _assert(false);
    // XXX: implement
}

void XmlMethod::Bind(ios::Reader *in, ios::Writer *out) {
    writer_ = (out == NULL ? NULL : new _H<xml::TextWriter>(*out));
    // XXX: implement
}

}
