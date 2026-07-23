#include "gfx/core/types/barycentric-triangle.h"

namespace gfx
{
    BarycentricTriangle::BarycentricTriangle(const Vec2d& v0, const Vec2d& v1, const Vec2d& v2)
        : _v0(v0)
        , _v1(v1)
        , _v2(v2)
    {
        precompute();
    }

    BarycentricTriangle::BarycentricTriangle()
        : _v0(Vec2d::zero())
        , _v1(Vec2d::zero())
        , _v2(Vec2d::zero())
    {
        precompute();
    }

    int BarycentricTriangle::corners_inside(const Box2d& box) const
    {
        Vec2d box_corners[4] = {
            { box.min.x, box.min.y },
            { box.max.x, box.min.y },
            { box.max.x, box.max.y },
            { box.min.x, box.max.y }
        };

        int count = 0;

        for (const auto& corner : box_corners)
        {
            if (point_inside(corner))
            {
                count++;
            }
        }

        return count;
    }

    bool BarycentricTriangle::point_inside(const Vec2d& point) const
    {
        const double s = _inv_area_2_x * (_a * point.x + _b * point.y + _c);
        const double t = _inv_area_2_x * (_d * point.x + _e * point.y + _f);

        return s >= 0 && t >= 0 && s + t <= 1;
    }

    double BarycentricTriangle::edge(const Vec2d& v_start, const Vec2d& v_end, const Vec2d& point)
    {
        return (v_end.x - v_start.x) * (point.y - v_start.y) - (v_end.y - v_start.y) * (point.x - v_start.x);
    }

    Vec3d BarycentricTriangle::barycentric_weights(const Vec2d& p) const
    {
        double w1 = _inv_area_2_x * (_a * p.x + _b * p.y + _c);
        double w2 = _inv_area_2_x * (_d * p.x + _e * p.y + _f);
        double w0 = 1.0 - w1 - w2;
        return { w0, w1, w2 };
    }

    Vec3d BarycentricTriangle::lerp_barycentric(
        const Vec3d& a,
        const Vec3d& b,
        const Vec3d& c,
        const Vec2d& point
    ) const
    {
        const Vec3d w = barycentric_weights(point);
        return a * w.x + b * w.y + c * w.z;
    }

    double BarycentricTriangle::get_area() const
    {
        return _area;
    }

    double BarycentricTriangle::get_a() const
    {
        return _a;
    }

    double BarycentricTriangle::get_b() const
    {
        return _b;
    }

    double BarycentricTriangle::get_c() const
    {
        return _c;
    }

    double BarycentricTriangle::get_d() const
    {
        return _d;
    }

    double BarycentricTriangle::get_e() const
    {
        return _e;
    }

    double BarycentricTriangle::get_f() const
    {
        return _f;
    }

    double BarycentricTriangle::get_g() const
    {
        return _g;
    }

    double BarycentricTriangle::get_h() const
    {
        return _h;
    }

    double BarycentricTriangle::get_i() const
    {
        return _i;
    }

    void BarycentricTriangle::precompute()
    {
        const double area_2x = _v0.x * (_v1.y - _v2.y) + _v1.x * (_v2.y - _v0.y) + _v2.x * (_v0.y - _v1.y);
        _area = area_2x / 2.0;

        if (std::abs(area_2x) < 1e-12)
        {
            _inv_area_2_x = 0.0;
        }
        else
        {
            _inv_area_2_x = 1.0 / area_2x;
        }

        _a = _v2.y - _v0.y;
        _b = _v0.x - _v2.x;
        _c = _v0.y * _v2.x - _v0.x * _v2.y;

        _d = _v0.y - _v1.y;
        _e = _v1.x - _v0.x;
        _f = _v0.x * _v1.y - _v0.y * _v1.x;

        _g = _v1.y - _v2.y;
        _h = _v2.x - _v1.x;
        _i = _v1.x * _v2.y - _v1.y * _v2.x;
    }
}
