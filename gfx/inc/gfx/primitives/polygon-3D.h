#pragma once

#include "gfx/core/primitive-3D.h"

namespace gfx
{
    class Polygon3D final : public Primitive3D
    {
    public:

        const PolygonMesh& get_mesh() const override;

        void set_mesh(const PolygonMesh& mesh) const;
    };
}
