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

#ifndef MENES_EXT_ALLOCATOR_HPP
#define MENES_EXT_ALLOCATOR_HPP

#include "cxx/platform.hpp"
#include "ext/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "ext/types.hpp"
#include "ext/workarounds.hpp"

namespace ext {

struct EXT_DECLARE MallocAllocator {
    static void *Allocate(size_t size);
    static void Deallocate(void *address);
};

struct EXT_DECLARE NewAllocator {
    static void *Allocate(size_t size);
    static void Deallocate(void *address);
};

template <typename Allocator_>
struct TypedAllocator {
    template <typename Type_>
    struct Template {
        struct Result :
            public Allocator_
        {
            void _initialize() {
            }

            EXT_PASSTHRU_CONSTRUCT(Result, Allocator_)

            _finline Type_ *AllocateEx() {
                return AllocateEx(1);
            }

            _finline Type_ *AllocateEx(size_t size) {
                return reinterpret_cast<Type_ *>(Allocator_::Allocate(size * sizeof(Type_)));
            }

            _finline void DeallocateEx(Type_ *address) {
                Allocator_::Deallocate(reinterpret_cast<void *>(address));
            }
        };
    };
};

// XXX: merge this into TypedAllocator... somehow
template <typename Allocator_>
struct StaticTypedAllocator {
    template <typename Type_>
    struct Template {
        struct Result :
            public Allocator_
        {
            _finline static Type_ *AllocateEx() {
                return AllocateEx(1);
            }

            _finline static Type_ *AllocateEx(size_t size) {
                return reinterpret_cast<Type_ *>(Allocator_::Allocate(size * sizeof(Type_)));
            }

            _finline static void DeallocateEx(Type_ *address) {
                Allocator_::Deallocate(reinterpret_cast<void *>(address));
            }
        };
    };
};

struct ConstructorAllocator {
    template <typename Type_>
    struct Template {
        struct Result {
            inline static Type_ *AllocateEx() {
                return new Type_();
            }

            template <typename Type2_>
            inline static Type_ *AllocateEx(const Type2_ &value) {
                return new Type_(value);
            }

            template <typename Type2_>
            inline static Type_ *AllocateEx(Type2_ &value) {
                return new Type_(value);
            }

            inline static void DeallocateEx(Type_ *address) {
                delete address;
            }
        };
    };
};

struct NewArrayAllocator {
    template <typename Type_>
    struct Template {
        struct Result {
            inline static Type_ *AllocateEx(size_t size) {
                return new Type_[size];
            }

            inline static void DeallocateEx(Type_ *address) {
                delete [] address;
            }
        };
    };
};

template < typename Allocator_ = TypedAllocator<NewAllocator> >
struct LeakAllocator {
    template <typename Type_>
    struct Template {
        struct Result :
            public Allocator_::template Template<Type_>::Result
        {
            typedef typename Allocator_::template Template<Type_>::Result Base_;

            inline static Type_ *AllocateEx() {
                return Base_::AllocateEx();
            }

            inline static Type_ *AllocateEx(size_t size) {
                return Base_::AllocateEx(size);
            }

            inline static void DeallocateEx(Type_ *address) {
            }
        };
    };
};

template < typename Allocator_ = TypedAllocator<NewAllocator> >
struct ObjectAllocator {
    template <typename Type_>
    struct Template {
        struct Result :
            public Allocator_::template Template<Type_>::Result
        {
            typedef typename Allocator_::template Template<Type_>::Result Base_;

            inline Type_ *AllocateEx() {
                Type_ *object(Base_::AllocateEx());

                // XXX: this should use the stack
                try {
                    new (object) Type_();
                } _catch {
                    Base_::DeallocateEx(object);
                    throw;
                }

                return object;
            }

            inline void DeallocateEx(Type_ *object) {
                if (object == NULL)
                    return;

                // XXX: this should use the stack
                try {
                    object->~Type_();
                } _catch {
                    Base_::DeallocateEx(object);
                    throw;
                }

                Base_::DeallocateEx(object);
            }
        };
    };
};

template <typename Type_>
struct NullAllocator {
    _finline static Type_ *AllocatorEx(size_t size) {
        return NULL;
    }

    _finline static void Deallocate(Type_ *address) {
    }
};

}

EXT_DECLARE void *operator new(size_t size, ext::Allocator &allocator);
EXT_DECLARE void operator delete(void *address, ext::Allocator &allocator);

#endif//MENES_EXT_ALLOCATOR_HPP
