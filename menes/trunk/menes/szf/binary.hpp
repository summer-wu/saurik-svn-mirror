/* Menes Optimization - C++ Profiling and Verification
 * Copyright (C) 2003-2004  Jay Freeman (saurik)
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

#ifndef MENES_SZF_BINARY_HPP
#define MENES_SZF_BINARY_HPP

#include "cxx/platform.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "com/factory.hpp"
#include "com/registrar.hpp"

#include "szf/buffer.hpp"
#include "szf/methodimpl.hpp"

namespace szf {

class SZF_DECLARE BinaryMethod :
    public MethodImpl,
    public IBufferMethod
{
    COM_CLASSNAME("szf::BinaryMethod")
    COM_CLASSUUID(ef,90,4e,3b,29,d8,40,08,90,e5,20,d2,98,e8,f7,44)

    SZF_COMINSTANCE()

  private:
    ios::Reader *in_;
    ios::Writer *out_;

  public:
    BinaryMethod();
    BinaryMethod(ios::Reader *in, ios::Writer *out);

    ~BinaryMethod();

    virtual void OpenScope(const char *name);
    virtual void CloseScope();

    virtual void SaveBoolean(bool value, const char *name);
    virtual void LoadBoolean(bool &value, const char *name);

    virtual void SaveSigned(int64_t value, const char *name);
    virtual void LoadSigned(int64_t &value, const char *name);

    virtual void SaveUnsigned(uint64_t value, const char *name);
    virtual void LoadUnsigned(uint64_t &value, const char *name);

    virtual void SaveFloating(double value, const char *name);
    virtual void LoadFloating(double &value, const char *name);

    virtual void SaveData(const char *data, size_t length, const char *name);
    virtual void LoadData(char *data, size_t length, const char *name);

    virtual void Bind(ios::Reader *in, ios::Writer *out);
};

}

#endif//MENES_SZF_BINARY_HPP