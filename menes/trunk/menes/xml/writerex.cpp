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

#include "xml/domwriter.hpp"
#include "xml/name.hpp"
#include "xml/writerex.hpp"

namespace xml {

void AttributeWriterEx::AFlushElement_() {
}

AttributeWriterEx::AttributeWriterEx() :
    opened_(false),
    depth_(0)
{
}

void AttributeWriterEx::OpenElement(const Name &name) {
    if (!dom_.IsEmpty())
        dom_->OpenElement(name);
    else if (!opened_)
        AOpenElement_(name);
    else {
        dom_ = new _H<DomWriter>(builder_);
        dom_->OpenElement(name);
    }
}

void AttributeWriterEx::CloseElement() {
    if (!dom_.IsEmpty())
        dom_->CloseElement();
    else
        ACloseElement_();
}

void AttributeWriterEx::FlushElement() {
    if (!dom_.IsEmpty())
        dom_->FlushElement();
    else
        AFlushElement_();
}

void AttributeWriterEx::OpenAttribute(const Name &name) {
    if (!dom_.IsEmpty()) {
        ++depth_;
        dom_->OpenAttribute(name);
    } else {
        _assert(!opened_);
        opened_ = true;
        name_ = name;
    }
}

void AttributeWriterEx::CloseAttribute() {
    if (depth_ != 0) {
        dom_->CloseAttribute();
        --depth_;
    } else {
        _assert(opened_);

        _R<Node> value;

        if (!value_.IsEmpty()) {
            value = value_;
            value_.Clear();
        } else if (!dom_.IsEmpty()) {
            value = dom_->GetRoot();
            dom_.Clear();
        } else
            _assert(false);

        if (name_.GetLocal().IsEmpty())
            AAddArgument_(value);
        else
            ASetAttribute_(name_, value);

        opened_ = false;
        name_.Clear();
    }
}

void AttributeWriterEx::OutputText(const cse::String &text) {
    if (!dom_.IsEmpty())
        dom_->OutputText(text);
    else if (!opened_)
        AOutputText_(text);
    else {
        _assert(value_.IsEmpty());
        value_ = builder_.NewText(text);
    }
}

void AttributeWriterEx::OutputComment(const cse::String &comment) {
    if (!dom_.IsEmpty())
        dom_->OutputComment(comment);
    else
        AOutputComment_(comment);
}

void ElementWriterEx::AOpenElement_(const Name &name) {
    AFlushElement_();
    names_.InsertLast(name);
    opened_ = true;
}

void ElementWriterEx::ACloseElement_() {
    AFlushElement_();
    ECloseElement_(names_.Last());
    names_.RemoveLast();
}

void ElementWriterEx::AAddArgument_(const _R<Node> &value) {
    _assert(opened_);
    arguments_.InsertLast(value);
}

void ElementWriterEx::ASetAttribute_(const Name &name, const _R<Node> &value) {
    _assert(opened_);
    attributes_.Insert(name, value);
}

void ElementWriterEx::AOutputComment_(const cse::String &comment) {
    AFlushElement_();
    EOutputComment_(comment);
}

void ElementWriterEx::AOutputText_(const cse::String &text) {
    AFlushElement_();
    EOutputText_(text);
}

void ElementWriterEx::AFlushElement_() {
    if (!opened_)
        return;

    Arguments arguments(arguments_);
    Attributes attributes(attributes_);

    arguments_.Clear();
    attributes_.Clear();
    opened_ = false;

    EOpenElement_(names_.Last(), arguments, attributes);
}

ElementWriterEx::ElementWriterEx() :
    opened_(false)
{
}

}
