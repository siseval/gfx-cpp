#include "circle-2D.h"

#include "gfx/geometry/transform-2D.h"

namespace gfx
{
    Primitive2D::RasterizeOutput Circle2D::rasterize(const Matrix3x3d& transform) const
    {
        RasterizeOutput output;

        if (_radius <= 0)
        {
            return output;
        }

        const double line_extent { _line_thickness / 2.0 };
        const auto [min, max] { get_axis_aligned_bounding_box(transform) };
        const Matrix3x3d inverse_transform { Transform2D::invert_affine(transform) };

        for (int y = min.y; y <= max.y; y++)
        {
            for (int x = min.x; x <= max.x; x++)
            {
                const Vec2d pos {
                    Transform2D::transform_point(
                        Vec2d { static_cast<double>(x), static_cast<double>(y) },
                        inverse_transform
                    ) - Vec2d(_radius)
                };

                const double r_outer { _radius + line_extent };
                const double r_inner { _radius - line_extent };

                const double distance { std::sqrt(pos.x * pos.x + pos.y * pos.y) };

                if (distance <= r_outer && (get_filled() || distance >= r_inner))
                {
                    output.pixels.push_back(Vec2i { x, y });
                }
            }
        }

        return output;
    }


    Box2d Circle2D::get_geometry_size() const
    {
        return Box2d { Vec2d::zero(), Vec2d(_radius * 2) };
    }

    Box2d Circle2D::get_axis_aligned_bounding_box(const Matrix3x3d& transform) const
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

    double Circle2D::get_radius() const
    {
        return _radius;
    }

    void Circle2D::set_radius(const double r)
    {
        _radius = r;
        set_obb_dirty();
    }

    double Circle2D::get_line_thickness() const
    {
        return _line_thickness;
    }

    void Circle2D::set_line_thickness(const double t)
    {
        _line_thickness = t;
        set_obb_dirty();
    }

    bool Circle2D::get_filled() const
    {
        return _filled;
    }

    void Circle2D::set_filled(const bool f)
    {
        _filled = f;
    }
}
