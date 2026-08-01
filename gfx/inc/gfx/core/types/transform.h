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

        VectorType transform_coordinate(const VectorType coordinate)
        {
            VectorType scaled_rotated { scale_rotate(coordinate) };
            return scaled_rotated + _translation;
        }

        void set_translation(const VectorType translation)
        {
            _translation = translation;
        }

        void set_scale(const VectorType scale)
        {
            _scale = scale;
            _scale_dirty = true;
        }

        void set_rotation(const RotationType rotation)
        {
            _rotation.rotation = rotation;
            _rotation_dirty = true;
        }

    private:

        void update_scale_rotation()
        {
            if constexpr (std::same_as<VectorType, Vec2d>)
            {
                const double c = std::cos(_rotation);
                const double s = std::sin(_rotation);

                _cached_scale_rotation = MatrixType {
                    { _scale.x * c, _scale.y * -s },
                    { _scale.x * s, _scale.y * c }
                };
            }
            else
            {
                const double cx = std::cos(_rotation.rotation.x);
                const double sx = std::sin(_rotation.rotation.x);
                const double cy = std::cos(_rotation.rotation.y);
                const double sy = std::sin(_rotation.rotation.y);
                const double cz = std::cos(_rotation.rotation.z);
                const double sz = std::sin(_rotation.rotation.z);

                _cached_scale_rotation = MatrixType {
                    {
                        _scale.x * (cy * cz),
                        _scale.y * (cz * sx * sy - cx * sz),
                        _scale.z * (cx * cz * sy + sx * sz)
                    },
                    {
                        _scale.x * (cy * sz),
                        _scale.y * (cx * cz + sx * sy * sz),
                        _scale.z * (cx * sy * sz - cz * sx)
                    },
                    {
                        _scale.x * -sy,
                        _scale.y * (cy * sx),
                        _scale.z * (cx * cy)
                    }
                };
            }

            _rotation_dirty = false;
            _scale_dirty = false;
        }

        VectorType scale_rotate(const VectorType coordinate) const
        {
            if (_rotation_dirty || _scale_dirty)
            {
                update_scale_rotation();
            }
            
            if constexpr (std::same_as<VectorType, Vec2d>)
            {
                return Vec2d {
                    _cached_scale_rotation(0, 0) * coordinate.x + _cached_scale_rotation(0, 1) * coordinate.y,
                    _cached_scale_rotation(1, 0) * coordinate.x + _cached_scale_rotation(1, 1) * coordinate.y
                };
            }
            else
            {
                return Vec3d {
                    _cached_scale_rotation(0, 0) * coordinate.x + _cached_scale_rotation(0, 1) * coordinate.y +
                    _cached_scale_rotation(0, 2) * coordinate.z,

                    _cached_scale_rotation(1, 0) * coordinate.x + _cached_scale_rotation(1, 1) * coordinate.y +
                    _cached_scale_rotation(1, 2) * coordinate.z,

                    _cached_scale_rotation(2, 0) * coordinate.x + _cached_scale_rotation(2, 1) * coordinate.y +
                    _cached_scale_rotation(2, 2) * coordinate.z
                };
            }
        }

        VectorType _translation;
        RotationType _rotation;
        VectorType _scale;

        using MatrixType = std::conditional_t<std::same_as<VectorType, Vec2d>, Matrix2x2d, Matrix3x3d>;
        mutable MatrixType _cached_scale_rotation;

        bool _rotation_dirty { true };
        bool _scale_dirty { true };
    };
}
