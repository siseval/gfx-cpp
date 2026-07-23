#pragma once

#include "gfx/math/vec3.h"

namespace gfx
{
    template <typename T>
    class Box3
    {
    public:

        Vec3<T> min;
        Vec3<T> max;

        static Box3 zero()
        {
            return Box3 { Vec3<T>::zero(), Vec3<T>::zero() };
        }

        Vec3<T> size() const
        {
            return max - min;
        }

        Vec3<T> center() const
        {
            return (min + max) / 2;
        }

        bool contains(const Vec3<T>& point) const
        {
            return point.x >= min.x && point.x <= max.x && point.y >= min.y && point.y <= max.y && point.z >= min.z &&
                point.z <= max.z;
        }

        bool intersects(const Box3& other) const
        {
            return !(other.min.x > max.x || other.max.x < min.x || other.min.y > max.y || other.max.y < min.y || other.
                min.z > max.z || other.max.z < min.z);
        }

        void expand(const Vec3<T>& point)
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
            if (point.z < min.z)
            {
                min.z = point.z;
            }
            if (point.z > max.z)
            {
                max.z = point.z;
            }
        }

        void expand(const std::vector<Vec3<T>>& points)
        {
            for (const auto& point : points)
            {
                expand(point);
            }
        }

        void expand(const Box3& box)
        {
            expand(box.min);
            expand(box.max);
        }

        std::vector<Vec3<T>> get_corners() const
        {
            return {
                Vec3<T> { min.x, min.y, min.z },
                Vec3<T> { max.x, min.y, min.z },
                Vec3<T> { min.x, max.y, min.z },
                Vec3<T> { max.x, max.y, min.z },
                Vec3<T> { min.x, min.y, max.z },
                Vec3<T> { max.x, min.y, max.z },
                Vec3<T> { min.x, max.y, max.z },
                Vec3<T> { max.x, max.y, max.z }
            };
        }

        static constexpr Box3 infinite()
        {
            return Box3 {
                Vec3<T> { std::numeric_limits<T>::lowest(), std::numeric_limits<T>::lowest() },
                Vec3<T> { std::numeric_limits<T>::max(), std::numeric_limits<T>::max() }
            };
        }

        static constexpr Box3 unexpanded()
        {
            return Box3 {
                Vec3<T> { std::numeric_limits<T>::max(), std::numeric_limits<T>::max() },
                Vec3<T> { std::numeric_limits<T>::lowest(), std::numeric_limits<T>::lowest() }
            };
        }
    };

    using Box3d = Box3<double>;
    using Box3i = Box3<int>;
    using Box3l = Box3<long>;
}
