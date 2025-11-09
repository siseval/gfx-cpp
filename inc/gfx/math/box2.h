#ifndef BOX2D_H
#define BOX2D_H

#include <gfx/math/vec2.h>

namespace gfx::math
{

template <typename T>
class Box2
{

public:

    Vec2<T> min;
    Vec2<T> max;

    static inline Box2<T> zero()
    {
        return Box2<T> { Vec2<T>::zero(), Vec2<T>::zero() };
    }

    inline Vec2<T> size() const
    {
        return max - min;
    }

    inline Vec2<T> center() const
    {
        return (min + max) / 2;
    }

    inline bool contains(const Vec2<T>& point) const
    {
        return point.x >= min.x && point.x <= max.x &&
               point.y >= min.y && point.y <= max.y;
    }

    inline bool intersects(const Box2<T>& other) const
    {
        return !(other.min.x > max.x || other.max.x < min.x ||
                 other.min.y > max.y || other.max.y < min.y);
    }

    inline void expand(const Vec2<T>& point)
    {
        if (point.x < min.x) min.x = point.x;
        if (point.x > max.x) max.x = point.x;
        if (point.y < min.y) min.y = point.y;
        if (point.y > max.y) max.y = point.y;
    }

    inline void expand(const std::vector<Vec2<T>>& points)
    {
        for (const auto& point : points)
        {
            expand(point);
        }
    }

    inline void expand(const Box2<T>& box)
    {
        expand(box.min);
        expand(box.max);
    }

    inline std::vector<gfx::math::Vec2<T>> get_corners() const
    {
        return {
            Vec2<T> { min.x, min.y },
            Vec2<T> { max.x, min.y },
            Vec2<T> { max.x, max.y },
            Vec2<T> { min.x, max.y }
        };
    }

};

using Box2d = Box2<double>;
using Box2i = Box2<int32_t>;

}

#endif // BOX2D_H


