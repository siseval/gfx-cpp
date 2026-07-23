#include "gfx/core/types/obb-3D.h"

namespace gfx
{
    OBB3D::OBB3D()
        : origin { 0, 0, 0 }
        , side_x { 0, 0, 0 }
        , side_y { 0, 0, 0 }
        , side_z { 0, 0, 0 } {}

    OBB3D::OBB3D(const Vec3d origin, const Vec3d side_x, const Vec3d side_y, const Vec3d side_z)
        : origin(origin)
        , side_x(side_x)
        , side_y(side_y)
        , side_z(side_z) {}

    OBB3D::OBB3D(const OBB3D& other)
        : origin(other.origin)
        , side_x(other.side_x)
        , side_y(other.side_y)
        , side_z(other.side_z) {}

    Vec3d OBB3D::get_center() const
    {
        return origin + side_x * 0.5 + side_y * 0.5 + side_z * 0.5;
    }

    Vec3d OBB3D::get_extent() const
    {
        return Vec3d { side_x.length(), side_y.length(), side_z.length() };
    }

    Vec3d OBB3D::get_uvw(const Vec3d point) const
    {
        const Vec3d d { point - origin };

        const double u { Vec3d::dot(d, side_x) / Vec3d::dot(side_x, side_x) };
        const double v { Vec3d::dot(d, side_y) / Vec3d::dot(side_y, side_y) };
        const double w { Vec3d::dot(d, side_z) / Vec3d::dot(side_z, side_z) };

        return Vec3d { u, v, w };
    }

    std::vector<Vec3d> OBB3D::get_corners() const
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
