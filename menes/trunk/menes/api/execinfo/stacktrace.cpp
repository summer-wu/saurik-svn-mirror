/* Menes - C++ High-Level Utility Library
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

#include "api/execinfo/stacktrace.hpp"

#if defined(CFG_WHICH_ADDR2LINE) && defined(API_HAS_PCRE)
#define ST_ADDR2LINE
#endif

#if defined(API_HAS_LIBDL)
#include "api/libdl/error.hpp"
#endif

#ifdef ST_ADDR2LINE
#include "api/pcre/regex.hpp"
#endif

#include "api/exename.hpp"
#include "api/process.hpp"

#include "cse/digits.hpp"

#include "ios/helpers.hpp"
#include "ios/printwriter.hpp"

#include <cstdlib> //OK

#include <execinfo.h>

namespace api {
namespace ExecInfo {

StackTrace::StackTrace(int depth) {
    for (size_t size(16); ; size *= 2) {
        ext::Vector<void *> frames;
        frames.SetSize(size);

        int retval(::backtrace(&*frames.Begin(), frames.GetSize()));
        _assert(retval >= 0);

        size_t stored(retval);
        _assert(stored <= frames.GetSize());

        if (stored == size)
            continue;
        else {
            frames.SetSize(stored);
            ext::Swap(frames_, frames);
            return;
        }
    }
}

void StackTrace::Pop(unsigned number) {
    for (unsigned i(0); i != number; ++i) {
        if (frames_.IsEmpty())
            return;
        frames_.RemoveAt(frames_.Begin());
    }
}

size_t StackTrace::GetSize() const {
    return frames_.GetSize();
}

bool StackTrace::IsEmpty() const {
    return GetSize() == 0;
}

bool StackTrace::operator <(const StackTrace &rhs) const {
    if (frames_.GetSize() < rhs.frames_.GetSize())
        return true;
    else if (frames_.GetSize() > rhs.frames_.GetSize())
        return false;

    for (size_t i(0); i != frames_.GetSize(); ++i)
        if (frames_[i] < rhs.frames_[i])
            return true;
        else if (frames_[i] > rhs.frames_[i])
            return false;

    return false;
}

ios::PrintWriter &operator <<(ios::PrintWriter &out, const StackTrace &trace) {
    char **names(::backtrace_symbols(&*trace.frames_.Begin(), trace.frames_.GetSize()));

#if defined(ST_ADDR2LINE)
    _L<cse::String> args;
    args.InsertLast("-Cfe");
    args.InsertLast();

    ext::RedBlackMap< cse::String, _R<api::Process> > addr2lines;
#endif

    // XXX: throw exception on names == NULL!!
    for (size_t i(0), size(trace.frames_.GetSize()); i != size; ++i) {
        if (i != 0)
            out << ios::NewLine;
        if (names[i] == NULL)
            continue;

#if defined(ST_ADDR2LINE)
        uintptr_t address;
        cse::String module;

        static api::Pcre::RegEx info0("^(.*?)(\\(([^\\(]*)\\+(0x[0-9a-f]*)\\))? \\[0x([0-9a-f]*)\\]$");
        static api::Pcre::RegEx info1("^0x([0-9a-f]*) <([^\\<]*)\\+([0-9]*)> at (.*)$");

#if defined(API_HAS_LIBDL)
        static void *base(0);
        if (base == 0) {
            ::dlerror();
            void *start(::dlsym(RTLD_DEFAULT, "_start"));
            if (start == NULL) {
                const char *error(::dlerror());
                if (error != NULL)
                    throw api::libdl::Error(error);
            }

            ::Dl_info info;
            if (::dladdr(start, &info) == 0)
                throw api::libdl::Error();
            base = info.dli_fbase;
        }
#endif

        if (api::Pcre::RegEx::Match match = info0(names[i])) {
            address = cse::FromDigits<16, uintptr_t>(match[5]);
            module = match[1];
        } else if (api::Pcre::RegEx::Match match = info1(names[i])) {
            address = cse::FromDigits<16, uintptr_t>(match[1]);
            module = match[4];
        }

#if defined(API_HAS_LIBDL)
        {
            ::Dl_info info;
            if (::dladdr(reinterpret_cast<void *>(address), &info) == 0)
                throw api::libdl::Error();

            if (info.dli_fbase == base)
                module = api::GetExecutablePath().GetPath();
            else
                address -= reinterpret_cast<uintptr_t>(info.dli_fbase);
        }
#endif

        _R<api::Process> &addr2line(addr2lines[module]);

        if (addr2line.IsEmpty()) {
            args.Last() = module;
            addr2line = new _H<api::Process>(CFG_WHICH_ADDR2LINE, args);
        }

        {
            _S<ios::FormatWriter> out(*addr2line->GetWriter());
            out << "0x" << ios::SetBase(16) << address << ios::NewLine;
        }

        cse::String symbol(ios::ReadLine(*addr2line->GetReader()));
        cse::String file(ios::ReadLine(*addr2line->GetReader()));

        out << "[" << module << "] " << symbol << "(" << file << ")";
#else
        out << (size - i) << ": " << (names[i] == NULL ? "(null)" : names[i]);
#endif
    }

    ::free(names);
    return out;
}

} }
