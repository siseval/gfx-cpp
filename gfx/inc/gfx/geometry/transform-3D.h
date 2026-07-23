#pragma once

#include "gfx/math/matrix.h"
#include "gfx/math/vec3.h"

namespace gfx
{
    class Transform3D
    {
    public:

        static Matrix4x4d translate(Vec3d pos);
        static Matrix4x4d rotate_x(double angle);
        static Matrix4x4d rotate_y(double angle);
        static Matrix4x4d rotate_z(double angle);
        static Matrix4x4d scale(Vec3d scale);

        static Vec3d extract_translation(const Matrix4x4d& transform);
        static Vec3d extract_scale(const Matrix4x4d& transform);

        static Vec3d transform_point(Vec3d pos, const Matrix4x4d& transform);
        static Vec3d transform_vector(Vec3d vec, const Matrix4x4d& transform);

        static std::vector<Vec3d> transform_points(std::vector<Vec3d> points, const Matrix4x4d& transform);
        static std::vector<Vec3d> transform_vectors(std::vector<Vec3d> vectors, const Matrix4x4d& transform);

        static Matrix4x4d invert_affine(const Matrix4x4d& m);
    };
}
