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

#include "cxx/standard.hh"
#pragma hdrstop("build/cxx/standard.pch")

#include "cse/digits.hpp"
#include "cse/lexical.hpp"

#include "ext/foreach.hpp"

#include "xml/name.hpp"
#include "xml/textwriter.hpp"

namespace xml {

namespace {
cse::String Escape_(const cse::String &text, bool attribute) {
    OPT_TIMER("xml::Escape_()")

    cse::String result;

    _foreach (const cse::String, c, text)
        if (*c < 0 || *c > 127 || *c < 32 && (attribute || *c != '\r' && *c != '\n')) {
            result.InsertAllLast("&#x");
            result.InsertAllLast(cse::ToDigits<16>(*c));
            result.InsertAllLast(";");
        } else if (attribute)
            switch (*c) {
                //case '\'': result.InsertAllLast("&apos;"); break;
                case '\"': result.InsertAllLast("&quot;"); break;
                default: goto otherwise;
            }
        else otherwise: switch (*c) {
            case '&': result.InsertAllLast("&amp;"); break;
            case '<': result.InsertAllLast("&lt;"); break;
            case '>': result.InsertAllLast("&gt;"); break;
            default: result.InsertLast(*c); break;
        }

    return result;
} }

void TextWriter::Indent(size_t start) {
    for (size_t i(start + 1); i < tags_.GetSize(); ++i)
        out_ << indent_;
}

cse::String TextWriter::GetPrefix(const cse::String &uri, Tag_ &tag) {
    if (uri == "http://www.w3.org/XML/1998/namespace")
        return "xmlns";

    cse::String prefix;

    _foreach (TagList_, tag1, tags_) {
        PrefixMap_::ConstIterator entry(tag1->uri2fix_.Find(uri));

        if (entry != tag1->uri2fix_.End()) {
            prefix = entry->Second();

            bool good(true);
            for (TagList_::ConstIterator tag2(tags_.Begin()); tag2 != tag1; ++tag2)
                if (tag2->fix2uri_.Contains(prefix)) {
                    good = false;
                    break;
                }

            if (good)
                return prefix;
        }
    }

    if (prefix.IsEmpty())
        prefix = _S<ios::String>() << "p" << lexical_cast<cse::String>(prefix_++);

    tag.uri2fix_.Insert(uri, prefix);
    tag.fix2uri_.Insert(prefix, uri);

    return prefix;
}

void TextWriter::EndChild_(bool child, size_t distance) {
    // XXX: .At(- is a bad idea?
    if (Tag_ *parent = tags_.GetSize() < distance ? NULL : &tags_.At(-static_cast<int>(distance))) {
        if (parent->hastext_)
            parent->hastext_ = false;
        else if (prettyprint_) {
            //std::cout << parent->children_ << std::endl;
            if (!parent->wastrivial_ || child && parent->children_ != 1)
                out_ << ios::NewLine;
            out_ << ios::NewLine;
            Indent(distance);
        }
    }
}

void TextWriter::FinishTag_(bool child) {
    OPT_TIMER("xml::TextWriter::FinishTag_()")

    Tag_ &tag(tags_.Last());

    EndChild_(child, 2);
    out_ << "<" << tag.qname_;

    typedef etl::Pair< cse::String, _R<Node> > NameValue;
    typedef ext::Vector<NameValue> NameValues;
    NameValues attributes;

    _foreach (Attributes, attribute, tag.attributes_) {
        cse::String prefix(GetPrefix(attribute->First().GetSpace(), tag));
        const cse::String &local(attribute->First().GetLocal());

        // ERR: error: operands to ?: have different types
        cse::String name(prefix.IsEmpty() ? local : cse::String(_S<ios::String>() << prefix << ":" << local));

        attributes.InsertLast(NameValue(name, attribute->Second()));
    }

    _foreach (PrefixMap_, fix, tag.fix2uri_) {
        if (fix->First().IsEmpty())
            out_ << " xmlns";
        else
            out_ << " xmlns:" << fix->First();

        out_ << "=\"" << Escape_(fix->Second(), true) << "\"";
    }

    _foreach (Arguments, argument, tag.arguments_)
        out_ << " ...";

    _foreach (NameValues, attribute, attributes) {
        out_ << " " << attribute->First() << "=";

        if (Text *text = dynamic_cast<Text *>(attribute->Second().GetValue()))
            out_ << "\"" << Escape_(text->GetNodeValue(), true) << "\"";
        else
            out_ << "...";
    }
}

void TextWriter::StartChild_() {
    OPT_TIMER("xml::TextWriter::StartChild_()")

    ResetBlackWhite_();

    if (tags_.Last().isopen_) {
        FinishTag_(true);
        out_ << ">";
        tags_.Last().isopen_ = false;
        if (prettyprint_ && tags_.GetSize() == 2)
            out_ << ios::NewLine;
    }

    ++tags_.Last().children_;
}

void TextWriter::ResetBlackWhite_() {
    whitespace_.Clear();
    black_ = false;
}

void TextWriter::EOutputText_(const cse::String &text) {
    OPT_TIMER("xml::TextWriter::EOutputText_()")

    if (!black_) {
        _foreach (const cse::String, i, text)
            if (!cse::IsLinearWhiteSpace::Execute(*i))
                goto nonwhite;
        whitespace_.InsertAllLast(text);
    } else nonwhite: {
        Tag_ &tag(tags_.Last());
        if (tag.isopen_) {
            FinishTag_(false);
            out_ << ">";
            tag.isopen_ = false;
        }

        tag.hastext_ = true;

        if (!black_) {
            black_ = true;

            if (!whitespace_.IsEmpty()) {
                out_ << Escape_(whitespace_, false);
                whitespace_.Clear();
            }
        }

        out_ << Escape_(text, false);
    }
}

void TextWriter::EOutputComment_(const cse::String &comment) {
    OPT_TIMER("xml::TextWriter::EOutputComment_()")

    StartChild_();
    EndChild_(ext::Contains(comment, '\n'), 1);

    out_ << "<!--" << comment << "-->";
}

void TextWriter::EOpenElement_(const Name &name, const Arguments &arguments, const Attributes &attributes) {
    OPT_TIMER("xml::TextWriter::EOpenElement_()")

    StartChild_();

    tags_.InsertLast();
    Tag_ &tag(tags_.Last());

    cse::String prefix(GetPrefix(name.GetSpace(), tag));
    if (!prefix.IsEmpty())
        tag.qname_ = _S<ios::String>() << prefix << ":";

    tag.qname_.InsertAllLast(name.GetLocal());
    tag.arguments_ = arguments;
    tag.attributes_ = attributes;
}

void TextWriter::ECloseElement_(const Name &name) {
    OPT_TIMER("xml::TextWriter::ECloseElement_()")

    ResetBlackWhite_();

    Tag_ &tag(tags_.Last());

    if (tag.isopen_) {
        FinishTag_(false);
        out_ << "/>";
    } else {
        if (tag.hastext_) {
            tag.hastext_ = false;
        } else if (prettyprint_) {
            if (tags_.GetSize() == 2)
                out_ << ios::NewLine;
            out_ << ios::NewLine;
            Indent();
        }

        out_ << "</" << tag.qname_ << ">";
    }

    bool trivial(tag.isopen_ || tag.children_ == 0);
    tags_.RemoveLast();
    tags_.Last().wastrivial_ = trivial;
}

TextWriter::TextWriter(ios::Writer &out, bool prettyprint) :
    out_(out),
    indent_("    "),
    prefix_(0),
    prettyprint_(prettyprint),
    black_(false)
{
    out_ << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";

    tags_.InsertLast(Tag_());
    Tag_ &tag(tags_.Last());
    tag.isopen_ = false;

    tag.uri2fix_.Insert("", "");
    tag.fix2uri_.Insert("", "");
}

TextWriter::~TextWriter() {
    if (!std::uncaught_exception()) {
        //_assert(tags_.size() == 1);
        if (prettyprint_)
            out_ << ios::NewLine;
    }
}

}
