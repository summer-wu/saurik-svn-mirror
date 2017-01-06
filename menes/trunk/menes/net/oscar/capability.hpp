/* Oscar - Multi-Threaded Implementation of OSCAR
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

#ifndef MENES_NET_OSCAR_CAPS_HPP
#define MENES_NET_OSCAR_CAPS_HPP

#include "cxx/platform.hpp"
#include "net/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "ext/listset.hpp"
#include "ext/uuid.hpp"

namespace net {
namespace Oscar {

typedef ext::Uuid Capability;
typedef ext::ListSet<Capability> Capabilities;

static const Capability NullCapability             (0x00000000, 0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
static const Capability ChatCapability             (0x748f2420, 0x6287, 0x11d1, 0x82, 0x22, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00);

static const Capability VoiceChatCapability        (0x09461341, 0x4c7f, 0x11d1, 0x82, 0x22, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00);
static const Capability DirectPlayCapability       (0x09461342, 0x4c7f, 0x11d1, 0x82, 0x22, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00);
static const Capability SendFileCapability         (0x09461343, 0x4c7f, 0x11d1, 0x82, 0x22, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00);
static const Capability RouteFinderCapability      (0x09461344, 0x4c7f, 0x11d1, 0x82, 0x22, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00);
static const Capability ImImageCapability          (0x09461345, 0x4c7f, 0x11d1, 0x82, 0x22, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00);
static const Capability BuddyIconCapability        (0x09461346, 0x4c7f, 0x11d1, 0x82, 0x22, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00);
static const Capability SaveStocksCapability       (0x09461347, 0x4c7f, 0x11d1, 0x82, 0x22, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00);
static const Capability ReceiveFileCapability      (0x09461348, 0x4c7f, 0x11d1, 0x82, 0x22, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00);
static const Capability IcqServerRelayCapability   (0x09461349, 0x4c7f, 0x11d1, 0x82, 0x22, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00);
static const Capability Games1Capability           (0x0946134a, 0x4c7f, 0x11d1, 0x82, 0x22, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00);
static const Capability Games2Capability           (0x0946134a, 0x4c7f, 0x11d1, 0x22, 0x82, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00);
static const Capability BuddyListTransferCapability(0x0946134b, 0x4c7f, 0x11d1, 0x82, 0x22, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00);

static const Capability IcqCompatibleCapability    (0x0946134d, 0x4c7f, 0x11d1, 0x82, 0x22, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00);
static const Capability Utf8MessagesCapability     (0x0946134e, 0x4c7f, 0x11d1, 0x82, 0x22, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00);

static const Capability RtfMessagesCapability      (0x97b12751, 0x243c, 0x4334, 0xad, 0x22, 0xd6, 0xab, 0xf7, 0x3f, 0x14, 0x92);

static const Capability IcqUnknown1Capability      (0xa0e93f37, 0x4c7f, 0x11d1, 0x82, 0x22, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00);
static const Capability IcqUnknown2Capability      (0x10cf40d1, 0x4c7f, 0x11d1, 0x82, 0x22, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00);
static const Capability IcqUnknown3Capability      (0x2e7a6475, 0xfadf, 0x4dc8, 0x88, 0x6f, 0xea, 0x35, 0x95, 0xfd, 0xb6, 0xdf);
static const Capability IcqUnknown4Capability      (0x563fc809, 0x0b6f, 0x41bd, 0x9f, 0x79, 0x42, 0x26, 0x09, 0xdf, 0xa2, 0xf3);

static const Capability TrillianSecureImCapability (0xf2e7c7f4, 0xfead, 0x4dfb, 0xb2, 0x35, 0x36, 0x79, 0x8b, 0xdf, 0x00, 0x00);

// XXX: the ??'s were marked in the documentation as "xx"; maybe version number?
// static const Capability SimKopeteMarkerCapability  (0x97b12751, 0x243c, 0x4334, 0xad, 0x22, 0xd6, 0xab, 0xf7, 0x3f, 0x14, 0x??);

// XXX: I have found no documentation of this other than from this file
// static const Capability ApInfoCapability(0xaa4a32b5, 0xf884, 0x48c6, 0xa3, 0xd7, 0x8c, 0x50, 0x97, 0x19, 0xfD, 0x5b);

} }

#endif//MENES_NET_OSCAR_CAPS_HPP
