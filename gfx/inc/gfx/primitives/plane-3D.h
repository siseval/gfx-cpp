#pragma once

#include "gfx/core/primitive.h"
#include "gfx/math/vec2.h"

namespace gfx
{
    class Plane3D final : public Primitive
    {
    public:

        Plane3D() = default;

        const TriangleMesh& get_mesh() const override;

        void set_size(Vec2d new_size);
        void set_size(double width, double height);
        Vec2d get_size() const;

    private:

        Vec2d _size;
    };
}
