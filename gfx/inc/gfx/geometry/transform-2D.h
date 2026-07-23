#pragma once

#include "gfx/math/matrix.h"
#include "gfx/math/vec2.h"

namespace gfx
{
    class Transform2D
    {
    public:

        static Matrix3x3d translate(Vec2d pos);
        static Matrix3x3d rotate(double angle);
        static Matrix3x3d scale(Vec2d scale);

        static Vec2d extract_translation(const Matrix3x3d& transform);
        static double extract_rotation(const Matrix3x3d& transform);
        static Vec2d extract_scale(const Matrix3x3d& transform);

        static Vec2d transform_point(Vec2d pos, const Matrix3x3d& transform);
        static Vec2d transform_vector(Vec2d vec, const Matrix3x3d& transform);

        static std::vector<Vec2d> transform_points(std::vector<Vec2d> points, const Matrix3x3d& transform);
        static std::vector<Vec2d> transform_vectors(std::vector<Vec2d> vectors, const Matrix3x3d& transform);

        static Matrix3x3d invert_affine(const Matrix3x3d& m);
    };
}
