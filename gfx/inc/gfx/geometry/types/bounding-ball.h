#pragma once

namespace gfx
{
    template <typename VectorType>
    struct BoundingBall
    {
        BoundingBall();
        BoundingBall(VectorType center, double radius);

        BoundingBall transformed(VectorType translation, VectorType scale, VectorType anchor_offset) const;

        VectorType center { VectorType::zero() };
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
        const VectorType translation,
        const VectorType scale,
        const VectorType anchor_offset
    ) const
    {
        const VectorType transformed_center { (center - anchor_offset) * scale + translation };
        const double max_scale { scale.abs().max_element() };

        return BoundingBall { transformed_center, radius * max_scale };
    }
}
