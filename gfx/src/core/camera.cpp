#include "gfx/core/camera.h"

#include "gfx/geometry/transform-3D.h"

namespace gfx
{
    Camera::Camera()
        : _position { 0.0, 0.0, 0.0 }
        , _rotation { 0.0, 0.0, 0.0 }
        , _z_near { 0.1 }
        , _z_far { 1000.0 }
        , _fov { std::numbers::pi / 4.0 } {}

    Camera::Camera(
        const Vec3d position,
        const Vec3d rotation,
        const double z_near,
        const double z_far,
        const double fov
    )
        : _position { position }
        , _rotation { rotation }
        , _z_near { z_near }
        , _z_far { z_far }
        , _fov { fov } {}

    Matrix4x4d Camera::get_view_matrix() const
    {
        const Matrix4x4d translation = Transform3D::translate({ -_position.x, -_position.y, -_position.z });
        const Matrix4x4d rotation_x = Transform3D::rotate_x(-_rotation.x);
        const Matrix4x4d rotation_y = Transform3D::rotate_y(-_rotation.y);
        const Matrix4x4d rotation_z = Transform3D::rotate_z(-_rotation.z);

        return rotation_x * rotation_y * rotation_z * translation;
    }

    Matrix4x4d Camera::get_projection_matrix(const double aspect_ratio) const
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

    Vec3d Camera::get_forward() const
    {
        const double pitch_rad { _rotation.x };
        const double yaw_rad { _rotation.y };

        const Vec3d forward {
            std::cos(pitch_rad) * std::sin(yaw_rad),
            -std::sin(pitch_rad),
            std::cos(pitch_rad) * std::cos(yaw_rad)
        };

        return forward.normalize();
    }

    void Camera::set_position(const Vec3d pos)
    {
        _position = pos;
    }

    void Camera::set_position(const double x, const double y, const double z)
    {
        _position = Vec3d { x, y, z };
    }

    void Camera::set_fov(const double f)
    {
        _fov = f;
    }

    void Camera::set_fov_degrees(const double fov_degrees)
    {
        _fov = fov_degrees * std::numbers::pi / 180;
    }

    void Camera::set_rotation(const Vec3d rot)
    {
        _rotation = rot;
    }

    void Camera::set_rotation(const double pitch, const double yaw, const double roll)
    {
        _rotation = Vec3d { pitch, yaw, roll };
    }

    void Camera::set_rotation_degrees(const Vec3d rot_deg)
    {
        _rotation = Vec3d {
            rot_deg.x * std::numbers::pi / 180,
            rot_deg.y * std::numbers::pi / 180,
            rot_deg.z * std::numbers::pi / 180
        };
    }

    void Camera::set_rotation_degrees(const double pitch_deg, const double yaw_deg, const double roll_deg)
    {
        _rotation = Vec3d {
            pitch_deg * std::numbers::pi / 180,
            yaw_deg * std::numbers::pi / 180,
            roll_deg * std::numbers::pi / 180
        };
    }

    void Camera::set_z_near(const double zn)
    {
        _z_near = zn;
    }

    void Camera::set_z_far(const double zf)
    {
        _z_far = zf;
    }

    Vec3d Camera::get_position() const
    {
        return _position;
    }

    double Camera::get_fov() const
    {
        return _fov;
    }

    Vec3d Camera::get_rotation() const
    {
        return _rotation;
    }

    double Camera::get_z_near() const
    {
        return _z_near;
    }

    double Camera::get_z_far() const
    {
        return _z_far;
    }

    Frustum Camera::get_frustum(const double aspect_ratio) const
    {
        const Vec3d forward { get_forward() };

        const double half_height_near { std::tan(_fov / 2.0) * _z_near };
        const double half_width_near { half_height_near * aspect_ratio };

        const Vec3d near_center { _position + forward * _z_near };
        const Vec3d far_center { _position + forward * _z_far };

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

        const Vec3d left_normal { Vec3d::cross(near_bottom_left - _position, near_top_left - _position).normalize() };
        const double left_d { -Vec3d::dot(left_normal, _position) };

        const Vec3d right_normal {
            Vec3d::cross(near_top_right - _position, near_bottom_right - _position).normalize()
        };
        const double right_d { -Vec3d::dot(right_normal, _position) };

        const Vec3d top_normal { Vec3d::cross(near_top_left - _position, near_top_right - _position).normalize() };
        const double top_d { -Vec3d::dot(top_normal, _position) };

        const Vec3d bottom_normal {
            Vec3d::cross(near_bottom_right - _position, near_bottom_left - _position).normalize()
        };
        const double bottom_d { -Vec3d::dot(bottom_normal, _position) };

        return Frustum {
            Frustum::Plane { top_normal, top_d },
            Frustum::Plane { bottom_normal, bottom_d },
            Frustum::Plane { left_normal, left_d },
            Frustum::Plane { right_normal, right_d },
            Frustum::Plane { near_normal, near_d },
            Frustum::Plane { far_normal, far_d }
        };
    }
}
