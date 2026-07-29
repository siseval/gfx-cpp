#pragma once

#include "gfx/core/primitive-3D.h"

namespace gfx
{
    class Cuboid3D final : public Primitive3D
    {
    public:

        Cuboid3D() = default;

        const TriangleMesh& get_mesh() const override;

        void set_size(const Vec3d& new_size);
        void set_size(double width, double height, double depth);
        Vec3d get_size() const;

    private:

        Vec3d _size;
    };
}
