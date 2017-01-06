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

#ifndef MENES_XML_BUFFER_HPP
#define MENES_XML_BUFFER_HPP

#include "cxx/platform.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "ext/suballoc.hpp"
#include "ext/vector.hpp"

#include "xml/linkage.hpp"
#include "xml/name.hpp"
#include "xml/writer.hpp"

namespace xml {

class Name;

class XML_DECLARE Buffer :
    public Writer
{
  public:
    struct Action :
        public ext::ReferenceCounted
    {
        enum Type {
            Text,
            OpenAttribute,
            CloseAttribute,
            OpenElement,
            CloseElement
        } type_;

        Name name_;
        cse::String value_;
        _R<Action> parent_;

        Action(Type type, const Name &name, const cse::String &value, const _R<Action> &parent) :
            type_(type),
            name_(name),
            value_(value),
            parent_(parent)
        {
        }

        void WriteTo(Writer &writer) const;

        bool operator <(const Action &rhs) const {
            if (type_ != rhs.type_)
                return type_ < rhs.type_;
            else if (name_ != rhs.name_)
                return name_ < rhs.name_;
            else
                return value_ < rhs.value_;
        }

        bool operator ==(const Action &rhs) const {
            return type_ == rhs.type_ && name_ == rhs.name_ && value_ == rhs.value_;
        }

        bool operator !=(const Action &rhs) const {
            return type_ != rhs.type_ || name_ != rhs.name_ || value_ != rhs.value_;
        }
    };

  private:
    typedef _L< _R<Action> > Actions_;
    Actions_ actions_;

    unsigned decline_;
    unsigned incline_;

    void AddAction_(Action::Type type, const Name &name = Name(), const cse::String &value = cse::String());

  public:
    Buffer();

    virtual void OpenElement(const Name &name);
    virtual void CloseElement();

    virtual void OpenAttribute(const Name &name);
    virtual void CloseAttribute();

    virtual void OutputText(const cse::String &text);
    virtual void OutputComment(const cse::String &comment);

    void WriteTo(Writer &writer) const;

    void Append(const Buffer &buffer);

    bool IsEmpty() const;
    void Clear();

    bool IsWellFormed() const;

    // XXX: fix these!

    //cse::String ToString() const;

    /*bool operator <(const Buffer &rhs) const;
    bool operator ==(const Buffer &rhs) const;
    bool operator !=(const Buffer &rhs) const;*/
};

//ios::PrintWriter &operator <<(ios::PrintWriter &out, const Buffer &rhs);

}

#endif//MENES_XML_BUFFER_HPP
