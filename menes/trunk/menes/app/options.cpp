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

#include "api/console.hpp"
#include "app/options.hpp"

namespace app {

Options::Options() {
}

Options::Options(const ext::Buffer &extra) {
    RegisterExtra(extra);
}

void Options::Parse(const ArgumentList::ConstIterator &begin, const ArgumentList::ConstIterator &end) {
    bool dashed(false);

    for (ArgumentList::ConstIterator arg(begin); arg != end; ++arg)
        if (dashed || arg->IsEmpty() || (*arg)[0] != '-' || arg->GetSize() == 1) {
            args_.InsertLast(*arg);
        } else if ((*arg)[1] != '-') {
            ext::Buffer switches(*arg);
            _foreach (ext::Buffer, i, switches) {
                ext::Buffer option(*i), value;
                if (extra_.Contains(*i))
                    if (++arg != end)
                        value = *arg;
                    else {
                        // XXX: should just give the user an error
                        _assert(false);
                    }
                options_.Insert(option, value);
            }
        } else if (arg->GetSize() == 2) {
            dashed = true;
        } else {
            ext::Buffer option, value, text(arg->Substring(2));
            ext::Buffer::Iterator equal(ext::FindFirst(text, '='));
            if (equal == text.End())
                option = text;
            else {
                option = ext::Buffer(text.Begin(), equal);
                value = ext::Buffer(equal + 1, text.End());
            }

            options_.Insert(option, value);
        }
}

const ArgumentList &Options::GetArguments() const {
    return args_;
}

void Options::RegisterExtra(const ext::Buffer &options) {
    _foreach (const ext::Buffer, option, options)
        extra_.Insert(*option);
}

const ext::Buffer *Options::operator [](const ext::Buffer &option) const {
    OptionMap_::ConstIterator i(options_.Find(option));
    return i == options_.End() ? NULL : &i->Second();
}

const ext::Buffer *Options::operator [](unsigned index) const {
    return index < args_.GetSize() ? &args_[index] : NULL;
}

}
