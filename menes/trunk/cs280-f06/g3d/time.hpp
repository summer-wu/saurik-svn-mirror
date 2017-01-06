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

#ifndef MENES_G3D_TIME_HPP
#define MENES_G3D_TIME_HPP

#include "cxx/platform.hpp"
#include "g3d/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

namespace g3d {

class Updated {
    friend class Time;

  private:
    double before_;
    float timestep_;

    virtual bool Update_() = 0;

    bool Update(double now) {
        bool redisplay(false);
        unsigned maximum(2);

        while (
            maximum-- != 0 &&
            before_ + timestep_ < now
        ) {
            redisplay = Update_() || redisplay;
            before_ += timestep_;
        }

        return redisplay;
    }

  public:
    _property(float, TimeStep, {
        return timestep_;
    }, {
        timestep_ = _value;
    })

    Updated() :
        before_(0)
    {
    }

    virtual ~Updated() {
    }
};

class Time {
  private:
    double start_;

    typedef std::vector<Updated *> Updateds_;
    Updateds_ updateds_;

  public:
    /* XXX: support unregister! */
    void Register(Updated *updated) {
        updateds_.push_back(updated);
    }

    void Start() {
        start_ = api::GetWallTimerSeconds();
    }

    bool Update() {
        bool redisplay(false);

        double now(api::GetWallTimerSeconds());
        for (Updateds_::iterator updated(updateds_.begin()); updated != updateds_.end(); ++updated)
            redisplay = (*updated)->Update(now - start_) || redisplay;

        return redisplay;
    }
};

}

#endif/*MENES_G3D_TIME_HPP*/
