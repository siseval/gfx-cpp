#pragma once

#include "gfx/math/vec3.h"

namespace gfx
{
    template <typename VectorType>
    struct OrientedBox
    {
        VectorType origin;
        VectorType side_x;
        VectorType side_y;
        VectorType side_z;

        OrientedBox();
        OrientedBox(VectorType origin, VectorType side_x, VectorType side_y, VectorType side_z);
        OrientedBox(const OrientedBox& other);

        VectorType get_center() const;
        VectorType get_extent() const;
        VectorType get_sample_coords(VectorType point) const;
        std::vector<VectorType> get_corners() const;
    };

    template <typename VectorType>
    OrientedBox<VectorType>::OrientedBox()
        : origin { 0, 0, 0 }
        , side_x { 0, 0, 0 }
        , side_y { 0, 0, 0 }
        , side_z { 0, 0, 0 } {}

    template <typename VectorType>
    OrientedBox<VectorType>::OrientedBox(
        const VectorType origin,
        const VectorType side_x,
        const VectorType side_y,
        const VectorType side_z
    )
        : origin(origin)
        , side_x(side_x)
        , side_y(side_y)
        , side_z(side_z) {}

    template <typename VectorType>
    OrientedBox<VectorType>::OrientedBox(const OrientedBox& other)
        : origin(other.origin)
        , side_x(other.side_x)
        , side_y(other.side_y)
        , side_z(other.side_z) {}

    template <typename VectorType>
    VectorType OrientedBox<VectorType>::get_center() const
    {
        return origin + side_x * 0.5 + side_y * 0.5 + side_z * 0.5;
    }

    template <typename VectorType>
    VectorType OrientedBox<VectorType>::get_extent() const
    {
        return VectorType { side_x.length(), side_y.length(), side_z.length() };
    }

    template <typename VectorType>
    VectorType OrientedBox<VectorType>::get_sample_coords(const VectorType point) const
    {
        const VectorType d { point - origin };

        const double u { VectorType::dot(d, side_x) / VectorType::dot(side_x, side_x) };
        const double v { VectorType::dot(d, side_y) / VectorType::dot(side_y, side_y) };
        const double w { VectorType::dot(d, side_z) / VectorType::dot(side_z, side_z) };

        return VectorType { u, v, w };
    }

    template <typename VectorType>
    std::vector<VectorType> OrientedBox<VectorType>::get_corners() const
    {
        return {
            origin,
            origin + side_x,
            origin + side_y,
            origin + side_z,
            origin + side_x + side_y,
            origin + side_x + side_z,
            origin + side_y + side_z,
            origin + side_x + side_y + side_z
        };
    }
}
