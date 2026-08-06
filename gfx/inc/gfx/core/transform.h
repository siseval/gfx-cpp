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

        void set_transform(
            VectorType translation,
            RotationType rotation,
            VectorType scale,
            VectorType anchor_offset = VectorType::zero()
        );
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
        const VectorType scale,
        const VectorType anchor_offset
    )
    {
        if constexpr (std::same_as<VectorType, Vec2d>)
        {
            const double cos_angle = std::cos(rotation);
            const double sin_angle = std::sin(rotation);

            const double m00 = scale.x * cos_angle;
            const double m01 = scale.y * -sin_angle;
            const double m10 = scale.x * sin_angle;
            const double m11 = scale.y * cos_angle;

            const double tx = translation.x - (m00 * anchor_offset.x + m01 * anchor_offset.y);
            const double ty = translation.y - (m10 * anchor_offset.x + m11 * anchor_offset.y);

            _matrix = MatrixType {
                { m00, m01, tx },
                { m10, m11, ty },
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

            const double m00 = scale.x * (cos_y * cos_z);
            const double m01 = scale.y * (cos_z * sin_x * sin_y - cos_x * sin_z);
            const double m02 = scale.z * (cos_x * cos_z * sin_y + sin_x * sin_z);

            const double m10 = scale.x * (cos_y * sin_z);
            const double m11 = scale.y * (cos_x * cos_z + sin_x * sin_y * sin_z);
            const double m12 = scale.z * (cos_x * sin_y * sin_z - cos_z * sin_x);

            const double m20 = scale.x * -sin_y;
            const double m21 = scale.y * (cos_y * sin_x);
            const double m22 = scale.z * (cos_x * cos_y);

            const double tx = translation.x - (m00 * anchor_offset.x + m01 * anchor_offset.y + m02 * anchor_offset.z);
            const double ty = translation.y - (m10 * anchor_offset.x + m11 * anchor_offset.y + m12 * anchor_offset.z);
            const double tz = translation.z - (m20 * anchor_offset.x + m21 * anchor_offset.y + m22 * anchor_offset.z);

            _matrix = MatrixType {
                { m00, m01, m02, tx },
                { m10, m11, m12, ty },
                { m20, m21, m22, tz },
                { 0.0, 0.0, 0.0, 1.0 }
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
