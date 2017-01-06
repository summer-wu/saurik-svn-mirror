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

#include "api/posix/socket.hpp"
#include "ios/helpers.hpp"

namespace api {
namespace Posix {

void Socket::Initialize_() {
#ifdef CFG_DEFINE_SO_NOSIGPIPE
    int value(1);
    CheckError(::setsockopt(sock_, SOL_SOCKET, SO_NOSIGPIPE, &value, sizeof(int)));
#endif
}

Socket::Socket(int family, int type, int protocol) {
    sock_.Mend(CheckError(::socket(family, type, protocol)));
    Initialize_();
}

Socket::Socket(Socket &server, Address *address) {
    socklen_t length(static_cast<int>(address == NULL ? 0 : address->GetSize()));
    sock_.Mend(CheckError(::accept(server.sock_, address == NULL ? NULL : address->GetAddr(), &length)));
    Initialize_();
}

Address Socket::GetRemoteAddress() const {
    Address address;
    for (socklen_t length(16);;) {
        address.SetSize(length);

        if (-1 != ::getpeername(sock_, address.GetAddr(), &length)) {
            address.SetSize(length);
            return address;
        }

        int error(ApiTraits::GetLastError());
        if (error != EFAULT)
            throw Error(error);
    }
}

/*void Socket::Keep() {
    ShutdownWrite();
    ios::Discard(*this);
}*/

void Socket::SetLocalAddress(const Address &address, bool force) {
    if (force) {
        int value(1);
        CheckError(::setsockopt(sock_, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(int)));
    }

    CheckError(::bind(sock_, address.GetAddr(), static_cast<int>(address.GetSize())));
}

Address Socket::GetLocalAddress() const {
    Address address;
    for (socklen_t length(16);;) {
        address.SetSize(length);

        if (-1 != ::getsockname(sock_, address.GetAddr(), &length)) {
            address.SetSize(length);
            return address;
        }

        int error(ApiTraits::GetLastError());
        if (error != EFAULT)
            throw Error(error);
    }
}

size_t Socket::Read(const iovec *buff, int count, Address *address) {
    msghdr header;

    header.msg_name = (address == NULL ? NULL :
#ifdef __APPLE__
        reinterpret_cast<caddr_t>(address->GetAddr())
#else
        address->GetAddr()
#endif
    );

    header.msg_namelen = (address == NULL ? 0 : address->GetSize());
    header.msg_iov = const_cast<iovec *>(buff);
    header.msg_iovlen = count;
    header.msg_control = NULL;
    header.msg_controllen = 0;
    header.msg_flags = 0;

    int bytes(CheckError(::recvmsg(sock_, &header, 0)));
    return bytes;
}

size_t Socket::Write(iovec *buff, int count, const Address *address) {
    msghdr header;

    header.msg_name = (address == NULL ? NULL :
#ifdef __APPLE__
        reinterpret_cast<caddr_t>(const_cast<sockaddr *>(address->GetAddr()))
#else
        const_cast<sockaddr *>(address->GetAddr())
#endif
    );

    header.msg_namelen = (address == NULL ? 0 : address->GetSize());
    header.msg_iov = buff;
    header.msg_iovlen = count;
    header.msg_control = NULL;
    header.msg_controllen = 0;
    header.msg_flags = 0;

    int bytes(CheckError(::sendmsg(sock_, &header,
#ifdef CFG_DEFINE_MSG_NOSIGNAL
        MSG_NOSIGNAL
#else
        0
#endif
    )));

    return bytes;
}

size_t Socket::Read(char *data, size_t length, Address *address) {
    iovec iov = {data, length};
    return Read(&iov, 1, address);
}

// XXX: this function is capable of losing parts of large UDP packets
size_t Socket::Read(ios::Writer &writer, Address *address) {
    static const int Size_(1024);
    char buffer[Size_];

    size_t size(Read(buffer, Size_, address));
    writer.WriteFully(buffer, size);
    return size;
}

} }
