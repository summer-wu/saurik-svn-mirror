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

#include "cxx/standard.hh"
#pragma hdrstop("build/cxx/standard.pch")

#include "szf/saveload.hpp"
#include "szf/serialize.hpp"

namespace szf {

SZF_DECLARE void Save(Method *method, bool value, const char *name) {
    method->SaveBoolean(value, name);
}

SZF_DECLARE void Load(Method *method, bool &value, const char *name) {
    method->LoadBoolean(value, name);
}

SZF_DECLARE void Save(Method *method, float value, const char *name) {
    method->SaveFloating(value, name);
}

SZF_DECLARE void Load(Method *method, float &value, const char *name) {
    double temp;
    method->LoadFloating(temp, name);
    value = static_cast<float>(temp);
}

SZF_DECLARE void Save(Method *method, double value, const char *name) {
    method->SaveFloating(value, name);
}

SZF_DECLARE void Load(Method *method, double &value, const char *name) {
    method->LoadFloating(value, name);
}

SZF_DECLARE void Save(Method *method, const ISaveLoad &value, const char *name) {
    method->OpenScope(name);
    value.Save(method);
    method->CloseScope();
}

SZF_DECLARE void Load(Method *method, ISaveLoad &value, const char *name) {
    method->OpenScope(name);
    value.Load(method);
    method->CloseScope();
}

SZF_DECLARE void Save(Method *method, Serializable *value, const char *name) {
    method->SaveObject(value, name);
}

SZF_DECLARE void Load(Method *method, _R<Serializable> &value, const char *name) {
    method->LoadObject(value, name);
}

}
