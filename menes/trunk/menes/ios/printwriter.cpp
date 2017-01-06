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

#include "cxx/standard.hh"
#pragma hdrstop("build/cxx/standard.pch")

#include "ios/printwriter.hpp"

#include <cstdio>

namespace ios {

PrintWriter::PrintWriter() :
    buffered_(0),
    alignment_(Right),
    base_(10),
    precision_(7),
    fill_(' '),
    width_(~size_t()),
    separator_(
// XXX: this would be awesome to configure somehow
#if defined(_WIN32)
        _B("\r\n")
/* XXX: but... but I thought... damn :(
#elif defined(__APPLE__)
        _B("\r") */
#else
        _B("\n")
#endif
    )
{
}

size_t PrintWriter::Write(const byte_t *data, size_t length) {
    if (buffered_ != 0) {
        buffer_.InsertAllLast(data, length);
        return length;
    }

    unsigned left(0), right(0);
    if (width_ != ~size_t() && length < width_) {
        unsigned pad(width_ - length);
        switch (alignment_) {
            case Left:
                right = pad;
            break;

            case Right:
                left = pad;
            break;

            case Center:
                left = pad / 2;
                right = pad - left;
            break;
        }
    }

    _forall (unsigned, i, 0, left) {
        // XXX: this isn't unicode sufficient
        byte_t fill(static_cast<byte_t>(fill_));
        WriteFully_(&fill, 1);
    }

    WriteFully_(data, length);

    _forall (unsigned, i, 0, right) {
        // XXX: this isn't unicode sufficient
        byte_t fill(static_cast<byte_t>(fill_));
        WriteFully_(&fill, 1);
    }

    width_ = ~size_t();
    return length;
}

void PrintWriter::Flush() {
}

void PrintWriter::Finish() {
    _assert(buffered_ != 0);
    if (--buffered_ == 0) {
        WriteFully(buffer_.Begin(), buffer_.GetSize());
        buffer_.Clear();
    }
}

PrintWriter &operator <<(PrintWriter &out, const void *value) {
    const unsigned digits(sizeof(value) * 2);

    byte_t buff[digits + 2];
    buff[0] = '0';
    buff[1] = 'x';

    _forall (size_t, i, 0, digits)
        buff[i + 2] = be::Numerals_[(reinterpret_cast<uintptr_t>(value) >> (4 * (digits - i - 1))) & 0xf];

    out.WriteFully(buff, digits + 2);
    return out;
}

PrintWriter &operator <<(PrintWriter &out, float value) {
    return out << double(value);
}

// XXX: I need to deal with this issue... it's a doozy
#ifdef _MSC_VER
#define snprintf _snprintf
#endif

PrintWriter &operator <<(PrintWriter &out, double value) {
    byte_t buff[1024];
    int size(snprintf(buff, 1024, "%.*f", out.GetPrecision(), value));
    out.WriteFully(buff, size);
    return out;
}

PrintWriter &operator <<(PrintWriter &out, bool value) {
    return out << (value ? "true" : "false");
}

}
