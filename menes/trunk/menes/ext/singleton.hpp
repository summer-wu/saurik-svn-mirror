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

#ifndef MENES_EXT_SINGLETON_HPP
#define MENES_EXT_SINGLETON_HPP

#include "cxx/platform.hpp"
#include "ext/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "etl/alignment.hpp"

#include "ext/heapobject.hpp"
#include "ext/pointer.hpp"

namespace ext {

template <typename Interface_>
class PublicSingleton :
    public SmartPointer< Interface_, Interface_ *, PublicSingleton<Interface_> >
{
    friend class SmartPointer< Interface_, Interface_ *, PublicSingleton<Interface_> >;

  private:
    virtual Interface_ *GetSmartPointer_() const = 0;

  public:
    virtual void Create() = 0;
    virtual void Clear() = 0;
};

template <typename Interface_, typename Type_>
class PrivateSingleton :
    public PublicSingleton<Interface_>
{
  private:
    // XXX: LazyHeapObject is the wrong abstraction... close, though
    LazyHeapObject<Type_> instance_;

    virtual Type_ *GetSmartPointer_() const {
        _assert(!instance_.IsEmpty());
        return instance_;
    }

  public:
    virtual void Create() {
        instance_.Create();
    }

    virtual void Clear() {
        instance_.Clear();
    }
};

// XXX: ok, so where do these get deconstructed?!
template <typename Type_>
struct StaticPermanent {
    static Type_ &GetInstance() {
        // this cannot be unnamed on Mac OSX
        static union {
            typename etl::PlainOldAlignment< etl::Cons<Type_> >::Result align;
            char buffer[sizeof(Type_)];
        } data;

        Type_ *type(reinterpret_cast<Type_ *>(data.buffer));

        // XXX: this obviously needs a lock
        static bool allocated(false);
        if (!allocated) {
            allocated = true;
            new (type) Type_();
        }

        return *type;
    }
};

}

template <typename Type_>
_finline Type_ &_1() {
    static Type_ instance;
    return instance;
}

#endif//MENES_EXT_SINGLETON_HPP
