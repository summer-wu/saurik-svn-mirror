/* Ninetjer - _Truly_ Free (non-GPL) Unix .NET Runtime
 * Copyright (C) 2004-2005  Jay Freeman (saurik)
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

#include "api/console.hpp"
#include "api/files.hpp"

#include "app/simple.hpp"
#include "hop/bind.hpp"

#include "ext/refcount.hpp"
#include "ext/shared.hpp"
#include "ext/stackref.hpp"
#include "cse/string.hpp"
#include "ext/uuid.hpp"

#include "imc/msn/client.hpp"
#include "imc/oscar/client.hpp"

#include "opt/memory.hpp"
#include "opt/profile.hpp"
#include "opt/reports.hpp"

void BuddyOnline(imc::Oscar::Client *client, const cse::String &username) {
    api::Cout << "(" << username << ") [" << client->GetAwayMessage(username) << "]" << ios::NewLine;
}

void MessageReceive(imc::Oscar::Client *client, const cse::String &username, const cse::String &message) {
    api::Cout << "<" << username << "> " << message << ios::NewLine;
}

int Main(const app::Options &options) {
    imc::Oscar::Client oscar(*options[0], *options[1]);

    oscar.OnBuddyOnline += hop::Bind<0>(&BuddyOnline, &oscar);
    oscar.OnMessageReceive += hop::Bind<0>(&MessageReceive, &oscar);

    {
        net::Oscar::StringSet buddies;

        buddies.Insert("RobComp007");
        buddies.Insert("Coelafishie");
        buddies.Insert("MirrorOfHumanity");
        buddies.Insert("saurik"); // Jay
        buddies.Insert("DouglasWTh"); // Douglas
        buddies.Insert("fish of goat"); // Seth K.
        buddies.Insert("phusfloen");
        buddies.Insert("MirthMaid");
        buddies.Insert("Tobasc0Cat"); // Mika
        buddies.Insert("TheDarkestHobbit"); // Matt
        buddies.Insert("agenthh"); // Hans
        buddies.Insert("ttyjoe"); // Hans

        oscar.AddBuddies(buddies);
    }

    //imc::MsnMessenger::Client msn(*options[2], *options[3]);

    ext::Buffer line;
    while (ios::ReadLine(api::Cin, line)) {
        ext::Buffer::Iterator space(ext::FindFirst(line, ' '));
        ext::Buffer name(line.Begin(), space);
        ext::Buffer text(space + 1, line.End());
        oscar.SendMessage(name, text);
    }

    return 0;
}
