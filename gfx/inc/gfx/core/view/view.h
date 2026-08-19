#pragma once

#include "gfx/core/transform.h"

namespace gfx
{
    template <typename VectorType>
    class View
    {
    public:

        using RotationType = Transform<VectorType>::RotationType;
        using MatrixType = Transform<VectorType>::MatrixType;

        View() = default;

        View(VectorType position, RotationType rotation);

        MatrixType get_matrix() const;

        void set_position(VectorType position);
        void set_rotation(RotationType rotation);

        VectorType get_position() const;
        RotationType get_rotation() const;


        /* 2D specific helpers */

        void set_position(double x, double y) requires std::same_as<VectorType, Vec2d>;
        void set_rotation(double angle) requires std::same_as<RotationType, double>;
        void set_rotation_degrees(double angle) requires std::same_as<RotationType, double>;


        /* 3D specific helpers */

        void set_position(double x, double y, double z) requires std::same_as<VectorType, Vec3d>;
        void set_rotation(double pitch, double yaw, double roll) requires std::same_as<RotationType, Vec3d>;
        void set_rotation_degrees(Vec3d rotation) requires std::same_as<RotationType, Vec3d>;
        void set_rotation_degrees(double pitch, double yaw, double roll) requires std::same_as<RotationType, Vec3d>;

        Vec3d get_forward() const requires std::same_as<VectorType, Vec3d>;

    private:

        VectorType _position;
        RotationType _rotation { 0.0 };
    };

    template <typename VectorType>
    View<VectorType>::View(
        const VectorType position,
        const RotationType rotation
    )
        : _position { position }
        , _rotation { rotation } {}

    template <typename VectorType>
    View<VectorType>::MatrixType View<VectorType>::get_matrix() const
    {
        return Transform<VectorType>(_position, _rotation, VectorType::one()).get_matrix().inverse();
    }

    template <typename VectorType>
    void View<VectorType>::set_position(const VectorType position)
    {
        _position = position;
    }

    template <typename VectorType>
    void View<VectorType>::set_rotation(const RotationType rotation)
    {
        _rotation = rotation;
    }

    template <typename VectorType>
    VectorType View<VectorType>::get_position() const
    {
        return _position;
    }

    template <typename VectorType>
    View<VectorType>::RotationType View<VectorType>::get_rotation() const
    {
        return _rotation;
    }


    template <typename VectorType>
    void View<VectorType>::set_position(const double x, const double y) requires std::same_as<VectorType, Vec2d>
    {
        _position = Vec2d { x, y };
    }

    template <typename VectorType>
    void View<VectorType>::set_rotation(const double angle) requires std::same_as<RotationType, double>
    {
        _rotation = angle;
    }

    template <typename VectorType>
    void View<VectorType>::set_rotation_degrees(const double angle) requires std::same_as<RotationType, double>
    {
        _rotation = angle * std::numbers::pi / 180;
    }

    template <typename VectorType>
    void View<VectorType>::set_position(const double x, const double y, const double z) requires std::same_as<
        VectorType, Vec3d>
    {
        _position = Vec3d { x, y, z };
    }

    template <typename VectorType>
    void View<VectorType>::set_rotation(const double pitch, const double yaw, const double roll) requires std::same_as<
        RotationType, Vec3d>
    {
        _rotation = Vec3d { pitch, yaw, roll };
    }

    template <typename VectorType>
    void View<VectorType>::set_rotation_degrees(const Vec3d rotation) requires std::same_as<RotationType, Vec3d>
    {
        _rotation = Vec3d {
            rotation.x * std::numbers::pi / 180,
            rotation.y * std::numbers::pi / 180,
            rotation.z * std::numbers::pi / 180
        };
    }

    template <typename VectorType>
    void View<VectorType>::set_rotation_degrees(const double pitch, const double yaw, const double roll) requires
        std::same_as<RotationType, Vec3d>
    {
        _rotation = Vec3d {
            pitch * std::numbers::pi / 180,
            yaw * std::numbers::pi / 180,
            roll * std::numbers::pi / 180
        };
    }

    template <typename VectorType>
    Vec3d View<VectorType>::get_forward() const requires std::same_as<VectorType, Vec3d>
    {
        const double pitch { _rotation.x };
        const double yaw { _rotation.y };

        const Vec3d forward {
            std::cos(pitch) * std::sin(yaw),
            -std::sin(pitch),
            std::cos(pitch) * std::cos(yaw)
        };

        return forward.normalize();
    }
}
