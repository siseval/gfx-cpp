#pragma once

#include <algorithm>
#include <array>

#include "gfx/math/vec2.h"
#include "gfx/math/vec3.h"

namespace gfx
{
    template <typename VectorType>
    class OrientedBox
    {
    public:

        static constexpr size_t D { VectorType::DIMENSION };

        OrientedBox() = default;

        OrientedBox(Vec2d origin, Vec2d side_x, Vec2d side_y) requires (D == 2);
        OrientedBox(Vec3d origin, Vec3d side_x, Vec3d side_y, Vec3d side_z) requires (D == 3);

        bool contains(VectorType point) const;
        bool intersects(VectorType center, double radius) const;

        VectorType get_center() const;
        VectorType get_extent() const;
        VectorType get_sample_coords(VectorType point) const;

        VectorType get_side_x() const;
        VectorType get_side_y() const;
        VectorType get_side_z() const requires (D == 3);

    private:

        VectorType origin;
        std::array<VectorType, D> sides;
    };

    template <typename VectorType>
    OrientedBox<VectorType>::OrientedBox(Vec2d origin, Vec2d side_x, Vec2d side_y) requires (D == 2)
        : origin(origin)
        , sides({ side_x, side_y }) {}

    template <typename VectorType>
    OrientedBox<VectorType>::OrientedBox(Vec3d origin, Vec3d side_x, Vec3d side_y, Vec3d side_z) requires (D == 3)
        : origin(origin)
        , sides({ side_x, side_y, side_z }) {}

    template <typename VectorType>
    bool OrientedBox<VectorType>::contains(const VectorType point) const
    {
        const VectorType d { point - origin };

        for (size_t i = 0; i < D; ++i)
        {
            const double projection = VectorType::dot(d, sides[i]) / VectorType::dot(sides[i], sides[i]);

            if (projection < 0.0 || projection > 1.0)
            {
                return false;
            }
        }

        return true;
    }

    template <typename VectorType>
    bool OrientedBox<VectorType>::intersects(const VectorType center, const double radius) const
    {
        const VectorType d { center - origin };
        VectorType closest_point { origin };

        for (size_t i = 0; i < D; ++i)
        {
            double projection { VectorType::dot(d, sides[i]) / VectorType::dot(sides[i], sides[i]) };

            projection = std::clamp(projection, 0.0, 1.0);

            closest_point = closest_point + sides[i] * projection;
        }

        const VectorType diff { center - closest_point };

        const double squared_distance { VectorType::dot(diff, diff) };
        const double squared_radius { radius * radius };

        return squared_distance <= squared_radius;
    }

    template <typename VectorType>
    VectorType OrientedBox<VectorType>::get_center() const
    {
        VectorType center { origin };
        for (size_t i = 0; i < D; ++i)
        {
            center = center + sides[i] * 0.5;
        }
        return center;
    }

    template <typename VectorType>
    VectorType OrientedBox<VectorType>::get_extent() const
    {
        if constexpr (D == 2)
        {
            return VectorType { sides[0].length(), sides[1].length() };
        }

        return VectorType { sides[0].length(), sides[1].length(), sides[2].length() };
    }

    template <typename VectorType>
    VectorType OrientedBox<VectorType>::get_sample_coords(const VectorType point) const
    {
        const VectorType d { point - origin };

        if constexpr (D == 2)
        {
            const double u { VectorType::dot(d, sides[0]) / VectorType::dot(sides[0], sides[0]) };
            const double v { VectorType::dot(d, sides[1]) / VectorType::dot(sides[1], sides[1]) };

            return VectorType { u, v };
        }

        const double u { VectorType::dot(d, sides[0]) / VectorType::dot(sides[0], sides[0]) };
        const double v { VectorType::dot(d, sides[1]) / VectorType::dot(sides[1], sides[1]) };
        const double w { VectorType::dot(d, sides[2]) / VectorType::dot(sides[2], sides[2]) };

        return VectorType { u, v, w };
    }

    template <typename VectorType>
    VectorType OrientedBox<VectorType>::get_side_x() const
    {
        return sides[0];
    }

    template <typename VectorType>
    VectorType OrientedBox<VectorType>::get_side_y() const
    {
        return sides[1];
    }

    template <typename VectorType>
    VectorType OrientedBox<VectorType>::get_side_z() const requires (D == 3)
    {
        return sides[2];
    }
}
