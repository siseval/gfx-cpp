#pragma once

namespace gfx
{
    template <typename VectorType>
    struct BoundingBall
    {
        BoundingBall();
        BoundingBall(VectorType center, double radius);

        BoundingBall transformed(const VectorType& translation, const VectorType& scale) const;

        VectorType center { VectorType::zero };
        double radius { 0.0 };
    };

    template <typename VectorType>
    BoundingBall<VectorType>::BoundingBall() {}

    template <typename VectorType>
    BoundingBall<VectorType>::BoundingBall(const VectorType center, const double radius)
        : center(center)
        , radius(radius) {}

    template <typename VectorType>
    BoundingBall<VectorType> BoundingBall<VectorType>::transformed(
        const VectorType& translation,
        const VectorType& scale
    ) const
    {
        return BoundingBall { center + translation, radius * scale.max_element() };
    }
}
