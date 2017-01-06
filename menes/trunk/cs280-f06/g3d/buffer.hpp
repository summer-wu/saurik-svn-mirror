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

#ifndef MENES_G3D_BUFFER_HPP
#define MENES_G3D_BUFFER_HPP

#include "cxx/platform.hpp"
#include "g3d/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

namespace g3d {

class Buffer :
    public ext::ReferenceCounted
{
  private:
    GLenum target_;
    GLuint buffer_;

  public:
    Buffer(GLenum target) :
        target_(target)
    {
        ::glGenBuffers(1, &buffer_);
    }

    virtual ~Buffer() {
        ::glDeleteBuffers(1, &buffer_);
    }

    void Bind() {
        ::glBindBuffer(target_, buffer_);
    }

    GLenum GetTarget() const {
        return target_;
    }

    void Copy(GLsizeiptr size, const GLvoid *data, GLenum usage) {
        Bind();
        ::glBufferData(target_, size, data, usage);
    }

    void Free() {
        ::glBufferData(target_, 0, NULL, 0);
    }
};

template <typename Type_, GLenum Access_>
class Map {
  private:
    GLenum target_;
    Type_ *data_;

  public:
    _finline Map(Buffer *buffer) :
        target_(buffer->GetTarget())
    {
        buffer->Bind();
        data_ = reinterpret_cast<Type_ *>(::glMapBuffer(target_, Access_));
        _assert(data_ != NULL);
    }

    _finline ~Map() {
        ::glUnmapBuffer(target_);
    }

    _finline Type_ &operator [](size_t offset) const {
        return data_[offset];
    }

    _finline Type_ &operator ()(size_t offset) const {
        return operator [](offset);
    }
};

template <typename Type_, GLenum Access_>
class ScopeMap :
    public Map<Type_, Access_>
{
    typedef Map<Type_, Access_> Base_;

  public:
    _finline ScopeMap(Buffer *buffer) :
        Base_(buffer)
    {
    }

    _finline operator bool() const {
        return false;
    }
};

#define _map(buffer, type, access) \
    if (const g3d::ScopeMap<type, access> &_value = g3d::ScopeMap<type, access>(buffer)); else

}

#endif/*MENES_G3D_BUFFER_HPP*/
