/* Oscar - Multi-Threaded Implementation of OSCAR
 * Copyright (C) 2004  Jay Freeman (saurik)
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

#ifndef MENES_NET_OSCAR_USERINFO_HPP
#define MENES_NET_OSCAR_USERINFO_HPP

#include "cxx/platform.hpp"
#include "net/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "ios/getput.hpp"
#include "ios/streams.hpp"

namespace net {
namespace Oscar {

struct UserInfo {
    cse::String screenname;
    uint16_t warnlevel;

    //TlvParser tlv_;

    UserInfo(ios::Reader &stream) {
        screenname.Assign(stream.GetIterator(), ios::GetBig<uint8_t>(stream));
        warnlevel = ios::GetBig<uint16_t>(stream);
        TlvParser tlv(stream, ios::GetBig<uint16_t>(stream));
    }

    bool operator ==(const UserInfo &rhs) const {
        return screenname == rhs.screenname && warnlevel == rhs.warnlevel;
    }

    bool operator !=(const UserInfo &rhs) const {
        return !operator ==(rhs);
    }

    bool operator <(const UserInfo &rhs) const {
        if (screenname != rhs.screenname)
            return screenname < rhs.screenname;
        else if (warnlevel != rhs.warnlevel)
            return warnlevel < rhs.warnlevel;
        return false;
    }
};

} }

#endif//MENES_NET_OSCAR_USERINFO_HPP
