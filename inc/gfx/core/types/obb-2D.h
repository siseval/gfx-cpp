#ifndef OBB_2D_H
#define OBB_2D_H

#include <gfx/math/vec2.h>

namespace gfx::core::types
{

class OBB2D
{

public:

    OBB2D() : origin { 0, 0 }, side_x { 1, 0 }, side_y { 0, 1 } {}
    OBB2D(const gfx::math::Vec2d origin, const gfx::math::Vec2d side_x, const gfx::math::Vec2d side_y) : origin(origin), side_x(side_x), side_y(side_y) {}
    OBB2D(const OBB2D &other) : origin(other.origin), side_x(other.side_x), side_y(other.side_y) {}

    gfx::math::Vec2d origin;
    gfx::math::Vec2d side_x;
    gfx::math::Vec2d side_y;

    gfx::math::Vec2d get_center() const
    {
        return origin + (side_x * 0.5) + (side_y * 0.5);
    }

    gfx::math::Vec2d get_extent() const
    {
        return gfx::math::Vec2d { side_x.length(), side_y.length() };
    }

    gfx::math::Vec2d get_uv(const gfx::math::Vec2d point) const
    {
        gfx::math::Vec2d d { point - origin };
        double u { gfx::math::Vec2d::dot(d, side_x) / gfx::math::Vec2d::dot(side_x, side_x) };
        double v { gfx::math::Vec2d::dot(d, side_y) / gfx::math::Vec2d::dot(side_y, side_y) };
        return gfx::math::Vec2d { u, v };
    }

    std::vector<gfx::math::Vec2d> get_corners() const
    {
        return {
            origin,
            origin + side_x,
            origin + side_x + side_y,
            origin + side_y
        };
    }

};

}

#endif // OBB_2D_H
