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

#ifndef MENES_TEXTWRITER_HPP
#define MENES_TEXTWRITER_HPP

#include "cxx/platform.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "cse/string.hpp"

#include "ext/listset.hpp"
#include "ext/map.hpp"
#include "ext/vector.hpp"

#include "ios/printwriter.hpp"

#include "xml/linkage.hpp"
#include "xml/writerex.hpp"

namespace xml {

class XML_DECLARE TextWriter :
    public ElementWriterEx
{
  private:
    _S<ios::FormatWriter> out_;
    cse::String indent_;
    unsigned prefix_;

    bool prettyprint_;

    typedef ext::ListMap<cse::String, cse::String> PrefixMap_;

    struct Tag_ {
        cse::String qname_;
        PrefixMap_ uri2fix_;
        PrefixMap_ fix2uri_;

        bool hastext_;
        bool isopen_;

        unsigned children_;
        bool wastrivial_;

        Arguments arguments_;
        Attributes attributes_;

        Tag_() :
            hastext_(false),
            isopen_(true),
            children_(0),
            wastrivial_(true)
        {
        }
    };

    bool black_;
    cse::String whitespace_;

    typedef ext::Vector<Tag_> TagList_;
    TagList_ tags_;

    void Indent(size_t start = 2);
    cse::String GetPrefix(const cse::String &uri, Tag_ &tag);

    void EndChild_(bool child, size_t distance);
    void FinishTag_(bool child);
    void StartChild_();
    void ResetBlackWhite_();

    virtual void EOutputText_(const cse::String &text);
    virtual void EOutputComment_(const cse::String &comment);

    virtual void EOpenElement_(const Name &name, const Arguments &arguments, const Attributes &attributes);
    virtual void ECloseElement_(const Name &name);

  public:
    TextWriter(ios::Writer &out, bool prettyprint = true);
    ~TextWriter();
};

}

#endif//MENES_TEXTWRITER_HPP
