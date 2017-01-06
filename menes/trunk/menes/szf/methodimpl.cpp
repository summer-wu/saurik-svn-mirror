/* Menes Optimization - C++ Profiling and Verification
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

#include "com/registrar.hpp"

#include "cse/string.hpp"
#include "ext/uuid.hpp"

#include "szf/methodimpl.hpp"
#include "szf/saveload.hpp"

namespace szf {

void *MethodImpl::GetContext_(const std::type_info *type) const {
    return context_.GetOr(type, NULL);
}

void MethodImpl::SetContext_(const std::type_info *type, void *context) {
    context_[type] = context;
}

MethodImpl::MethodImpl() :
    offset_(0),
    byname_(false)
{
}

void MethodImpl::ClearObjects() {
    offset_ += objects_.GetSize();
    objects_.Clear();
}

void MethodImpl::SaveObject(const _R<Serializable> &value, const char *name) {
    OpenScope(name);

    if (value.IsEmpty()) {
        SaveBoolean(true, "cached");
        SaveUnsigned(0, "index");
    } else {
        ObjectList::Iterator object(ext::FindFirst(objects_, value));

        if (object != objects_.End()) {
            SaveBoolean(true, "cached");
            SaveUnsigned(object - objects_.Begin() + offset_ + 1, "index");
        } else {
            SaveBoolean(false, "cached");
            objects_.InsertLast(value);
            SaveUuid(value->GetClassId(), "type");

            OpenScope("serialization");
            value->Save(this);
            CloseScope();
        }
    }

    CloseScope();
}

void MethodImpl::LoadObject(_R<Serializable> &value, const char *name) {
    OpenScope(name);

    bool cached;
    LoadBoolean(cached, "cached");
    if (cached) {
        size_t index;
        szf::Load(this, index, "index");
        value = (index == 0 ? NULL : objects_[index - offset_ - 1]);
    } else {
        ext::Uuid type;

        if (!byname_)
            LoadUuid(type, "type");
        else {
            cse::String name;
            LoadString(name, "type");
            type = com::Resolve(name);
        }

        value = com::CreateInstance<Serializable>(type);
        if (value == NULL)
            throw com::ClassNotFoundException(type);

        objects_.InsertLast(value);
        value->Load(this);
    }

    CloseScope();
}

void MethodImpl::SaveUuid(const ext::Uuid &value, const char *name) {
    // XXX: this doesn't take into account endian-ness
    SaveData(reinterpret_cast<const char *>(value.GetData()), sizeof(value), name);
}

void MethodImpl::LoadUuid(ext::Uuid &value, const char *name) {
    // XXX: this doesn't take into account endian-ness
    LoadData(reinterpret_cast<char *>(value.GetData()), sizeof(value), name);
}

void MethodImpl::SaveString(const cse::String &value, const char *name) {
    OpenScope(name);
    size_t size(value.GetData().GetSize());
    szf::Save(this, size, "@size");
    SaveData(value.NullTerminate(), size, "string");
    CloseScope();
}

void MethodImpl::LoadString(cse::String &value, const char *name) {
    OpenScope(name);
    size_t size;
    szf::Load(this, size, "@size");
    ext::Vector<char> data(size);
    LoadData(data.Begin(), size, "string");
    value.Assign(data.Begin(), data.GetSize());
    CloseScope();
}

}
