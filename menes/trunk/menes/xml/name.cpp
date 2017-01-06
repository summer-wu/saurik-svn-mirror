/* Menes XML - Light Yet Extensible XML Library
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

#include "ext/exception.hpp"
#include "xml/name.hpp"

namespace xml {

Name::Name() {
}

Name::Name(const cse::String &local) :
    local_(local)
{
}

Name::Name(const ext::Buffer &local) :
    local_(local)
{
}

Name::Name(const char *local) :
    local_(local)
{
}

Name::Name(const cse::String &local, const cse::String &space) :
    space_(space),
    local_(local)
{
}

Name::Name(const cse::String &local, const cse::String &space, const cse::String &prefix) :
    space_(space),
    local_(local),
    prefix_(prefix)
{
}

Name::Name(const Name &name) :
    space_(name.space_),
    local_(name.local_),
    prefix_(name.prefix_)
{
}

const cse::String &Name::GetSpace() const {
    return space_;
}

void Name::SetSpace(const cse::String &space) {
    space_ = space;
}

const cse::String &Name::GetLocal() const {
    return local_;
}

void Name::SetLocal(const cse::String &local) {
    local_ = local;
}

const cse::String &Name::GetPrefix() const {
    return prefix_;
}

void Name::SetPrefix(const cse::String &prefix) {
    prefix_ = prefix;
}

void Name::Clear() {
    local_.Clear();
    space_.Clear();
    prefix_.Clear();
}

/*Name::operator cse::String() const {
    return GetLocal();
}*/

bool Name::operator <(const Name &rhs) const {
    int comp(space_.Compare(rhs.space_));
    if (comp != 0) return comp < 0;
    return local_ < rhs.local_;
}

bool Name::operator ==(const Name &rhs) const {
    return space_ == rhs.space_ && local_ == rhs.local_;
}

bool Name::operator !=(const Name &rhs) const {
    return space_ != rhs.space_ || local_ != rhs.local_;
}

bool Name::Split(const cse::String &qname, cse::String &prefix, cse::String &local) {
    cse::String::ConstIterator colon(ext::FindLast(qname, ':'));
    if (colon == qname.End()) {
        prefix.Clear();
        local = qname;
        return false;
    } else {
        prefix = cse::String(qname.Begin(), colon);
        local = cse::String(colon + 1, qname.End());
        return true;
    }
}

ios::PrintWriter &operator <<(ios::PrintWriter &out, const Name &rhs) {
    out << rhs.GetLocal() << "[" << rhs.GetSpace() << "]";
    return out;
}

}
