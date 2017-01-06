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

#include "ios/string.hpp"

#include "xml/element.hpp"
#include "xml/nodeset.hpp"

namespace xml {

NodeSet::NodeSet() {
}

NodeSet::NodeSet(Node *node) {
    Insert(node);
}

void NodeSet::Insert(Node *node) {
    nodes_.Insert(node);
}

void NodeSet::Erase(Node *node) {
    nodes_.Remove(node);
}

void NodeSet::WriteTo(Writer &writer) const {
    _foreach (const Nodes_, node, nodes_)
        (*node)->WriteTo(writer);
}

cse::String NodeSet::GetNodeValue() const {
    _S<ios::String> data;
    _foreach (const Nodes_, node, nodes_)
        data << (*node)->GetNodeValue();
    return data;
}

NodeSet &NodeSet::operator |=(const NodeSet &nodes) {
    _foreach (const Nodes_, node, nodes.nodes_)
        nodes_.Insert(*node);
    return *this;
}

NodeSet NodeSet::operator /(const Name &child) const {
    NodeSet result;
    _foreach (const Nodes_, node, nodes_)
        if (Element *element = dynamic_cast<Element *>(node->GetValue()))
            result |= *element / child;
    return result;
}

NodeSet NodeSet::operator %(const cse::String &path) const {
    NodeSet result;
    _foreach (const Nodes_, node, nodes_)
        if (Element *element = dynamic_cast<Element *>(node->GetValue()))
            result |= *element % path;
    return result;
}

/*NodeSet NodeSet::operator [](const cse::String &xpath) const {
    NodeSet nodes;
    _foreach (Nodes_, node, nodes_)
        if ((**node)(xpath))
            nodes.Insert(*node);
    return nodes;
}*/

const _R<Node> NodeSet::operator [](size_t index) const {
    if (index >= nodes_.GetSize())
        return NULL;
    Nodes_::ConstIterator node(nodes_.Begin());
    while (index--) ++node;
    return *node;
}

/*NodeSet::operator bool() const {
    return !nodes_.IsEmpty();
}*/

NodeSet::operator cse::String() const {
    return GetNodeValue();
}

NodeSet::operator _R<Node>() const {
    _assert(GetSize() < 2);
    return IsEmpty() ? NULL : operator [](0);
}

size_t NodeSet::GetSize() const {
    return nodes_.GetSize();
}

bool NodeSet::IsEmpty() const {
    return nodes_.IsEmpty();
}

NodeSet::ConstIterator NodeSet::Begin() const {
    return nodes_.Begin();
}

NodeSet::ConstIterator NodeSet::End() const {
    return nodes_.End();
}

NodeSet::Iterator NodeSet::Begin() {
    return nodes_.Begin();
}

NodeSet::Iterator NodeSet::End() {
    return nodes_.End();
}

ios::PrintWriter &operator <<(ios::PrintWriter &out, const NodeSet &rhs) {
    return out << rhs.GetNodeValue();
}

}
