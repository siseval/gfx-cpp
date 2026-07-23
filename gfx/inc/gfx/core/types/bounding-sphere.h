#pragma once

#include "gfx/math/vec3.h"

namespace gfx
{
    struct BoundingSphere
    {
        BoundingSphere();
        BoundingSphere(Vec3d center, double radius);

        BoundingSphere transformed(const Vec3d& translation, const Vec3d& scale) const;

        Vec3d center;
        double radius;
    };
}
