#pragma once

#include "gfx/math/matrix.h"
#include "gfx/math/vec2.h"
#include "gfx/math/vec3.h"

namespace gfx
{
    template <typename VectorType>
        requires (std::same_as<VectorType, Vec2d> || std::same_as<VectorType, Vec3d>)
    class Transform
    {
    public:

        using RotationType = std::conditional_t<std::same_as<VectorType, Vec2d>, double, Vec3d>;
        using MatrixType = std::conditional_t<std::same_as<VectorType, Vec2d>, Matrix2x2d, Matrix3x3d>;

        VectorType transform_coordinate(const VectorType& coordinate) const
        {
            if constexpr (std::same_as<VectorType, Vec2d>)
            {
                return Vec2d {
                    _scale_rotation(0, 0) * coordinate.x + _scale_rotation(0, 1) * coordinate.y + _translation.x,
                    _scale_rotation(1, 0) * coordinate.x + _scale_rotation(1, 1) * coordinate.y + _translation.y
                };
            }
            else
            {
                return Vec3d {
                    _scale_rotation(0, 0) * coordinate.x + _scale_rotation(0, 1) * coordinate.y + _scale_rotation(0, 2)
                    * coordinate.z + _translation.x,
                    _scale_rotation(1, 0) * coordinate.x + _scale_rotation(1, 1) * coordinate.y + _scale_rotation(1, 2)
                    * coordinate.z + _translation.y,
                    _scale_rotation(2, 0) * coordinate.x + _scale_rotation(2, 1) * coordinate.y + _scale_rotation(2, 2)
                    * coordinate.z + _translation.z
                };
            }
        }

        void set_translation(const VectorType translation)
        {
            _translation = translation;
        }

        void apply_scale(const VectorType& scale)
        {
            if constexpr (std::same_as<VectorType, Vec2d>)
            {
                Matrix2x2d scale_matrix {
                    { scale.x, 0.0 },
                    { 0.0, scale.y }
                };

                _scale_rotation = _scale_rotation * scale_matrix;
            }
            else
            {
                Matrix3x3d scale_matrix {
                    { scale.x, 0.0, 0.0 },
                    { 0.0, scale.y, 0.0 },
                    { 0.0, 0.0, scale.z }
                };

                _scale_rotation = _scale_rotation * scale_matrix;
            }
        }

        void apply_rotation(const RotationType& rotation)
        {
            if constexpr (std::same_as<VectorType, Vec2d>)
            {
                double cos_angle = std::cos(rotation);
                double sin_angle = std::sin(rotation);

                Matrix2x2d rotation_matrix {
                    { cos_angle, -sin_angle },
                    { sin_angle, cos_angle }
                };

                _scale_rotation = _scale_rotation * rotation_matrix;
            }
            else
            {
                const double cos_x = std::cos(rotation.x);
                const double sin_x = std::sin(rotation.x);
                const double cos_y = std::cos(rotation.y);
                const double sin_y = std::sin(rotation.y);
                const double cos_z = std::cos(rotation.z);
                const double sin_z = std::sin(rotation.z);

                Matrix3x3d rotation_matrix {
                    {
                        (cos_y * cos_z),
                        (cos_z * sin_x * sin_y - cos_x * sin_z),
                        (cos_x * cos_z * sin_y + sin_x * sin_z)
                    },
                    {
                        (cos_y * sin_z),
                        (cos_x * cos_z + sin_x * sin_y * sin_z),
                        (cos_x * sin_y * sin_z - cos_z * sin_x)
                    },
                    {
                        (-sin_y),
                        (cos_y * sin_x),
                        (cos_x * cos_y)
                    }
                };

                _scale_rotation = _scale_rotation * rotation_matrix;
            }
        }

        Transform combine(const Transform& child) const
        {
            return Transform {
                ._scale_rotation = _scale_rotation * child._scale_rotation,
                ._translation    = transform_coordinate(child._translation)
            };
        }

    private:

        VectorType _translation;
        MatrixType _scale_rotation;
    };
}
