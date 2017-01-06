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

#include "xml/builder.hpp"
#include "xml/comment.hpp"
#include "xml/domwriter.hpp"
#include "xml/element.hpp"
#include "xml/text.hpp"

namespace xml {

EXT_HANDLESHIM(DomWriter)

void DomWriter::AOpenElement_(const Name &name) {
    _assert(!IsComplete());

    Tag_ tag(builder_.NewElement(name));
    tag.element_->SetOrder(order_++);
    if (!tags_.IsEmpty())
        tags_.Last().element_->AppendChild(tag.element_);
    tags_.InsertLast(tag);
}

void DomWriter::ACloseElement_() {
    _assert(!IsComplete());
    _assert(!tags_.IsEmpty());

    if (tags_.GetSize() == 1)
        order_ = ~unsigned();
    else
        tags_.RemoveLast();
}

void DomWriter::AAddArgument_(const _R<Node> &value) {
    _assert(!IsComplete());
    _assert(!tags_.IsEmpty());

    // XXX: technically this might _have_ to copy these nodes?
    //      maybe nodes can keep references to their builder so you can check...

    value->Reorder(order_);
    tags_.Last().element_->AddArgument(value);
}

void DomWriter::ASetAttribute_(const Name &name, const _R<Node> &value) {
    _assert(!IsComplete());
    _assert(!tags_.IsEmpty());

    // XXX: technically this might _have_ to copy these nodes?
    //      maybe nodes can keep references to their builder so you can check...

    value->Reorder(order_);
    tags_.Last().element_->SetAttribute(name, value);
}

void DomWriter::AOutputText_(const cse::String &text) {
    _assert(!IsComplete());
    _assert(!tags_.IsEmpty());

    tags_.Last().element_->AppendChild(builder_.NewText(text));
}

void DomWriter::AOutputComment_(const cse::String &comment) {
    _assert(!IsComplete());
    _assert(!tags_.IsEmpty());

    tags_.Last().element_->AppendChild(builder_.NewComment(comment));
}

DomWriter::DomWriter(Builder &builder) :
    builder_(builder),
    order_(0)
{
}

DomWriter::DomWriter(Builder &builder, IElement *root) :
    builder_(builder),
    order_(0)
{
    tags_.InsertLast(Tag_(root));
}

DomWriter::~DomWriter() {
}

_R<IElement> DomWriter::GetRoot() const {
    _assert(!tags_.IsEmpty());

    return tags_.First().element_;
}

bool DomWriter::IsComplete() const {
    return order_ == ~unsigned();
}

}
