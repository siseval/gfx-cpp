#include <gfx/geometry/types/frustum.h>

namespace gfx
{
    Frustum::Plane::Plane(const Vec3d normal, const double d)
        : normal(normal)
        , d(d) {}

    double Frustum::Plane::signed_distance_to_point(const Vec3d& point) const
    {
        return Vec3d::dot(normal, point) + d;
    }

    Frustum::Frustum(
        const Plane& top,
        const Plane& bottom,
        const Plane& left,
        const Plane& right,
        const Plane& near,
        const Plane& far
    )
        : top(top)
        , bottom(bottom)
        , left(left)
        , right(right)
        , near(near)
        , far(far) {}

    bool Frustum::contains(const Vec3d point) const
    {
        return top.signed_distance_to_point(point) >= 0 && bottom.signed_distance_to_point(point) >= 0 && left.
               signed_distance_to_point(point) >= 0 && right.signed_distance_to_point(point) >= 0 && near.
               signed_distance_to_point(point) >= 0 && far.signed_distance_to_point(point) >= 0;
    }

    bool Frustum::intersects(const Vec3d center, const double radius) const
    {
        return top.signed_distance_to_point(center) >= -radius && bottom.signed_distance_to_point(center) >= -radius &&
               left.signed_distance_to_point(center) >= -radius && right.signed_distance_to_point(center) >= -radius &&
               near.signed_distance_to_point(center) >= -radius && far.signed_distance_to_point(center) >= -radius;
    }
}
