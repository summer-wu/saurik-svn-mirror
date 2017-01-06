/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2002  Jay Freeman (saurik)
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

#ifndef MENES_API_PTHREADS_SEMAPHORE_HPP
#define MENES_API_PTHREADS_SEMAPHORE_HPP

#include "cxx/platform.hpp"
#include "api/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "api/posix/error.hpp"
#include "ext/refcount.hpp"

#include <semaphore.h>

namespace api {
namespace Pthreads {

// XXX: this looks like it _should_ be api::Posix...
class Semaphore {
  protected:
    sem_t handle_;

    operator sem_t() const;

  private:
    Semaphore(const Semaphore &rhs);
    Semaphore &operator =(const Semaphore &rhs);

  public:
    explicit Semaphore(unsigned initial = 0) {
        api::Posix::CheckError(::sem_init(&handle_, 0, initial));
    }

    ~Semaphore() {
        api::Posix::CheckError(::sem_destroy(&handle_));
    }

    void Acquire() {
        api::Posix::CheckError(::sem_wait(&handle_));
    }

    void Release(unsigned number = 1) {
        api::Posix::CheckError(::sem_post(&handle_));
    }
};

} }

#endif//MENES_API_PTHREADS_SEMAPHORE_HPP
