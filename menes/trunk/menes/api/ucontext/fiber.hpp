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

#ifndef MENES_API_UCONTEXT_FIBER_HPP
#define MENES_API_UCONTEXT_FIBER_HPP

#include "cxx/platform.hpp"
#include "api/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "ext/array.hpp"
#include "ext/refcount.hpp"

#include "hop/function.hpp"

#include <ucontext.h>

namespace api {
namespace UContext {

namespace be {
    struct LocalInfo_;
}

class Fiber :
    public ext::ReferenceCounted
{
    friend class ext::HeapReference<Fiber>;

  public:
    typedef hop::Function<int ()> Function;

    class StopException {
    };

  private:
    ext::Array<32 * 1024, byte_t> stack_;

    volatile enum State_ {
        Running_,
        Stopping_,
        Complete_,
        Failed_
    } state_;

    Function function_;
    ::ucontext_t context_;

    static void Start_(Fiber *_this);

    void Continue_(be::LocalInfo_ &local);

    Fiber();

  protected:
    virtual void _finalize();

  public:
    Fiber(const Function &function);

    const Function &GetFunction() const;

    bool IsStopped() const;

    void Switch();

    void Call();
    static void Return();

    static Fiber &Current();
};

} }

#endif//MENES_API_UCONTEXT_FIBER_HPP
