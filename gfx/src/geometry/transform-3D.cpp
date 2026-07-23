#include "gfx/geometry/transform-3D.h"

#include <iostream>

namespace gfx
{
    Matrix4x4d Transform3D::translate(const Vec3d pos)
    {
        return Matrix4x4d { { 1, 0, 0, pos.x }, { 0, 1, 0, pos.y }, { 0, 0, 1, pos.z }, { 0, 0, 0, 1 } };
    }

    Matrix4x4d Transform3D::rotate_x(const double angle)
    {
        double sin_angle { std::sin(angle) };
        double cos_angle { std::cos(angle) };

        return Matrix4x4d {
            { 1, 0, 0, 0 },
            { 0, cos_angle, -sin_angle, 0 },
            { 0, sin_angle, cos_angle, 0 },
            { 0, 0, 0, 1 }
        };
    }

    Matrix4x4d Transform3D::rotate_y(const double angle)
    {
        double sin_angle { std::sin(angle) };
        double cos_angle { std::cos(angle) };

        return Matrix4x4d {
            { cos_angle, 0, sin_angle, 0 },
            { 0, 1, 0, 0 },
            { -sin_angle, 0, cos_angle, 0 },
            { 0, 0, 0, 1 }
        };
    }

    Matrix4x4d Transform3D::rotate_z(const double angle)
    {
        double sin_angle { std::sin(angle) };
        double cos_angle { std::cos(angle) };

        return Matrix4x4d {
            { cos_angle, -sin_angle, 0, 0 },
            { sin_angle, cos_angle, 0, 0 },
            { 0, 0, 1, 0 },
            { 0, 0, 0, 1 }
        };
    }

    Matrix4x4d Transform3D::scale(const Vec3d scale)
    {
        return Matrix4x4d { { scale.x, 0, 0, 0 }, { 0, scale.y, 0, 0 }, { 0, 0, scale.z, 0 }, { 0, 0, 0, 1 } };
    }

    Vec3d Transform3D::extract_translation(const Matrix4x4d& transform)
    {
        return Vec3d { transform(0, 3), transform(1, 3), transform(2, 3) };
    }

    Vec3d Transform3D::extract_scale(const Matrix4x4d& transform)
    {
        const double scale_x {
            std::sqrt(
                transform(0, 0) * transform(0, 0) + transform(1, 0) * transform(1, 0) + transform(2, 0) * transform(
                    2,
                    0
                )
            )
        };
        const double scale_y {
            std::sqrt(
                transform(0, 1) * transform(0, 1) + transform(1, 1) * transform(1, 1) + transform(2, 1) * transform(
                    2,
                    1
                )
            )
        };
        const double scale_z {
            std::sqrt(
                transform(0, 2) * transform(0, 2) + transform(1, 2) * transform(1, 2) + transform(2, 2) * transform(
                    2,
                    2
                )
            )
        };
        return Vec3d { scale_x, scale_y, scale_z };
    }

    Vec3d Transform3D::transform_point(const Vec3d pos, const Matrix4x4d& transform)
    {
        const Matrix4x1d column_matrix { { pos.x }, { pos.y }, { pos.z }, { 1 } };

        Matrix4x1d transformed { transform * column_matrix };
        return Vec3d { transformed(0, 0), transformed(1, 0), transformed(2, 0) };
    }

    Vec3d Transform3D::transform_vector(const Vec3d vec, const Matrix4x4d& transform)
    {
        const Matrix4x1d column_matrix { { vec.x }, { vec.y }, { vec.z }, { 0 } };

        Matrix4x1d transformed { transform * column_matrix };
        return Vec3d { transformed(0, 0), transformed(1, 0), transformed(2, 0) };
    }

    std::vector<Vec3d> Transform3D::transform_points(const std::vector<Vec3d> points, const Matrix4x4d& transform)
    {
        std::vector<Vec3d> transformed_points;
        for (const auto point : points)
        {
            transformed_points.push_back(transform_point(point, transform));
        }
        return transformed_points;
    }

    std::vector<Vec3d> Transform3D::transform_vectors(const std::vector<Vec3d> vectors, const Matrix4x4d& transform)
    {
        std::vector<Vec3d> transformed_vectors;
        for (const auto vec : vectors)
        {
            transformed_vectors.push_back(transform_vector(vec, transform));
        }
        return transformed_vectors;
    }

    Matrix4x4d Transform3D::invert_affine(const Matrix4x4d& m)
    {
        const double a { m(0, 0) };
        const double b { m(0, 1) };
        const double c { m(0, 2) };
        const double tx { m(0, 3) };

        const double d { m(1, 0) };
        const double e { m(1, 1) };
        const double f { m(1, 2) };
        const double ty { m(1, 3) };

        const double g { m(2, 0) };
        const double h { m(2, 1) };
        const double i { m(2, 2) };
        const double tz { m(2, 3) };

        const double det { a * (e * i - f * h) - b * (d * i - f * g) + c * (d * h - e * g) };

        if (det == 0)
        {
            std::cerr << "Matrix is not invertible";
        }

        const double inv_det { 1.0 / det };

        Matrix4x4d inv;

        inv(0, 0) = (e * i - f * h) * inv_det;
        inv(0, 1) = (c * h - b * i) * inv_det;
        inv(0, 2) = (b * f - c * e) * inv_det;
        inv(0, 3) = -(inv(0, 0) * tx + inv(0, 1) * ty + inv(0, 2) * tz);

        inv(1, 0) = (f * g - d * i) * inv_det;
        inv(1, 1) = (a * i - c * g) * inv_det;
        inv(1, 2) = (c * d - a * f) * inv_det;
        inv(1, 3) = -(inv(1, 0) * tx + inv(1, 1) * ty + inv(1, 2) * tz);

        inv(2, 0) = (d * h - e * g) * inv_det;
        inv(2, 1) = (b * g - a * h) * inv_det;
        inv(2, 2) = (a * e - b * d) * inv_det;
        inv(2, 3) = -(inv(2, 0) * tx + inv(2, 1) * ty + inv(2, 2) * tz);

        inv(3, 0) = 0;
        inv(3, 1) = 0;
        inv(3, 2) = 0;
        inv(3, 3) = 1;

        return inv;
    }
}
