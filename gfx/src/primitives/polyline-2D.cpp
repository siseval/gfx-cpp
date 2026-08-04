#include "../../2D-bak/polyline-2D.h"

#include "gfx/geometry/types/polygon.h"
#include "../../2D-bak/rasterize.h"
#include "gfx/geometry/transform-2D.h"
#include "../../2D-bak/triangulate.h"


namespace gfx
{
    Primitive2D::RasterizeOutput Polyline2D::rasterize(const Matrix3x3d& transform) const
    {
        RasterizeOutput output;

        if (_points.size() < 2)
        {
            return output;
        }

        for (int i = 0; i < _points.size() - 1; ++i)
        {
            if (_segments_visible.size() > i && !_segments_visible[i])
            {
                continue;
            }
            rasterize_edge(_points[i], _points[i + 1], transform, output.pixels);
        }

        if (_do_close)
        {
            rasterize_edge(_points.back(), _points.front(), transform, output.pixels);
        }

        if (_do_rounded_corners)
        {
            rasterize_rounded_corners(transform, output.pixels);
        }

        if (_do_fill)
        {
            const std::vector transformed_points { Transform2D::transform_points(_points, transform) };

            const Polygon<double> polygon(transformed_points, _clockwise);
            const std::vector<Triangle<double>> triangles { Triangulate::triangulate_polygon(polygon) };

            for (const auto& triangle : triangles)
            {
                Rasterize::rasterize_filled_triangle(triangle, output.pixels);
            }
        }

        return output;
    }


    Box2d Polyline2D::get_geometry_size() const
    {
        Box2d bounds { Vec2d::zero(), Vec2d::zero() };

        for (auto point : _points)
        {
            bounds.expand(point);
        }

        return bounds;
    }

    Box2d Polyline2D::get_axis_aligned_bounding_box(const Matrix3x3d& transform) const
    {
        const auto [min, max] { get_geometry_size() };
        const Vec2d line_extent { std::ceil(_line_thickness / 2.0), std::ceil(_line_thickness / 2.0) };
        Vec2d top_left { min - line_extent };
        Vec2d bot_right { max + line_extent };

        const std::vector<Vec2d> corners {
            { top_left.x, top_left.y },
            { bot_right.x, top_left.y },
            { top_left.x, bot_right.y },
            { bot_right.x, bot_right.y },
        };
        const std::vector transformed_corners { Transform2D::transform_points(corners, transform) };

        Box2d bounds { transformed_corners[0], transformed_corners[0] };
        bounds.expand(transformed_corners);

        return bounds;
    }

    bool Polyline2D::cache_clockwise()
    {
        double sum = 0.0;
        for (int i = 0; i < _points.size(); ++i)
        {
            const Vec2d p0 { _points[i] };
            const Vec2d p1 { _points[(i + 1) % _points.size()] };
            sum += (p1.x - p0.x) * (p1.y + p0.y);
        }
        _clockwise = sum < 0.0;
        return _clockwise;
    }

    void Polyline2D::add_point(const Vec2d point)
    {
        _points.push_back(point);
        cache_clockwise();
        set_obb_dirty();
    }

    void Polyline2D::add_point(const double x, const double y)
    {
        _points.push_back(Vec2d { x, y });
        cache_clockwise();
        set_obb_dirty();
    }

    void Polyline2D::add_points(const std::vector<Vec2d>& new_points)
    {
        _points.insert(_points.end(), new_points.begin(), new_points.end());
        cache_clockwise();
        set_obb_dirty();
    }

    void Polyline2D::set_point(const size_t index, const Vec2d point)
    {
        if (index < _points.size())
        {
            _points[index] = point;
            cache_clockwise();
            set_obb_dirty();
        }
    }

    void Polyline2D::set_point(const size_t index, const double x, const double y)
    {
        if (index < _points.size())
        {
            _points[index] = Vec2d { x, y };
            cache_clockwise();
            set_obb_dirty();
        }
    }

    void Polyline2D::set_points(const std::vector<Vec2d>& new_points)
    {
        _points = new_points;
        cache_clockwise();
        set_obb_dirty();
    }

    void Polyline2D::clear_points()
    {
        _points.clear();
    }

    void Polyline2D::set_segment_visible(const size_t index, const bool visible)
    {
        if (_segments_visible.size() < _points.size())
        {
            _segments_visible.resize(_points.size(), true);
        }
        if (index < _points.size())
        {
            _segments_visible[index] = visible;
        }
    }

