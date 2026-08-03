#pragma once

#include "gfx/core/view/projection.h"
#include "gfx/math/matrix.h"

namespace gfx
{
    class ViewportProjection : Projection<Vec2d>
    {
    public:

        ViewportProjection() = default;

        Matrix3x3d get_matrix(double) const override;

        ViewBounds<Vec2d> get_view_bounds(const View<Vec2d>&, const Viewport& viewport) const override;
    };
}
