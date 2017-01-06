/* Menes - C++ High-Level Utility Library
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

#ifndef MENES_IOS_FORMATS_HPP
#define MENES_IOS_FORMATS_HPP

#include "cxx/platform.hpp"
#include "ios/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "etl/limits.hpp"
#include "etl/template.hpp"
#include "etl/traits.hpp"

#include "ext/refcount.hpp"
#include "ext/stackref.hpp"

#include "ios/writer.hpp"

namespace ios {

enum Alignment {
    Left,
    Right,
    Center
};

class IOS_DECLARE PrintWriter :
    public Writer
{
  private:
    ext::Buffer buffer_;
    unsigned buffered_;

    Alignment alignment_;

    unsigned base_;
    unsigned precision_;

    cse::CodePoint fill_;
    size_t width_;

    cse::String separator_;

    virtual void WriteFully_(const byte_t *data, size_t length) = 0;

  public:
    PrintWriter();

    virtual size_t Write(const byte_t *data, size_t length);
    virtual void Flush();

    void SetAlignment(Alignment alignment) {
        alignment_ = alignment;
    }

    Alignment GetAlignment() const {
        return alignment_;
    }

    void SetBase(unsigned base) {
        base_ = base;
    }

    unsigned GetBase() const {
        return base_;
    }

    void SetPrecision(unsigned precision) {
        precision_ = precision;
    }

    unsigned GetPrecision() const {
        return precision_;
    }

    void SetFill(cse::CodePoint fill) {
        fill_ = fill;
    }

    cse::CodePoint GetFill() const {
        return fill_;
    }

    void SetWidth(unsigned width) {
        width_ = width;
    }

    unsigned GetWidth() const {
        return width_;
    }

    void SetLineSeparator(const cse::String &separator_) {
        this->separator_ = separator_;
    }

    const cse::String &GetLineSeparator() const {
        return separator_;
    }

    void NewLine() {
        *this << separator_;
    }

    void Start() {
        ++buffered_;
    }

    void Finish();

    template <typename Encoding_, typename Buffer_>
    inline PrintWriter &operator <<(const cse::UnicodeString<Encoding_, Buffer_> &rhs) {
        cse::UnicodeString<cse::Utf8Encoding, Buffer_> string(rhs);
        WriteFully(string.GetData().Begin(), string.GetData().GetSize());
        return *this;
    }

    template <typename Buffer_>
    inline PrintWriter &operator <<(const cse::UnicodeString<cse::Utf8Encoding, Buffer_> &rhs) {
        WriteFully(rhs.GetData().Begin(), rhs.GetData().GetSize());
        return *this;
    }
};

class FormatWriter :
    public PrintWriter
{
  private:
    Writer &out_;

    virtual void WriteFully_(const byte_t *data, size_t length) {
        out_.WriteFully(data, length);
    }

  public:
    FormatWriter(Writer &out) :
        out_(out)
    {
    }

    virtual void Flush() {
        return out_.Flush();
    }

    Writer &GetStream() {
        return out_;
    }
};

class FormatOperation {
  public:
    virtual void operator()(PrintWriter &out) const = 0;
};

_finline PrintWriter &operator <<(PrintWriter &out, const FormatOperation &operation) {
    operation(out);
    return out;
}

_finline PrintWriter &operator <<(PrintWriter &out, const ext::Buffer &value) {
    out.WriteFully(value.Begin(), value.GetSize());
    return out;
}

inline PrintWriter &operator <<(PrintWriter &out, const char *value) {
    size_t length(0);
    while (value[length] != '\0')
        ++length;
    out.WriteFully(value, length);
    return out;
}

namespace be {
static const byte_t Numerals_[] = "0123456789abcdefghijklmnopqrstuvwxyz";

template <typename Type_, bool Signed_ = etl::Limits<Type_>::IsSigned>
struct WriteSign_ {
// XXX: technically this cannot print out the most negative number
_finline static void Execute(PrintWriter &out, Type_ &value) {
    if (value < 0) {
        value = -value;
        out << "-";
    }
} };

template <typename Type_>
struct WriteSign_<Type_, false> {
_finline static void Execute (PrintWriter &out, Type_ &value) {
} };

template <typename Type_>
void WriteInteger_(PrintWriter &out, Type_ value) {
    unsigned base(out.GetBase());
    out.Start();

    WriteSign_<Type_>::Execute(out, value);

    Type_ divisor(1);
    for (Type_ smaller(value / base); smaller >= divisor; divisor *= base);

    do {
        // XXX: byte_t is just so damned convenient
        Put(out, be::Numerals_[value / divisor]);
        value %= divisor;
        divisor /= base;
    } while (divisor != 0);

    out.Finish();
}

}

template <typename Type_>
typename etl::IfTrue<etl::IsIntegral<Type_>::value, PrintWriter &>::Result
operator <<(PrintWriter &out, Type_ value) {
    be::WriteInteger_(out, value);
    return out;
}

template <typename Type_>
typename etl::IfTrue<etl::IsEnum<Type_>::value, PrintWriter &>::Result
operator <<(PrintWriter &out, Type_ value) {
    return out << static_cast<unsigned>(value);
}

IOS_DECLARE PrintWriter &operator <<(PrintWriter &out, const void *value);
IOS_DECLARE PrintWriter &operator <<(PrintWriter &out, float value);
IOS_DECLARE PrintWriter &operator <<(PrintWriter &out, double value);
IOS_DECLARE PrintWriter &operator <<(PrintWriter &out, bool value);

#define IOS_FORCEINTEGRAL(Type) \
_finline PrintWriter &operator <<(PrintWriter &out, Type value) { \
    be::WriteInteger_(out, value); \
    return out; \
}

#define IOS_FORCEINTEGRAL_MODS(Type) \
IOS_FORCEINTEGRAL(unsigned Type) \
IOS_FORCEINTEGRAL(signed Type)

IOS_FORCEINTEGRAL_MODS(short)
IOS_FORCEINTEGRAL_MODS(int)
IOS_FORCEINTEGRAL_MODS(long)

#undef IOS_FORCEINTEGRAL_MODS
#undef IOS_FORCEINTEGRAL

template <size_t Size_>
PrintWriter &operator <<(PrintWriter &out, const byte_t value[Size_]) {
    out.WriteFully(value, Size_);
    return out;
}

struct FormatFlushOperation :
    FormatOperation
{
    virtual void operator()(PrintWriter &out) const {
        out.Flush();
    }
};

struct FormatNewLineOperation :
    FormatOperation
{
    virtual void operator()(PrintWriter &out) const {
        out.NewLine();
        out.Flush();
    }
};

struct FormatNewLineNoFlushOperation :
    FormatOperation
{
    virtual void operator()(PrintWriter &out) const {
        out.NewLine();
    }
};

class SetAlignment :
    public FormatOperation
{
  private:
    Alignment alignment_;

  public:
    SetAlignment(Alignment alignment) :
        alignment_(alignment)
    {
    }

    virtual void operator()(PrintWriter &out) const {
        out.SetAlignment(alignment_);
    }
};

class SetBase :
    public FormatOperation
{
  private:
    unsigned base_;

  public:
    SetBase(unsigned base) :
        base_(base)
    {
    }

    virtual void operator()(PrintWriter &out) const {
        out.SetBase(base_);
    }
};

class SetPrecision :
    public FormatOperation
{
  private:
    unsigned precision_;

  public:
    SetPrecision(unsigned precision) :
        precision_(precision)
    {
    }

    virtual void operator()(PrintWriter &out) const {
        out.SetPrecision(precision_);
    }
};

class SetLineSeparator :
    public FormatOperation
{
  private:
    cse::String separator_;

  public:
    SetLineSeparator(const cse::String &separator) :
        separator_(separator)
    {
    }

    virtual void operator()(PrintWriter &out) const {
        out.SetLineSeparator(separator_);
    }
};

class SetFill :
    public FormatOperation
{
  private:
    cse::CodePoint fill_;

  public:
    SetFill(cse::CodePoint fill) :
        fill_(fill)
    {
    }

    virtual void operator()(PrintWriter &out) const {
        out.SetFill(fill_);
    }
};

class SetWidth :
    public FormatOperation
{
  private:
    unsigned width_;

  public:
    SetWidth(unsigned width) :
        width_(width)
    {
    }

    virtual void operator()(PrintWriter &out) const {
        out.SetWidth(width_);
    }
};

static FormatFlushOperation Flush;
static FormatNewLineOperation NewLine;
static FormatNewLineNoFlushOperation NewLineNoFlush;

}

#endif//MENES_IOS_FORMATS_HPP
