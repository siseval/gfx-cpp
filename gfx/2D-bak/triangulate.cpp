#include "gfx/geometry/triangulate.h"

#include <numeric>

#include "gfx/geometry/types/triangle.h"
#include "gfx/math/box2.h"

namespace gfx
{
    static constexpr int FRACTIONAL_BITS { 8 };

    std::vector<Triangle<double>> Triangulate::triangulate_polygon(const Polygon<double>& polygon)
    {
        const Polygon<double>::Contour merged_contour = merge_holes(polygon);

        const std::vector<Vec2d>& floating_point_vertices { merged_contour.vertices };

        std::vector<Vec2i> fixed_point_vertices;
        fixed_point_vertices.reserve(floating_point_vertices.size());

        for (size_t i = 0; i < floating_point_vertices.size(); ++i)
        {
            fixed_point_vertices.push_back(
                Vec2i {
                    static_cast<int>(std::round(floating_point_vertices[i].x * (1 << FRACTIONAL_BITS))),
                    static_cast<int>(std::round(floating_point_vertices[i].y * (1 << FRACTIONAL_BITS)))
                }
            );
        }

        std::vector<Triangle<double>> triangles;
        if (fixed_point_vertices.size() < 3)
        {
            return triangles;
        }

        std::vector<size_t> indices(fixed_point_vertices.size());
        std::iota(indices.begin(), indices.end(), 0);

        if (indices.empty())
        {
            return {};
        }

        while (indices.size() > 3)
        {
            bool ear_found = false;
            for (size_t i = 0; i < indices.size(); ++i)
            {
                const size_t prev_index { indices[i == 0 ? indices.size() - 1 : i - 1] };
                const size_t cur_index { indices[i] };
                const size_t next_index { indices[i + 1 >= indices.size() ? 0 : i + 1] };

                const Triangle<int> fixed_point_candidate {
                    fixed_point_vertices[prev_index],
                    fixed_point_vertices[cur_index],
                    fixed_point_vertices[next_index]
                };

                if (is_ear(
                    indices,
                    fixed_point_vertices,
                    fixed_point_candidate,
                    prev_index,
                    cur_index,
                    next_index,
                    merged_contour.clockwise
                ))
                {
                    triangles.emplace_back(
                        floating_point_vertices[prev_index],
                        floating_point_vertices[cur_index],
                        floating_point_vertices[next_index]
                    );

                    indices.erase(indices.begin() + i);
                    ear_found = true;
                    break;
                }
            }
            if (!ear_found)
            {
                return {};
            }
        }

        triangles.emplace_back(
            floating_point_vertices[indices[0]],
            floating_point_vertices[indices[1]],
            floating_point_vertices[indices[2]]
        );

        return triangles;
    }

    bool Triangulate::is_convex(const Triangle<int>& triangle, const bool clockwise)
    {
        const Vec2d ab { triangle.v1 - triangle.v0 };
        const Vec2d ac { triangle.v2 - triangle.v0 };

        const double cross { Vec2d::cross(ab, ac) };
        return clockwise ? cross > 0 : cross < 0;
    }

    bool Triangulate::are_collinear(const Vec2d a, const Vec2d b, const Vec2d c)
    {
        const Vec2d ab { b - a };
        const Vec2d bc { c - b };

        return std::abs(Vec2d::cross(ab, bc)) <= 1e-9;
    }

    std::vector<Vec2d> Triangulate::trimmed_vertices(const std::vector<Vec2d>& vertices)
    {
        std::vector<Vec2d> unique_vertices;
        unique_vertices.reserve(vertices.size());

        for (size_t i = 0; i < vertices.size(); ++i)
        {
            const size_t b_index { i + 1 < vertices.size() ? i + 1 : i + 1 - vertices.size() };
            const Vec2d distance { vertices[i] - vertices[b_index] };
            if (std::abs(distance.x) <= 1e-9 && std::abs(distance.y) <= 1e-9)
            {
                continue;
            }
            unique_vertices.push_back(vertices[b_index]);
        }

        std::vector<Vec2d> non_collinear_vertices;
        non_collinear_vertices.reserve(unique_vertices.size());

        for (size_t i = 0; i < unique_vertices.size(); ++i)
        {
            const size_t b_index { i + 1 < unique_vertices.size() ? i + 1 : i + 1 - unique_vertices.size() };
            const size_t c_index { i + 2 < unique_vertices.size() ? i + 2 : i + 2 - unique_vertices.size() };

            if (are_collinear(unique_vertices[i], unique_vertices[b_index], unique_vertices[c_index]))
            {
                continue;
            }
            non_collinear_vertices.push_back(unique_vertices[b_index]);
        }

        return non_collinear_vertices;
    }

