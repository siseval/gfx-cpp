#pragma once

#include "gfx/core/primitive.h"
#include "gfx/core/material/material.h"
#include "gfx/core/material/vertex-shader.h"

namespace gfx
{
    class Primitive2D : Primitive<Vec2d>
    {
    public:

        void set_position(double x, double y);
        void set_scale(double x, double y);
        void set_rotation_degrees(double angle);
        void set_anchor(double x, double y);
    };

    inline void Primitive2D::set_position(const double x, const double y)
    {
        _position = Vec2d { x, y };
        transform_updated();
    }

    inline void Primitive2D::set_scale(const double x, const double y)
    {
        _scale = Vec2d { x, y };
        transform_updated();
    }

    inline void Primitive2D::set_rotation_degrees(const double angle)
    {
        _rotation = angle * std::numbers::pi / 180;
        transform_updated();
    }

    inline void Primitive2D::set_anchor(const double x, const double y)
    {
        _anchor = Vec2d { x, y };
        transform_updated();
    }
}
