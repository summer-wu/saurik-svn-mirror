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

#include "api/error.hpp"
#include "api/freebsd/exename.hpp"
#include "api/environment.hpp"

#include "ext/singleton.hpp"
#include "opt/memory.hpp"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysctl.h>
#include <unistd.h>

namespace api {
namespace FreeBsd {

api::Path RealPath_(cse::String argv) {
    if (!ext::Contains(argv, '/')) {
        _L<cse::String> paths(ext::SplitAll(TheEnvironment.Get("PATH"), _B(":")));

        _foreach(_L<cse::String>, path, paths) {
            _S<ios::String> candidate(*path);
            candidate << "/" << argv;

            if (::access(candidate.NullTerminate(), X_OK) == 0) {
                struct ::stat file;

                if (::stat(candidate.NullTerminate(), &file) == 0 && S_ISREG(file.st_mode) && (::getuid() != 0 || (file.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)) != 0)) {
                    argv = candidate;
                    break;
                }
            }
        }
    }

    return api::Path(argv).GetRealPath();
}

const api::Path &GetExecutablePath() {
    opt::SetupAllocation();
    static api::Path path;

    if (path.IsEmpty()) {
        int mib[4] = { CTL_KERN, KERN_PROC, KERN_PROC_ARGS, ::getpid() };

        char argv[65536];
        size_t size(sizeof(argv));

        if (::sysctl(mib, 4, argv, &size, NULL, 0) == -1)
            throw api::Posix::Error();

        path = RealPath_(argv);
    }

    return path;
}

} }
