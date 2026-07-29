#pragma once

#include "gfx/core/primitive-3D.h"

namespace gfx
{
    class Polygon3D final : public Primitive3D
    {
    public:

        const TriangleMesh& get_mesh() const override;

        void set_mesh(const TriangleMesh& mesh) const;
    };
}
