/* Genessus - _Truly_ Free (non-GPL) Unix .NET Runtime
 * Copyright (C) 2005  Jay Freeman (saurik)
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

/*
  1 Now the serpent was more crafty than any of the wild animals the LORD God had made. He said to the woman, "Did God really say, 'You must not eat from any tree in the garden'?"

  2 The woman said to the serpent, "We may eat fruit from the trees in the garden,
  3 but God did say, 'You must not eat fruit from the tree that is in the middle of the garden, and you must not touch it, or you will die.' "

  4 "You will not surely die," the serpent said to the woman.
  5 "For God knows that when you eat of it your eyes will be opened, and you will be like God, knowing good and evil."

  6 When the woman saw that the fruit of the tree was good for food and pleasing to the eye, and also desirable for gaining wisdom, she took some and ate it. She also gave some to her husband, who was with her, and he ate it.
  7 Then the eyes of both of them were opened, and they realized they were naked; so they sewed fig leaves together and made coverings for themselves.

  8 Then the man and his wife heard the sound of the LORD God as he was walking in the garden in the cool of the day, and they hid from the LORD God among the trees of the garden.
  9 But the LORD God called to the man, "Where are you?"
*/

#include "cxx/standard.hh"
#pragma hdrstop("build/cxx/standard.pch")

#include "api/console.hpp"
#include "api/files.hpp"

#include "app/simple.hpp"

#include "ext/refcount.hpp"
#include "ext/shared.hpp"
#include "ext/stackref.hpp"
#include "cse/string.hpp"
#include "ext/uuid.hpp"

#include "opt/memory.hpp"
#include "opt/profile.hpp"
#include "opt/reports.hpp"

#include "xml/concisexml.hpp"
#include "xml/domwriter.hpp"
#include "xml/element.hpp"
#include "xml/name.hpp"
#include "xml/parse.hpp"
#include "xml/resolver.hpp"
#include "xml/textwriter.hpp"

void Out(_L<int> &a) {
    api::Cout << a.GetSize() << "," << a.GetCapacity() << ">";
    _foreach (_L<int>, i, a)
        api::Cout << *i << ":";
    api::Cout << ios::NewLine;
}

int Main(const app::Options &options) {
    _L<int> ark;
    ark.InsertLast(5);

    _L<int> bark;
    bark.InsertLast(15);
    bark.InsertLast(16);

    Out(ark);
    Out(bark);

    ark = bark;
    Out(ark);

    cse::String script(*options[0]);
    return 0;
}
