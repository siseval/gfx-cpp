#pragma once

#include "gfx/core/primitive-2D.h"
#include "gfx/math/box2.h"
#include "gfx/math/matrix.h"

namespace gfx
{
    class Circle2D final : public Primitive2D
    {
    public:

        RasterizeOutput rasterize(const Matrix3x3d& transform) const override;

        Box2d get_geometry_size() const override;
        Box2d get_axis_aligned_bounding_box(const Matrix3x3d& transform) const override;

        double get_radius() const;
        void set_radius(double r);

        double get_line_thickness() const;
        void set_line_thickness(double t);

        bool get_filled() const;
        void set_filled(bool f);

    private:

        double _radius { 0.0 };
        bool _filled { false };
        double _line_thickness { 1.0 };
    };
}
