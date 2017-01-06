/* Menes - C++ High-Level Utility Library
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

#include "api/process.hpp"
#include "ios/helpers.hpp"
#include "mak/compiler.hpp"

namespace mak {

namespace {
    cse::String makecmd_;
    _L<cse::String> makeargs_;
}

void Compile(const cse::String &cpp, const cse::String &so) {
    if (makecmd_.IsEmpty()) {
        OPT_TIMER("mak::Compile()[Setup]")

        _L<cse::String> args;

        /* XXX: this isn't neccessarily local
        args.InsertLast(_B("-C"));
        args.InsertLast(_B(CFG_MENES_ROOT));
        args.InsertLast(_B("net-tools"));*/

        args.InsertLast(_B("-dc++"));

        // XXX: should I think about this path better?
        _S<api::Process> make(_B("./make.sh"), args);
        make.ClearWriter();

        cse::String line(ios::ReadLine(*make.GetReader()));

        ios::Discard(*make.GetReader());

        // XXX: this doesn't handle directories with spaces in them!!!
        makeargs_ = ext::SplitAll(line, _B(" "));

        /* // XXX: this isn't neccessarily local
        makecmd_ = (_S<ios::String>() << CFG_MENES_ROOT << "/" << makeargs_.First());*/

        makecmd_ = makeargs_.First();
        makeargs_.RemoveFirst();
        makeargs_.InsertLast(_B("-shared"));
    }

    OPT_TIMER("mak::Compile()[Compile]")

    _L<cse::String> args(makeargs_);
    args.InsertLast(_B("-o"));
    args.InsertLast(so);
    args.InsertLast(cpp);

    _S<api::Process> make(makecmd_, args);
    make.ClearWriter();

    _S<ios::String> error;
    ios::ReadToWrite(*make.GetReader(), error);

    if (make.Join() != 0)
        throw CompileException(error);
}

}
