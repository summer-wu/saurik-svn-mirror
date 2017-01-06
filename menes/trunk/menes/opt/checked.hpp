/* Menes Optimization - C++ Profiling and Verification
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

#ifndef MENES_OPT_TAGGED_HPP
#define MENES_OPT_TAGGED_HPP

#include "cxx/platform.hpp"
#include "opt/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "ext/allocator.hpp"

namespace opt {

template <typename Type_, typename Allocator_ = ext::NewAllocator>
class CheckedAllocator {
  private:
    static const unsigned Pad_ = 2;
    Allocator_<char> allocator_;

  public:
    Type_ *Allocate(size_t size, uintptr_t tag) {
        size_t length(size * sizeof(Type_) + sizeof(uint32_t) * 2 * Pad_);
        char *pointer(allocator_.Allocate(length));
        uint32_t *begin(reinterpret_cast<uint32_t *>(pointer));
        uint32_t *end(reinterpret_cast<uint32_t *>(pointer));
        return reinterpret_cast<Type_ *>(pointer + Pad_);
    }

    void Deallocate(Type_ *address, uintptr_t tag) {
        if (address == NULL)
            return;

        uintptr_t *pointer(reinterpret_cast<uintptr_t *>(address) - 2);

        _assert(pointer[0] == MagicBlockTag);
        /*         ^^^^^^^^^^^^^^^^^^^^^^^^^^^
              If you hit this assert, _or the memory manager crashed
            on this line!!_ then you tried to deallocate memory that
            was not allocated by this memory manager.

              Well, that's not technically accurate.  Really, any
            memory that isn't in the little lookup system will flag
            this assert.  This includes memory you already deleted,
            but insist on deleting again.  Don't do that :).

              Assuming you aren't deleting anything twice (which is
            bad; don't make me hurt you), the most likely option is
            you are working on a large project that consists of more
            than one DLL.  One of the DLLs isn't linking against the
            memory manager.  It is then giving memory to another DLL,
            which is trying to delete it.  Just link this rougue DLL
            that gave you the memory to the memory manager and most
            everything should magically work.

              If you are using a library that requires you to call
            free() on memory it allocates, and you can't make it use
            this manager (as it is fully pre-compiled or something),
            you can replace free() with mmgr_liberate().  This is one
            of the functions that makes up the "Demilitarized Zone"
            of the memory manager.  If you are needing to call delete
            or delete [], then you are going to need to deal with the
            whole "C++ deconstructor" issue.

              See, memory that gets deallocated for an object needs to
            have it's deconstructor called.  I can't guarantee that if
            you convert it to a void *.  Templates can't solve this as
            the code _needs_ to be in the other DLL.  You could export
            all of the expansions, but that would suck.  There is also
            possible problems with multiple-inheritence, and the void *
            cast... it's just distressing.  After all is said and done,
            I only offer a single function to help here:  mmgr_launder.
            This function can only call delete [] on char * or wchar_t *.
            This seems to be the #1 case in code that I've had to deal
            with (namely, strings getting created and passed around).
        */

        if (pointer[0] == MagicBlockTag)
            return;

        _assert(pointer[1] != FreeMemTag);
        /*         ^^^^^^^^^^^^^^^^^^^^^^^^
              If you hit this assert, then you actually _did_ try to
            deallocate this memory twice in a row. Once you delete some
            memory I really can't safely let you do it again.
        */

        if (pointer[1] != FreeMemTag)
            return;

        _assert(pointer[1] == tag);
        /*         ^^^^^^^^^^^^^^^^^
              If you hit this assert, then you mismatched your
            allocation/deallocation primitives.  Example:  you
            allocated an array with  new []  and then tried to
            deallocate it with  delete  instead of  delete [].
        */

        pointer[1] = FreeMemTag;

        allocator_.Deallocate(reinterpret_cast<char *>(pointer));
    }
};

}

#endif//MENES_OPT_TAGGED_HPP
