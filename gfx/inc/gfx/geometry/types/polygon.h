#pragma once

#include <vector>

#include "gfx/math/vec2.h"

namespace gfx
{
    template <typename T>
    struct Polygon
    {
        struct Contour
        {
            Contour(std::vector<Vec2<T>> verts, const bool cw)
                : vertices(verts)
                , clockwise(cw) {}

            Contour()
                : clockwise(true) {}

            std::vector<Vec2<T>> vertices;
            bool clockwise;
        };

        Polygon(std::vector<Vec2<T>> vertices, const bool clockwise)
            : contour(vertices, clockwise) {}

        explicit Polygon(Contour cont)
            : contour(cont) {}

        Polygon(std::vector<Vec2<T>> vertices, const bool cw, std::vector<Contour> holes)
            : contour(vertices, cw)
            , holes(holes) {}

        Polygon() {}

        Contour contour;
        std::vector<Contour> holes;
    };
}
