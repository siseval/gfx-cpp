#include "gfx/geometry/triangulate.h"

#include "gfx/geometry/types/triangle.h"
#include "gfx/math/box2.h"

namespace gfx
{
    static constexpr int FRACTIONAL_BITS { 8 };

    std::vector<Triangle<double>> Triangulate::triangulate_polygon(const Polygon<double>& component)
    {
        auto& merged_contour = component.holes.size() > 0 ?
                               Polygon<double>::Contour { merge_holes(component.contour, component.holes) } :
                               component.contour;

        const std::vector<Vec2d>& floating_point_vertices { merged_contour.vertices };

        std::vector<Vec2i> fixed_point_vertices;
        fixed_point_vertices.reserve(component.contour.vertices.size());
        for (size_t i = 0; i < floating_point_vertices.size(); ++i)
        {
            fixed_point_vertices.push_back(
                Vec2i {
                    static_cast<int>(std::round(floating_point_vertices[i].x * (1 << FRACTIONAL_BITS))),
                    static_cast<int>(std::round(floating_point_vertices[i].y * (1 << FRACTIONAL_BITS)))
                }
            );
        }

        const bool clockwise { merged_contour.clockwise };

        std::vector<Triangle<double>> triangles;
        if (fixed_point_vertices.size() < 3)
        {
            return triangles;
        }

        std::vector<size_t> indices { get_non_collinear_indices(fixed_point_vertices) };
        if (indices.empty())
        {
            return {};
        }

        while (indices.size() > 3)
        {
            bool ear_found = false;
            for (int i = 0; i < indices.size(); ++i)
            {
                const size_t prev_index { indices[i == 0 ? indices.size() - 1 : i - 1] };
                const size_t cur_index { indices[i] };
                const size_t next_index { indices[i + 1 >= indices.size() ? 0 : i + 1] };

                const Triangle<double> floating_point_candidate {
                    floating_point_vertices[prev_index],
                    floating_point_vertices[cur_index],
                    floating_point_vertices[next_index]
                };

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
                    clockwise
                ))
                {
                    triangles.push_back(floating_point_candidate);
                    indices.erase(indices.begin() + i);
                    ear_found = true;
                    break;
                }
            }
            if (!ear_found)
            {
                triangles.clear();
                return triangles;
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
        const double cross { ab.x * ac.y - ab.y * ac.x };
        return clockwise ? cross > 0 : cross < 0;
    }

    bool Triangulate::are_collinear(const Vec2i a, const Vec2i b, const Vec2i c)
    {
        const Vec2d ab { b - a };
        const Vec2d bc { c - b };
        return Vec2d::cross(ab, bc) == 0;
    }

    std::vector<size_t> Triangulate::get_non_collinear_indices(const std::vector<Vec2i>& vertices)
    {
        std::vector<size_t> unique_indices;
        unique_indices.reserve(vertices.size());

        for (size_t i = 0; i < vertices.size(); ++i)
        {
            const size_t b_index { i + 1 < vertices.size() ? i + 1 : i + 1 - vertices.size() };
            if (vertices[i] == vertices[b_index])
            {
                continue;
            }
            unique_indices.push_back(i);
        }

        std::vector<size_t> non_collinear_indices;
        non_collinear_indices.reserve(unique_indices.size());

        for (size_t i = 0; i < unique_indices.size(); ++i)
        {
            const size_t b_index { i + 1 < unique_indices.size() ? i + 1 : i + 1 - unique_indices.size() };
            const size_t c_index { i + 2 < unique_indices.size() ? i + 2 : i + 2 - unique_indices.size() };

            if (are_collinear(
                vertices[unique_indices[i]],
                vertices[unique_indices[b_index]],
                vertices[unique_indices[c_index]]
            ))
            {
                continue;
            }
            non_collinear_indices.push_back(unique_indices[b_index]);
        }

        return non_collinear_indices;
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
            if (i == i0 || i == i1 || i == i2)
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

    Polygon<double>::Contour Triangulate::merge_holes(
        const Polygon<double>::Contour& contour,
        const std::vector<Polygon<double>::Contour>& holes
    )
    {
        std::vector<Vec2d> merged = contour.vertices;

        for (const auto& hole : holes)
        {
            if (hole.vertices.size() < 3)
            {
                continue;
            }

            std::vector<Vec2d> reversed;
            auto& hole_vertices = hole.clockwise == contour.clockwise ?
                                  (reversed = { hole.vertices.rbegin(), hole.vertices.rend() }, reversed) :
                                  hole.vertices;

            int hole_index = 0;

            for (int i = 1; i < hole_vertices.size(); ++i)
            {
                if (hole_vertices[i].x > hole_vertices[hole_index].x)
                {
                    hole_index = i;
                }
            }

            Vec2d bridge_start = hole_vertices[hole_index];

            double best_x = std::numeric_limits<double>::infinity();
            int best_edge = -1;
            Vec2d best_point;

            for (int i = 0; i < merged.size(); ++i)
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

            if (best_edge == -1)
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
