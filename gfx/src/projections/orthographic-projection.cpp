#include "gfx/projections/orthographic-projection.h"

#include "gfx/math/matrix.h"

namespace gfx
{
    OrthographicProjection::OrthographicProjection(const double zoom)
        : _zoom(zoom) {}

    Matrix3x3d OrthographicProjection::get_matrix(const double aspect_ratio) const
    {
        Matrix3x3d matrix { Matrix3x3d::zero() };

        matrix(0, 0) = 2.0 / (_zoom * aspect_ratio);
        matrix(1, 1) = 2.0 / _zoom;
        matrix(2, 2) = 1.0;

        return matrix;
    }

    void OrthographicProjection::set_zoom(const double zoom)
    {
        _zoom = zoom;
    }

    double OrthographicProjection::get_zoom() const
    {
        return _zoom;
    }

    ViewBounds<Vec2d> OrthographicProjection::get_view_bounds(const View<Vec2d>& view, const Viewport& viewport) const
    {
        const double aspect_ratio { viewport.get_aspect_ratio() };
        const double rotation { view.get_rotation() };

        const double width { _zoom * aspect_ratio };
        const double height { _zoom };

        const double cos_r { std::cos(rotation) };
        const double sin_r { std::sin(rotation) };

        const Vec2d axis_x { cos_r, sin_r };
        const Vec2d axis_y { -sin_r, cos_r };

        const Vec2d side_x { axis_x * width };
        const Vec2d side_y { axis_y * height };

        const Vec2d origin { view.get_position() - (side_x + side_y) * 0.5 };

        return ViewBounds<Vec2d>(OrientedBox<Vec2d> { origin, side_x, side_y });
    }
}
