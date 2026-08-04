#pragma once

#include "gfx/core/primitive.h"
#include "gfx/math/vec2.h"

namespace gfx
{
    class Plane3D final : public Primitive<Vec3d>
    {
    public:

        Plane3D() = default;

        void set_size(Vec2d new_size);
        void set_size(double width, double height);
        Vec2d get_size() const;
        
    protected:
        
        void generate_mesh() const override;

    private:

        Vec2d _size;
    };
}
