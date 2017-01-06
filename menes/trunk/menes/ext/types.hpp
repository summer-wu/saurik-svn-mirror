/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2004-2005  Jay Freeman (saurik)
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
 * LIABLE FOR ANY DITYPES, INDITYPES, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef MENES_EXT_TYPES_HPP
#define MENES_EXT_TYPES_HPP

#include "cxx/platform.hpp"
#include "ext/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "etl/bind.hpp"
#include "etl/types.hpp"

namespace ext {

struct LessThan;
struct Identity;
struct First;

struct NewAllocator;

template <typename Allocator_>
struct TypedAllocator;

struct ConstructorAllocator;

template < typename Type_, typename Allocator_ = TypedAllocator<NewAllocator> >
class Pointer;

template < typename Element_, typename Allocator_ = TypedAllocator<NewAllocator> >
class Block;

template < typename Element_, typename Allocator_ = NewAllocator >
class SharedBlock;

template < typename Element_, typename Block_ = Block< Element_, TypedAllocator<NewAllocator> > >
class Vector;

typedef Vector< byte_t, SharedBlock<byte_t> > Buffer;

typedef Vector< uint8_t, SharedBlock<uint8_t> > Buffer8;
typedef Vector< uint16_t, SharedBlock<uint16_t> > Buffer16;
typedef Vector< uint32_t, SharedBlock<uint32_t> > Buffer32;

template <
    typename Element_,
    typename Key_ = Element_,
    typename KeyOf_ = Identity,
    typename List_ = Vector<Element_>
> class ListSet;

template <
    typename Element_,
    typename Key_ = Element_,
    typename KeyOf_ = Identity,
    typename LessThan_ = LessThan
> class RedBlackSet;

template <typename Set_, typename Key_, typename Value_> class Map;

template <typename Key_, typename Value_>
class ListMap :
    public Map<ListSet<etl::Pair<Key_, Value_>, Key_, First>, Key_, Value_>
{
};

template <
    typename Key_,
    typename Value_,
    typename LessThan_ = LessThan
> class RedBlackMap :
    public Map<RedBlackSet<etl::Pair<Key_, Value_>, Key_, First, LessThan_>, Key_, Value_>
{
};

class Uuid;

class FreeLock;

template <
    typename Allocator_ = TypedAllocator<NewAllocator>,
    typename Lock_ = FreeLock
> class SubAllocator;

template <typename Type_>
struct ReferenceAllocator;

template < typename Counted_, typename Allocator_ = etl::Bind1<ReferenceAllocator> >
class ReferenceManager;

template < typename Counted_, typename Manager_ = ReferenceManager<Counted_> >
class Handle;

struct Allocator;

template <typename Type_>
struct AllocatorEx;

template <typename Type_>
class IndirectAllocatorEx;

}

#endif//MENES_EXT_TYPES_HPP
