#include "ellipse-2D.h"

#include "gfx/geometry/transform-2D.h"


namespace gfx
{
    Primitive2D::RasterizeOutput Ellipse2D::rasterize(const Matrix3x3d& transform) const
    {
        RasterizeOutput output;

        if (_radius.x <= 0 || _radius.y <= 0)
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
                    ) - _radius
                };
                const Vec2d r_outer { _radius + Vec2d(line_extent) };
                const Vec2d r_inner { _radius - Vec2d(line_extent) };

                const double sdf_outer {
                    pos.x * pos.x / (r_outer.x * r_outer.x) + pos.y * pos.y / (r_outer.y * r_outer.y)
                };
                const double sdf_inner {
                    pos.x * pos.x / (r_inner.x * r_inner.x) + pos.y * pos.y / (r_inner.y * r_inner.y)
                };

                if (sdf_outer <= 1.0 && (get_filled() || sdf_inner >= 1.0))
                {
                    output.pixels.push_back(Vec2i { x, y });
                }
            }
        }

        return output;
    }


    Box2d Ellipse2D::get_geometry_size() const
    {
        return Box2d { Vec2d::zero(), _radius * 2 };
    }

    Box2d Ellipse2D::get_axis_aligned_bounding_box(const Matrix3x3d& transform) const
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

    Vec2d Ellipse2D::get_radius() const
    {
        return _radius;
    }

    void Ellipse2D::set_radius(const Vec2d r)
    {
        _radius = r;
        set_obb_dirty();
    }

    void Ellipse2D::set_radius(const double rx, const double ry)
    {
        _radius = Vec2d { rx, ry };
        set_obb_dirty();
    }

    double Ellipse2D::get_line_thickness() const
    {
        return _line_thickness;
    }

    void Ellipse2D::set_line_thickness(const double t)
    {
        _line_thickness = t;
        set_obb_dirty();
    }

    bool Ellipse2D::get_filled() const
    {
        return _filled;
    }

    void Ellipse2D::set_filled(const bool f)
    {
        _filled = f;
    }


    // void Ellipse2D::rasterize_polygon_ring(std::shared_ptr<RenderSurface> surface, const Matrix3x3d &transform) const
    // {
    //     std::vector<Vec2d> outer_ring;
    //     std::vector<Vec2d> inner_ring;
    //     double line_extent = line_thickness / 2.0;
    //
    //     for (int i = 0; i < SEGMENTS; ++i)
    //     {
    //         double progress = static_cast<double>(i) / static_cast<double>(SEGMENTS);
    //         double theta = 2.0 * std::numbers::pi * progress;
    //
    //         double outer_x = (radius.x + line_extent) * std::cos(theta) + radius.x;
    //         double outer_y = (radius.y + line_extent) * std::sin(theta) + radius.y;
    //         outer_ring.push_back(Vec2d { outer_x, outer_y });
    //
    //         double inner_x = (radius.x - line_extent) * std::cos(theta) + radius.x;
    //         double inner_y = (radius.y - line_extent) * std::sin(theta) + radius.y;
    //         inner_ring.push_back(Vec2d { inner_x, inner_y });
    //     }
    //
    //     std::vector<Vec2d> transformed_outer_ring = transform_points(outer_ring, transform);
    //     std::vector<Vec2d> transformed_inner_ring = transform_points(inner_ring, transform);
    //
    //     for (int i = 0; i < SEGMENTS; ++i)
    //     {
    //         Vec2d v0 = transformed_outer_ring[i];
    //         Vec2d v1 = transformed_outer_ring[(i + 1) % SEGMENTS];
    //         Vec2d v2 = transformed_inner_ring[i];
    //         Vec2d v3 = transformed_inner_ring[(i + 1) % SEGMENTS];
    //
    //         rasterize_filled_triangle(surface, Triangle { v0, v1, v2 }, get_color());
    //         rasterize_filled_triangle(surface, Triangle { v1, v3, v2 }, get_color());
    //     }
    // }

    // std::vector<Vec2i> Ellipse2D::rasterize(const Matrix3x3d &transform) const
    // {
    //     std::vector<Vec2i> pixels;
    //
    //     if (radius.x <= 0 || radius.y <= 0)
    //     {
    //         return pixels;
    //     }
    //
    //     double line_extent { line_thickness / 2.0 };
    //     Box2d AABB { get_axis_aligned_bounding_box(transform) };
    //     Matrix3x3d inverse_transform { Transform2D::invert_affine(transform) };
    //
    //     auto worker = [&](int start_y, int end_y) {
    //         for (int y = start_y; y <= end_y; y++)
    //         {
    //             for (int x = AABB.min.x; x <= AABB.max.x; x++)
    //             {
    //                 Vec2d pos { Transform2D::transform_point(Vec2d { static_cast<double>(x) , static_cast<double>(y) }, inverse_transform) - radius };
    //                 Vec2d r_outer { radius + Vec2d(line_extent) };
    //                 Vec2d r_inner { radius - Vec2d(line_extent) };
    //
    //                 double sdf_outer { (pos.x * pos.x) / (r_outer.x * r_outer.x) + (pos.y * pos.y) / (r_outer.y * r_outer.y) };
    //                 double sdf_inner { (pos.x * pos.x) / (r_inner.x * r_inner.x) + (pos.y * pos.y) / (r_inner.y * r_inner.y) };
    //
    //                 if (sdf_outer <= 1.0 && (get_filled() || sdf_inner >= 1.0))
    //                 {
    //                     pixels.push_back(Vec2i { x, y });
    //                     continue;
    //                 }
    //             }
    //         }
    //     };
    //
    //     Vec2d size { AABB.size() };
    //     if (size.x * size.y < MIN_MULTITHREAD_PIXELS)
    //     {
    //         worker(static_cast<int>(AABB.min.y), static_cast<int>(AABB.max.y));
    //         return pixels;
    //     }
    //
    //     unsigned int num_threads { std::thread::hardware_concurrency() };
    //     std::vector<std::thread> threads;
    //
    //     for (unsigned int i = 0; i < num_threads; ++i)
    //     {
    //         int start_y { static_cast<int>(AABB.min.y) + static_cast<int>(i * size.y / num_threads) };
    //         int end_y { static_cast<int>(AABB.min.y) + static_cast<int>((i + 1) * size.y / num_threads) - 1 };
    //
    //         if (i == num_threads - 1)
    //         {
    //             end_y = static_cast<int>(AABB.max.y);
    //         }
    //
    //         threads.emplace_back(worker, start_y, end_y);
    //     }
    //
    //     for (auto &thread : threads)
    //     {
    //         thread.join();
    //     }
    //
    //     return pixels;
    // }
}
