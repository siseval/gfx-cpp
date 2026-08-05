#pragma once

#include <cmath>
#include <concepts>
#include <type_traits>

#include "gfx/math/matrix.h"
#include "gfx/math/vec2.h"
#include "gfx/math/vec3.h"

namespace gfx
{
    template <typename VectorType>
        requires (std::same_as<VectorType, Vec3d> || std::same_as<VectorType, Vec2d>)
    class Transform
    {
    public:

        using RotationType = std::conditional_t<std::same_as<VectorType, Vec3d>, Vec3d, double>;
        using MatrixType = std::conditional_t<std::same_as<VectorType, Vec3d>, Matrix4x4d, Matrix3x3d>;

        Transform();
        Transform(VectorType translation, RotationType rotation, VectorType scale);
        explicit Transform(const MatrixType& matrix);

        void set_transform(VectorType translation, RotationType rotation, VectorType scale);
        const MatrixType& get_matrix() const;

        Transform combine(const Transform& child) const;
        VectorType transform_coordinate(const VectorType& coordinate) const;
        VectorType transform_vector(const VectorType& vector) const;

    private:

        MatrixType _matrix;
    };

    template <typename VectorType>
        requires (std::same_as<VectorType, Vec3d> || std::same_as<VectorType, Vec2d>)
    Transform<VectorType>::Transform()
        : _matrix(MatrixType::identity()) {}

    template <typename VectorType>
        requires (std::same_as<VectorType, Vec3d> || std::same_as<VectorType, Vec2d>)
    Transform<VectorType>::Transform(VectorType translation, RotationType rotation, VectorType scale)
    {
        set_transform(translation, rotation, scale);
    }

    template <typename VectorType>
        requires (std::same_as<VectorType, Vec3d> || std::same_as<VectorType, Vec2d>)
    Transform<VectorType>::Transform(const MatrixType& matrix)
        : _matrix(matrix) {}

    template <typename VectorType>
        requires (std::same_as<VectorType, Vec3d> || std::same_as<VectorType, Vec2d>)
    void Transform<VectorType>::set_transform(
        const VectorType translation,
        const RotationType rotation,
        const VectorType scale
    )
    {
        if constexpr (std::same_as<VectorType, Vec2d>)
        {
            double cos_angle = std::cos(rotation);
            double sin_angle = std::sin(rotation);

            _matrix = MatrixType {
                { scale.x * cos_angle, scale.y * -sin_angle, translation.x },
                { scale.x * sin_angle, scale.y * cos_angle, translation.y },
                { 0.0, 0.0, 1.0 }
            };
        }
        else
        {
            const double cos_x = std::cos(rotation.x);
            const double sin_x = std::sin(rotation.x);
            const double cos_y = std::cos(rotation.y);
            const double sin_y = std::sin(rotation.y);
            const double cos_z = std::cos(rotation.z);
            const double sin_z = std::sin(rotation.z);

            _matrix = MatrixType {
                {
                    scale.x * (cos_y * cos_z),
                    scale.y * (cos_z * sin_x * sin_y - cos_x * sin_z),
                    scale.z * (cos_x * cos_z * sin_y + sin_x * sin_z),
                    translation.x
                },
                {
                    scale.x * (cos_y * sin_z),
                    scale.y * (cos_x * cos_z + sin_x * sin_y * sin_z),
                    scale.z * (cos_x * sin_y * sin_z - cos_z * sin_x),
                    translation.y
                },
                {
                    scale.x * -sin_y,
                    scale.y * (cos_y * sin_x),
                    scale.z * (cos_x * cos_y),
                    translation.z
                },
                {
                    0.0,
                    0.0,
                    0.0,
                    1.0
                }
            };
        }
    }

    template <typename VectorType>
        requires (std::same_as<VectorType, Vec3d> || std::same_as<VectorType, Vec2d>)
    const Transform<VectorType>::MatrixType& Transform<VectorType>::get_matrix() const
    {
        return _matrix;
    }

    template <typename VectorType>
        requires (std::same_as<VectorType, Vec3d> || std::same_as<VectorType, Vec2d>)
    Transform<VectorType> Transform<VectorType>::combine(const Transform& child) const
    {
        return Transform(_matrix * child._matrix);
    }

    template <typename VectorType>
        requires (std::same_as<VectorType, Vec3d> || std::same_as<VectorType, Vec2d>)
    VectorType Transform<VectorType>::transform_coordinate(const VectorType& coordinate) const
    {
        if constexpr (std::same_as<VectorType, Vec2d>)
        {
            return Vec2d {
                _matrix(0, 0) * coordinate.x + _matrix(0, 1) * coordinate.y + _matrix(0, 2) * 1.0,
                _matrix(1, 0) * coordinate.x + _matrix(1, 1) * coordinate.y + _matrix(1, 2) * 1.0
            };
        }
        else
        {
            return Vec3d {
                _matrix(0, 0) * coordinate.x + _matrix(0, 1) * coordinate.y + _matrix(0, 2) * coordinate.z +
                _matrix(0, 3) * 1.0,
                _matrix(1, 0) * coordinate.x + _matrix(1, 1) * coordinate.y + _matrix(1, 2) * coordinate.z +
                _matrix(1, 3) * 1.0,
                _matrix(2, 0) * coordinate.x + _matrix(2, 1) * coordinate.y + _matrix(2, 2) * coordinate.z +
                _matrix(2, 3) * 1.0
            };
        }
    }
    
    template <typename VectorType>
        requires (std::same_as<VectorType, Vec3d> || std::same_as<VectorType, Vec2d>)
    VectorType Transform<VectorType>::transform_vector(const VectorType& vector) const
    {
        if constexpr (std::same_as<VectorType, Vec2d>)
        {
            return Vec2d {
                _matrix(0, 0) * vector.x + _matrix(0, 1) * vector.y,
                _matrix(1, 0) * vector.x + _matrix(1, 1) * vector.y
            };
        }
        else
        {
            return Vec3d {
                _matrix(0, 0) * vector.x + _matrix(0, 1) * vector.y + _matrix(0, 2) * vector.z,
                _matrix(1, 0) * vector.x + _matrix(1, 1) * vector.y + _matrix(1, 2) * vector.z,
                _matrix(2, 0) * vector.x + _matrix(2, 1) * vector.y + _matrix(2, 2) * vector.z
            };
        }
    }
}
