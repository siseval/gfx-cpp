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

            Plane();
            Plane(Vec3d normal, double d);

            double signed_distance_to_point(const Vec3d& point) const;
        };

        Frustum();
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

        bool point_in_frustum(const Vec3d& point) const;
        bool sphere_in_frustum(const Vec3d& center, double radius) const;
    };
}
