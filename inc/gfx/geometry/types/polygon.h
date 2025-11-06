#ifndef POLYGON_H
#define POLYGON_H

#include <vector>
#include <gfx/math/vec2.h>

namespace gfx::geometry::types
{

struct Contour
{
    Contour(std::vector<gfx::math::Vec2d> verts, bool cw)
        : vertices(verts), clockwise(cw) {}

    Contour() : vertices(), clockwise(true) {}

    std::vector<gfx::math::Vec2d> vertices;
    bool clockwise;
};

struct Component
{
    Component(std::vector<gfx::math::Vec2d> vertices, bool clockwise)
        : contour(vertices, clockwise) {}

    Component(Contour cont)
        : contour(cont) {}

    Component(std::vector<gfx::math::Vec2d> vertices, bool cw, std::vector<Contour> holes)
        : contour(vertices, cw), holes(holes) {}

    Component() : contour(), holes() {}

    Contour contour;
    std::vector<Contour> holes;
};

}

#endif // POLYGON_H
