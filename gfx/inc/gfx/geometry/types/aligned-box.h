#pragma once

#include "gfx/math/vec2.h"
#include "gfx/math/vec3.h"

namespace gfx
{
    template <typename VectorType>
    class AlignedBox
    {
        static constexpr size_t D { VectorType::DIMENSION };

    public:

        VectorType min;
        VectorType max;

        static AlignedBox zero();

        VectorType size() const;
        VectorType center() const;

        bool contains(const VectorType& point) const;
        bool intersects(const AlignedBox& other) const;

        void expand(const VectorType& point);
        void expand(const std::vector<VectorType>& points);
        void expand(const AlignedBox& box);
        
        VectorType get_sample_coords(const VectorType& point) const;
       
        static constexpr AlignedBox infinite();
        static constexpr AlignedBox unexpanded();
     
    };

    template <typename VectorType>
    AlignedBox<VectorType> AlignedBox<VectorType>::zero()
    {
        return AlignedBox { VectorType::zero(), VectorType::zero() };
    }

    template <typename VectorType>
    VectorType AlignedBox<VectorType>::size() const
    {
        return max - min;
    }

    template <typename VectorType>
    VectorType AlignedBox<VectorType>::center() const
    {
        return (min + max) / 2;
    }

    template <typename VectorType>
    bool AlignedBox<VectorType>::contains(const VectorType& point) const
    {
        if constexpr (D == 3)
        {
            return point.x >= min.x && point.x <= max.x &&
                   point.y >= min.y && point.y <= max.y &&
                   point.z >= min.z && point.z <= max.z;
        }
        else
        {
            return point.x >= min.x && point.x <= max.x &&
                   point.y >= min.y && point.y <= max.y;
        }
    }

    template <typename VectorType>
    bool AlignedBox<VectorType>::intersects(const AlignedBox& other) const
    {
        if constexpr (D == 3)
        {
            return !(other.min.x > max.x || other.max.x < min.x ||
                     other.min.y > max.y || other.max.y < min.y ||
                     other.min.z > max.z || other.max.z < min.z);
        }
        else
        {
            return !(other.min.x > max.x || other.max.x < min.x ||
                     other.min.y > max.y || other.max.y < min.y);
        }
    }

    template <typename VectorType>
    void AlignedBox<VectorType>::expand(const VectorType& point)
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
        if constexpr (D == 3)
        {
            if (point.z < min.z)
            {
                min.z = point.z;
            }
            if (point.z > max.z)
            {
                max.z = point.z;
            }
        }
    }

    template <typename VectorType>
    void AlignedBox<VectorType>::expand(const std::vector<VectorType>& points)
    {
        for (const auto& point : points)
        {
            expand(point);
        }
    }

    template <typename VectorType>
    void AlignedBox<VectorType>::expand(const AlignedBox& box)
    {
        expand(box.min);
        expand(box.max);
    }
    
    template <typename VectorType>
    VectorType AlignedBox<VectorType>::get_sample_coords(const VectorType& point) const
    {
        if (point.is_all_smaller_than(min))
        {
            return VectorType::zero();
        }
        if (point.is_all_larger_than(max))
        {
            return size();
        }
        
        return (point - min) / size();
    }

    template <typename VectorType>
    constexpr AlignedBox<VectorType> AlignedBox<VectorType>::infinite()
    {
        return AlignedBox {
            VectorType::min(),
            VectorType::max()
        };
    }

    template <typename VectorType>
    constexpr AlignedBox<VectorType> AlignedBox<VectorType>::unexpanded()
    {
        return AlignedBox {
            VectorType::max(),
            VectorType::min()
        };
    }

    using Box2d = AlignedBox<Vec2d>;
    using Box2i = AlignedBox<Vec2i>;
    using Box2l = AlignedBox<Vec2l>;

    using Box3d = AlignedBox<Vec3d>;
    using Box3i = AlignedBox<Vec3i>;
    using Box3l = AlignedBox<Vec3l>;
}
