#pragma once

#include "gfx/core/primitive.h"

namespace gfx
{
    class Polygon3D final : public Primitive
    {
    public:

        const TriangleMesh& get_mesh() const override;

        void set_mesh(const TriangleMesh& mesh) const;
    };
}
