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

#include "gui1/win32/win32_surface.hpp"

#include "api/win32/error.hpp"

namespace gui1 {
namespace Win32 {

ext::Uuid CanvasWin32::TypeId(0xcf60d1d5,0xe322,0x4c7c,0x82,0x30,0xb4,0xeb,0x22,0x00,0x72,0x2f);

void CanvasWin32::SetHandle_(HWND handle) {
    _assert(usage_ == 0 && context_ == NULL);
    handle_ = handle;
}

CanvasWin32::CanvasWin32() :
    handle_(NULL),
    context_(NULL),
    usage_(0)
{
}

CanvasWin32::CanvasWin32(HWND handle) :
    handle_(handle),
    context_(NULL),
    usage_(0)
{
}

CanvasWin32::~CanvasWin32() {
    SetHandle_(NULL);
}

void CanvasWin32::BeginDraw() {
    if (usage_++ == 0) {
        context_ = ::GetDC(handle_);
        if (context_ == NULL)
            throw api::Win32::Error();
    }
}

void CanvasWin32::EndDraw() {
    _assert(usage_ != 0);
    if (--usage_ == 0) {
        // XXX: error handling?!?
        ::ReleaseDC(handle_, context_);
    }
}

void CanvasWin32::SetColor(const Color &color) {
    if (CLR_INVALID == ::SetDCPenColor(context_, RGB(color[0], color[1], color[2])))
        throw api::Win32::Error();
}

void CanvasWin32::MoveTo(const alg::Vector2f &pos) {
    if (FALSE == ::MoveToEx(context_, pos[0], pos[1], NULL))
        throw api::Win32::Error();
}

void CanvasWin32::LineTo(const alg::Vector2f &pos) {
    if (FALSE == ::LineTo(context_, pos[0], pos[1]))
        throw api::Win32::Error();
}

void CanvasWin32::SetPoint(const alg::Vector2f &pos, const Color &color) {
    // XXX: there's a way to do this that preserves alpha blending...
    if (CLR_INVALID == ::SetPixel(context_, pos[0], pos[1], RGB(color[0], color[1], color[2])))
        throw api::Win32::Error();
}

HWND CanvasWin32::GetHandle() {
    return handle_;
}

} }
