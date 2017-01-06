/* Menes - C++ High-Level Utility Library
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

#ifndef MENES_API_WINSOCK_LOCKS_HPP
#define MENES_API_WINSOCK_LOCKS_HPP

#include "cxx/platform.hpp"
#include "api/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "errors.hpp"
#include "api/winsock/error.hpp"

namespace api {
namespace Winsock {

class Waitable {
};

class Event :
    public Waitable
{
  protected:
    WSAEVENT event_;

  public:
    Event() :
        event_(::WSACreateEvent())
    {
        if (event_ == WSA_INVALID_EVENT)
            throw Error();
    }

    ~Event() {
        if (FALSE == ::WSACloseEvent(event_))
            throw Error();
    }

    void Wait() {
        if (WSA_WAIT_FAILED == ::WSAWaitForMultipleEvents(1, &event_, FALSE, WSA_INFINITE, FALSE))
            throw Error();
    }

    void Set() {
        if (FALSE == ::WSASetEvent(event_))
            throw Error();
    }

    void Reset() {
        if (FALSE == ::WSAResetEvent(event_))
            throw Error();
    }
};

} }

#endif//MENES_API_WINSOCK_LOCKS_HPP
