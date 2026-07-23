#pragma once

#include <vector>

#include "gfx/core/types/barycentric-triangle.h"
#include "gfx/math/vec2.h"
#include "types/polygon.h"
#include "types/triangle.h"

namespace gfx
{
    class Triangulate
    {
    public:

        static std::vector<Triangle<double>> triangulate_polygon(const Polygon<double>& component);

    private:

        static bool is_convex(const Triangle<int>& triangle, bool clockwise);

        static bool are_collinear(Vec2i a, Vec2i b, Vec2i c);
        static std::vector<size_t> get_non_collinear_indices(const std::vector<Vec2i>& vertices);

        static bool point_in_triangle(Vec2i p, Triangle<int> triangle);

        static bool is_ear(
            const std::vector<size_t>& indices,
            const std::vector<Vec2i>& vertices,
            const Triangle<int>& triangle,
            int i0,
            int i1,
            int i2,
            bool clockwise
        );
        static Polygon<double>::Contour merge_holes(
            const Polygon<double>::Contour& contour,
            const std::vector<Polygon<double>::Contour>& holes
        );
    };
}
