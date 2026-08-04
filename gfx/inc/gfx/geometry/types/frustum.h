#pragma once

#include "gfx/math/vec3.h"

namespace gfx
{
    struct Frustum
    {
        struct Plane
        {
            Vec3d normal;
            double d;

            Plane() = default;
            Plane(Vec3d normal, double d);

            double signed_distance_to_point(const Vec3d& point) const;
        };

        Frustum() = default;
        
        Frustum(
            const Plane& top,
            const Plane& bottom,
            const Plane& left,
            const Plane& right,
            const Plane& near,
            const Plane& far
        );

        Plane top;
        Plane bottom;
        Plane left;
        Plane right;
        Plane near;
        Plane far;

        bool contains(Vec3d point) const;
        bool intersects(Vec3d center, double radius) const;
    };
}
