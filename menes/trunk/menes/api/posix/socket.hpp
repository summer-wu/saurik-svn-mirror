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

#ifndef MENES_API_POSIX_SOCKET_HPP
#define MENES_API_POSIX_SOCKET_HPP

#include "cxx/platform.hpp"
#include "api/linkage.hpp"

#include "api/posix/address.hpp"
#include "api/posix/error.hpp"
#include "api/posix/handle.hpp"

#include "ios/streams.hpp"

#include <sys/uio.h>
#include <unistd.h>

namespace api {
namespace Posix {

class Socket :
    public ios::Reader,
    public ios::Writer
{
  protected:
    Handle sock_;

    void Initialize_();

  public:
    Socket(int family, int type, int protocol);

    // XXX: this is really dangerous as it looks like a copy constructor
    Socket(Socket &server, Address *address);

    void Connect(const Address &address) {
        CheckError(::connect(sock_, address.GetAddr(), address.GetSize()));
    }

    Address GetRemoteAddress() const;

    void ShutdownRead() {
        CheckError(::shutdown(sock_, SHUT_RD));
    }

    void ShutdownWrite() {
        CheckError(::shutdown(sock_, SHUT_WR));
    }

    /* // XXX: this should be rethought
    void Keep(); */

    void SetLocalAddress(const Address &address, bool force = false);
    Address GetLocalAddress() const;

    // XXX: why didn't this work?
    virtual void Flush() {
        //CheckError(::fsync(sock_));
    }

    size_t Read(const iovec *buff, int count, Address *address = NULL);
    size_t Read(char *data, size_t length, Address *address);

    virtual size_t Read(char *data, size_t length) {
        return Read(data, length, NULL);
    }

    size_t Read(ios::Writer &writer, Address *address = NULL);
    size_t Write(iovec *buff, int count, const Address *address = NULL);

    size_t Write(const byte_t *data, size_t length, const Address *address) {
        iovec iov = {const_cast<char *>(data), length};
        return Write(&iov, 1, address);
    }

    virtual size_t Write(const byte_t *data, size_t length) {
        return Write(data, length, NULL);
    }

    void Listen(int backlog = 64) {
        CheckError(::listen(sock_, backlog));
    }

    /*unsigned GetMaximumMessageLength() {
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
    }*/
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

#endif//MENES_API_POSIX_SOCKET_HPP
