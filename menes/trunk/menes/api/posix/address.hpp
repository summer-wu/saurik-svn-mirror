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

#ifndef MENES_API_POSIX_ADDRESS_HPP
#define MENES_API_POSIX_ADDRESS_HPP

#include "cxx/platform.hpp"
#include "api/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "api/error.hpp"
#include "ext/vector.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

namespace api {
namespace Posix {

struct API_DECLARE GaiApiTraits  :
    public ErrorTraits<int>
{
    static const unsigned ErrorBase = 10;

    static const char *GetName()  { return "getaddrinfo"; }

    static ErrorCode GetLastError() {
        throw;
    }

    static cse::String GetMessage(const ErrorCode &code) {
        return ::gai_strerror(code);
    }
};

API_DECLARE typedef ErrorImpl<GaiApiTraits> GaiError;

_finline int CheckGaiError(int status) {
    if (status != 0)
        throw GaiError(status);
    return status;
}

class API_DECLARE Address :
    public ext::Vector<uint8_t>
{
  private:
    typedef ext::Vector<uint8_t> Base_;

  public:
    Address() {
    }

    Address(const Address &rhs) :
        Base_(rhs)
    {
    }

    Address(const sockaddr *address, size_t length) :
        Base_(reinterpret_cast<const uint8_t *>(address), length)
    {
    }

    Address(const cse::String &node, const cse::String &service) {
        const _L<Address> &addresses(Resolve(node, service));
        if (addresses.IsEmpty())
            *this = addresses[0];
    }

    int GetFamily() const {
        return GetAddr()->sa_family;
    }

    sockaddr *GetAddr() {
        return reinterpret_cast<sockaddr *>(Begin());
    }

    const sockaddr *GetAddr() const {
        return reinterpret_cast<const sockaddr *>(Begin());
    }

    void ToString(cse::String &host, cse::String &service, bool resolve = false) const;

    static _L<Address> Resolve(const cse::String &node, const cse::String &service);
};

template <typename Traits_>
class AddressImpl :
    public Address
{
  public:
    typedef typename Traits_::SockAddr SockAddr;

  private:
    void Create_() {
        Set(0, sizeof(SockAddr));
        GetAddr()->sa_family = Traits_::Family;
    }

  protected:
    AddressImpl() {
        Create_();
    }

    AddressImpl(const Address &rhs) :
        Address(rhs)
    {
    }

    inline SockAddr *GetAddr_() {
        return reinterpret_cast<SockAddr *>(Begin());
    }

    inline const SockAddr *GetAddr_() const {
        return reinterpret_cast<const SockAddr *>(Begin());
    }

  public:
    inline SockAddr *operator ->() {
        return GetAddr_();
    }
};

class InternetTraits {
  public:
    typedef sockaddr_in SockAddr;
    static const int Family = AF_INET;
};

class API_DECLARE InternetAddress :
    public AddressImpl<InternetTraits>
{
  private:
    typedef AddressImpl<InternetTraits> Base_;

  public:
    typedef uint16_t Port;

    enum {
        Any = 0x00000000,
        Loopback = 0x7f000001,
        Broadcast = 0xffffffff,
        None = 0xffffffff
    };

  public:
    InternetAddress() {
    }

    InternetAddress(const Address &rhs) :
        Base_(rhs)
    {
    }

    InternetAddress(uint32_t host, Port port = 0) {
        SetHost(host);
        SetPort(port);
    }

    Port GetPort() const;
    void SetPort(Port port);

    uint32_t GetHost() const;
    void SetHost(uint32_t host);
};

API_DECLARE extern ios::PrintWriter &operator <<(ios::PrintWriter &out, const Address &address);

} }

#endif//MENES_API_POSIX_ADDRESS_HPP
