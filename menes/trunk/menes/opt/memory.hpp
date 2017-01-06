/* Menes Optimization - C++ Profiling and Verification
 * Copyright (C) 2002-2004  Jay Freeman (saurik)
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

#ifndef MENES_OPT_MEMORY_HPP
#define MENES_OPT_MEMORY_HPP

#include "cxx/platform.hpp"
#include "opt/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "api/threadlocal.hpp"

namespace ext {
    struct Allocator;
}

namespace opt {

typedef api::ThreadLocal<ext::Allocator, false> LocalAllocator_;

OPT_DECLARE LocalAllocator_ &GetAllocatorIndirect();
OPT_DECLARE ext::Allocator &GetGlobalMalloc();

OPT_DECLARE void SetupAllocation();

OPT_DECLARE void *Allocate(size_t size, uintptr_t tag);
OPT_DECLARE void Reallocate(void *address, size_t size, uintptr_t tag);
OPT_DECLARE void Deallocate(void *address, uintptr_t tag);

OPT_DECLARE void ReportLeaks();

OPT_DECLARE void SetOwner(const char *file, unsigned line, const char *func);

OPT_DECLARE void Collect();

struct AllocationSetup {
    AllocationSetup() {
        SetupAllocation();
    }
};

}

#define OPT_SETUPALLOCATION() \
opt::AllocationSetup allocation_;

#endif//MENES_OPT_MEMORY_HPP
