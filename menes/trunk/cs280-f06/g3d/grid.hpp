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

#ifndef MENES_G3D_GRID_HPP
#define MENES_G3D_GRID_HPP

#include "cxx/platform.hpp"
#include "g3d/linkage.hpp"

#ifdef MENES_PRAGMA_ONCE
#pragma once
#endif

#include "alg/constants.hpp"
#include "cxx/property.hpp"
#include "g3d/buffer.hpp"

namespace g3d {

class Grid :
    public Node
{
  private:
    float radius_;
    unsigned resolution_;

    _R<Buffer> normals_;
    _R<Buffer> vertices_;
    _R<Buffer> indices_;
    _R<Buffer> coords_;

    virtual float GetDisplacement_(unsigned row, unsigned col) const {
        return 0.0f;
    }

  protected:
    virtual void Display_() const {
        ::glEnableClientState(GL_VERTEX_ARRAY);
        vertices_->Bind();
        ::glVertexPointer(3, GL_FLOAT, 0, NULL);

        ::glEnableClientState(GL_NORMAL_ARRAY);
        normals_->Bind();
        ::glNormalPointer(GL_FLOAT, 0, NULL);

        ::glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        coords_->Bind();
        ::glTexCoordPointer(2, GL_FLOAT, 0, NULL);

        indices_->Bind();
        ::glDrawElements(GL_TRIANGLES, alg::VERTICES_PER_TRIANGLE * alg::TRIANGLES_PER_SQUARE * alg::Square(GetResolution() - 3), GL_UNSIGNED_SHORT, NULL);
    }

  public:
    Grid(float radius, unsigned resolution) :
        radius_(radius),
        resolution_(resolution)
    {
        indices_ = new Buffer(GL_ELEMENT_ARRAY_BUFFER);
        indices_->Copy(sizeof(uint16_t) * alg::VERTICES_PER_TRIANGLE * alg::TRIANGLES_PER_SQUARE * alg::Square(GetResolution() - 3), NULL, GL_STATIC_DRAW);

        _map (indices_, uint16_t, GL_WRITE_ONLY) {
            size_t offset(0);

            for (unsigned y(1); y != GetResolution() - 2; ++y)
            for (unsigned x(1); x != GetResolution() - 2; ++x) {
                _value(offset++) = (y + 1) * GetResolution() + (x + 0);
                _value(offset++) = (y + 0) * GetResolution() + (x + 0);
                _value(offset++) = (y + 1) * GetResolution() + (x + 1);
                _value(offset++) = (y + 0) * GetResolution() + (x + 1);
                _value(offset++) = (y + 1) * GetResolution() + (x + 1);
                _value(offset++) = (y + 0) * GetResolution() + (x + 0);
            }
        }

        normals_ = new Buffer(GL_ARRAY_BUFFER);
        normals_->Copy(sizeof(alg::Vector3f) * alg::Square(GetResolution()), NULL, GL_STATIC_DRAW);

        vertices_ = new Buffer(GL_ARRAY_BUFFER);
        vertices_->Copy(sizeof(alg::Vector3f) * alg::Square(GetResolution()), NULL, GL_STATIC_DRAW);

        float delta(GetDelta());

        _map (vertices_, alg::Point3f, GL_WRITE_ONLY)
            for (unsigned y(0); y != GetResolution(); ++y)
            for (unsigned x(0); x != GetResolution(); ++x)
                _value(y * GetResolution() + x).Set(
                    -radius_ + delta * x,
                    -radius_ + delta * y,
                    0
                );

        coords_ = new Buffer(GL_ARRAY_BUFFER);
        coords_->Copy(sizeof(alg::Vector2f) * alg::Square(GetResolution()), NULL, GL_STATIC_DRAW);

        _map (coords_, alg::Point2f, GL_WRITE_ONLY)
            for (unsigned y(0); y != GetResolution(); ++y)
            for (unsigned x(0); x != GetResolution(); ++x)
                _value(y * GetResolution() + x).Set(
                    x / static_cast<float>(GetResolution() - 1),
                    y / static_cast<float>(GetResolution() - 1)
                );

        Update();
    }

    _finline float GetDelta() const {
        return 2 * radius_ / resolution_;
    }

    _property(float, Radius, {
        return radius_;
    }, {
        _assert(false);
        radius_ = _value;
    })

    _property(unsigned, Resolution, {
        return resolution_;
    }, {
        _assert(false);
        resolution_ = _value;
    })

    void Update() const {
        std::vector<alg::Point3f> vertices(alg::Square(GetResolution()));
        std::vector<alg::Offset3f> normals(alg::Square(GetResolution() - 1));

        float delta(GetDelta());

        _map (vertices_, alg::Point3f, GL_WRITE_ONLY)
            _forall (unsigned, y, 0, GetResolution())
            for (unsigned x(0); x != GetResolution(); ++x) {
                size_t offset(y * GetResolution() + x);
                float displacement(GetDisplacement_(x, y));

                _value(offset)[2] = displacement;

                vertices[offset].Set(
                    -radius_ + delta * x,
                    -radius_ + delta * y,
                    displacement
                );
            }

        for (unsigned y(0); y != GetResolution() - 1; ++y)
        for (unsigned x(0); x != GetResolution() - 1; ++x) {
            const alg::Point3f &ll(vertices[(y + 0) * GetResolution() + (x + 0)]);
            const alg::Point3f &ul(vertices[(y + 1) * GetResolution() + (x + 0)]);
            const alg::Point3f &ur(vertices[(y + 1) * GetResolution() + (x + 1)]);
            const alg::Point3f &lr(vertices[(y + 0) * GetResolution() + (x + 1)]);

            const alg::Offset3f diagonal(ur - ll);

            normals[y * (GetResolution() - 1) + x] =
                alg::Cross(diagonal, ul - ll) +
                alg::Cross(lr - ll, diagonal);
        }

        _map (normals_, alg::Offset3f, GL_WRITE_ONLY)
            for (unsigned y(1); y != GetResolution() - 1; ++y)
            for (unsigned x(1); x != GetResolution() - 1; ++x)
                _value(y * GetResolution() + x) = alg::Normalize(
                    normals[(y - 1) * (GetResolution() - 1) + (x - 1)] +
                    normals[(y - 0) * (GetResolution() - 1) + (x - 1)] +
                    normals[(y - 0) * (GetResolution() - 1) + (x - 0)] +
                    normals[(y - 1) * (GetResolution() - 1) + (x - 0)]
                );
    }
};

}

#endif/*MENES_G3D_GRID_HPP*/