    bool Polyline2D::get_segment_visible(const size_t index) const
    {
        if (index < _points.size())
        {
            return _segments_visible[index];
        }
        return false;
    }

    std::vector<Vec2d> Polyline2D::get_points() const
    {
        return _points;
    }

    Vec2d Polyline2D::get_point(const size_t index) const
    {
        if (index < _points.size())
        {
            return _points[index];
        }
        return Vec2d::zero();
    }

    size_t Polyline2D::get_num_points() const
    {
        return _points.size();
    }

    void Polyline2D::set_close(const bool close)
    {
        _do_close = close;
    }

    bool Polyline2D::get_close() const
    {
        return _do_close;
    }

    void Polyline2D::set_rounded_corners(const bool rounded)
    {
        _do_rounded_corners = rounded;
        set_obb_dirty();
    }

    bool Polyline2D::get_rounded_corners() const
    {
        return _do_rounded_corners;
    }

    void Polyline2D::set_line_thickness(const double t)
    {
        _line_thickness = t;
        set_obb_dirty();
    }

    double Polyline2D::get_line_thickness() const
    {
        return _line_thickness;
    }

    void Polyline2D::set_fill(const bool f)
    {
        _do_fill = f;
    }

    bool Polyline2D::get_fill() const
    {
        return _do_fill;
    }

    void Polyline2D::rasterize_rounded_corner(
        const Vec2d pos,
        const double angle0,
        const double angle1,
        const Matrix3x3d& transform,
        std::vector<Vec2i>& pixels
    ) const
    {
        std::vector<Vec2d> vertices;

        for (int i = 0; i <= CORNER_SEGMENTS; ++i)
        {
            const double progress { static_cast<double>(i) / static_cast<double>(CORNER_SEGMENTS) };
            const double theta { angle0 + (angle1 - angle0) * progress };

            const Vec2d vertex {
                pos.x + _line_thickness / 2.0 * std::cos(theta),
                pos.y + _line_thickness / 2.0 * std::sin(theta)
            };

            vertices.push_back(Transform2D::transform_point(vertex, transform));
        }
        const Vec2d transformed_pos = Transform2D::transform_point(pos, transform);

        for (int i = 0; i < vertices.size() - 1; ++i)
        {
            Rasterize::rasterize_filled_triangle({ transformed_pos, vertices[i], vertices[i + 1] }, pixels);
        }
    }

    void Polyline2D::rasterize_rounded_corners(const Matrix3x3d& transform, std::vector<Vec2i>& pixels) const
    {
        for (int i = 0; i < _points.size(); ++i)
        {
            Vec2d p0 { _points[(i - 1 + _points.size()) % _points.size()] };
            Vec2d p1 { _points[i] };
            Vec2d p2 { _points[(i + 1) % _points.size()] };

            const Vec2d normal0 { (p0 - p1).normal().normalize() };
            const Vec2d normal1 { (p1 - p2).normal().normalize() };

            Vec2d between { (p1 - p0 + (p1 - p2)).normalize() };

            const double angle0 { std::atan2(normal0.y, normal0.x) };
            const double angle1 { std::atan2(normal1.y, normal1.x) };

            double angle_diff { angle1 - angle0 };
            if (angle_diff <= 0)
            {
                angle_diff += 2 * std::numbers::pi;
            }

            constexpr double angle_overlap = 0.1;
            double pos_overlap = 0.2;

            rasterize_rounded_corner(
                p1 - between * pos_overlap,
                angle0 - angle_overlap,
                angle0 + angle_diff + angle_overlap,
                transform,
                pixels
            );
        }
    }

    void Polyline2D::rasterize_edge(
        const Vec2d start,
        const Vec2d end,
        const Matrix3x3d& transform,
        std::vector<Vec2i>& pixels
    ) const
    {
        const double line_extent { _line_thickness / 2.0 };
        const Vec2d normal { (end - start).normal().normalize() };

        const Vec2d offset { normal * line_extent };

        Vec2d v0 { start + offset };
        Vec2d v1 { start - offset };
        Vec2d v2 { end + offset };
        Vec2d v3 { end - offset };

        v0 = Transform2D::transform_point(v0, transform);
        v1 = Transform2D::transform_point(v1, transform);
        v2 = Transform2D::transform_point(v2, transform);
        v3 = Transform2D::transform_point(v3, transform);

        Rasterize::rasterize_filled_triangle({ v0, v1, v2 }, pixels);
        Rasterize::rasterize_filled_triangle({ v1, v3, v2 }, pixels);
    }
}
