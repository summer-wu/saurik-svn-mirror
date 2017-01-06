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

#ifndef MENES_API_WINSOCK_SOCKET_HPP
#define MENES_API_WINSOCK_SOCKET_HPP

#include "cxx/platform.hpp"
#include "api/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "api/winsock/address.hpp"
#include "api/winsock/error.hpp"

#include "ios/streams.hpp"

#include <winsock2.h>
#include "api/win32/undefine.hpp"

namespace api {
namespace Winsock {

class API_DECLARE Socket :
    public ios::Reader,
    public ios::Writer
{
  protected:
    SOCKET sock_;

    int family_;
    int type_;
    int protocol_;

    WSAPROTOCOL_INFO *protoinfo_;

  private:
    void Create_() {
        _assert(sock_ == INVALID_SOCKET);
        sock_ = ::WSASocket(family_, type_, protocol_, protoinfo_, 0, WSA_FLAG_OVERLAPPED);
        if (sock_ == INVALID_SOCKET)
            throw Error();
    }

  public:
    Socket(int family, int type, int protocol) :
        sock_(INVALID_SOCKET),
        family_(family),
        type_(type),
        protocol_(protocol),
        protoinfo_(NULL)
    {
        Create_();
    }

    explicit Socket(WSAPROTOCOL_INFO *protoinfo) :
        sock_(INVALID_SOCKET),
        family_(FROM_PROTOCOL_INFO),
        type_(FROM_PROTOCOL_INFO),
        protocol_(FROM_PROTOCOL_INFO),
        protoinfo_(protoinfo)
    {
        Create_();
    }

    // XXX: this is dangerous as it looks like a copy constructor
    Socket(Socket &socket, Address *address) {
        int length(address == NULL ? 0 : static_cast<int>(address->GetSize()));
        sock_ = ::WSAAccept(sock_, address == NULL ? NULL : address->GetAddr(), &length, NULL, NULL);
        if (sock_ == INVALID_SOCKET)
            throw Error();
    }

    ~Socket() {
        Close();
    }

    void Close() {
        if (sock_ == INVALID_SOCKET)
            return;
        if (SOCKET_ERROR == ::closesocket(sock_))
            throw Error();
        sock_ = INVALID_SOCKET;
    }

    void Connect(const Address &address) {
        // add quality of service and caller data
        if (SOCKET_ERROR == ::WSAConnect(sock_, address.GetAddr(), static_cast<int>(address.GetSize()), NULL, NULL, NULL, NULL))
            throw Error();
    }

    Address GetRemoteAddress() const;

    void ShutdownRead() {
        // add disconnect data
        if (SOCKET_ERROR == ::WSARecvDisconnect(sock_, NULL))
            throw Error();
    }

    void ShutdownWrite() {
        // add disconnect data
        if (SOCKET_ERROR == ::WSASendDisconnect(sock_, NULL))
            throw Error();
    }

    void SetLocalAddress(const Address &address, bool force = false) {
        if (force) {
            int value(1);
            CheckError(::setsockopt(sock_, SOL_SOCKET, SO_EXCLUSIVEREUSEADDR, &value, sizeof(int)));
        }

        if (SOCKET_ERROR == ::bind(sock_, address.GetAddr(), static_cast<int>(address.GetSize())))
            throw Error();
    }

    Address GetLocalAddress() const;

    virtual void Flush() {
    }

    size_t Read(WSABUF *buff, DWORD count, Address *address = NULL, bool *partial = NULL);
    size_t Read(char *data, size_t length, Address *address, bool *partial = NULL);

    virtual size_t Read(char *data, size_t length) {
        return Read(data, length, NULL);
    }

    size_t Read(ios::Writer &writer, Address *address = NULL);

    size_t Write(WSABUF *buff, DWORD count, const Address *address = NULL) {
        DWORD bytes, flags(0);
        int size(address == NULL ? 0 : static_cast<int>(address->GetSize()));
        if (SOCKET_ERROR == ::WSASendTo(sock_, buff, count, &bytes, flags, address == NULL ? NULL : address->GetAddr(), size, NULL, NULL))
            throw Error();
        return bytes;
    }

    size_t Write(const byte_t *data, size_t length, const Address *address) {
        // XXX: wtf is a u_long?!?
        WSABUF wsabuf = {static_cast<u_long>(length), const_cast<char *>(data)};
        return Write(&wsabuf, 1, address);
    }

    virtual size_t Write(const byte_t *data, size_t length) {
        return Write(data, length, NULL);
    }

    size_t Write(const cse::String &data, const Address *address = NULL) {
        return Write(data.GetData().Begin(), static_cast<int>(data.GetData().GetSize()), address);
    }

    void Listen(int backlog = 64) {
        if (SOCKET_ERROR == ::listen(sock_, backlog))
            throw Error();
    }

    unsigned GetMaximumMessageLength() {
        unsigned value;
        int length(sizeof(value));
        if (SOCKET_ERROR == ::getsockopt(sock_, SOL_SOCKET, SO_MAX_MSG_SIZE, (char *) &value, &length))
            throw Error();
        return value;
    }

    // this should be a separate mutator
    bool GetTcpNoDelay() {
        BOOL value;
        int length(sizeof(value));
        if (SOCKET_ERROR == ::getsockopt(sock_, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char *>(&value), &length))
            throw Error();
        return value != FALSE;
    }

    // this should be a separate mutator
    void SetTcpNoDelay(bool delay) {
        BOOL value(delay);
        if (SOCKET_ERROR == ::setsockopt(sock_, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<const char *>(&value), sizeof(value)))
            throw Error();
    }
};

namespace be {
template <int Family_, int Type_, int Protocol_>
class SocketType_ :
    public Socket
{
  public:
    SocketType_() :
        Socket(Family_, Type_, Protocol_)
    {
    }
}; }

typedef be::SocketType_<AF_INET, SOCK_STREAM, IPPROTO_TCP> TcpSocket;
typedef be::SocketType_<AF_INET, SOCK_DGRAM, IPPROTO_UDP> UdpSocket;

} }

#endif//MENES_API_WINSOCK_SOCKET_HPP
