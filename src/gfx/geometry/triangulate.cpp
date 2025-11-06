#include <gfx/geometry/triangulate.h>
#include <gfx/math/box2.h>

namespace gfx::geometry
{

using namespace gfx::math;
using namespace gfx::geometry::types;


bool is_convex(const Triangle &triangle, const bool clockwise)
{
    Vec2d ab { triangle.v1 - triangle.v0 };
    Vec2d ac { triangle.v2 - triangle.v0 };
    double cross { ab.x * ac.y - ab.y * ac.x };
    return clockwise ? cross > 0 : cross < 0;
}

bool is_ear(const int index, const std::vector<int> &indices, const std::vector<Vec2d> &vertices, const Triangle &triangle, const int i0, const int i1, const int i2, const bool clockwise)
{
    if (!is_convex(triangle, clockwise))
    {
        return false;
    }

    for (int i = 0 ; i < indices.size(); ++i)
    {
        if (i == i0 || i == i1 || i == i2)
        {
            continue;
        }

        Box2d bounds { triangle.v0, triangle.v0 };
        for (auto v : { triangle.v1, triangle.v2 })
        {
            bounds.expand(v);
        }

        Vec2d point { vertices[indices[i]] };
        if (!bounds.contains(point))
        {
            continue;
        }

        if (point == triangle.v0 || point == triangle.v1 || point == triangle.v2)
        {
            continue;
        }

        if (triangle.point_inside(point))
        {
            return false;
        }
    }
    return true;
}

Contour merge_holes(const Contour &contour, const std::vector<Contour> &holes)
{
    std::vector<Vec2d> merged = contour.vertices;

    for (const auto& hole : holes)
    {
        if (hole.vertices.size() < 3)
        {
            continue;
        }

        std::vector<Vec2d> reversed;
        auto& hole_vertices = 
            (hole.clockwise == contour.clockwise)
                ? (reversed = { hole.vertices.rbegin(), hole.vertices.rend() }, reversed)
                : hole.vertices;

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
            Vec2d a = merged[i];
            Vec2d b = merged[(i + 1) % merged.size()];

            if ((a.y > bridge_start.y) != (b.y > bridge_start.y))
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

    return Contour { merged, contour.clockwise };
}

std::vector<Triangle> triangulate_polygon(const gfx::geometry::types::Component &component)
{
    Contour merged;
    auto& merged_contour = 
        (component.holes.size() > 0)
            ? (merged = { merge_holes(component.contour, component.holes) }, merged)
            : component.contour;

    const std::vector<Vec2d> &vertices { merged_contour.vertices };
    const bool clockwise { merged_contour.clockwise };

    std::vector<Triangle> triangles;
    if (vertices.size() < 3)
    {
        return triangles;
    }

    std::vector<int> indices;
    indices.reserve(vertices.size());
    for (int i = 0; i < vertices.size(); ++i)
    {
        indices.push_back(i);
    }

    while (indices.size() > 3)
    {
        bool ear_found = false;
        for (int i = 0; i < indices.size(); ++i)
        {
            int prev_index { indices[(i == 0) ? indices.size() - 1 : i - 1] };
            int cur_index { indices[i] };
            int next_index { indices[(i + 1) >= indices.size() ? 0 : i + 1] };

            Triangle candidate { vertices[prev_index], vertices[cur_index], vertices[next_index] };

            if (is_ear(i, indices, vertices, candidate, prev_index, cur_index, next_index, clockwise))
            {
                triangles.push_back(candidate);
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
    triangles.emplace_back(vertices[indices[0]], vertices[indices[1]], vertices[indices[2]]);

    return triangles;
}

}
