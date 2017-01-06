/* Menes GUI - C++ Sensible Abstract GUI Toolkit
 * Copyright (C) 2003  Jay Freeman (saurik)
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

#ifndef MENES_GUI1_WIN32_KEYBOARD
#define MENES_GUI1_WIN32_KEYBOARD

#include "gui1/keyboard.hpp"

namespace gui1 {
namespace Win32 {

static gui1::Key Keys_[256] = {
    /*0x00*/ Key_Invalid,
    /*0x01*/ Key_Invalid,
    /*0x02*/ Key_Invalid,
    /*0x03*/ Key_Invalid,
    /*0x04*/ Key_Invalid,
    /*0x05*/ Key_Invalid,
    /*0x06*/ Key_Invalid,
    /*0x07*/ Key_Invalid,
    /*0x08*/ Key_Backspace,
    /*0x09*/ Key_Tab,
    /*0x0a*/ Key_Invalid,
    /*0x0b*/ Key_Invalid,
    /*0x0c*/ Key_Invalid,
    /*0x0d*/ Key_Enter,
    /*0x0e*/ Key_Invalid,
    /*0x0f*/ Key_Invalid,

    /*0x10*/ Key_LShift,
    /*0x11*/ Key_LControl,
    /*0x12*/ Key_Invalid,
    /*0x13*/ Key_Invalid,
    /*0x14*/ Key_Invalid,
    /*0x15*/ Key_Invalid,
    /*0x16*/ Key_Invalid,
    /*0x17*/ Key_Invalid,
    /*0x18*/ Key_Invalid,
    /*0x19*/ Key_Invalid,
    /*0x1a*/ Key_Invalid,
    /*0x1b*/ Key_Escape,
    /*0x1c*/ Key_Invalid,
    /*0x1d*/ Key_Invalid,
    /*0x1e*/ Key_Invalid,
    /*0x1f*/ Key_Invalid,

    /*0x20*/ Key_Space,
    /*0x21*/ Key_Invalid,
    /*0x22*/ Key_Invalid,
    /*0x23*/ Key_Invalid,
    /*0x24*/ Key_Invalid,
    /*0x25*/ Key_Left,
    /*0x26*/ Key_Up,
    /*0x27*/ Key_Right,
    /*0x28*/ Key_Down,
    /*0x29*/ Key_Invalid,
    /*0x2a*/ Key_Invalid,
    /*0x2b*/ Key_Invalid,
    /*0x2c*/ Key_Invalid,
    /*0x2d*/ Key_Invalid,
    /*0x2e*/ Key_Invalid,
    /*0x2f*/ Key_Invalid,

    /*0x30*/ Key_0,
    /*0x31*/ Key_1,
    /*0x32*/ Key_2,
    /*0x33*/ Key_3,
    /*0x34*/ Key_4,
    /*0x35*/ Key_5,
    /*0x36*/ Key_6,
    /*0x37*/ Key_7,
    /*0x38*/ Key_8,
    /*0x39*/ Key_9,
    /*0x3a*/ Key_Invalid,
    /*0x3b*/ Key_Invalid,
    /*0x3c*/ Key_Invalid,
    /*0x3d*/ Key_Invalid,
    /*0x3e*/ Key_Invalid,
    /*0x3f*/ Key_Invalid,

    /*0x40*/ Key_Invalid,
    /*0x41*/ Key_A,
    /*0x42*/ Key_B,
    /*0x43*/ Key_C,
    /*0x44*/ Key_D,
    /*0x45*/ Key_E,
    /*0x46*/ Key_F,
    /*0x47*/ Key_G,
    /*0x48*/ Key_H,
    /*0x49*/ Key_I,
    /*0x4a*/ Key_J,
    /*0x4b*/ Key_K,
    /*0x4c*/ Key_L,
    /*0x4d*/ Key_M,
    /*0x4e*/ Key_N,
    /*0x4f*/ Key_O,

    /*0x50*/ Key_P,
    /*0x51*/ Key_Q,
    /*0x52*/ Key_R,
    /*0x53*/ Key_S,
    /*0x54*/ Key_T,
    /*0x55*/ Key_U,
    /*0x56*/ Key_V,
    /*0x57*/ Key_W,
    /*0x58*/ Key_X,
    /*0x59*/ Key_Y,
    /*0x5a*/ Key_Z,
    /*0x5b*/ Key_LWindows,
    /*0x5c*/ Key_RWindows,
    /*0x5d*/ Key_Apps,
    /*0x5e*/ Key_Invalid,
    /*0x5f*/ Key_Invalid,

    /*0x60*/ Key_NumPad0,
    /*0x61*/ Key_NumPad1,
    /*0x62*/ Key_NumPad2,
    /*0x63*/ Key_NumPad3,
    /*0x64*/ Key_NumPad4,
    /*0x65*/ Key_NumPad5,
    /*0x66*/ Key_NumPad6,
    /*0x67*/ Key_NumPad7,
    /*0x68*/ Key_NumPad8,
    /*0x69*/ Key_NumPad9,
    /*0x6a*/ Key_Multiply,
    /*0x6b*/ Key_Add,
    /*0x6c*/ Key_Seperator,
    /*0x6d*/ Key_Subtract,
    /*0x6e*/ Key_Decimal,
    /*0x6f*/ Key_Divide,

    /*0x70*/ Key_F1,
    /*0x71*/ Key_F2,
    /*0x72*/ Key_F3,
    /*0x73*/ Key_F4,
    /*0x74*/ Key_F5,
    /*0x75*/ Key_F6,
    /*0x76*/ Key_F7,
    /*0x77*/ Key_F8,
    /*0x78*/ Key_F9,
    /*0x79*/ Key_F10,
    /*0x7a*/ Key_F11,
    /*0x7b*/ Key_F12,
    /*0x7c*/ Key_F13,
    /*0x7d*/ Key_F14,
    /*0x7e*/ Key_F15,
    /*0x7f*/ Key_F16,

    /*0x80*/ Key_F17,
    /*0x81*/ Key_F18,
    /*0x82*/ Key_F19,
    /*0x83*/ Key_F20,
    /*0x84*/ Key_F21,
    /*0x85*/ Key_F22,
    /*0x86*/ Key_F23,
    /*0x87*/ Key_F24,
    /*0x88*/ Key_Invalid,
    /*0x89*/ Key_Invalid,
    /*0x8a*/ Key_Invalid,
    /*0x8b*/ Key_Invalid,
    /*0x8c*/ Key_Invalid,
    /*0x8d*/ Key_Invalid,
    /*0x8e*/ Key_Invalid,
    /*0x8f*/ Key_Invalid,

    /*0x90*/ Key_NumLock,
    /*0x91*/ Key_ScrollLock,
    /*0x92*/ Key_Invalid,
    /*0x93*/ Key_Invalid,
    /*0x94*/ Key_Invalid,
    /*0x95*/ Key_Invalid,
    /*0x96*/ Key_Invalid,
    /*0x97*/ Key_Invalid,
    /*0x98*/ Key_Invalid,
    /*0x99*/ Key_Invalid,
    /*0x9a*/ Key_Invalid,
    /*0x9b*/ Key_Invalid,
    /*0x9c*/ Key_Invalid,
    /*0x9d*/ Key_Invalid,
    /*0x9e*/ Key_Invalid,
    /*0x9f*/ Key_Invalid,

    /*0xa0*/ Key_Invalid,
    /*0xa1*/ Key_Invalid,
    /*0xa2*/ Key_Invalid,
    /*0xa3*/ Key_Invalid,
    /*0xa4*/ Key_LAlt,
    /*0xa5*/ Key_RAlt,
    /*0xa6*/ Key_Invalid,
    /*0xa7*/ Key_Invalid,
    /*0xa8*/ Key_Invalid,
    /*0xa9*/ Key_Invalid,
    /*0xaa*/ Key_Invalid,
    /*0xab*/ Key_Invalid,
    /*0xac*/ Key_Invalid,
    /*0xad*/ Key_Invalid,
    /*0xae*/ Key_Invalid,
    /*0xaf*/ Key_Invalid,

    /*0xb0*/ Key_Invalid,
    /*0xb1*/ Key_Invalid,
    /*0xb2*/ Key_Invalid,
    /*0xb3*/ Key_Invalid,
    /*0xb4*/ Key_Invalid,
    /*0xb5*/ Key_Invalid,
    /*0xb6*/ Key_Invalid,
    /*0xb7*/ Key_Invalid,
    /*0xb8*/ Key_Invalid,
    /*0xb9*/ Key_Invalid,
    /*0xba*/ Key_SemiColon,
    /*0xbb*/ Key_Invalid,
    /*0xbc*/ Key_Comma,
    /*0xbd*/ Key_Period,
    /*0xbe*/ Key_Invalid,
    /*0xbf*/ Key_Slash,

    /*0xc0*/ Key_BackTick,
    /*0xc1*/ Key_Invalid,
    /*0xc2*/ Key_Invalid,
    /*0xc3*/ Key_Invalid,
    /*0xc4*/ Key_Invalid,
    /*0xc5*/ Key_Invalid,
    /*0xc6*/ Key_Invalid,
    /*0xc7*/ Key_Invalid,
    /*0xc8*/ Key_Invalid,
    /*0xc9*/ Key_Invalid,
    /*0xca*/ Key_Invalid,
    /*0xcb*/ Key_Invalid,
    /*0xcc*/ Key_Invalid,
    /*0xcd*/ Key_Invalid,
    /*0xce*/ Key_Invalid,
    /*0xcf*/ Key_Invalid,

    /*0xd0*/ Key_Invalid,
    /*0xd1*/ Key_Invalid,
    /*0xd2*/ Key_Invalid,
    /*0xd3*/ Key_Invalid,
    /*0xd4*/ Key_Invalid,
    /*0xd5*/ Key_Invalid,
    /*0xd6*/ Key_Invalid,
    /*0xd7*/ Key_Invalid,
    /*0xd8*/ Key_Invalid,
    /*0xd9*/ Key_Invalid,
    /*0xda*/ Key_Invalid,
    /*0xdb*/ Key_LBracket,
    /*0xdc*/ Key_BackSlash,
    /*0xdd*/ Key_RBracket,
    /*0xde*/ Key_Apostrophy,
    /*0xdf*/ Key_Invalid,

    /*0xe0*/ Key_Invalid,
    /*0xe1*/ Key_Invalid,
    /*0xe2*/ Key_Invalid,
    /*0xe3*/ Key_Invalid,
    /*0xe4*/ Key_Invalid,
    /*0xe5*/ Key_Invalid,
    /*0xe6*/ Key_Invalid,
    /*0xe7*/ Key_Invalid,
    /*0xe8*/ Key_Invalid,
    /*0xe9*/ Key_Invalid,
    /*0xea*/ Key_Invalid,
    /*0xeb*/ Key_Invalid,
    /*0xec*/ Key_Invalid,
    /*0xed*/ Key_Invalid,
    /*0xee*/ Key_Invalid,
    /*0xef*/ Key_Invalid,

    /*0xf0*/ Key_Invalid,
    /*0xf1*/ Key_Invalid,
    /*0xf2*/ Key_Invalid,
    /*0xf3*/ Key_Invalid,
    /*0xf4*/ Key_Invalid,
    /*0xf5*/ Key_Invalid,
    /*0xf6*/ Key_Invalid,
    /*0xf7*/ Key_Invalid,
    /*0xf8*/ Key_Invalid,
    /*0xf9*/ Key_Invalid,
    /*0xfa*/ Key_Invalid,
    /*0xfb*/ Key_Invalid,
    /*0xfc*/ Key_Invalid,
    /*0xfd*/ Key_Invalid,
    /*0xfe*/ Key_Invalid,
    /*0xff*/ Key_Invalid
};

} }

#endif//MENES_GUI1_WIN32_KEYBOARD
