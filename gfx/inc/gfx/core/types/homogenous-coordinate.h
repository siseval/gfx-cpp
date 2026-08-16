#pragma once
#include "gfx/core/transform.h"

namespace gfx
{
    template <typename VectorType>
    struct HomogenousCoordinate
    {
        using MatrixType = Transform<VectorType>::MatrixType;

        VectorType pos;
        double w;

        HomogenousCoordinate(VectorType pos, double w);
        HomogenousCoordinate(VectorType vector, double homogenous, const MatrixType& transform);
    };

    template <typename VectorType>
    HomogenousCoordinate<VectorType>::HomogenousCoordinate(const VectorType pos, const double w)
        : pos(pos)
        , w(w) {}

    template <typename VectorType>
    HomogenousCoordinate<VectorType>::HomogenousCoordinate(
        const VectorType vector,
        const double homogenous,
        const MatrixType& transform
    )
    {
        if constexpr (std::same_as<VectorType, Vec3d>)
        {
            const double x = vector.x;
            const double y = vector.y;
            const double z = vector.z;

            pos.x = transform(0, 0) * x + transform(0, 1) * y + transform(0, 2) * z + transform(0, 3) * homogenous;
            pos.y = transform(1, 0) * x + transform(1, 1) * y + transform(1, 2) * z + transform(1, 3) * homogenous;
            pos.z = transform(2, 0) * x + transform(2, 1) * y + transform(2, 2) * z + transform(2, 3) * homogenous;
            w = transform(3, 0) * x + transform(3, 1) * y + transform(3, 2) * z + transform(3, 3) * homogenous;
        }
        else
        {
            const double x = vector.x;
            const double y = vector.y;

            pos.x = transform(0, 0) * x + transform(0, 1) * y + transform(0, 2) * homogenous;
            pos.y = transform(1, 0) * x + transform(1, 1) * y + transform(1, 2) * homogenous;
            w = transform(2, 0) * x + transform(2, 1) * y + transform(2, 2) * homogenous;
        }
    }
}
