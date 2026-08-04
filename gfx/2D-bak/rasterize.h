#pragma once

#include "gfx/math/vec2.h"
#include "../inc/gfx/geometry/types/triangle.h"

namespace gfx
{
    class Rasterize
    {
    public:

        static void rasterize_filled_triangle(
            const Triangle<double>& triangle,
            std::vector<Vec2i>& pixels,
            Vec2d clip_bounds = Vec2d { -1.0, -1.0 }
        );

        // static void rasterize_filled_triangle(
        //     const BarycentricTriangle& triangle,
        //     std::vector<Vec2i>& pixels,
        //     Vec2d clip_bounds = Vec2d { -1.0, -1.0 }
        // );

    private:

        static constexpr int CORNER_SEGMENTS { 8 };
        static constexpr int MIN_MULTITHREAD_PIXELS { 20000 * 20000 };
    };
}
