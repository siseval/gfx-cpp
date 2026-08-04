#include "gfx/projections/perspective-projection.h"

#include "gfx/math/matrix.h"

namespace gfx
{
    PerspectiveProjection::PerspectiveProjection(const double z_near, const double z_far, const double fov)
        : _z_near(z_near)
        , _z_far(z_far)
        , _fov(fov) {}

    Matrix4x4d PerspectiveProjection::get_matrix(const double aspect_ratio) const
    {
        const double f { 1.0 / std::tan(_fov / 2.0) };

        Matrix4x4d projection { Matrix4x4d::zero() };
        projection(0, 0) = f / aspect_ratio;
        projection(1, 1) = -f;

        projection(2, 2) = _z_far / (_z_far - _z_near);
        projection(2, 3) = -_z_near * _z_far / (_z_far - _z_near);
        projection(3, 2) = 1.0;

        return projection;
    }

    void PerspectiveProjection::set_fov(const double fov)
    {
        _fov = fov;
    }

    void PerspectiveProjection::set_fov_degrees(const double fov)
    {
        _fov = fov * std::numbers::pi / 180;
    }

    void PerspectiveProjection::set_z_near(const double z_near)
    {
        _z_near = z_near;
    }

    void PerspectiveProjection::set_z_far(const double z_far)
    {
        _z_far = z_far;
    }

    double PerspectiveProjection::get_fov() const
    {
        return _fov;
    }

    double PerspectiveProjection::get_z_near() const
    {
        return _z_near;
    }

    double PerspectiveProjection::get_z_far() const
    {
        return _z_far;
    }

    ViewBounds<Vec3d> PerspectiveProjection::get_view_bounds(const View<Vec3d>& view, const Viewport& viewport) const
    {
        const Vec3d pos { view.get_position() };
        const Vec3d forward { view.get_forward() };

        const double aspect_ratio { viewport.get_aspect_ratio() };

        const double half_height_near { std::tan(_fov / 2.0) * _z_near };
        const double half_width_near { half_height_near * aspect_ratio };

        const Vec3d near_center { pos + forward * _z_near };
        const Vec3d far_center { pos + forward * _z_far };

        const Vec3d world_up { std::abs(forward.y) > 0.999 ? Vec3d { 0.0, 0.0, 1.0 } : Vec3d { 0.0, 1.0, 0.0 } };
        const Vec3d right { Vec3d::cross(forward, world_up).normalize() };
        const Vec3d up { Vec3d::cross(right, forward).normalize() };

        const Vec3d near_top_left { near_center + up * half_height_near - right * half_width_near };
        const Vec3d near_top_right { near_center + up * half_height_near + right * half_width_near };
        const Vec3d near_bottom_left { near_center - up * half_height_near - right * half_width_near };
        const Vec3d near_bottom_right { near_center - up * half_height_near + right * half_width_near };

        const Vec3d near_normal { forward };
        const double near_d { -Vec3d::dot(near_normal, near_center) };

        const Vec3d far_normal { -forward };
        const double far_d { -Vec3d::dot(far_normal, far_center) };

        const Vec3d left_normal { Vec3d::cross(near_bottom_left - pos, near_top_left - pos).normalize() };
        const double left_d { -Vec3d::dot(left_normal, pos) };

        const Vec3d right_normal {
            Vec3d::cross(near_top_right - pos, near_bottom_right - pos).normalize()
        };
        const double right_d { -Vec3d::dot(right_normal, pos) };

        const Vec3d top_normal { Vec3d::cross(near_top_left - pos, near_top_right - pos).normalize() };
        const double top_d { -Vec3d::dot(top_normal, pos) };

        const Vec3d bottom_normal {
            Vec3d::cross(near_bottom_right - pos, near_bottom_left - pos).normalize()
        };
        const double bottom_d { -Vec3d::dot(bottom_normal, pos) };

        return ViewBounds<Vec3d>(
            Frustum {
                Frustum::Plane { top_normal, top_d },
                Frustum::Plane { bottom_normal, bottom_d },
                Frustum::Plane { left_normal, left_d },
                Frustum::Plane { right_normal, right_d },
                Frustum::Plane { near_normal, near_d },
                Frustum::Plane { far_normal, far_d }
            }
        );
    }
}
