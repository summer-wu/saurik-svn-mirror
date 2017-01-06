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

#include "xml/name.hpp"
#include "xml/node.hpp"
#include "xml/nodeset.hpp"

namespace xml {

Node::Node() :
    order_(~unsigned()),
    row_(~unsigned()),
    column_(~unsigned())
{
}

Node::~Node() {
}


void Node::SetOrder(unsigned order) {
    order_ = order;
}

unsigned Node::GetOrder() const {
    return order_;
}

void Node::Reorder(unsigned &order) {
    SetOrder(order++);

    // XXX: this doesn't handle attributes
    _foreach (const NodeSet, node, *this / "*")
        (*node)->Reorder(order);
}

void Node::SetPosition(unsigned row, unsigned column) {
    row_ = row;
    column_ = column;
}

Name Node::GetName() const {
    return Name();
}

cse::String Node::GetNodeValue() const {
    return cse::String();
}

_R<Node> Node::GetAttribute(const Name &name) const {
    // XXX: this is definitely not the right behavior
    _assert(false);
    throw;
}

NodeSet Node::operator /(const Name &child) const {
    return NodeSet();
}

_R<Node> Node::operator [](const Name &attribute) {
    return GetAttribute(attribute);
}

NodeSet Node::operator %(const cse::String &path) const {
    cse::String::ConstIterator slash(ext::FindFirst(path, '/'));
    return slash == path.End() ? operator /(path) : operator /(cse::String(path.Begin(), slash)) % cse::String(slash + 1, path.End());
}

}
