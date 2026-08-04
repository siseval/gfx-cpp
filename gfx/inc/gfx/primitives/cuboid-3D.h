#pragma once

#include "gfx/core/primitive.h"

namespace gfx
{
    class Cuboid3D final : public Primitive<Vec3d>
    {
    public:

        Cuboid3D() = default;

        void set_size(const Vec3d& new_size);
        void set_size(double width, double height, double depth);
        Vec3d get_size() const;
        
    protected:
        
        void generate_mesh() const override;

    private:

        Vec3d _size;
    };
}
