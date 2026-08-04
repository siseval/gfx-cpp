#pragma once

#include "gfx/core/primitive.h"

namespace gfx
{
    class Polygon3D final : public Primitive<Vec3d>
    {
    public:

        void set_mesh(const TriangleMesh<Vec3d>& mesh) const;

    protected:

        void generate_mesh() const override;
    };
}
