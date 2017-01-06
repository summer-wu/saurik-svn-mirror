/* Menes - C++ High-Level Utility Library
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

#ifndef MENES_EXT_UUID_HPP
#define MENES_EXT_UUID_HPP

#include "cxx/platform.hpp"
#include "ext/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#ifdef MENES_STL_STREAMABLE
#include <iostream> //OK
#endif

#include "ios/printreader.hpp"
#include "ios/printwriter.hpp"

namespace ext {

class EXT_DECLARE Uuid {
  public:
    enum Variant {
        Ncs = 0, // Network Computing System
        Dce = 1, // Distributed Computing Environment
        Guid = 3, // Microsoft
        Reserved = 7
    };

    enum Version {
        DceRegular = 1,
        DceSecurity = 2
    };

    static const Uuid Null;
    static const size_t Size = 16;

  private:
    uint8_t data_[Size];

  public:
    Uuid();

    Uuid(uint32_t data1, uint16_t data2, uint16_t data3, const uint8_t data4[8]);
    Uuid(uint32_t data1, uint16_t data2, uint16_t data3, uint32_t data4a, uint32_t data4b);
    Uuid(uint32_t data1, uint16_t data2, uint16_t data3, uint64_t data4);
    Uuid(uint32_t data1, uint16_t data2, uint16_t data3, uint8_t data4a, uint8_t data4b, uint8_t data4c, uint8_t data4d, uint8_t data4e, uint8_t data4f, uint8_t data4g, uint8_t data4h);
    Uuid(uint8_t data1a, uint8_t data1b, uint8_t data1c, uint8_t data1d, uint8_t data2a, uint8_t data2b, uint8_t data3a, uint8_t data3b, uint8_t data4a, uint8_t data4b, uint8_t data4c, uint8_t data4d, uint8_t data4e, uint8_t data4f, uint8_t data4g, uint8_t data4h);

    bool operator <(const Uuid &rhs) const;
    bool operator ==(const Uuid &rhs) const;
    bool operator !=(const Uuid &rhs) const;

    Uuid &operator =(const Uuid &rhs);

    uint64_t GetTimestamp() const;
    uint64_t GetClockSequence() const;

    Variant GetVariant() const;
    Version GetVersion() const;

    const uint8_t *GetData() const;
    uint8_t *GetData();
};

EXT_DECLARE ios::PrintReader &operator >>(ios::PrintReader &lhs, Uuid &rhs);
EXT_DECLARE ios::PrintWriter &operator <<(ios::PrintWriter &lhs, const Uuid &rhs);

}

#endif//MENES_EXT_UUID_HPP
