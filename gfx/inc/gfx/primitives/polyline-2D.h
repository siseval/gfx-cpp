#pragma once

#include <vector>

#include "gfx/core/primitive-2D.h"
#include "gfx/math/box2.h"
#include "gfx/math/matrix.h"
#include "gfx/math/vec2.h"

namespace gfx
{
    class Polyline2D final : public Primitive2D
    {
    public:

        RasterizeOutput rasterize(const Matrix3x3d& transform) const override;

        Box2d get_geometry_size() const override;
        Box2d get_axis_aligned_bounding_box(const Matrix3x3d& transform) const override;

        bool cache_clockwise();

        void add_point(Vec2d point);
        void add_point(double x, double y);
        void add_points(const std::vector<Vec2d>& new_points);

        void set_point(size_t index, Vec2d point);
        void set_point(size_t index, double x, double y);
        void set_points(const std::vector<Vec2d>& new_points);
        void clear_points();

        void set_segment_visible(size_t index, bool visible);
        bool get_segment_visible(size_t index) const;

        std::vector<Vec2d> get_points() const;
        Vec2d get_point(size_t index) const;

        size_t get_num_points() const;

        void set_close(bool close);
        bool get_close() const;

        void set_rounded_corners(bool rounded);
        bool get_rounded_corners() const;

        void set_line_thickness(double t);
        double get_line_thickness() const;

        void set_fill(bool f);
        bool get_fill() const;

        void rasterize_rounded_corner(
            Vec2d pos,
            double angle0,
            double angle1,
            const Matrix3x3d& transform,
            std::vector<Vec2i>& pixels
        ) const;

        void rasterize_rounded_corners(const Matrix3x3d& transform, std::vector<Vec2i>& pixels) const;

        void rasterize_edge(Vec2d start, Vec2d end, const Matrix3x3d& transform, std::vector<Vec2i>& pixels) const;

        std::vector<Vec2d> _points;
        std::vector<bool> _segments_visible;
        bool _do_close { false };
        bool _do_fill { false };
        double _line_thickness { 1.0 };
        bool _do_rounded_corners { false };
        bool _clockwise { false };
        static constexpr int CORNER_SEGMENTS { 8 };
    };
}
