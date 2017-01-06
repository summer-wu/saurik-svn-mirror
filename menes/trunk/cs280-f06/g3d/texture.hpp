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

#ifndef MENES_G3D_TEXTURE_HPP
#define MENES_G3D_TEXTURE_HPP

#include "cxx/platform.hpp"
#include "g3d/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "ImfRgbaFile.h"

namespace g3d {

class Texture :
    public ext::ReferenceCounted
{
  private:
    GLenum target_;
    GLuint texture_;

  protected:
    Texture(GLenum target) :
        target_(target)
    {
        ::glGenTextures(1, &texture_);

        Bind();
        ::glTexParameteri(target_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        ::glTexParameteri(target_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }

  public:
    virtual ~Texture() {
        ::glDeleteTextures(1, &texture_);
    }

    GLuint GetId() const {
        return texture_;
    }

    void Bind() {
        ::glBindTexture(target_, texture_);
    }

    void Unbind() {
        ::glBindTexture(target_, 0);
    }

    void SetWrap(GLenum wrap) {
        Bind();
        ::glTexParameteri(target_, GL_TEXTURE_WRAP_S, wrap);
        ::glTexParameteri(target_, GL_TEXTURE_WRAP_T, wrap);
        ::glTexParameteri(target_, GL_TEXTURE_WRAP_R, wrap);
    }

    void Copy(GLint iformat, GLsizei width, GLsizei height, GLenum eformat, GLenum etype, const GLvoid *data) {
        Bind();
        ::glTexImage2D(target_, 0, iformat, width, height, 0, eformat, etype, data);
    }
};

class Texture1d :
    public Texture
{
  public:
    Texture1d() :
        Texture(GL_TEXTURE_1D)
    {
    }
};

class Image2d :
    public ext::ReferenceCounted
{
  private:
    unsigned width_;
    unsigned height_;

    void *data_;

  public:
    Image2d(const char *path) {
        /* XXX: this is very un-menes like */
        Imf::RgbaInputFile file(path);
        Imath::Box2i box(file.dataWindow());
        width_ = box.max.x + 1;
        height_ = box.max.y + 1;
        _assert(box.min.x == 0);
        _assert(box.min.y == 0);
        Imf::Rgba *data(new Imf::Rgba[width_ * height_]);
        data_ = data;
        file.setFrameBuffer(data, 1, width_);
        file.readPixels(box.min.y, box.max.y);
        printf("(%u, %u) %.8x%.8x%.8x%.8x %f %f %f %f\n",
            width_, height_,
            reinterpret_cast<uint32_t *>(data_)[0],
            reinterpret_cast<uint32_t *>(data_)[1],
            reinterpret_cast<uint32_t *>(data_)[2],
            reinterpret_cast<uint32_t *>(data_)[3],
            static_cast<float>(data[0].r),
            static_cast<float>(data[0].g),
            static_cast<float>(data[0].b),
            static_cast<float>(data[0].a)
        );
    }

    unsigned GetWidth() const {
        return width_;
    }

    unsigned GetHeight() const {
        return height_;
    }

    const void *GetData() const {
        return data_;
    }

    void *GetData() {
        return data_;
    }
};

class Texture2d :
    public Texture
{
  public:
    Texture2d() :
        Texture(GL_TEXTURE_2D)
    {
    }

    Texture2d(const _R<Image2d> &rhs) :
        Texture(GL_TEXTURE_RECTANGLE_NV)
    {
        ::glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        Copy(GL_FLOAT_RGBA16_NV, rhs->GetWidth(), rhs->GetHeight(), GL_RGBA, GL_HALF_FLOAT_NV, rhs->GetData());
    }

    void Load(const char *path) {
        ::ILuint image;
        ::ilGenImages(1, &image);

        ::ilBindImage(image);
        ::ilLoadImage(path);

        ::ilutGLTexImage(0);
        ::ilutGLBuildMipmaps();
        ::ilutGLBindMipmaps();

        ::ilDeleteImages(1, &image);
    }
};

class Texture3d :
    public Texture
{
  public:
    Texture3d() :
        Texture(GL_TEXTURE_3D)
    {
    }
};

class TextureCube :
    public Texture
{
  public:
    TextureCube() :
        Texture(GL_TEXTURE_CUBE_MAP)
    {
    }
};

}

#endif/*MENES_G3D_TEXTURE_HPP*/
