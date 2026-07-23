#pragma once

#include "gfx/math/vec3.h"

namespace gfx
{
    struct OBB3D
    {
        Vec3d origin;
        Vec3d side_x;
        Vec3d side_y;
        Vec3d side_z;

        OBB3D();
        OBB3D(Vec3d origin, Vec3d side_x, Vec3d side_y, Vec3d side_z);
        OBB3D(const OBB3D& other);

        Vec3d get_center() const;
        Vec3d get_extent() const;
        Vec3d get_uvw(Vec3d point) const;
        std::vector<Vec3d> get_corners() const;
    };
}
