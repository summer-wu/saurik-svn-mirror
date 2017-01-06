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

#include "api/threadlocal.hpp"
#include "api/posix/error.hpp"
#include "api/ucontext/fiber.hpp"

#include "app/entrypoint.hpp"
#include "ext/singleton.hpp"
#include "hop/bind.hpp"

namespace api {
namespace UContext {

namespace be {

struct LocalInfo_ {
    _R<Fiber> primary_;
    _R<Fiber> current_;
    _L< _R<Fiber> > return_;
};

struct FiberInfo_ {
    api::ThreadLocal<LocalInfo_> local_;
};

LocalInfo_ &GetLocal_() {
    FiberInfo_ &info(_1<FiberInfo_>());
    if (LocalInfo_ *local = info.local_)
        return *local;

    ext::Pointer<LocalInfo_> local(new LocalInfo_(), true);
    info.local_ = local;
    return *local.Release();
}

Fiber &Current_(LocalInfo_ &local) {
    if (Fiber *current = local.current_)
        return *current;

    _R<Fiber> current(new _H<Fiber>());
    local.primary_ = current;
    local.current_ = current;
    return *current;
}

}

void Fiber::Start_(Fiber *_this) {
    try {
        be::GetLocal_().return_.RemoveLast();
        Return();

        // XXX: might be cool to return this
        int result(_this->GetFunction()());
    } catch (const StopException &) {
    } catch (...) {
        api::Cout << "?" << ios::NewLine;
        _this->state_ = Failed_;
        Return();
    }

    api::Cout << "!" << ios::NewLine;
    _this->state_ = Complete_;
    Return();
}

void Fiber::Continue_(be::LocalInfo_ &local) {
    {
        Fiber &current(Current_(local));
        local.current_ = this;
        api::Posix::CheckError(::swapcontext(&current.context_, &context_));
        local.return_.RemoveLast();
    }

    if (state_ == Failed_)
        throw;

    if (Current().state_ == Stopping_)
        throw StopException();
}

Fiber::Fiber() :
    state_(Running_)
{
    //::memset(&context_, 0, sizeof(context_));
    // XXX: should this ask api::Thread for the thread's startup function?
}

void Fiber::_finalize() {
    if (state_ == Running_) {
        state_ = Stopping_;
        Call();
    }

    return ext::ReferenceCounted::_finalize();
}

Fiber::Fiber(const Function &function) :
    state_(Running_),
    function_(function)
{
    api::Posix::CheckError(::getcontext(&context_));

    context_.uc_link = NULL;
    context_.uc_stack.ss_sp = stack_.Begin();
    context_.uc_stack.ss_size = stack_.GetSize();

    ::makecontext(&context_, reinterpret_cast<void (*)()>(&Fiber::Start_), 1, this);

    Call();
}

const Fiber::Function &Fiber::GetFunction() const {
    return function_;
}

bool Fiber::IsStopped() const {
    return state_ != Running_;
}

void Fiber::Switch() {
    be::LocalInfo_ &local(be::GetLocal_());
    local.return_.InsertLast();
    Continue_(local);
}

void Fiber::Call() {
    be::LocalInfo_ &local(be::GetLocal_());
    local.return_.InsertLast(&Current_(local));
    local.return_.InsertLast();
    Continue_(local);
}

void Fiber::Return() {
    be::LocalInfo_ &local(be::GetLocal_());
    local.return_.Last()->Continue_(local);
}

Fiber &Fiber::Current() {
    return be::Current_(be::GetLocal_());
}

} }
