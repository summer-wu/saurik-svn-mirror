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

#ifndef MENES_G3D_OBJECT_HPP
#define MENES_G3D_OBJECT_HPP

#include "cxx/platform.hpp"
#include "g3d/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "cxx/property.hpp"
#include "g3d/material.hpp"

#include <set>

namespace g3d {

class System;

class Node :
    public ext::ReferenceCounted
{
    friend class System;

  private:
    System *system_;

    alg::Point3f preposition_;
    alg::Point3f postposition_;

    float roll_, pitch_, yaw_;

  protected:
    virtual void Display_() const = 0;

  public:
    Node() :
        system_(NULL),
        preposition_(0),
        postposition_(0),
        roll_(0),
        pitch_(0),
        yaw_(0)
    {
    }

    virtual ~Node() {
    }

    void Transform() const {
        ::glTranslatef(preposition_[0], preposition_[1], preposition_[2]);
        ::glRotatef(-yaw_, 0, 1, 0);
        ::glRotatef(-pitch_, 1, 0, 0);
        ::glRotatef(-roll_, 0, 0, 1);
        ::glTranslatef(postposition_[0], postposition_[1], postposition_[2]);
    }

    void FullTransform() const;
    void FullInvert() const;

    void Display() const {
        ::glPushMatrix();
            Transform();
            Display_();
        ::glPopMatrix();
    }

    _property(alg::Point3f, Preposition, {
        return preposition_;
    }, {
        preposition_ = _value;
    })

    _property(alg::Point3f, Postposition, {
        return postposition_;
    }, {
        postposition_ = _value;
    })

    _property(float, Roll, {
        return roll_;
    }, {
        roll_ = _value;
    })

    _property(float, Pitch, {
        return pitch_;
    }, {
        pitch_ = _value;
    })

    _property(float, Yaw, {
        return yaw_;
    }, {
        yaw_ = _value;
    })

    void SetSystem(const _R<System> &system);
};

class System :
    public Node
{
    friend class Node;

  private:
    /* XXX: fuck me with a fork */
    typedef std::set< _R<Node> > Children_;
    Children_ children_;

  protected:
    virtual void Display_() const {
        _foreach (const Children_, child, children_)
            (*child)->Display();
    }

  public:
    virtual ~System() {
        _foreach (Children_, child, children_)
            (*child)->system_ = NULL;
    }
};

void Node::FullTransform() const {
    if (system_ != NULL)
        system_->FullTransform();
    Transform();
}

void Node::FullInvert() const {
    ::glTranslatef(-postposition_[0], -postposition_[1], -postposition_[2]);
    ::glRotatef(roll_, 0, 0, 1);
    ::glRotatef(pitch_, 1, 0, 0);
    ::glRotatef(yaw_, 0, 1, 0);
    ::glTranslatef(-preposition_[0], -preposition_[1], -preposition_[2]);

    if (system_ != NULL)
        system_->FullInvert();
}

void Node::SetSystem(const _R<System> &system) {
    if (system_ != NULL)
        system_->children_.erase(this);
    system_ = system;
    if (system_ != NULL)
        system->children_.insert(this);
}

}

#endif/*MENES_G3D_OBJECT_HPP*/
