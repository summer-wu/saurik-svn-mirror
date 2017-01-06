/* Skiazo - Menes-Compatible OpenGL Graphics Engine
 * Copyright (C) 2006  Jay Freeman (saurik)
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

#ifndef MENES_G3D_PROGRAM_HPP
#define MENES_G3D_PROGRAM_HPP

#include "cxx/platform.hpp"
#include "g3d/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "g3d/context.hpp"
#include "g3d/texture.hpp"

namespace g3d {

class Program :
    public ext::ReferenceCounted
{
  private:
    _R<Context> context_;

    ::CGprofile profile_;
    ::CGprogram program_;

  public:
    Program(const _R<Context> &context, ::CGprofile profile, const char *path) :
        context_(context),
        profile_(profile)
    {
        program_ = ::cgCreateProgramFromFile(*context_, CG_SOURCE, path, profile_, NULL, NULL);
        ::cgGLLoadProgram(program_);
    }

    void SetParameter(const char *name, const _R<Texture> &value) {
        ::CGparameter parameter(::cgGetNamedParameter(program_, name));
        _assert(parameter != NULL);
        ::cgGLSetTextureParameter(parameter, value->GetId());
    }

    void SetParameter(const char *name, const float &value) {
        ::CGparameter parameter(::cgGetNamedParameter(program_, name));
        _assert(parameter != NULL);
        ::cgGLSetParameter1f(parameter, value);
    }

    void SetParameter(const char *name, const alg::Point3f &value) {
        ::CGparameter parameter(::cgGetNamedParameter(program_, name));
        _assert(parameter != NULL);
        ::cgGLSetParameter3fv(parameter, value.GetData());
    }

    bool MaybeSetStateMatrixParameter(const char *name, CGGLenum matrix, CGGLenum transform) {
        ::CGparameter parameter(::cgGetNamedParameter(program_, name));
        if (parameter == NULL)
            return false;
        ::cgGLSetStateMatrixParameter(parameter, matrix, transform);
        return true;
    }

    void SetStateMatrixParameter(const char *name, CGGLenum matrix, CGGLenum transform) {
        bool set(MaybeSetStateMatrixParameter(name, matrix, transform));
        (void) set;
        _assert(set);
    }

    virtual ~Program() {
        ::cgDestroyProgram(program_);
    }

    void Bind() {
        /* XXX: ICKY STICKY POO! */
        MaybeSetStateMatrixParameter("mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
        MaybeSetStateMatrixParameter("mvit", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
        MaybeSetStateMatrixParameter("mvi", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE);
        MaybeSetStateMatrixParameter("mv", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
        MaybeSetStateMatrixParameter("p", CG_GL_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);

        ::cgGLBindProgram(program_);
        ::cgGLEnableProfile(profile_);
    }

    void Unbind() {
        ::cgGLDisableProfile(profile_);
        ::cgGLUnbindProgram(profile_);
    }
};

}

#endif/*MENES_G3D_PROGRAM_HPP*/
