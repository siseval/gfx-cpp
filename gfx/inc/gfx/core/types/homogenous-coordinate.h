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

        HomogenousCoordinate() = default;
        HomogenousCoordinate(VectorType pos, double w);
        HomogenousCoordinate(VectorType vector, double homogenous, const MatrixType& matrix);
        
        void transform(const MatrixType& matrix); 
    };

    template <typename VectorType>
    HomogenousCoordinate<VectorType>::HomogenousCoordinate(const VectorType pos, const double w)
        : pos(pos)
        , w(w) {}

    template <typename VectorType>
    HomogenousCoordinate<VectorType>::HomogenousCoordinate(
        const VectorType vector,
        const double homogenous,
        const MatrixType& matrix
    )
    {
        if constexpr (std::same_as<VectorType, Vec3d>)
        {
            const double x = vector.x;
            const double y = vector.y;
            const double z = vector.z;

            pos.x = matrix(0, 0) * x + matrix(0, 1) * y + matrix(0, 2) * z + matrix(0, 3) * homogenous;
            pos.y = matrix(1, 0) * x + matrix(1, 1) * y + matrix(1, 2) * z + matrix(1, 3) * homogenous;
            pos.z = matrix(2, 0) * x + matrix(2, 1) * y + matrix(2, 2) * z + matrix(2, 3) * homogenous;
            w = matrix(3, 0) * x + matrix(3, 1) * y + matrix(3, 2) * z + matrix(3, 3) * homogenous;
        }
        else
        {
            const double x = vector.x;
            const double y = vector.y;

            pos.x = matrix(0, 0) * x + matrix(0, 1) * y + matrix(0, 2) * homogenous;
            pos.y = matrix(1, 0) * x + matrix(1, 1) * y + matrix(1, 2) * homogenous;
            w = matrix(2, 0) * x + matrix(2, 1) * y + matrix(2, 2) * homogenous;
        }
    }

    template <typename VectorType>
    void HomogenousCoordinate<VectorType>::transform(const MatrixType& matrix)
    {
        if constexpr (std::same_as<VectorType, Vec3d>)
        {
            const double x = pos.x;
            const double y = pos.y;
            const double z = pos.z;
            const double homogenous = w;
            pos.x = matrix(0, 0) * x + matrix(0, 1) * y + matrix(0, 2) * z + matrix(0, 3) * homogenous;
            pos.y = matrix(1, 0) * x + matrix(1, 1) * y + matrix(1, 2) * z + matrix(1, 3) * homogenous;
            pos.z = matrix(2, 0) * x + matrix(2, 1) * y + matrix(2, 2) * z + matrix(2, 3) * homogenous;
            w = matrix(3, 0) * x + matrix(3, 1) * y + matrix(3, 2) * z + matrix(3, 3) * homogenous;
        }
        else
        {
            const double x = pos.x;
            const double y = pos.y;
            const double homogenous = w;
            pos.x = matrix(0, 0) * x + matrix(0, 1) * y + matrix(0, 2) * homogenous;
            pos.y = matrix(1, 0) * x + matrix(1, 1) * y + matrix(1, 2) * homogenous;
            w = matrix(2, 0) * x + matrix(2, 1) * y + matrix(2, 2) * homogenous;
        }
    }
}
