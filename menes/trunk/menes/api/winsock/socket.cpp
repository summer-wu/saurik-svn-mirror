/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2002-2005  Jay Freeman (saurik)
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

#include "api/winsock/socket.hpp"

namespace api {
namespace Winsock {

namespace {
class PerProcess {
    virtual void Start() {
        WSADATA data;
        int error(::WSAStartup(MAKEWORD(2,2), &data));
        if (error != 0)
            throw Error(error);
    }

    virtual void Stop() {
        if (SOCKET_ERROR == ::WSACleanup())
            throw Error();
    }
} perprocess_; }

Address Socket::GetRemoteAddress() const {
    Address address;
    for (int length(16);;) {
        address.SetSize(length);

        if (SOCKET_ERROR != ::getpeername(sock_, address.GetAddr(), &length)) {
            address.SetSize(length);
            return address;
        }

        int error(ApiTraits::GetLastError());
        if (error != WSAEFAULT)
            throw Error(error);
    }
}

Address Socket::GetLocalAddress() const {
    Address address;
    for (int length(16);;) {
        address.SetSize(length);

        if (SOCKET_ERROR != ::getsockname(sock_, address.GetAddr(), &length)) {
            address.SetSize(length);
            return address;
        }

        int error(ApiTraits::GetLastError());
        if (error != WSAEFAULT)
            throw Error(error);
    }
}

size_t Socket::Read(WSABUF *buff, DWORD count, Address *address, bool *partial) {
    DWORD bytes, flags(0);
    sockaddr *saddr(NULL);

    for (int fromlen(address == NULL ? 0 : 16);;) {
        if (address != NULL) {
            address->SetSize(fromlen);
            saddr = address->GetAddr();
        }

        if (SOCKET_ERROR != ::WSARecvFrom(sock_, buff, count, &bytes, &(flags = 0), saddr, &fromlen, NULL, NULL)) {
            if (address != NULL)
                address->SetSize(fromlen);
            break;
        }

        int error(ApiTraits::GetLastError());
        /* // XXX: this code bounces between here and msg::Router::Route()
        if (error == WSAEINTR)
            return 0;*/
        if (address == NULL || error != WSAEFAULT || size_t(fromlen) < address->GetSize())
            throw Error(error);
    }

    if (partial != NULL)
        *partial = ((flags & MSG_PARTIAL) != 0);
    return bytes;
}

size_t Socket::Read(char *data, size_t length, Address *address, bool *partial) {
    WSABUF wsabuf = {static_cast<u_long>(length), data};
    return Read(&wsabuf, 1, address, partial);
}

size_t Socket::Read(ios::Writer &writer, Address *address) {
    static const int Size_(1024);
    char buffer[Size_];

    size_t total(0);
    for (bool partial(true); partial; ) {
        size_t size(Read(buffer, Size_, address, &partial));
        total += size;
        writer.Write(buffer, size);
    }

    return total;
}

} }
