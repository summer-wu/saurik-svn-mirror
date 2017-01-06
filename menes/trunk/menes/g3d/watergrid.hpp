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

#ifndef MENES_G3D_WATERGRID_HPP
#define MENES_G3D_WATERGRID_HPP

#include "cxx/platform.hpp"
#include "g3d/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "g3d/grid.hpp"
#include "g3d/time.hpp"

namespace g3d {

class WaterGrid :
    public Grid,
    private Updated
{
    typedef Grid Base_;

  private:
    float *data_;

    float *current_;
    float *before_;

    float velocity_;
    float viscosity_;

    _finline float &Get_(float *data, unsigned row, unsigned col) const {
        return data[row * GetResolution() + col];
    }

    virtual float GetDisplacement_(unsigned row, unsigned col) const {
        return Get_(current_, row, col);
    }

    void UpdateStep_() {
        float distance(GetRadius() / GetResolution());
        float c2od2((velocity_ * velocity_) / (distance * distance));
        float step((viscosity_ + sqrt(viscosity_ * viscosity_ + 32 * c2od2)) / (8 * c2od2) * 0.75);
        SetTimeStep(ext::Min(step, 1.0f / 30));
    }

    virtual bool Update_() {
        float distance(GetRadius() / GetResolution());
        float c2t2od2((velocity_ * velocity_) * (GetTimeStep() * GetTimeStep()) / (distance * distance));
        float mt(viscosity_ * GetTimeStep());
        float coeff0(4 - 8 * c2t2od2);
        float coeff1(mt - 2);

        for (unsigned row(1); row != GetResolution() - 1; ++row)
        for (unsigned col(1); col != GetResolution() - 1; ++col)
            Get_(before_, row, col) = (
                coeff0 * Get_(current_, row, col) +
                coeff1 * Get_(before_, row, col) +
                (2 * c2t2od2) * (
                    Get_(current_, row + 1, col) +
                    Get_(current_, row - 1, col) +
                    Get_(current_, row, col + 1) +
                    Get_(current_, row, col - 1)
                )
            ) / (mt + 2);

        for (unsigned col(0); col != GetResolution(); ++col)
            _assert(Get_(current_, col, 0) == 0.0f);

        ext::Swap(current_, before_);

        return true;
    }

  public:
    WaterGrid(float radius, unsigned resolution) :
        Base_(radius, resolution)
    {
        size_t size(GetResolution() * GetResolution());
        data_ = new float[2 * size];

        ext::FillAssign(data_, data_ + 2 * size, 0.0f);

        current_ = data_;
        before_ = data_ + size;

        Time_.Register(this);
    }

    virtual ~WaterGrid() {
        delete data_;
    }

    void Impulse(unsigned row, unsigned col) {
        Get_(current_, row, col) -= 0.5f;

        for (int i(-1); i != 2; ++i)
        for (int j(-1); j != 2; ++j)
            Get_(current_, row + i, col + j) -= 0.2f;
    }

    _property(float, Velocity, {
        return velocity_;
    }, {
        velocity_ = _value;
        UpdateStep_();
    })

    _property(float, Viscosity, {
        return viscosity_;
    }, {
        viscosity_ = _value;
        UpdateStep_();
    })
};

class Rain :
    public ext::ReferenceCounted,
    public Updated
{
  private:
    _R<WaterGrid> grid_;

    virtual bool Update_() {
        if (paused_)
            return false;

        grid_->Impulse(
            genrand_int32() % (Resolution_ - 4) + 2,
            genrand_int32() % (Resolution_ - 4) + 2
        );

        return true;
    }

  public:
    Rain(const _R<WaterGrid> grid) :
        grid_(grid)
    {
        Time_.Register(this);
    }
};

}

#endif/*MENES_G3D_WATERGRID_HPP*/
