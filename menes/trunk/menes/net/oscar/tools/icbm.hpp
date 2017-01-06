/* Oscar - Multi-Threaded Implementation of OSCAR
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

#ifndef MENES_NET_OSCAR_TOOLS_ICBM_HPP
#define MENES_NET_OSCAR_TOOLS_ICBM_HPP

#include "cxx/platform.hpp"
#include "net/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "hop/delegate.hpp"

#include "net/oscar/protocol.hpp"
#include "net/oscar/session.hpp"
#include "net/oscar/tool.hpp"

#include <string>

namespace net {
namespace Oscar {

struct IcbmParams {
    uint16_t maxchan_;
    uint32_t flags_;
    uint16_t maxmsglen_;
    uint16_t maxsenderwarn_;
    uint16_t maxrecverwarn_;
    uint32_t minmsginterval_;

    IcbmParams(ios::Reader &segment) :
        maxchan_(ios::GetBig<uint16_t>(segment)),
        flags_(ios::GetBig<uint32_t>(segment)),
        maxmsglen_(ios::GetBig<uint16_t>(segment)),
        maxsenderwarn_(ios::GetBig<uint16_t>(segment)),
        maxrecverwarn_(ios::GetBig<uint16_t>(segment)),
        minmsginterval_(ios::GetBig<uint32_t>(segment))
    {
    }

    void WriteTo(ios::Writer &stream) const {
        ios::PutBig<uint16_t>(stream, 0); // read-only
        ios::PutBig<uint32_t>(stream, flags_);
        ios::PutBig<uint16_t>(stream, maxmsglen_);
        ios::PutBig<uint16_t>(stream, maxsenderwarn_);
        ios::PutBig<uint16_t>(stream, maxrecverwarn_);
        ios::PutBig<uint32_t>(stream, minmsginterval_);
    }
};

std::string NET_DECLARE collapse(std::string screenname);

class NET_DECLARE IcbmTool :
    public Tool
{
  protected:
    Session &session_;

  public:
    hop::Delegate<void (const cse::String &screenname, const cse::String &message)> OnReceive;

    IcbmTool(Session &session);

    virtual bool Process(FlapStream &flaps, const SnacHeader &snac, ios::Buffer &segment);

    IcbmParams GetParams();
    void SetParams(const IcbmParams &parms);

    void Send(const cse::String &screenname, uint16_t channel, uint16_t type, const ios::Buffer &data);

    void Simple(const cse::String &screenname, const cse::String &text);
    void Special(const cse::String &screenname, const Capability &cap, const ios::Buffer &data);

    void Invite(const cse::String &screenname, const cse::String &text, const cse::String &room, uint16_t instance);
};

} }

#endif//MENES_NET_OSCAR_TOOLS_ICBM_HPP