    bool Triangulate::point_in_triangle(const Vec2i p, const Triangle<int> triangle)
    {
        auto sign {
            [&](const Vec2i a, const Vec2i b, const Vec2i c) {
                return (a.x - c.x) * (b.y - c.y) - (b.x - c.x) * (a.y - c.y);
            }
        };

        const int d0 { sign(p, triangle.v0, triangle.v1) };
        const int d1 { sign(p, triangle.v1, triangle.v2) };
        const int d2 { sign(p, triangle.v2, triangle.v0) };

        const bool has_negative { d0 < 0 || d1 < 0 || d2 < 0 };
        const bool has_positive { d0 > 0 || d1 > 0 || d2 > 0 };

        return !(has_negative && has_positive);
    }

    bool Triangulate::is_ear(
        const std::vector<size_t>& indices,
        const std::vector<Vec2i>& vertices,
        const Triangle<int>& triangle,
        const int i0,
        const int i1,
        const int i2,
        const bool clockwise
    )
    {
        if (!is_convex(triangle, clockwise))
        {
            return false;
        }

        for (int i = 0; i < indices.size(); ++i)
        {
            if (indices[i] == static_cast<size_t>(i0) ||
                indices[i] == static_cast<size_t>(i1) ||
                indices[i] == static_cast<size_t>(i2))
            {
                continue;
            }

            Box2i bounds { triangle.v0, triangle.v0 };
            for (auto v : { triangle.v1, triangle.v2 })
            {
                bounds.expand(v);
            }

            Vec2i point { vertices[indices[i]] };
            if (!bounds.contains(point))
            {
                continue;
            }

            if (point == triangle.v0 || point == triangle.v1 || point == triangle.v2)
            {
                continue;
            }

            if (point_in_triangle(point, triangle))
            {
                return false;
            }
        }
        return true;
    }

    Polygon<double>::Contour Triangulate::merge_holes(const Polygon<double>& polygon)
    {
        const auto& contour = polygon.contour;
        const auto& holes = polygon.holes;

        std::vector<Vec2d> merged = trimmed_vertices(contour.vertices);

        for (const auto& hole : holes)
        {
            if (hole.vertices.size() < 3)
            {
                continue;
            }

            auto hole_vertices = hole.clockwise == contour.clockwise ?
                                 trimmed_vertices({ hole.vertices.rbegin(), hole.vertices.rend() }) :
                                 trimmed_vertices(hole.vertices);

            size_t hole_index = 0;

            for (size_t i = 1; i < hole_vertices.size(); ++i)
            {
                if (hole_vertices[i].x > hole_vertices[hole_index].x)
                {
                    hole_index = i;
                }
            }

            Vec2d bridge_start = hole_vertices[hole_index];

            double best_x = std::numeric_limits<double>::infinity();
            static constexpr size_t INVALID_EDGE { std::numeric_limits<size_t>::max() };
            size_t best_edge = INVALID_EDGE;
            Vec2d best_point;

            for (size_t i = 0; i < merged.size(); ++i)
            {
                const Vec2d a = merged[i];
                const Vec2d b = merged[(i + 1) % merged.size()];

                if (a.y > bridge_start.y != b.y > bridge_start.y)
                {
                    double x = a.x + (b.x - a.x) * (bridge_start.y - a.y) / (b.y - a.y);
                    if (x > bridge_start.x && x < best_x)
                    {
                        best_x = x;
                        best_edge = i;
                        best_point = { x, bridge_start.y };
                    }
                }
            }

            if (best_edge == INVALID_EDGE)
            {
                continue;
            }

            std::vector<Vec2d> new_merged;
            new_merged.insert(new_merged.end(), merged.begin(), merged.begin() + best_edge + 1);
            new_merged.push_back(best_point);

            for (int i = 0; i < hole_vertices.size(); ++i)
            {
                new_merged.push_back(hole_vertices[(hole_index + i) % hole_vertices.size()]);
            }

            new_merged.push_back(bridge_start);
            new_merged.push_back(best_point);
            new_merged.insert(new_merged.end(), merged.begin() + best_edge + 1, merged.end());

            merged = std::move(new_merged);
        }

        return Polygon<double>::Contour { merged, contour.clockwise };
    }
}
