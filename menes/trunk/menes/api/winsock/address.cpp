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

#include "api/winsock/address.hpp"

#include <ws2tcpip.h>
#include "api/win32/undefine.hpp"

namespace api {
namespace Winsock {

// XXX: this is hardcoded for AF_INET
void Address::Parse(const cse::String &address) {
    SetSize(ext::max<size_t>(GetCapacity(), 16));

    for (;;) {
        int size(static_cast<int>(GetSize()));
        if (SOCKET_ERROR != ::WSAStringToAddressA(const_cast<char *>(address.NullTerminate()), AF_INET, NULL, GetAddr(), &size)) {
            _assert(size_t(size) <= GetSize());
            SetSize(size);
            return;
        }

        if (ApiTraits::GetLastError() != WSAEFAULT)
            throw Error();
        SetSize(size);
    }
}

_L<Address> Address::Resolve(const cse::String &node, const cse::String &service) {
    ::addrinfo hints, *info(NULL);
    ::memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    if (0 != ::getaddrinfo(node.NullTerminate(), service.NullTerminate(), &hints, &info))
        throw Error();
    _L<Address> addresses;
    for (::addrinfo *entry(info); entry != NULL; entry = entry->ai_next)
        addresses.InsertLast(Address(entry->ai_addr, entry->ai_addrlen));
    ::freeaddrinfo(info);
    return addresses;
}

InternetAddress::Port InternetAddress::GetPort() const {
    return ::ntohs(GetAddr_()->sin_port);
}

void InternetAddress::SetPort(Port port) {
    GetAddr_()->sin_port = ::htons(port);
}

uint32_t InternetAddress::GetHost() const {
    return ::ntohl(GetAddr_()->sin_addr.s_addr);
}

void InternetAddress::SetHost(uint32_t host) {
    GetAddr_()->sin_addr.s_addr = ::htonl(host);
}

ios::PrintWriter &operator <<(ios::PrintWriter &out, const Address &address) {
    char data[128];
    DWORD length(128);

    // XXX: this should use WSAAddressToStringW()

    if (SOCKET_ERROR != ::WSAAddressToStringA(const_cast<sockaddr *>(address.GetAddr()), static_cast<DWORD>(address.GetSize()), NULL, data, &length)) {
        out.WriteFully(data, length - 1);
        return out;
    }

    int error(ApiTraits::GetLastError());
    if (error != WSAEFAULT)
        throw Error(error);

    ext::Block<char> buffer(length);
    if (SOCKET_ERROR == ::WSAAddressToStringA(const_cast<sockaddr *>(address.GetAddr()), static_cast<DWORD>(address.GetSize()), NULL, buffer.Begin(), &length))
        throw Error();

    out.WriteFully(buffer.Begin(), length - 1);
    return out;
}

} }
