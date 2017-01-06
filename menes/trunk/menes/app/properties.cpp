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

#include "api/exename.hpp"
#include "api/files.hpp"

#include "app/properties.hpp"

#include "ios/helpers.hpp"
#include "ios/string.hpp"

namespace app {

void Properties::Read(ios::Reader &reader) {
    for (ext::Buffer line; ios::ReadLine(reader, line);) {
        if (line.IsEmpty() || line[0] == '#')
            continue;
        if (line.StartsWithAll(_B("import ")))
            Read(line.Substring(6));
        else {
            ext::Buffer::Iterator equal(ext::FindFirst(line, '='));
            cse::String &value(values_[Replace(cse::String(line.Begin(), equal).TrimRight<cse::IsLinearWhiteSpace>())]);
            _forever {
                line = cse::String(equal + 1, line.End());
                bool slashed(line.Last() == '\\');
                if (line.First() != '#') {
                    if (slashed)
                        line = cse::String(line.Begin(), line.End() - 1);
                    value.InsertAllLast(line);
                }
                if (!slashed)
                    break;
            }
        }
    }
}

void Properties::Read(const cse::String &file) {
    _S<api::FileReader> reader(_S<ios::String>() << api::GetExecutablePath().GetParent().GetPath() << "/../data/properties/" << file << ".properties");
    Read(reader);
}

}
