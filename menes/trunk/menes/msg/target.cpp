/* Menes Messaging - Remote Method Call for C++
 * Copyright (C) 2004-2005  Jay Freeman (saurik)
*/

/*
 *        Redistribution and use in source and binary
 * forms, with or without modification, are permitted
 * provided that the following conditions are met:
 *·
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
 *·
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

#include "ext/uuid.hpp"
#include "ios/stdadapters.hpp"

#include "msg/router.hpp"
#include "msg/target.hpp"

#include "log/logging.hpp"
#include "opt/profile.hpp"
#include "szf/saveload-ext.hpp"

namespace msg {

EXT_HANDLESHIM(Target)

void Target::Invoke_(const msg::msgid_t &mid, const MessageBuffer &msg) {
    _assert(false);
}

void Target::Invoke(const MessageBuffer &msg) {
    OPT_TIMER("Target::Invoke()")

    _S<MessageBuffer> copy(msg);

    _S<szf::BinaryMethod> method(&copy, _null<ios::Writer>());

    ext::Uuid id;
    szf::Load(&method, id, "id");

    LOG_THREAD() << "Target::Invoke(" << id << ")" << ios::NewLine;
    Invoke_(id, copy);
}

}
