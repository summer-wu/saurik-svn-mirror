/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2004  Jay Freeman (saurik)
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

#ifndef MENES_APP_SETUP_HPP
#define MENES_APP_SETUP_HPP

#include "cxx/platform.hpp"
#include "app/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "ext/listset.hpp"
#include "ext/vector.hpp"

namespace app {

class Per;

namespace be {
    typedef ext::ListSet<
        Per *,
        Per *,
        ext::Identity,
        ext::Vector<
            Per *,
            ext::Block<
                Per *,
                ext::TypedAllocator<ext::MallocAllocator>
            >
        >
    > Pers_;

    struct Perss_ {
        Pers_ process_;
        Pers_ thread_;
    };

    APP_DECLARE Perss_ &GetPerss_();
}

class APP_DECLARE Per {
  public:
    virtual void Start() = 0;
    virtual void Stop() = 0;
};

class APP_DECLARE PerProcess :
    public Per
{
  public:
    PerProcess();
    ~PerProcess();
};

class APP_DECLARE PerThread :
    public Per
{
  public:
    PerThread();
    ~PerThread();
};

}

#endif//MENES_APP_SETUP_HPP
