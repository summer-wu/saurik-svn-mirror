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

#include "ios/printwriter.hpp"
#include "xml/buffer.hpp"

namespace xml {

void Buffer::Action::WriteTo(Writer &writer) const {
    if (!parent_.IsEmpty())
        parent_->WriteTo(writer);

    switch (type_) {
        case Text:
            writer.OutputText(value_);
        break;

        case OpenAttribute:
            writer.OpenAttribute(name_);
        break;

        case CloseAttribute:
            writer.CloseAttribute();
        break;

        case OpenElement:
            writer.OpenElement(name_);
        break;

        case CloseElement:
            writer.CloseElement();
        break;
    }
}

void Buffer::AddAction_(Action::Type type, const Name &name, const cse::String &value) {
    OPT_TIMER("xml::Buffer::AddAction_()")

    if (actions_.IsEmpty())
        actions_.InsertLast();
    actions_.Last() = new _H<Action>(type, name, value, actions_.Last());
}

Buffer::Buffer() :
    decline_(0),
    incline_(0)
{
}

void Buffer::OpenElement(const Name &name) {
    AddAction_(Action::OpenElement, name);

    ++incline_;
}

void Buffer::CloseElement() {
    AddAction_(Action::CloseElement);

    if (incline_ == 0)
        ++decline_;
    else
        --incline_;
}

void Buffer::OpenAttribute(const Name &name) {
    AddAction_(Action::OpenAttribute, name);
}

void Buffer::CloseAttribute() {
    AddAction_(Action::CloseAttribute);
}

void Buffer::OutputText(const cse::String &text) {
    //if (actions_.IsEmpty() || actions_.Last().type_ != Action::Text)
        AddAction_(Action::Text, Name(), text);
    /*else {
        actions_.Touch();
        actions_.Last().value_.InsertAllLast(text);
    }*/
}

void Buffer::OutputComment(const cse::String &comment) {
}

void Buffer::WriteTo(Writer &writer) const {
    OPT_TIMER("xml::Buffer::WriteTo()")

    _foreach (const Actions_, action, actions_)
        (*action)->WriteTo(writer);
}

void Buffer::Append(const Buffer &buffer) {
    OPT_TIMER("xml::Buffer::Append()")

    if (incline_ >= buffer.decline_)
        incline_ -= buffer.decline_;
    else {
        decline_ += buffer.decline_ - incline_;
        incline_ = 0;
    }

    incline_ += buffer.incline_;

    actions_.InsertAllLast(buffer.actions_);
}

bool Buffer::IsEmpty() const {
    return actions_.IsEmpty();
}

void Buffer::Clear() {
    OPT_TIMER("xml::Buffer::Clear()")

    actions_.Clear();

    decline_ = 0;
    incline_ = 0;
}

bool Buffer::IsWellFormed() const {
    return decline_ == 0 && incline_ == 0;
}

/*cse::String Buffer::ToString() const {
    cse::String value;
    _foreach (Actions, action, actions_)
        if (action->type_ == Action::Text)
            value.InsertAllLast(action->value_);
    return value;
}*/

/*bool Buffer::operator <(const Buffer &rhs) const {
    return actions_ < rhs.actions_;
}

bool Buffer::operator ==(const Buffer &rhs) const {
    return actions_ == rhs.actions_;
}

bool Buffer::operator !=(const Buffer &rhs) const {
    return actions_ != rhs.actions_;
}*/

/*ios::PrintWriter &operator <<(ios::PrintWriter &out, const Buffer &rhs) {
    const Buffer::Actions &actions(rhs.GetActions());
    _foreach (Buffer::Actions, action, actions)
        out << "{" << action->type_ << "," << action->name_ << "," << action->value_ << "}";
    return out;
}*/

}
