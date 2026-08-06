#pragma once

#include <type_traits>

#include "../../geometry/types/bounding-ball.h"
#include "gfx/geometry/types/frustum.h"
#include "gfx/geometry/types/oriented-box.h"
#include "gfx/math/vec2.h"

namespace gfx
{
    template <typename VectorType>
    class ViewBounds
    {
        using BoundsType = std::conditional_t<std::same_as<VectorType, Vec2d>, OrientedBox<Vec2d>, Frustum>;

    public:

        ViewBounds() = default;
        explicit ViewBounds(const BoundsType& bounds);

        bool point_in_view(VectorType point) const;
        bool ball_in_view(const BoundingBall<VectorType>& ball) const;

        BoundsType get_bounds() const;

    private:

        BoundsType _bounds;
    };

    template <typename VectorType>
    ViewBounds<VectorType>::ViewBounds(const BoundsType& bounds)
        : _bounds(bounds) {}

    template <typename VectorType>
    bool ViewBounds<VectorType>::point_in_view(const VectorType point) const
    {
        return _bounds.contains(point);
    }

    template <typename VectorType>
    bool ViewBounds<VectorType>::ball_in_view(const BoundingBall<VectorType>& ball) const
    {
        return _bounds.intersects(ball.center, ball.radius);
    }

    template <typename VectorType>
    ViewBounds<VectorType>::BoundsType ViewBounds<VectorType>::get_bounds() const
    {
        return _bounds;
    }
}
