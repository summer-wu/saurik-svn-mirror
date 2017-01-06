/* Menes GUI - C++ Sensible Abstract GUI Toolkit
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

#include "cxx/standard.hh"
#pragma hdrstop("build/cxx/standard.pch")

#include "gui1/desktop.hpp"

namespace gui1 {

ext::Uuid Desktop::TypeId(0xe090dbda,0xdf85,0x4df2,0x88,0x13,0x45,0x03,0x0d,0xe1,0xf2,0x29);

float Desktop::GetExtent_(axis_t axis, unit_t unit) const {
    return impl_->GetExtent_(axis, unit);
}

Desktop::Desktop() :
    ImplHolder<Container, IDesktopImpl>(TypeId)
{
}

const Desktop *Desktop::GetDesktop() const {
    return this;
}

Desktop *Desktop::GetDesktop() {
    return this;
}

cse::String Desktop::OpenFile(const cse::String &filter) {
    _assert(impl_ != NULL);
    return impl_->OpenFile(filter);
}

}
