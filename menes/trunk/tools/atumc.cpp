/* Ninetjer - _Truly_ Free (non-GPL) Unix .NET Runtime
 * Copyright (C) 2005  Jay Freeman (saurik)
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

#include "app/simple.hpp"

#include "ios/helpers.hpp"
#include "ios/pushback.hpp"

struct Expression :
    public ext::ReferenceCounted
{
    virtual void Output(ios::PrintWriter &writer) const = 0;
};

typedef _L< _R<Expression> > Expressions;

struct Value :
    public Expression
{
    cse::String value_;

    Value(const cse::String &value) :
        value_(value)
    {
    }
};

struct Symbol :
    public Value
{
    Symbol(const cse::String &value) :
        Value(value)
    {
    }

    virtual void Output(ios::PrintWriter &writer) const {
        writer << value_;
    }
};

struct String :
    public Value
{
    String(const cse::String &value) :
        Value(value)
    {
    }

    virtual void Output(ios::PrintWriter &writer) const {
        writer << "`" << value_ << "'";
    }
};

struct Sequence :
    public Expression
{
    _R<Expression> lhs_;
    _R<Expression> rhs_;

    Sequence(const _R<Expression> &lhs, const _R<Expression> &rhs) :
        lhs_(lhs),
        rhs_(rhs)
    {
    }

    virtual void Output(ios::PrintWriter &writer) const {
        if (!lhs_.IsEmpty())
            lhs_->Output(writer);
        writer << ";" << ios::NewLine;
        if (!rhs_.IsEmpty())
            rhs_->Output(writer);
    }
};

struct Block :
    public Expression
{
    _R<Expression> expression_;

    Block(const _R<Expression> &expression) :
        expression_(expression)
    {
    }

    virtual void Output(ios::PrintWriter &writer) const {
        writer << "{";
        expression_->Output(writer);
        writer << "}";
    }
};

struct List :
    public Expression,
    public Expressions
{
    virtual void Output(ios::PrintWriter &writer) const {
    }
};

struct Call :
    public Expression
{
    Expressions arguments_;

    virtual void Output(ios::PrintWriter &writer) const {
        _foreach (const _L< _R<Expression> >, argument, arguments_) {
            if (argument != arguments_.Begin())
                writer << " ";
            (*argument)->Output(writer);
        }
    }
};

struct ParseException :
    public ext::Exception
{
  private:
    bool expected_;
    cse::String token_;

  public:
    ParseException(bool expected, const cse::String &token) :
        expected_(expected),
        token_(token)
    {
    }

    virtual cse::String GetMessage() const {
        return _S<ios::String>() << (expected_ ? _B("Expected: ") : _B("Spurious: ")) << token_;
    }
};

cse::String GetToken(ios::PushBackReader &reader) {
    static cse::String specials(",;{}()[]`'");

    _S<ios::Buffer> token;
    _forever {
        byte_t next;

        if (!reader.Get(next))
            next = '\0';

        if (next == '\0') {
            reader.PushBack('\0');
            return token;
        } else if (cse::IsWhiteSpace::Execute(next)) {
            if (!token.IsEmpty())
                return token;
        } else if (ext::Contains(specials, next))
            if (token.IsEmpty())
                return cse::CodePoint(next);
            else {
                reader.PushBack(next);
                return token;
            }
        else
            token.InsertLast(next);
    }
}

_R<Expression> Parse_(ios::PushBackReader &reader, const cse::CodePoint *until) {
    _R<List> list;

    _forever {
        cse::String token(GetToken(reader));

        if (token.IsEmpty())
            if (until)
                throw ParseException(true, *until);
            else
                return list;
        else if (until && token == *until)
            return list;
        else if (token == ";")
            return new _H<Sequence>(list, Parse_(reader, until));
        else if (token == ",") {
            list->InsertLast();
        } else {
            _R<Expression> rhs;
            if (token == "{")
                rhs = new _H<Block>(Parse_(reader, &_unconst(cse::CodePoint('}'))));
            else if (token == "(")
                rhs = Parse_(reader, &_unconst(cse::CodePoint(')')));
            else if (token == "`")
                rhs = new _H<String>(ios::ReadUntil(reader, '\''));
            else
                rhs = new _H<Symbol>(token);

            /*if (Call *call = dynamic_cast<Call *>(list->Last().GetValue()))
                call->expressions_.InsertLast(rhs);
            else {
            }*/
        }
    }
}

_R<Expression> Parse(ios::Reader &reader) {
    _S<ios::PushBackReader> pushback(&reader);
    _R<Expression> expression(Parse_(pushback, NULL));
    _assert(ios::Get(pushback) == '\0');
    return expression;
}

int Main(const app::Options &options) {
    _R<Expression> code(Parse(api::Cin));
    code->Output(api::Cout);
    return 0;
}
