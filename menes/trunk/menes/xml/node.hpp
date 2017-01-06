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

#ifndef MENES_XML_NODE_HPP
#define MENES_XML_NODE_HPP

#include "cxx/platform.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "ext/types.hpp"
#include "xml/linkage.hpp"

namespace xml {

class Name;
class NodeSet;
class Writer;

class XML_DECLARE Node :
    public ext::ReferenceCounted
{
  private:
    unsigned order_;

    unsigned row_;
    unsigned column_;

  public:
    Node();
    virtual ~Node();

    void SetOrder(unsigned order);
    unsigned GetOrder() const;
    void Reorder(unsigned &order);

    void SetPosition(unsigned row, unsigned column);

    virtual void WriteTo(Writer &writer) const = 0;

    virtual Name GetName() const;
    virtual cse::String GetNodeValue() const;

    operator cse::String() const {
        return GetNodeValue();
    }

#ifdef MENES_STL_CONVERTABLE
    operator std::string() const {
        return GetNodeValue();
    }
#endif

    virtual _R<Node> GetAttribute(const Name &name) const;

    virtual NodeSet operator /(const Name &child) const;
    _R<Node> operator [](const Name &attribute);

    virtual NodeSet operator %(const cse::String &path) const;
};

}

#endif//MENES_XML_NODE_HPP
