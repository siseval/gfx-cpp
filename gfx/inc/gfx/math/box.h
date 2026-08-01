#pragma once

namespace gfx
{
    template <typename VectorType>
    class Box
    {
    public:

        VectorType min;
        VectorType max;

        static Box zero()
        {
            return Box { VectorType::zero(), VectorType::zero() };
        }

        VectorType size() const
        {
            return max - min;
        }

        VectorType center() const
        {
            return (min + max) / 2;
        }

        bool contains(const VectorType& point) const
        {
            return point.x >= min.x && point.x <= max.x && point.y >= min.y && point.y <= max.y && point.z >= min.z &&
                   point.z <= max.z;
        }

        bool intersects(const Box& other) const
        {
            return !(other.min.x > max.x || other.max.x < min.x || other.min.y > max.y || other.max.y < min.y || other.
                     min.z > max.z || other.max.z < min.z);
        }

        void expand(const VectorType& point)
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

        void expand(const std::vector<VectorType>& points)
        {
            for (const auto& point : points)
            {
                expand(point);
            }
        }

        void expand(const Box& box)
        {
            expand(box.min);
            expand(box.max);
        }

        std::vector<VectorType> get_corners() const
        {
            return {
                VectorType { min.x, min.y, min.z },
                VectorType { max.x, min.y, min.z },
                VectorType { min.x, max.y, min.z },
                VectorType { max.x, max.y, min.z },
                VectorType { min.x, min.y, max.z },
                VectorType { max.x, min.y, max.z },
                VectorType { min.x, max.y, max.z },
                VectorType { max.x, max.y, max.z }
            };
        }

        static constexpr Box infinite()
        {
            return Box {
                VectorType::min(),
                VectorType::max()
            };
        }

        static constexpr Box unexpanded()
        {
            return Box {
                VectorType::max(),
                VectorType::min()
            };
        }
    };

    using Box2d = Box<Vec2d>;
    using Box2i = Box<Vec2i>;
    using Box2l = Box<Vec2l>;
    
    using Box3d = Box<Vec3d>;
    using Box3i = Box<Vec3i>;
    using Box3l = Box<Vec3l>;
}
