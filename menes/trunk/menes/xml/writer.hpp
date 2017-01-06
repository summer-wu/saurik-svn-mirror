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

#ifndef MENES_WRITER_HPP
#define MENES_WRITER_HPP

#include "cxx/platform.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "ext/listset.hpp"
#include "ext/map.hpp"
#include "ext/refcount.hpp"
#include "ext/types.hpp"

#include "xml/linkage.hpp"
#include "xml/name.hpp"

namespace xml {

class Name;
class Node;

class XML_DECLARE Writer :
    public ext::ReferenceCounted
{
  public:
    /*enum Complex {
        Attribute,
        Element
    };

    enum Simple {
        Comment,
        Pi,
        Text,
    };*/

  public:
    virtual void OutputPi(const cse::String &target, const cse::String &value) {
        // XXX: purify and implement
    }

    virtual void MapPrefix(const cse::String &prefix, const cse::String &uri);
    virtual void UnmapPrefix(const cse::String &prefix);

    virtual void OpenElement(const Name &name) = 0;
    virtual void CloseElement() = 0;
    virtual void FlushElement();

    virtual void OpenAttribute(const Name &name) = 0;
    virtual void CloseAttribute() = 0;

    virtual void OutputText(const cse::String &text) = 0;
    virtual void OutputComment(const cse::String &comment) = 0;

    void SetAttribute(const Name &name, const cse::String &value) {
        OpenAttribute(name);
        OutputText(value);
        CloseAttribute();
    }
};

class ScopeElement {
  private:
    Writer &writer_;

  public:
    ScopeElement(Writer &writer, const Name &name) :
        writer_(writer)
    {
        writer_.OpenElement(name);
    }

    ~ScopeElement() {
        writer_.CloseElement();
    }
};

}

#endif//MENES_WRITER_HPP
