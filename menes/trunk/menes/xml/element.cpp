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

#include "ext/foreach.hpp"
#include "cse/string.hpp"

#include "ios/string.hpp"

#include "xml/builder.hpp"
#include "xml/comment.hpp"
#include "xml/document.hpp"
#include "xml/element.hpp"
#include "xml/instruction.hpp"
#include "xml/nodeset.hpp"
#include "xml/text.hpp"

namespace xml {

EXT_HANDLESHIM(Element)

Element::Element() {
}

Element::Element(const Name &name) :
    name_(name)
{
}

Element::~Element() {
}

Name Element::GetName() const {
    return name_;
}

void Element::SetName(const Name &name) {
    name_ = name;
}

// XXX: why doesn't this recurse?
cse::String Element::GetNodeValue() const {
    _S<ios::String> value;
    _foreach (const Children, child, children_)
        value << (*child)->GetNodeValue();
    return value;
}

Name Element::Resolve(const cse::String &qname) const {
    cse::String prefix, local;
    Name::Split(qname, prefix, local);
    // XXX: implement qualified name resolution
    return qname;
}

_R<Node> Element::GetAttribute(const Name &name) const {
    Attributes::ConstIterator attr(attributes_.Find(name));
    if (attr == attributes_.End())
        throw;
    return attr->Second();
}

NodeSet Element::operator /(const Name &child) const {
    cse::String local(child.GetLocal());
    if (local.IsEmpty())
        return NodeSet();

    if (local[0] == '@') {
        Attributes::ConstIterator attribute(attributes_.Find(Name(local.Substring(1), child.GetSpace())));
        if (attribute == attributes_.End())
            return NodeSet();
        return new _H<Attribute>(this, attribute->First());
    } else if (local.EndsWithAll("()")) {
        NodeSet result;

        cse::String function(local.Substring(0, local.GetSize() - 2));
        if (function == "comment") {
            _foreach (const Children, i, children_)
                if (Comment *comment = dynamic_cast<Comment *>(i->GetValue()))
                    result.Insert(comment);
        } else if (function == "node") {
            _foreach (const Children, i, children_)
                result.Insert(*i);
        } else throw;

        return result;
    }

    // ERR: warning: control reaches end of non-void function

    NodeSet result;

    _foreach (const Children, i, children_)
        if ((*i)->GetName() == child)
            result.Insert(*i);

    return result;
}

}
