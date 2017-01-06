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
 * LIABLE FOR ANY DISTANDARD, INDISTANDARD, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef MENES_EXT_STANDARD_HH
#define MENES_EXT_STANDARD_HH

#include "cxx/platform.hpp"

#include "cxx/assert.hpp"
#include "cxx/casts.hpp"
#include "cxx/for.hpp"
#include "cxx/null.hpp"

#include "ext/foreach.hpp"

#ifndef CFG_SMALLQUOTA
#include "api/console.hpp"
#include "api/locks.hpp"

#include "cse/lexical.hpp"
#include "cse/string.hpp"

#include "ext/exception.hpp"
#include "ext/heapref.hpp"
#include "ext/helpers.hpp"
#include "ext/iterator.hpp"
#include "ext/list.hpp"
#include "ext/listset.hpp"
#include "ext/map.hpp"
#include "ext/redblackset.hpp"
#include "ext/refcount.hpp"
#include "ext/scopes.hpp"
#include "ext/stackref.hpp"
#include "ext/vector.hpp"

#include "hop/function.hpp"

#include "ios/buffer.hpp"
#include "ios/helpers.hpp"
#include "ios/printreader.hpp"
#include "ios/printwriter.hpp"
#include "ios/streams.hpp"
#include "ios/string.hpp"

#include "opt/profile.hpp"
#endif

#endif//MENES_EXT_STANDARD_HH
