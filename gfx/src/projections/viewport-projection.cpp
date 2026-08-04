#include "gfx/projections/viewport-projection.h"

#include "gfx/math/matrix.h"

namespace gfx
{
    Matrix3x3d ViewportProjection::get_matrix(const double) const
    {
        return Matrix3x3d::identity();
    }

    ViewBounds<Vec2d> ViewportProjection::get_view_bounds(const View<Vec2d>&, const Viewport& viewport) const
    {
        return ViewBounds<Vec2d>(
            OrientedBox<Vec2d>(
                static_cast<Vec2d>(viewport.offset),
                Vec2d(viewport.size.x, 0.0),
                Vec2d(0.0, viewport.size.y)
            )
        );
    }
}
