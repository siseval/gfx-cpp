#include "gfx/core/types/bounding-sphere.h"

namespace gfx
{
    BoundingSphere::BoundingSphere()
        : center { 0, 0, 0 }
        , radius(0) {}

    BoundingSphere::BoundingSphere(const Vec3d center, const double radius)
        : center(center)
        , radius(radius) {}

    BoundingSphere BoundingSphere::transformed(const Vec3d& translation, const Vec3d& scale) const
    {
        return BoundingSphere { center + translation, radius * std::max({ scale.x, scale.y, scale.z }) };
    }
}
