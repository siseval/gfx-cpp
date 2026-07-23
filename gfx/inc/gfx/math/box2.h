#pragma once

#include "gfx/math/vec2.h"

namespace gfx
{
    template <typename T>
    struct Box2
    {
        Vec2<T> min;
        Vec2<T> max;

        static Box2 zero()
        {
            return Box2 { Vec2<T>::zero(), Vec2<T>::zero() };
        }

        Vec2<T> size() const
        {
            return max - min;
        }

        Vec2<T> center() const
        {
            return (min + max) / 2;
        }

        bool contains(const Vec2<T>& point) const
        {
            return point.x >= min.x && point.x <= max.x && point.y >= min.y && point.y <= max.y;
        }

        bool contains(const Box2& box) const
        {
            return box.min.x >= min.x && box.min.x <= max.x && box.min.y >= min.y && box.min.y <= max.y;
        }

        bool intersects(const Box2& other) const
        {
            return !(other.min.x > max.x || other.max.x < min.x || other.min.y > max.y || other.max.y < min.y);
        }

        void expand(const Vec2<T>& point)
        {
            if (point.x < min.x)
            {
                min.x = point.x;
            }
            if (point.x > max.x)
            {
                max.x = point.x;
            }
            if (point.y < min.y)
            {
                min.y = point.y;
            }
            if (point.y > max.y)
            {
                max.y = point.y;
            }
        }

        void expand(const std::vector<Vec2<T>>& points)
        {
            for (const auto& point : points)
            {
                expand(point);
            }
        }

        void expand(const Box2& box)
        {
            expand(box.min);
            expand(box.max);
        }

        std::vector<Vec2<T>> get_corners() const
        {
            return {
                Vec2<T> { min.x, min.y },
                Vec2<T> { max.x, min.y },
                Vec2<T> { max.x, max.y },
                Vec2<T> { min.x, max.y }
            };
        }

        static constexpr Box2 infinite()
        {
            return Box2 {
                Vec2<T> { std::numeric_limits<T>::lowest(), std::numeric_limits<T>::lowest() },
                Vec2<T> { std::numeric_limits<T>::max(), std::numeric_limits<T>::max() }
            };
        }

        static constexpr Box2 unexpanded()
        {
            return Box2 {
                Vec2<T> { std::numeric_limits<T>::max(), std::numeric_limits<T>::max() },
                Vec2<T> { std::numeric_limits<T>::lowest(), std::numeric_limits<T>::lowest() }
            };
        }
    };

    using Box2d = Box2<double>;
    using Box2f = Box2<float>;
    using Box2i = Box2<int>;
    using Box2l = Box2<long>;
}
