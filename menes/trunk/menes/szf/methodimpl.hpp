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

#ifndef MENES_SZF_METHODIMPL_HPP
#define MENES_SZF_METHODIMPL_HPP

#include "cxx/platform.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "ext/listset.hpp"
#include "ext/map.hpp"
#include "ext/vector.hpp"

#include "szf/linkage.hpp"
#include "szf/method.hpp"
#include "szf/serialize.hpp"

namespace szf {

class SZF_DECLARE MethodImpl :
    public Method
{
  private:
    typedef ext::ListMap<const std::type_info *, void *> ContextMap_;
    ContextMap_ context_;

    typedef ext::Vector< _R<Serializable> > ObjectList;
    ObjectList objects_;

    size_t offset_;
    bool byname_;

    virtual void *GetContext_(const std::type_info *type) const;
    virtual void SetContext_(const std::type_info *type, void *context);

  public:
    MethodImpl();

    virtual void ClearObjects();

    virtual void SaveObject(const _R<Serializable> &value, const char *name);
    virtual void LoadObject(_R<Serializable> &value, const char *name);

    virtual void SaveUuid(const ext::Uuid &value, const char *name);
    virtual void LoadUuid(ext::Uuid &value, const char *name);

    virtual void SaveString(const cse::String &value, const char *name);
    virtual void LoadString(cse::String &value, const char *name);
};

}

#endif//MENES_SZF_METHODIMPL_HPP
