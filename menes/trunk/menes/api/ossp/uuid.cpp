/* Menes - C++ High-Level Utility Library
 * Copyright (C) 2007  Jay Freeman (saurik)
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

#include "api/ossp/uuid.hpp"
#include "ext/uuid.hpp"

#include <cstdlib>

namespace api {
namespace Ossp {

ext::Uuid Uuid::Create() {
    return CreateSequential();
}

ext::Uuid Uuid::CreateRandom() {
    Type id;
    ::uuid_create(&id);
    ::uuid_make(id, UUID_MAKE_V4);
    ext::Uuid value;
    Convert(value, id);
    ::uuid_destroy(id);
    return value;
}

ext::Uuid Uuid::CreateSequential() {
    Type id;
    ::uuid_create(&id);
    ::uuid_make(id, UUID_MAKE_V1);
    ext::Uuid value;
    Convert(value, id);
    ::uuid_destroy(id);
    return value;
}

void Uuid::Convert(ext::Uuid &to, const Type &from) {
    void *data;
    size_t size;
    ::uuid_export(from, UUID_FMT_BIN, &data, &size);
    _assert(size == ext::Uuid::Size);
    ::memcpy(to.GetData(), data, size);
    ::free(data);
}

void Uuid::Convert(Type &to, const ext::Uuid &from) {
    ::uuid_import(to, UUID_FMT_BIN, from.GetData(), ext::Uuid::Size);
}

ext::Uuid Uuid::Convert(const Type &from) {
    ext::Uuid to;
    Convert(to, from);
    return to;
}

} }
