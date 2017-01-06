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

#ifndef MENES_G3D_CAMERA_HPP
#define MENES_G3D_CAMERA_HPP

#include "cxx/platform.hpp"
#include "g3d/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "cxx/property.hpp"
#include "g3d/object.hpp"

namespace g3d {

class Camera :
    public Node
{
  private:
    double field_of_view_;
    double aspect_ratio_;
    double near_clip_;
    double far_clip_;

    virtual void Display_() const {
    }

  public:
    Camera() :
        field_of_view_(45.0),
        near_clip_(1.0),
        far_clip_(400.0)
    {
    }

    _property(double, AspectRatio, {
        return aspect_ratio_;
    }, {
        aspect_ratio_ = _value;
    })

    _property(double, FieldOfView, {
        return field_of_view_;
    }, {
        field_of_view_ = _value;
    })

    void Project() {
        ::gluPerspective(
            field_of_view_,
            aspect_ratio_,
            near_clip_,
            far_clip_
        );

        //::glOrtho(-10.0, 10.0, -10.0, 10.0, -100.0f, 100.0f);
    }

    void Bind() {
        ::glMatrixMode(GL_PROJECTION);
            ::glLoadIdentity();
            Project();
        ::glMatrixMode(GL_MODELVIEW);
            ::glLoadIdentity();
            FullInvert();
    }
};

}

#endif/*MENES_G3D_CAMERA_HPP*/
