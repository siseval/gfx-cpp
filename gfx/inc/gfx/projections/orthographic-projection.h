#pragma once

#include "gfx/core/view/projection.h"
#include "gfx/math/matrix.h"

namespace gfx
{
    class OrthographicProjection : public Projection<Vec2d>
    {
    public:

        OrthographicProjection() = default;
        explicit OrthographicProjection(double zoom);

        Matrix3x3d get_matrix(double aspect_ratio) const override;

        void set_zoom(double zoom);
        double get_zoom() const;

        ViewBounds<Vec2d> get_view_bounds(const View<Vec2d>& view, const Viewport& viewport) const override;

    private:

        double _zoom { 1.0 };
    };
}
