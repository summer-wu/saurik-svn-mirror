/* Menes XML - Light Yet Extensible XML Library
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

#include "xml/name.hpp"
#include "xml/resolver.hpp"

namespace xml {

Name Resolver::Resolve_(const cse::String &qname, bool attribute) const {
    OPT_TIMER("xml::Resolver::Resolve_()")

    cse::String prefix, local;
    if (!Name::Split(qname, prefix, local) && attribute)
        return qname;

    if (prefix == "xml")
        return Name(local, "http://www.w3.org/XML/1998/namespace", prefix);
    else if (prefix == "xmlns")
        return Name(local, "http://www.w3.org/2000/xmlns/", prefix);
    else if (prefix == "" && local == "xmlns")
        // XXX: this might be wrong behavior, but it's better than running Resolve_() on it...
        return Name(local);

    Maps_::ConstIterator fix(maps_.End());
    while (fix != maps_.Begin()) {
        const Map_ &entry(*--fix);
        Map_::ConstIterator uri(entry.Find(prefix));
        if (uri != entry.End())
            return Name(local, uri->Second(), prefix);
    }

    throw;
}

void Resolver::EOutputText_(const cse::String &text) {
    out_->OutputText(text);
}

void Resolver::EOutputComment_(const cse::String &comment) {
    out_->OutputComment(comment);
}

void Resolver::EOpenElement_(const Name &name, const Arguments &arguments, const Attributes &attributes) {
    maps_.InsertLast();
    Map_ &map(maps_.Last());

    _foreach (const Attributes, attribute, attributes) {
        _assert(attribute->First().GetSpace().IsEmpty());
        _assert(attribute->First().GetPrefix().IsEmpty());

        cse::String prefix;

        const cse::String &local(attribute->First().GetLocal());
        if (local.StartsWithAll("xmlns:"))
            prefix = local.Substring(6);
        else if (local != "xmlns")
            continue;

        _assert(!map.Contains(prefix));
        map[prefix] = attribute->Second()->GetNodeValue();
        out_->MapPrefix(prefix, attribute->Second()->GetNodeValue());
    }

    out_->OpenElement(
        name.GetSpace().IsEmpty() ?
            Resolve_(name.GetLocal(), false) :
            name
    );

    // XXX: this might need to write back to itself...

    _foreach (const Arguments, argument, arguments) {
        out_->OpenAttribute(Name());
        (*argument)->WriteTo(*out_);
        out_->CloseAttribute();
    }

    _foreach (const Attributes, attribute, attributes) {
        const cse::String &local(attribute->First().GetLocal());

        if (local != "xmlns" && !local.StartsWithAll("xmlns:")) {
            out_->OpenAttribute(Resolve_(attribute->First().GetLocal(), true));
            attribute->Second()->WriteTo(*out_);
            out_->CloseAttribute();
        }
    }

    out_->FlushElement();
}

void Resolver::ECloseElement_(const Name &name) {
    out_->CloseElement();

    _foreach (Map_, map, maps_.Last())
        out_->UnmapPrefix(map->First());

    maps_.RemoveLast();
    _assert(!maps_.IsEmpty());
}

Resolver::Resolver(const _R<Writer> &out) :
    out_(out),
    maps_(1)
{
    maps_.Last()[cse::EmptyString] = cse::EmptyString;
}

Resolver::~Resolver() {
    if (!std::uncaught_exception())
        _assert(maps_.GetSize() == 1);
}

}
