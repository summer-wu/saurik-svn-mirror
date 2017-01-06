/* Menes - C++ High-Level Utility Library
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

#include "ios/helpers.hpp"
#include "ios/streams.hpp"

namespace ios {

IOS_DECLARE void ReadToWrite(Reader &reader, Writer &writer) {
    _forever {
        byte_t buffer[1024];
        size_t read(reader.Read(buffer, sizeof(buffer)));
        if (read == 0)
            return;
        writer.WriteFully(buffer, read);
    }
}

IOS_DECLARE void ReadToWriteFully(Reader &reader, Writer &writer, size_t size) {
    while (size != 0) {
        byte_t buffer[1024];
        size_t read(ext::min(size, sizeof(buffer)));
        reader.ReadFully(buffer, read);
        writer.WriteFully(buffer, read);
        size -= read;
    }
}

// XXX: mumma-mia, christa-pania
IOS_DECLARE void ReadUntil(Reader &reader, byte_t value, ext::Buffer &buffer) {
    buffer.Clear();

    for (;;) {
        byte_t c(ios::Get(reader));
        if (c == value)
            return;
        buffer.InsertLast(c);
    }
}

IOS_DECLARE ext::Buffer ReadUntil(Reader &reader, byte_t value) {
    ext::Buffer buffer;
    ReadUntil(reader, value, buffer);
    return buffer;
}

IOS_DECLARE void Discard(Reader &reader) {
    ReadToWrite(reader, ios::Null);
}

IOS_DECLARE void DiscardFully(Reader &reader, size_t size) {
    ReadToWriteFully(reader, ios::Null, size);
}

IOS_DECLARE ext::Buffer ReadAll(Reader &reader) {
    _S<ios::Buffer> buffer;
    ReadToWrite(reader, buffer);
    return buffer;
}

// XXX: this doesn't handle sole '\r' newlines
// XXX: this function just feels built poorly
IOS_DECLARE bool ReadLine(Reader &reader, ext::Buffer &line) {
    for (ext::Buffer buffer;;) {
        byte_t c;
        if (!reader.Get(c))
            if (buffer.IsEmpty())
                return false;
            else {
                line = buffer;
                return true;
            }
        if (c == '\n') {
            line = buffer;
            return true;
        }
        if (c != '\r')
            buffer.InsertLast(c);
    }
}

IOS_DECLARE ext::Buffer ReadLine(Reader &reader) {
    ext::Buffer line;
    if (!ReadLine(reader, line))
        throw ext::EosException();
    return line;
}

}
