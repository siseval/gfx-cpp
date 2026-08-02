#pragma once
#include <type_traits>

#include "gfx/core/types/bounding-ball.h"
#include "gfx/geometry/types/frustum.h"
#include "gfx/math/oriented-box.h"
#include "gfx/math/vec2.h"

namespace gfx
{
    template <typename VectorType>
    class ViewBounds
    {
        using BoundsType = std::conditional_t<std::same_as<VectorType, Vec2d>, OrientedBox<Vec2d>, Frustum>;

    public:

        bool point_in_view(VectorType point) const;
        bool ball_in_view(BoundingBall<VectorType> ball) const;

    private:

        BoundsType bounds;
    };

    template <typename VectorType>
    bool ViewBounds<VectorType>::point_in_view(const VectorType point) const
    {
        if constexpr (std::is_same_v<VectorType, Vec2d>)
        {
            return true;
        }
        else
        {
            return bounds.point_in_frustum(point);
        }
    }

    template <typename VectorType>
    bool ViewBounds<VectorType>::ball_in_view(const BoundingBall<VectorType> ball) const
    {
        if constexpr (std::is_same_v<VectorType, Vec2d>)
        {
            return true;
        }
        else
        {
            return bounds.sphere_in_frustum(ball);
        }
    }
}
