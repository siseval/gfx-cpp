#pragma once

#include "gfx/math/vec2.h"

namespace gfx
{
    struct OBB2D
    {
        Vec2d origin;
        Vec2d side_x;
        Vec2d side_y;

        OBB2D();
        OBB2D(Vec2d origin, Vec2d side_x, Vec2d side_y);
        OBB2D(const OBB2D& other);

        Vec2d get_center() const;
        Vec2d get_extent() const;
        Vec2d get_uv(Vec2d point) const;
        std::vector<Vec2d> get_corners() const;
    };
}
