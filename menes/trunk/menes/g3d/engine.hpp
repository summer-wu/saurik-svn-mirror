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

#ifndef MENES_G3D_ENGINE_HPP
#define MENES_G3D_ENGINE_HPP

#include "cxx/platform.hpp"
#include "g3d/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "g3d/object.hpp"
#include "g3d/time.hpp"
#include "g3d/window.hpp"
#include "g3d/world.hpp"

namespace g3d {

enum Face {
    Front, Back, Both
};

class Engine :
    public ext::ReferenceCounted
{
  private:
    _R<World> world_;
    Time time_;

    Face face_;
    bool wireframe_;

  public:
    Engine(const _R<g3d::Window> &window) {
        ::glEnable(GL_DEPTH_TEST);
        ::glShadeModel(GL_SMOOTH);

        SetFace(Both);
        SetWireframe(false);

        //::glClearColor(0.5, 0.5, 0.5, 0.0);
        ::glClearColor(0.0, 0.0, 0.0, 0.0);

        time_.Start();
    }

    void Display() {
        world_->Display();
    }

    bool Update() {
        return time_.Update();
    }

    _property(Face, Face, {
        return face_;
    }, {
        face_ = _value;

        switch (face_) {
            case Front:
                ::glEnable(GL_CULL_FACE);
                ::glCullFace(GL_BACK);
            break;

            case Back:
                ::glEnable(GL_CULL_FACE);
                ::glCullFace(GL_FRONT);
            break;

            case Both:
                ::glDisable(GL_CULL_FACE);
            break;

            _nodefault
        }
    });

    _property(bool, Wireframe, {
        return wireframe_;
    }, {
        wireframe_ = _value;
        ::glPolygonMode(GL_FRONT_AND_BACK, wireframe_ ? GL_LINE : GL_FILL);
    });

    _property(_R<World>, World, {
        return world_;
    }, {
        world_ = _value;
    });
};

}

#endif/*MENES_G3D_ENGINE_HPP*/
