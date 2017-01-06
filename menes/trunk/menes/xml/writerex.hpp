/* Menes XML - Light Yet Extensible XML Library
 * Copyright (C) 2002-2005  Jay Freeman (saurik)
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

#ifndef MENES_WRITEREX_HPP
#define MENES_WRITEREX_HPP

#include "cxx/platform.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "xml/builder.hpp"
#include "xml/linkage.hpp"
#include "xml/writer.hpp"

namespace xml {

XML_DECLARE EXT_HANDLESTUB(class DomWriter)

class Name;
class Node;

class XML_DECLARE AttributeWriterEx :
    public Writer
{
  private:
    bool opened_;
    Name name_;

    _R<Node> value_;
    Builder builder_;

    _R<DomWriter> dom_;
    unsigned depth_;

    virtual void AOpenElement_(const Name &name) = 0;
    virtual void ACloseElement_() = 0;
    virtual void AFlushElement_();

    virtual void AAddArgument_(const _R<Node> &value) = 0;
    virtual void ASetAttribute_(const Name &name, const _R<Node> &value) = 0;

    virtual void AOutputText_(const cse::String &text) = 0;
    virtual void AOutputComment_(const cse::String &comment) = 0;

  public:
    AttributeWriterEx();

    virtual void OpenElement(const Name &name);
    virtual void CloseElement();
    virtual void FlushElement();

    virtual void OpenAttribute(const Name &name);
    virtual void CloseAttribute();

    virtual void OutputComment(const cse::String &comment);
    virtual void OutputText(const cse::String &text);
};

class XML_DECLARE ElementWriterEx :
    public AttributeWriterEx
{
  public:
    typedef ext::Vector< _R<Node> > Arguments;
    typedef ext::ListMap< Name, _R<Node> > Attributes;

  private:
    bool opened_;
    ext::Vector<Name> names_;

    Arguments arguments_;
    Attributes attributes_;

    virtual void EOutputComment_(const cse::String &comment) = 0;
    virtual void EOutputText_(const cse::String &text) = 0;

    virtual void EOpenElement_(const Name &name, const Arguments &arguments, const Attributes &attributes) = 0;
    virtual void ECloseElement_(const Name &name) = 0;

    virtual void AOpenElement_(const Name &name);
    virtual void ACloseElement_();
    virtual void AFlushElement_();

    virtual void AAddArgument_(const _R<Node> &value);
    virtual void ASetAttribute_(const Name &name, const _R<Node> &value);

    virtual void AOutputComment_(const cse::String &comment);
    virtual void AOutputText_(const cse::String &text);

  public:
    ElementWriterEx();
};

}

#endif//MENES_WRITEREX_HPP
