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

#ifndef MENES_NET_OSCAR_FAMILIES_HPP
#define MENES_NET_OSCAR_FAMILIES_HPP

#include "cxx/platform.hpp"
#include "net/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

namespace net {
namespace Oscar {

enum Families {
    GenericFamily               = 0x0001,
    InformationFamily           = 0x0002,
    BuddyListFamily             = 0x0003,
    IcbmFamily                  = 0x0004,
    AdvertisementsFamily        = 0x0005,
    InvitationFamily            = 0x0006,
    AdministrativeFamily        = 0x0007,
    PopupNoticesFamily          = 0x0008,
    PrivacyFamily               = 0x0009,
    UserLookupFamily            = 0x000a,
    UsageStatsFamily            = 0x000b,
    TranslationFamily           = 0x000c,
    ChatNavigationFamily        = 0x000d,
    ChatFamily                  = 0x000e,
    DirectorySearchFamily       = 0x000f,
    BuddyIconsFamily            = 0x0010,
    ServerSideInformationFamily = 0x0013,
    IcqSpecificFamily           = 0x0015,
    AuthorizationFamily         = 0x0017
    //oft                         = 0xfffe
};

} }

#endif//MENES_NET_OSCAR_FAMILIES_HPP
