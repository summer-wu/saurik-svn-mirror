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

#ifndef MENES_IELEMENT_HPP
#define MENES_IELEMENT_HPP

#include "cxx/platform.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "ext/foreach.hpp"
#include "ext/map.hpp"
#include "ext/redblackset.hpp"

#include "xml/linkage.hpp"
#include "xml/name.hpp"
#include "xml/node.hpp"
#include "xml/writer.hpp"

namespace xml {

class XML_DECLARE IElement :
    public Node
{
  public:
    virtual Name GetName() const = 0;
    virtual void SetName(const Name &name) = 0;

    virtual void SetArgument(unsigned index, const _R<Node> &value) {
        throw;
    }

    virtual void AddArgument(const _R<Node> &value) {
        throw;
    }

    virtual _R<Node> GetArgument(unsigned index) const {
        throw;
    }

    virtual void SetAttribute(const Name &name, const _R<Node> &value) {
        throw;
    }

    virtual _R<Node> GetAttribute(const Name &name) const {
        throw;
    }

    virtual void AppendChild(Node *child) = 0;
};

XML_DECLARE extern EXT_HANDLESTUB(IElement)

template <typename Type_>
class ElementImpl :
    public IElement
{
  public:
    typedef ext::Vector< _R<Type_> > Children;

  protected:
    Children children_;

    virtual void WriteArguments_(Writer &writer) const = 0;
    virtual void WriteAttributes_(Writer &writer) const = 0;

  public:
    const Children &GetChildren() const {
        return children_;
    }

    Children &GetChildren() {
        return children_;
    }

    virtual void AppendChild(Node *child) {
        Type_ *node(dynamic_cast<Type_ *>(child));
        /*if (node == NULL)
            if (Element *element = dynamic_cast<Element *>(child))
                api::Cout << element->GetName() << ios::NewLine;*/
        _assert(node != NULL);
        children_.InsertLast(node);
    }

    virtual void WriteTo(Writer &writer) const {
        writer.OpenElement(GetName());
        WriteArguments_(writer);
        WriteAttributes_(writer);

        _tforeach (const Children, child, GetChildren())
            (*child)->WriteTo(writer);

        writer.CloseElement();
    }
};

typedef ext::Vector< _R<Node> > Arguments;
typedef ext::RedBlackMap< Name, _R<Node> > Attributes;

template <typename Base_>
class AttributeImpl :
    public Base_
{
  protected:
    Arguments arguments_;
    Attributes attributes_;

    virtual void WriteArguments_(Writer &writer) const {
        const Arguments &arguments(GetArguments());

        _foreach (const Arguments, argument, arguments) {
            writer.OpenAttribute(Name());
            (*argument)->WriteTo(writer);
            writer.CloseAttribute();
        }
    }

    virtual void WriteAttributes_(Writer &writer) const {
        const Attributes &attributes(GetAttributes());

        _foreach (const Attributes, attribute, attributes) {
            writer.OpenAttribute(attribute->First());
            attribute->Second()->WriteTo(writer);
            writer.CloseAttribute();
        }
    }

  public:
    const Arguments &GetArguments() const {
        return arguments_;
    }

    const Attributes &GetAttributes() const {
        return attributes_;
    }

    Attributes &GetAttributes() {
        return attributes_;
    }

    virtual void SetArgument(unsigned index, const _R<Node> &value) {
        if (arguments_.GetSize() < index + 1)
            arguments_.SetSize(index + 1);
        arguments_[index] = value;
    }

    virtual void AddArgument(const _R<Node> &value) {
        arguments_.InsertLast(value);
    }

    virtual _R<Node> GetArgument(unsigned index) const {
        return arguments_[index];
    }

    virtual void SetAttribute(const Name &name, const _R<Node> &value) {
        attributes_[name] = value;
    }

    virtual _R<Node> GetAttribute(const Name &name) const {
        Attributes::ConstIterator attribute(attributes_.Find(name));
        if (attribute == attributes_.End())
            throw;
        return attribute->Second();
    }
};

}

#endif//MENES_IELEMENT_HPP
