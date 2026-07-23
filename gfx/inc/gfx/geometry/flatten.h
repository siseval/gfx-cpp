#pragma once

#include <vector>

#include "gfx/math/vec2.h"

namespace gfx
{
    class Flatten
    {
    public:

        static std::vector<Vec2d> flatten_bezier(Vec2d start, Vec2d control, Vec2d end, double tolerance = 0.5);

        static std::vector<Vec2d> flatten_contour(const std::vector<std::pair<Vec2d, bool>>& points_on_off_curve);
    };
}
