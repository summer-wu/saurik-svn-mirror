/* Ninetjer - _Truly_ Free (non-GPL) Unix .NET Runtime
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

#include "cxx/standard.hh"
#pragma hdrstop("build/cxx/standard.pch")

#include "api/memorymap.hpp"
#include "app/command.hpp"

#include "ext/foreach.hpp"
#include "ext/linear.hpp"

#include "xml/buffer.hpp"
#include "xml/document.hpp"
#include "xml/nodeset.hpp"
#include "xml/parse.hpp"
#include "xml/resolver.hpp"
#include "xml/textwriter.hpp"

app::SimpleCommand<> main_;

static xml::Name bnf(const cse::String &local) {
    return xml::Name(local, _B("http://www.saurik.com/menes/bnf/0.9"));
}

// XXX: this is very very stupid
typedef cse::String::Iterator Iterator;

struct GlobalState {
    Iterator begin_;
    Iterator end_;

    Iterator farthest_;

    GlobalState(Iterator begin, Iterator end) :
        begin_(begin),
        end_(end),
        farthest_(begin)
    {
    }
};

struct Position;
struct Production;

struct LocalState {
    mutable unsigned depth_;
    mutable bool lambda_;

    bool debug_;
    bool reversed_;

    Iterator begin_;

    _S<xml::Buffer> buffer_;
    _L<const Production *> prefix_;

    LocalState(Iterator begin) :
        depth_(0),
        lambda_(false),
        debug_(true),
        reversed_(false),
        begin_(begin)
    {
    }

    bool AtEnd(const GlobalState &global) const {
        return reversed_ ? begin_ == global.begin_ : begin_ == global.end_;
    }

    cse::CodePoint MoveNext() {
        return reversed_ ? *(--begin_) : *(begin_++);
    }

    cse::CodePoint GetNext() const {
        return reversed_ ? *(begin_ - 1) : *(begin_);
    }
};

//typedef _L< LocalState, ext::Block< LocalState, ext::TypedAllocator<ext::IndirectAllocator> > > LocalStates;
typedef _L<LocalState> LocalStates;

namespace {
    bool debug_(false);

    void Indent_(const LocalState &local, unsigned offset = 0) {
        _forall (unsigned, i, 0, local.depth_ + offset)
            api::Cout << "  ";
    }
}

void Results_(LocalStates &results) {
    _foreach (LocalStates, result, results) {
        api::Cout << " " << static_cast<void *>(result->begin_.GetDataIterator()) << "{" << result->prefix_.GetSize();
        _rforeach (_L<const Production *>, prefix, result->prefix_)
            api::Cout << ":" << *prefix;
            api::Cout << "}";
        }

    api::Cout << ios::NewLine;
}

struct Token :
    public ext::ReferenceCounted
{
    bool ignore_;

    enum Lambda {
        Required,
        Ignorant,
        Rejected
    };

    Lambda lambda_;

    void Parse(GlobalState &global, const LocalState &local, LocalStates &nexts) const {
        if (global.farthest_ < local.begin_)
            global.farthest_ = local.begin_;

        bool prelambda(local.lambda_);
        if (lambda_ == Required)
            local.lambda_ = true;

        if (debug_ && local.debug_) {
            ++local.depth_;
            Indent_(local);
            api::Cout << typeid(*this).name() << "::Parse(): " << static_cast<void *>(local.begin_.GetDataIterator()) << "" << ios::NewLine;
        }

        LocalStates results;
        Parse_(global, local, results);

        if (debug_ && local.debug_) {
            Indent_(local);
            api::Cout << "Results<t>: " << results.GetSize();
            Results_(results);
        }

        {
            OPT_TIMER("bnf::Token::Parse_()[Lambda]")

            if (lambda_ == Required)
                _foreach (LocalStates, result, results)
                    result->lambda_ = prelambda;
            else if (lambda_ == Rejected) {
                LocalStates unlambdas;
                ext::Swap(results, unlambdas);

                _foreach (LocalStates, unlambda, unlambdas)
                    if (unlambda->begin_ != local.begin_)
                        results.InsertLast(*unlambda);
                    else if (debug_ && local.debug_) {
                        Indent_(local);
                        api::Cout << "Unlambda Prune!" << ios::NewLine;
                    }

                if (debug_ && local.debug_) {
                    Indent_(local);
                    api::Cout << "Results: " << results.GetSize() << ios::NewLine;
                }
            }
        }

        {
            OPT_TIMER("bnf::Token::Parse_()[PostFixup]")

            if (debug_ && local.debug_)
                _foreach (LocalStates, result, results)
                    --result->depth_;

            if (ignore_)
                _foreach (LocalStates, result, results)
                    result->buffer_ = local.buffer_;
        }

        {
            OPT_TIMER("bnf::Token::Parse_()[Insert]")
            nexts.InsertAllLast(results);
        }
    }

    virtual void Parse_(GlobalState &global, const LocalState &local, LocalStates &nexts) const = 0;

    void Success(LocalState &next, const LocalState &local, LocalStates &nexts) const {
        if (!local.lambda_ || next.begin_ == local.begin_)
            nexts.InsertLast(next);
        else if (debug_ && local.debug_) {
            Indent_(local);
            api::Cout << "Lambda Prune!" << ios::NewLine;
        }
    }
};

typedef _L< _R<Token> > Tokens;

struct Rule;

struct Production {
    Rule *rule_;
    Tokens tokens_;

    void Parse(GlobalState &global, const LocalState &local, LocalStates &nexts) const;
};

struct Rule {
    enum Mode {
        AddPrefix,
        BackTracking,
        IgnoreData,
        NegativeAssert,
        PathBranching,
        PositiveAssert,
        ReverseInput,
        ShortCircuit,
        UsePrefix
    };

    cse::String name_;
    bool skipdebug_;

    Mode mode_;
    _L<Production> productions_;

    mutable opt::Performance performance_;

    void Parse(GlobalState &global, const LocalState &local, LocalStates &nexts) const {
        opt::ScopeTimer timer(performance_);

        LocalState copy(local);

        if (debug_ && copy.debug_) {
            copy.buffer_.OutputText(_S<ios::String>() << "{" << name_ << "}");
            Indent_(copy);
            api::Cout << typeid(*this).name() << "::Parse(): " << name_ << ios::NewLine;
        }

        {
            OPT_TIMER("bnf::Rule::Parse()[PreFixup]")

            if (skipdebug_)
                copy.debug_ = false;

            if (mode_ == ReverseInput)
                copy.reversed_ = !copy.reversed_;
            else if (mode_ == UsePrefix) {
                if (debug_ && copy.debug_) {
                    Indent_(copy);
                    api::Cout << "UsePrefix!" << ios::NewLine;
                }

                copy.prefix_.Clear();
            } else if (mode_ == AddPrefix) {
                if (debug_ && copy.debug_) {
                    Indent_(copy);
                    api::Cout << "AddPrefix!" << ios::NewLine;
                }

                _assert(productions_.GetSize() == 1);
                copy.prefix_.InsertLast(&productions_.First());
                nexts.InsertLast(copy);
                return;
            }

            copy.buffer_.Clear();
        }

        if (debug_ && copy.debug_) {
            Indent_(copy);
            api::Cout << "Productions: " << productions_.GetSize() << ios::NewLine;
        }

        LocalStates results;

        {
            LocalStates result;

            _foreach (const _L<Production>, production, productions_) {
                {
                    OPT_TIMER("bnf::Rule::Parse()[Clear]")
                    result.Clear();
                }

                production->Parse(global, copy, result);

                {
                    OPT_TIMER("bnf::Rule::Parse()[Append]")

                    if (mode_ != NegativeAssert) {
                        results.InsertAllLast(result);
                        if (mode_ == ShortCircuit && !result.IsEmpty())
                            break;
                    } else if (result.IsEmpty())
                        results.InsertLast(copy);
                }
            }
        }

        if (mode_ == UsePrefix) {
            OPT_TIMER("bnf::Rule::Parse()[UsePrefix]")

            if (debug_ && copy.debug_) {
                Indent_(copy);
                api::Cout << "Using Prefix..." << ios::NewLine;
            }

            LocalStates befores;
            ext::Swap(results, befores);

            _foreach (LocalStates, before, befores) {
                LocalStates afters;
                afters.InsertLast(copy);

                _rforeach (_L<const Production *>, prefix, before->prefix_) {
                    if (debug_ && copy.debug_) {
                        Indent_(copy);
                        api::Cout << "Prefix!" << ios::NewLine;
                    }

                    LocalStates fixes;
                    ext::Swap(fixes, afters);

                    _foreach (LocalStates, fix, fixes)
                        (*prefix)->Parse(global, *fix, afters);
                }

                _foreach (LocalStates, after, afters) {
                    after->begin_ = before->begin_;
                    after->prefix_ = local.prefix_;
                    after->buffer_.Append(before->buffer_);
                }

                results.InsertAllLast(afters);
            }
        }

        if (debug_ && copy.debug_) {
            Indent_(copy);
            api::Cout << "Results<m>: " << results.GetSize();
            Results_(results);
        }


        {
            OPT_TIMER("bnf::Rule::Parse()[Merge]")

            LocalStates merges;
            ext::Swap(results, merges);

            typedef ext::RedBlackMap< Iterator, _L<const LocalState *> > Matches;
            Matches matches;

            _foreach (LocalStates, merge, merges)
                if (
                    merge->prefix_.IsEmpty() &&
                    merge->buffer_.IsWellFormed()
                ) {
                    OPT_TIMER("bnf::Rule::Parse()[Merge/Insert]")
                    matches[merge->begin_].InsertLast(&*merge);
                } else {
                    OPT_TIMER("bnf::Rule::Parse()[Merge/Not]")

                    LocalState merged(local);
                    merged.begin_ = merge->begin_;
                    merged.prefix_ = merge->prefix_;
                    merged.buffer_.Append(merge->buffer_);
                    results.InsertLast(merged);
                }

            if (debug_ && !matches.IsEmpty())
                api::Cout << "attempting merge at: " << name_ << ios::NewLine;

            _foreach (Matches, match, matches)
                if (match->Second().GetSize() == 1) {
                    OPT_TIMER("bnf::Rule::Parse()[Merge/Short]")

                    const LocalState &state(*match->Second().First());
                    LocalState merged(local);
                    merged.begin_ = state.begin_;
                    merged.buffer_.Append(state.buffer_);
                    results.InsertLast(merged);
                } else {
                    OPT_TIMER("bnf::Rule::Parse()[Merge/Long]")

                    if (debug_)
                        api::Cout << "performing merge at: " << name_ << ios::NewLine;

                    LocalState merged(local);
                    merged.begin_ = match->First();

                    merged.buffer_.OpenElement(bnf("or"));

                    _foreach (_L<const LocalState *>, state, match->Second()) {
                        if (debug_) {
                            api::Cout << "  fragment:" << ios::NewLine;
                            _S<xml::TextWriter> writer(api::Cout.GetStream());
                            (*state)->buffer_.WriteTo(writer);
                            api::Cout << ios::NewLine;
                        }

                        merged.buffer_.OpenElement(bnf("case"));
                        merged.buffer_.Append((*state)->buffer_);
                        merged.buffer_.CloseElement();
                    }

                    merged.buffer_.CloseElement();
                    results.InsertLast(merged);
                }
        }

        if (debug_ && copy.debug_) {
            Indent_(copy);
            api::Cout << "Results<f>: " << results.GetSize();
            Results_(results);
        }

        {
            OPT_TIMER("bnf::Rule::Parse()[PostFixup]")

            _foreach (LocalStates, result, results) {
                result->debug_ = local.debug_;

                if (mode_ == PositiveAssert)
                    result->begin_ = local.begin_;
                else if (mode_ == ReverseInput)
                    result->reversed_ = !result->reversed_;
                else if (mode_ == IgnoreData) {
                    if (debug_ && copy.debug_) {
                        Indent_(copy);
                        api::Cout << "IgnoreData!" << ios::NewLine;
                    }

                    result->prefix_ = local.prefix_;
                    result->buffer_ = local.buffer_;
                }
            }
        }

        if (debug_ && copy.debug_) {
            Indent_(copy);
            api::Cout << "Results<r>: " << results.GetSize();
            Results_(results);
        }

        {
            OPT_TIMER("bnf::Rule::Parse()[Insert]")
            nexts.InsertAllLast(results);
        }
    }
};

void Production::Parse(GlobalState &global, const LocalState &local, LocalStates &nexts) const {
    if (debug_ && local.debug_) {
        Indent_(local);
        api::Cout << typeid(*this).name() << "::Parse()" << ios::NewLine;
        Indent_(local);
        api::Cout << "From: " << rule_->name_ << ios::NewLine;
    }

    {
        OPT_TIMER("bnf::Production::Parse()[Setup]")
        nexts.InsertLast(local);
    }

    LocalStates inputs;

    _foreach (const Tokens, token, tokens_) {
        {
            OPT_TIMER("bnf::Production::Parse()[Swap]")
            ext::Swap(nexts, inputs);
        }

        {
            OPT_TIMER("bnf::Production::Parse()[Clear]")
            nexts.Clear();
        }

        _foreach (LocalStates, input, inputs)
            (*token)->Parse(global, *input, nexts);
    }
}

struct Position {
    const Production *production_;
    Tokens::ConstIterator token_;

    explicit Position(const Production &production) :
        production_(&production),
        token_(production.tokens_.Begin())
    {
    }
};

typedef ext::RedBlackMap<cse::String, Rule> Rules;

struct Call :
    public Token
{
    Rule *rule_;

    virtual void Parse_(GlobalState &global, const LocalState &local, LocalStates &nexts) const {
        rule_->Parse(global, local, nexts);
    }
};

struct String :
    public Token
{
    cse::String value_;

    virtual void Parse_(GlobalState &global, const LocalState &local, LocalStates &nexts) const {
        OPT_TIMER("bnf::String::Parse_()")

        if (debug_ && local.debug_) {
            Indent_(local, 1);

            Iterator begin(local.begin_);
            Iterator end(local.begin_);

            _forall (size_t, i, 0, value_.GetSize())
                if (local.reversed_)
                    if (begin == global.begin_)
                        break;
                    else
                        --begin;
                else
                    if (end == global.end_)
                        break;
                    else
                        ++end;

            api::Cout << "\"" << value_ << "\" =? \"" << cse::String(begin, end) << "\"" << ios::NewLine;
        }

        LocalState next(local);

        cse::String::ConstIterator match(value_.Begin());

        scan: if (match == value_.End()) {
            next.buffer_.OutputText(value_);
            Success(next, local, nexts);
        } else if (!next.AtEnd(global) && *(match++) == next.MoveNext())
            goto scan;
    }
};

struct Range :
    public Token
{
    cse::CodePoint begin_;
    cse::CodePoint end_;

    virtual void Parse_(GlobalState &global, const LocalState &local, LocalStates &nexts) const {
        OPT_TIMER("bnf::Range::Parse_()")

        LocalState next(local);

        if (
            !next.AtEnd(global) &&
            next.GetNext() >= begin_ &&
            next.GetNext() <= end_
        ) {
            next.buffer_.OutputText(cse::String(next.MoveNext()));
            Success(next, local, nexts);
        }
    }
};

struct Open :
    public Token
{
    xml::Name name_;

    virtual void Parse_(GlobalState &global, const LocalState &local, LocalStates &nexts) const {
        OPT_TIMER("bnf::Open::Parse_()")

        if (debug_) {
            Indent_(local);
            api::Cout << "Open: <" << name_.GetLocal() << ">" << ios::NewLine;
        }

        nexts.InsertLast(local);
        nexts.Last().buffer_.OpenElement(name_);
    }
};

struct Close :
    public Token
{
    xml::Name name_;

    virtual void Parse_(GlobalState &global, const LocalState &local, LocalStates &nexts) const {
        OPT_TIMER("bnf::Close::Parse_()")

        if (debug_) {
            Indent_(local);
            api::Cout << "Close: </" << name_.GetLocal() << ">" << ios::NewLine;
        }

        nexts.InsertLast(local);
        nexts.Last().buffer_.CloseElement();
    }
};

struct Attribute :
    public Token
{
    xml::Name name_;
    cse::String value_;

    virtual void Parse_(GlobalState &global, const LocalState &local, LocalStates &nexts) const {
        OPT_TIMER("bnf::Attribute::Parse_()")

        nexts.InsertLast(local);

        xml::Buffer &buffer(nexts.Last().buffer_);
        buffer.OpenAttribute(name_);
        buffer.OutputText(value_);
        buffer.CloseAttribute();
    }
};

struct Special :
    public Token
{
    enum Type {
        Letter,
        Bof,
        Eof,
        Any
    };

    Type type_;

    virtual void Parse_(GlobalState &global, const LocalState &local, LocalStates &nexts) const {
        OPT_TIMER("bnf::Special::Parse_()")

        switch (type_) {
            case Letter:
                if (
                    !local.AtEnd(global) &&
                    cse::IsAlphabetic::Execute(local.GetNext())
                ) {
                    LocalState next(local);
                    next.buffer_.OutputText(cse::String(next.MoveNext()));
                    Success(next, local, nexts);
                }
            break;

            case Bof:
                if (local.begin_ == global.begin_)
                    nexts.InsertLast(local);
            break;

            case Eof:
                if (local.begin_ == global.end_)
                    nexts.InsertLast(local);
            break;

            case Any:
                if (!local.AtEnd(global)) {
                    LocalState next(local);
                    next.buffer_.OutputText(cse::String(next.MoveNext()));
                    Success(next, local, nexts);
                }
            break;

            default:
                throw;
        }
    }
};

class Parser {
  private:
    Rules rules_;
    const Rule *goal_;

  public:
    Parser(const cse::String &path) {
        _R<xml::Document> cbnf(xml::Parse(path));
        _R<xml::Node> grammar(*cbnf/bnf("grammar"));

        goal_ = &rules_[*grammar/"@goal"];

        _foreach (const xml::NodeSet, tag, *grammar/bnf("rule")) {
            cse::String name(**tag/"@name");

            Rule &rule(rules_[name]);

            if (rule.name_.IsEmpty()) {
                rule.name_ = name;

                rule.performance_.name_ = rule.name_.NullTerminate();
                rule.performance_.group_ = "bnf::Rule";
            }

            cse::String skipdebug(**tag/bnf("@skip-debug"));
            if (!skipdebug.IsEmpty() && lexical_cast<bool>(skipdebug))
                rule.skipdebug_ = true;
            else
                rule.skipdebug_ = false;

            cse::String mode(**tag/"@mode");
            if (mode == _B("add-prefix"))
                rule.mode_ = Rule::AddPrefix;
            else if (mode == _B("back-tracking"))
                rule.mode_ = Rule::BackTracking;
            else if (mode == _B("ignore-data"))
                rule.mode_ = Rule::IgnoreData;
            else if (mode == _B("negative-assert"))
                rule.mode_ = Rule::NegativeAssert;
            else if (mode == _B("path-branching") || mode.IsEmpty())
                rule.mode_ = Rule::PathBranching;
            else if (mode == _B("positive-assert"))
                rule.mode_ = Rule::PositiveAssert;
            else if (mode == _B("reverse-input"))
                rule.mode_ = Rule::ReverseInput;
            else if (mode == _B("short-circuit"))
                rule.mode_ = Rule::ShortCircuit;
            else if (mode == _B("use-prefix"))
                rule.mode_ = Rule::UsePrefix;
            else throw;

            _foreach (const xml::NodeSet, tag, **tag/bnf("production")) {
                rule.productions_.InsertLast();
                Production &production(rule.productions_.Last());
                production.rule_ = &rule;

                _foreach (const xml::NodeSet, tag, **tag/"node()") {
                    cse::String name((*tag)->GetName().GetLocal());
                    if ((*tag)->GetName() != bnf(name))
                        throw;
                    else if (name == _B("open")) {
                        _R<Open> open(new _H<Open>());
                        open->name_ = xml::Name(**tag/"@local", **tag/"@space");
                        production.tokens_.InsertLast(open);
                    } else if (name == _B("close")) {
                        _R<Close> close(new _H<Close>());
                        close->name_ = xml::Name(**tag/"@local", **tag/"@space");
                        production.tokens_.InsertLast(close);
                    } else if (name == _B("call")) {
                        _R<Call> call(new _H<Call>());
                        call->rule_ = &rules_[**tag/"@name"];
                        production.tokens_.InsertLast(call);
                    } else if (name == _B("range")) {
                        _R<Range> range(new _H<Range>());
                        cse::String begin(**tag/"@begin");
                        range->begin_ = begin.First();
                        cse::String end(**tag/"@end");
                        range->end_ = end.First();
                        production.tokens_.InsertLast(range);
                    } else if (name == _B("string")) {
                        _R<String> string(new _H<String>());
                        string->value_ = **tag/"@value";
                        production.tokens_.InsertLast(string);
                    } else if (name == _B("special")) {
                        _R<Special> special(new _H<Special>());

                        cse::String type(**tag/"@type");
                        if (type == _B("letter"))
                            special->type_ = Special::Letter;
                        else if (type == _B("bof"))
                            special->type_ = Special::Bof;
                        else if (type == _B("eof"))
                            special->type_ = Special::Eof;
                        else if (type == _B("any"))
                            special->type_ = Special::Any;
                        else throw;

                        production.tokens_.InsertLast(special);
                    } else if (name == _B("attribute")) {
                        _R<Attribute> attribute(new _H<Attribute>());
                        attribute->name_ = xml::Name(**tag/"@local", **tag/"@space");
                        attribute->value_ = **tag/"@value";
                        production.tokens_.InsertLast(attribute);
                    } else {
                        api::Cout << "unknown element: " << name << ios::NewLine;
                        throw;
                    }

                    cse::String ignore(**tag/bnf("@ignore"));
                    if (!ignore.IsEmpty() && lexical_cast<bool>(ignore))
                        production.tokens_.Last()->ignore_ = true;
                    else
                        production.tokens_.Last()->ignore_ = false;

                    production.tokens_.Last()->lambda_ = Token::Ignorant;

                    cse::String lambda(**tag/bnf("@lambda"));
                    if (!lambda.IsEmpty() && lexical_cast<bool>(lambda))
                        production.tokens_.Last()->lambda_ = Token::Required;

                    cse::String unlambda(**tag/bnf("@unlambda"));
                    if (!unlambda.IsEmpty() && lexical_cast<bool>(unlambda))
                        production.tokens_.Last()->lambda_ = Token::Rejected;
                }
            }
        }
    }

    ~Parser() {
        _assert(false);
    }

    _S<xml::Buffer> Parse(Iterator begin, Iterator end) {
        OPT_TIMER("bnf::Parser::Parse()")

        GlobalState global(begin, end);
        LocalState local(begin);

        LocalStates locals;
        goal_->Parse(global, local, locals);

        if (debug_)
            api::Cout << static_cast<const void *>(begin.GetDataIterator()) << " " << static_cast<const void *>(end.GetDataIterator()) << ios::NewLine;

        unsigned count(0);
        _foreach (LocalStates, local, locals)
            if (local->begin_ == end)
                ++count;

        if (count > 1)
            api::Cerr << "warning: " << count << " possible parses" << ios::NewLine;
        else if (count == 0)
            api::Cerr << "parse failure (maybe at byte " << (global.farthest_.GetDataIterator() - begin.GetDataIterator()) << "?)" << ios::NewLine;

        _foreach (LocalStates, local, locals) {
            if (debug_)
                api::Cout << static_cast<const void *>(local->begin_.GetDataIterator()) << " " << static_cast<const void *>(end.GetDataIterator()) << ios::NewLine;
            if (local->begin_ == end) {
                if (!debug_)
                    return local->buffer_;
                else {
                    _S<xml::TextWriter> writer(api::Cout.GetStream());
                    local->buffer_.WriteTo(writer);
                }
            }
        }

        return _S<xml::Buffer>();
    }
};

namespace app {
template <>
int SimpleCommand<>::Execute(const app::Options &options) {
    if (options["debug"] != NULL)
        debug_ = true;

    Parser &parser(*new Parser(*options[0]));
    api::MemoryMap file(*options[1]);

    _S<xml::Buffer> buffer(parser.Parse(file.Begin(), file.End()));

    _S<xml::TextWriter> writer(api::Cout.GetStream(), options["pretty"] != NULL);
    //_S<xml::Resolver> resolver(writer);

    buffer.WriteTo(writer);

    return 0;
} }
