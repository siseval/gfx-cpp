#pragma once

#include "gfx/core/primitive.h"
#include "gfx/core/material/material.h"
#include "gfx/core/material/vertex-shader.h"

namespace gfx
{
    class Primitive3D : Primitive<Vec3d>
    {
    public:

        void set_position(double x, double y, double z);
        void set_scale(double x, double y, double z);
        void set_rotation(double x, double y, double z);
        void set_rotation_degrees(Vec3d rot);
        void set_rotation_degrees(double x, double y, double z);
        void set_anchor(double x, double y, double z);
    };

    inline void Primitive3D::set_position(const double x, const double y, const double z)
    {
        _position = Vec3d { x, y, z };
        transform_updated();
    }

    inline void Primitive3D::set_scale(const double x, const double y, const double z)
    {
        _scale = Vec3d { x, y, z };
        transform_updated();
    }

    inline void Primitive3D::set_rotation(const double x, const double y, const double z)
    {
        _rotation = Vec3d { x, y, z };
        transform_updated();
    }

    inline void Primitive3D::set_rotation_degrees(const Vec3d rot)
    {
        _rotation = Vec3d {
            rot.x * std::numbers::pi / 180,
            rot.y * std::numbers::pi / 180,
            rot.z * std::numbers::pi / 180
        };
        transform_updated();
    }

    inline void Primitive3D::set_rotation_degrees(const double x, const double y, const double z)
    {
        _rotation = Vec3d {
            x * std::numbers::pi / 180, 
            y * std::numbers::pi / 180, 
            z * std::numbers::pi / 180
        };
        transform_updated();
    }
    
    inline void Primitive3D::set_anchor(const double x, const double y, const double z)
    {
        _anchor = Vec3d { x, y, z };
        transform_updated();
    }
}
