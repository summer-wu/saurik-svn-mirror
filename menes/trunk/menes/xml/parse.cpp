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

#include "cxx/standard.hh"
#pragma hdrstop("build/cxx/standard.pch")

#include "api/files.hpp"

#include "cse/digits.hpp"
#include "cse/string.hpp"
#include "cse/traits.hpp"

#include "ext/listset.hpp"
#include "ext/map.hpp"
#include "ext/traits.hpp"

#include "ios/buffer.hpp"
#include "ios/filter.hpp"
#include "ios/streambuf.hpp"
#include "ios/printwriter.hpp"

#include "xml/builder.hpp"
#include "xml/document.hpp"
#include "xml/domwriter.hpp"
#include "xml/element.hpp"
#include "xml/instruction.hpp"
#include "xml/nodeset.hpp"
#include "xml/parse.hpp"
#include "xml/resolver.hpp"
#include "xml/text.hpp"
#include "xml/writer.hpp"
#include "xml/xmlencode.hpp"

namespace xml {

XML_DECLARE _R<Document> Parse(const cse::String &filename, const Settings &settings) {
    _S<api::FileReader> fin(filename);
    _S<ios::ReaderBuffer> buffer(&fin);
    return Parse(buffer, settings);
}

XML_DECLARE _R<Document> Parse(ios::Reader &in, const Settings &settings) {
    _R<Document> document(new _H<Document>());

    xml::Builder builder;
    _S<xml::DomWriter> writer(builder, document);
    _S<xml::Resolver> resolver(&writer);

    xml::Parse(in, resolver, settings);
    return document;
}

namespace {

_finline void SyncWhite_(ios::Reader &in, byte_t &next) {
    while (xml::IsWhiteSpace::Execute(next = ios::Get(in)));
}

template <bool SkipLeadingWhiteSpace, bool FailOnWhiteSpace>
_finline ext::Buffer Until_(ios::Reader &in, const char *chk) {
    OPT_TIMER("xml::Until_()")

    ext::Buffer data;
    size_t length(strlen(chk));
    const char *done(chk);

    bool read(false);
    bool failed(false);

    _forever {
        byte_t next;

        {
            OPT_TIMER("xml::Until_()[Get]")
            next = ios::Get(in);
        }

        if (SkipLeadingWhiteSpace && !read && xml::IsWhiteSpace::Execute(next));
        else if (FailOnWhiteSpace && xml::IsWhiteSpace::Execute(next))
            failed = true;
        else if (next == *done) {
            if (++done == chk + length)
                return data;
        } else if (FailOnWhiteSpace && failed)
            throw;
        else {
            OPT_TIMER("xml::Until_()[Insert]")
            data.InsertAllLast(chk, done - chk);
            data.InsertLast(next);
            done = chk;
        }
    }
}

typedef ext::Vector<cse::String> PrefixStack_;

cse::String ExpandEntities_(const cse::String &input, const Settings &settings) {
    OPT_TIMER("xml::ExpandEntities_()")
    return ios::Filter<XmlDecoder>(input, settings);
}

struct Attribute_ {
    cse::String qname_;
    _R<Node> value_;
};

bool Match_(const cse::String &lhs, const cse::String &rhs) {
    return cse::ToLower(lhs) == cse::ToLower(rhs);
}

void Parse_(ios::Reader &in, bool inner, Writer &out, PrefixStack_ &stack, const Settings &settings) {
    do {
        char next(0);

        if (inner)
            next = ios::Get(in);
        else {
            _S<ios::Buffer> text;
            bool space(true);

            _forever {
                enum {
                    opened,
                    finished,
                    neither
                } state(neither);

                if (!in.Get(next))
                    state = finished;

                if (state != finished && next == '<')
                    if (!in.Get(next)) {
                        state = finished;
                        text.InsertLast('<');
                        space = false;
                    } else {
                        state = opened;
                    }

                // XXX: maybe add better whitespace control?
                if (state != neither && (settings.keepwhite_ || !space)) {
                    out.OutputText(ExpandEntities_(text, settings));
                    text.Clear();
                }

                if (state == finished)
                    return;

                if (state == opened)
                    break;

                text.InsertLast(next);
                if (space && !xml::IsWhiteSpace::Execute(next))
                    space = false;
            }
        }

        switch (next) {
            case '/': {
                cse::String qname(ExpandEntities_(Until_<false, true>(in, ">"), settings));

                if (!qname.IsEmpty() && !Match_(qname, stack.Last()))
                    throw;

                out.CloseElement();
                stack.RemoveLast();
            } return;

            case '?': {
                cse::String instruction(ExpandEntities_(Until_<false, false>(in, "?>"), settings));

                cse::String::Iterator space(ext::FindFirst(instruction, ' '));
                cse::String target, value;

                if (space == instruction.End())
                    target = instruction;
                else {
                    target = cse::String(instruction.Begin(), space);
                    value = cse::String(space + 1, instruction.End());
                }

                if (target != "xml")
                    out.OutputPi(target, value);
                else {
                    // XXX: implement xml declaration
                }
            } break;

            case '!':
                switch (ios::Get(in)) {
                    case '[': {
                        ext::Buffer keyword(Until_<true, true>(in, "["));
                        ext::Buffer data(Until_<false, false>(in, "]]>"));

                        if (keyword == _B("CDATA"))
                            out.OutputText(data);
                        else if (keyword == _B("RCDATA"))
                            out.OutputText(ExpandEntities_(data, settings));
                        else if (keyword == _B("INCLUDE"))
                            // XXX: implement
                            throw;
                        else if (keyword != _B("IGNORE"))
                            throw;
                    } break;

                    case '-':
                        if (ios::Get(in) != '-')
                            throw;
                        out.OutputComment(Until_<false, false>(in, "-->"));
                    break;

                    case 'D':
                        if (ios::Get(in) != 'O' ||
                            ios::Get(in) != 'C' ||
                            ios::Get(in) != 'T' ||
                            ios::Get(in) != 'Y' ||
                            ios::Get(in) != 'P' ||
                            ios::Get(in) != 'E')
                                throw;
                        // XXX: do something with doctype?
                        Until_<false, false>(in, ">");
                    break;

                    default:
                        throw;
                }
            break;

            default:
                _S<ios::Buffer> data;

                while (!xml::IsWhiteSpace::Execute(next) && next != '/' && next != '>') {
                    data.InsertLast(next);
                    next = ios::Get(in);
                }

                {
                    cse::String qname(ExpandEntities_(data, settings));
                    stack.InsertLast(qname);
                    out.OpenElement(qname);
                }

                typedef ext::Vector<Attribute_> Attributes_;
                Attributes_ attributes;

                _forever {
                    while (xml::IsWhiteSpace::Execute(next))
                        next = ios::Get(in);

                    cse::String qname;

                    if (next == '>' || next == '/')
                        break;
                    else if (next != '\'' && next != '\"' && next != '<') {
                        qname.InsertLast(next);
                        qname.InsertAllLast(Until_<false, true>(in, "="));
                        qname = ExpandEntities_(qname, settings);

                        SyncWhite_(in, next);
                    }

                    out.OpenAttribute(qname);

                    switch (next) {
                        case '\'':
                        case '\"': {
                            char chks[2] = { next, '\0' };
                            out.OutputText(ExpandEntities_(Until_<false, false>(in, chks), settings));
                            next = ios::Get(in);
                        } break;

                        case '<':
                            Parse_(in, true, out, stack, settings);
                            next = ios::Get(in);
                        break;

                        default: {
                            ext::Buffer value;

                            do {
                                value.InsertLast(next);
                                next = ios::Get(in);
                            } while (
                                // XXX: this needs work
                                next != '/' && next != '>' &&
                                !xml::IsWhiteSpace::Execute(next)
                            );

                            if (xml::IsWhiteSpace::Execute(next))
                                SyncWhite_(in, next);

                            out.OutputText(ExpandEntities_(value, settings));
                        } break;
                    }

                    out.CloseAttribute();
                }

                if (next != '/')
                    Parse_(in, false, out, stack, settings);
                else {
                    if (ios::Get(in) != '>')
                        throw;
                    out.CloseElement();
                    stack.RemoveLast();
                }
            break;
        }
    } while (!inner);
}

}

XML_DECLARE void Parse(ios::Reader &in, Writer &out, const Settings &settings) {
    OPT_TIMER("xml::Parse()")

    PrefixStack_ stack(1);
    Parse_(in, false, out, stack, settings);
    _assert(stack.GetSize() == 1);
}

}
