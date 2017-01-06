/* Menes GUI - C++ Sensible Abstract GUI Toolkit
 * Copyright (C) 2002-2003  Jay Freeman (saurik)
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

#ifndef MENES_GUI1_KEYBOARD_HPP
#define MENES_GUI1_KEYBOARD_HPP

namespace gui1 {

enum Key {
    Key_Invalid     = 0x00,

    Key_Backspace   = 0x08,
    Key_Tab         = 0x09,
    Key_Enter       = 0x0a,
    Key_Escape      = 0x1b,

    Key_Space       = 0x20,

    Key_Apostrophy  = 0x27,
    Key_Comma       = 0x2c,
    Key_Hyphen      = 0x2d,
    Key_Period      = 0x2e,
    Key_Slash       = 0x2f,

    Key_0           = 0x30,
    Key_1           = 0x31,
    Key_2           = 0x32,
    Key_3           = 0x33,
    Key_4           = 0x34,
    Key_5           = 0x35,
    Key_6           = 0x36,
    Key_7           = 0x37,
    Key_8           = 0x38,
    Key_9           = 0x39,

    Key_SemiColon   = 0x3b,
    Key_Equals      = 0x3d,

    Key_A           = 0x41,
    Key_B           = 0x42,
    Key_C           = 0x43,
    Key_D           = 0x44,
    Key_E           = 0x45,
    Key_F           = 0x46,
    Key_G           = 0x47,
    Key_H           = 0x48,
    Key_I           = 0x49,
    Key_J           = 0x4a,
    Key_K           = 0x4b,
    Key_L           = 0x4c,
    Key_M           = 0x4d,
    Key_N           = 0x4e,
    Key_O           = 0x4f,
    Key_P           = 0x50,
    Key_Q           = 0x51,
    Key_R           = 0x52,
    Key_S           = 0x53,
    Key_T           = 0x54,
    Key_U           = 0x55,
    Key_V           = 0x56,
    Key_W           = 0x57,
    Key_X           = 0x58,
    Key_Y           = 0x59,
    Key_Z           = 0x5a,

    Key_LBracket    = 0x5b,
    Key_BackSlash   = 0x5c,
    Key_RBracket    = 0x5d,

    Key_PrintScreen = 0x5e,
    Key_Pause       = 0x5f,

    Key_BackTick    = 0x60,

    Key_PageUp      = 0x70,
    Key_PageDown    = 0x71,
    Key_Home        = 0x72,
    Key_End         = 0x73,

    Key_Left        = 0x74,
    Key_Up          = 0x75,
    Key_Right       = 0x76,
    Key_Down        = 0x77,

    Key_Insert      = 0x78,
    Key_Delete      = 0x79,

    Key_LWindows    = 0x7a,
    Key_RWindows    = 0x7b,
    Key_Apps        = 0x7c,

    Key_NumLock     = 0x7d,
    Key_ScrollLock  = 0x7e,
    Key_CapsLock    = 0x7f, //!

    Key_NumPad0     = 0x80,
    Key_NumPad1     = 0x81,
    Key_NumPad2     = 0x82,
    Key_NumPad3     = 0x83,
    Key_NumPad4     = 0x84,
    Key_NumPad5     = 0x85,
    Key_NumPad6     = 0x86,
    Key_NumPad7     = 0x87,
    Key_NumPad8     = 0x88,
    Key_NumPad9     = 0x89,

    Key_Multiply    = 0x8a,
    Key_Add         = 0x8b,
    Key_Seperator   = 0x8c,
    Key_Subtract    = 0x8c,
    Key_Decimal     = 0x8e,
    Key_Divide      = 0x8f,

    Key_F1          = 0x90,
    Key_F2          = 0x91,
    Key_F3          = 0x92,
    Key_F4          = 0x93,
    Key_F5          = 0x94,
    Key_F6          = 0x95,
    Key_F7          = 0x96,
    Key_F8          = 0x97,
    Key_F9          = 0x98,
    Key_F10         = 0x99,
    Key_F11         = 0x9a,
    Key_F12         = 0x9b,
    Key_F13         = 0x9c,
    Key_F14         = 0x9d,
    Key_F15         = 0x9e,
    Key_F16         = 0x9f,
    Key_F17         = 0xa0,
    Key_F18         = 0xa1,
    Key_F19         = 0xa2,
    Key_F20         = 0xa3,
    Key_F21         = 0xa4,
    Key_F22         = 0xa5,
    Key_F23         = 0xa6,
    Key_F24         = 0xa7,

    Key_LShift      = 0xb0,
    Key_RShift      = 0xb1,

    Key_LControl    = 0xb2,
    Key_RControl    = 0xb3,

    Key_LAlt        = 0xb4,
    Key_RAlt        = 0xb5
};

}

#endif//MENES_GUI1_KEYBOARD_HPP
