/* Menes Optimization - C++ Profiling and Verification
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
#include "api/threadlocal.hpp"

#include "app/configuration.hpp"

#include "ext/singleton.hpp"
#include "ext/stackref.hpp"

#include "opt/debugged.hpp"
#include "opt/garbage.hpp"
#include "opt/memory.hpp"
#include "opt/reports.hpp"
#include "opt/tagged.hpp"
#include "opt/tracking.hpp"

namespace opt {

namespace be {

struct MemoryRegister_ {
    _S< ext::ThunkedAllocator<ext::MallocAllocator> > malloc_;
    _S<DebuggedAllocator> debugged_;
    _S<TrackingAllocator> tracking_;

    LocalAllocator_ allocator_;

    GarbageCollector<> collector_;

    struct Local_ {
    };

    api::ThreadLocal<Local_> local_;

    cse::String logfile_;

    Local_ *GetLocal() {
        if (Local_ *local = local_.GetValue())
            return local;

        Local_ *local(ext::ObjectAllocator< ext::TypedAllocator<ext::MallocAllocator> >::Template<Local_>::Result().AllocateEx());
        local_ = local;
        return local;
    }

    MemoryRegister_() :
        debugged_(&malloc_),
        tracking_(sizeof(_R<ext::Allocator>) + opt::TaggedAllocator::GetOverhead(), &debugged_),
        allocator_(&tracking_)
    {
    }
};

MemoryRegister_ &GetMemoryRegister_() {
    /* // XXX: this caused stupid asserts on close
    return _1<MemoryRegsiter_>(); */
    static ext::StaticPermanent<MemoryRegister_> register_;
    return register_.GetInstance();
}

// XXX: this needs memory allocation due to api::GetExecutablePath()
// XXX: this therefor reports a bogus memory leak in api::GetExecutablePath()
struct ReportLeaks_ {
    bool report_;
    _R<api::FileWriter> file_;

    ReportLeaks_() :
        report_(_C("menes/opt/memory/@debug-report", true))
    {
        if (report_)
            file_ = new _H<api::FileWriter>(_S<ios::String>() << api::GetExecutablePath().GetParent().GetPath() << "/Memory.log");
    }

    ~ReportLeaks_() {
        // XXX: this almost seems redundant now (as the _correct_ solution was to apply this to DebuggedAllocator::Report())
        ext::ScopeSet<LocalAllocator_, ext::Allocator *> allocator(GetMemoryRegister_().allocator_, &GetMemoryRegister_().malloc_);

        if (report_) {
            _S<ios::FormatWriter> writer(*file_);
            ReportLeaks(writer);
            file_.Clear();
        }
    }
} reportleaks_;

_finline void *Allocate_(size_t size, uintptr_t tag, ext::Allocator &allocator) {
    OPT_TIMER("opt::be::Allocate_()")

    typedef _R<ext::Allocator> Handle;
    Handle *handle(opt::TaggedAllocator::Allocate<Handle>(sizeof(Handle) + size, tag, allocator));
    new (handle) Handle(&allocator);

    char *end(reinterpret_cast<char *>(handle + 1) + sizeof(Handle) + size);
    //be::GetMemoryRegister_().collector_.Insert(handle + 1, end);

    return handle + 1;
}

_finline void *Reallocate_(void *address, size_t size, uintptr_t tag, ext::Allocator &allocator) {
    throw ext::NotImplementedException();
}

_finline void Deallocate_(void *address, uintptr_t tag, ext::Allocator &allocator) {
    //be::GetMemoryRegister_().collector_.Remove(reinterpret_cast<_R<ext::Allocator> *>(address) + 1, /*XXX:*/ NULL);
    opt::TaggedAllocator::Deallocate(address, tag, allocator);
}

}

OPT_DECLARE LocalAllocator_ &GetAllocatorIndirect() {
    return be::GetMemoryRegister_().allocator_;
}

OPT_DECLARE ext::Allocator &GetGlobalMalloc() {
    return be::GetMemoryRegister_().malloc_;
}

OPT_DECLARE void SetupAllocation() {
    be::GetMemoryRegister_();
}

OPT_DECLARE void *Allocate(size_t size, uintptr_t tag) {
    ext::ScopeSet<LocalAllocator_, ext::Allocator *> allocator(be::GetMemoryRegister_().allocator_, &be::GetMemoryRegister_().malloc_);
    return be::Allocate_(size, tag, *allocator.GetValue());
}

OPT_DECLARE void Deallocate(void *address, size_t size, uintptr_t tag) {
    if (address == NULL)
        return;

    OPT_TIMER("opt::Deallocate()")

    typedef _R<ext::Allocator> Handle;
    Handle *handle(reinterpret_cast<Handle *>(address) - 1);

    be::Reallocate_(handle, size, tag, **handle);
}

OPT_DECLARE void Deallocate(void *address, uintptr_t tag) {
    if (address == NULL)
        return;

    OPT_TIMER("opt::Deallocate()")

    typedef _R<ext::Allocator> Handle;
    Handle *handle(reinterpret_cast<Handle *>(address) - 1);
    //ext::Allocator *original(be::GetMemoryRegister_().allocator_);

    // for exception safety on the deallocator
    // ext::Swap() was used for speed purposes
    // XXX: but is it really faster? who said?
    Handle allocator;
    ext::Swap(allocator, *handle);
    handle->~Handle();

    be::Deallocate_(handle, tag, *allocator);
}

OPT_DECLARE void Collect() {
    be::GetMemoryRegister_().collector_.Collect();
}

OPT_DECLARE void ReportLeaks(ios::PrintWriter &out) {
    be::GetMemoryRegister_().debugged_.Report(out);
}

OPT_DECLARE void ReportMemory(ios::PrintWriter &out) {
    be::GetMemoryRegister_().tracking_.Report(out);
}

}

OPT_DECLARE void *operator new(size_t size, ext::Allocator &allocator) {
    return opt::be::Allocate_(size, opt::CppScalarTag, allocator);
}

OPT_DECLARE void operator delete(void *address, ext::Allocator &allocator) {
    OPT_TIMER("operator delete(void *, ext::Allocator &)")
    typedef _R<ext::Allocator> Handle;
    Handle *handle(reinterpret_cast<Handle *>(address) - 1);
    _assert(*handle == &allocator);
    opt::be::Deallocate_(handle, opt::CppScalarTag, allocator);
}
