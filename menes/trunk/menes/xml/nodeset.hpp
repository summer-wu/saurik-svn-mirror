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

#ifndef MENES_NODESET_HPP
#define MENES_NODESET_HPP

#include "cxx/platform.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "ext/redblackset.hpp"
#include "ios/printwriter.hpp"

#include "xml/linkage.hpp"
#include "xml/node.hpp"

namespace xml {

class XML_DECLARE NodeSet {
  private:
    struct DocumentOrdering_ {
        static _finline bool Execute(const _R<Node> &lhs, const _R<Node> &rhs) {
            return lhs->GetOrder() < rhs->GetOrder();
        }
    };

    typedef ext::RedBlackSet<_R<Node>, _R<Node>, ext::Identity, DocumentOrdering_> Nodes_;

  public:
    typedef Nodes_::Iterator Iterator;
    typedef Nodes_::ConstIterator ConstIterator;

  private:
    Nodes_ nodes_;

  public:
    NodeSet();
    NodeSet(Node *node);

    void Insert(Node *node);
    void Erase(Node *node);

    void WriteTo(Writer &writer) const;

    cse::String GetNodeValue() const;

    NodeSet &operator |=(const NodeSet &nodes);

    NodeSet operator /(const Name &child) const;
    NodeSet operator %(const cse::String &xpath) const;

    const _R<Node> operator [](size_t index) const;

    //virtual operator bool() const;
    virtual operator cse::String() const;
    virtual operator _R<Node>() const;

    size_t GetSize() const;
    bool IsEmpty() const;

    ConstIterator Begin() const;
    ConstIterator End() const;

    Iterator Begin();
    Iterator End();
};

XML_DECLARE ios::PrintWriter &operator <<(ios::PrintWriter &out, const NodeSet &rhs);

}

#endif//MENES_NODESET_HPP
