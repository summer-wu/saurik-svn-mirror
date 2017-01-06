/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2003-2005  Jay Freeman (saurik)
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

#include "api/fmod/error.hpp"

#include <fmod.h>

namespace api {
namespace FMOD {

const char *ApiTraits::GetName() {
    return "FMOD";
}

ApiTraits::ErrorCode ApiTraits::GetLastError() {
    return ::FSOUND_GetError();
}

cse::String ApiTraits::GetMessage(const ErrorCode &code) {
    switch (GetCode()) {
        case FMOD_ERR_NONE:             return _B("No errors");
        case FMOD_ERR_BUSY:             return _B("Cannot call this command after FSOUND_Init. Call FSOUND_Close first.");
        case FMOD_ERR_UNINITIALIZED:    return _B("This command failed because FSOUND_Init or FSOUND_SetOutput was not called");
        case FMOD_ERR_INIT:             return _B("Error initializing output device.");
        case FMOD_ERR_ALLOCATED:        return _B("Error initializing output device, but more specifically, the output device is already in use and cannot be reused.");
        case FMOD_ERR_PLAY:             return _B("Playing the sound failed.");
        case FMOD_ERR_OUTPUT_FORMAT:    return _B("Soundcard does not support the features needed for this soundsystem (16bit stereo output)");
        case FMOD_ERR_COOPERATIVELEVEL: return _B("Error setting cooperative level for hardware.");
        case FMOD_ERR_CREATEBUFFER:     return _B("Error creating hardware sound buffer.");
        case FMOD_ERR_FILE_NOTFOUND:    return _B("File not found");
        case FMOD_ERR_FILE_FORMAT:      return _B("Unknown file format");
        case FMOD_ERR_FILE_BAD:         return _B("Error loading file");
        case FMOD_ERR_MEMORY:           return _B("Not enough memory or resources");
        case FMOD_ERR_VERSION:          return _B("The version number of this file format is not supported");
        case FMOD_ERR_INVALID_PARAM:    return _B("An invalid parameter was passed to this function");
        case FMOD_ERR_NO_EAX:           return _B("Tried to use an EAX command on a non EAX enabled channel or output.");
        case FMOD_ERR_CHANNEL_ALLOC:    return _B("Failed to allocate a new channel");
        case FMOD_ERR_RECORD:           return _B("Recording is not supported on this machine");
        case FMOD_ERR_MEDIAPLAYER:      return _B("Windows Media Player not installed so cannot play wma or use internet streaming.");
        case FMOD_ERR_CDDEVICE:         return _B("An error occured trying to open the specified CD device");
    }

    return _B("Unknown Error");
}

} }
