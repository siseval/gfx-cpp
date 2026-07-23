#include "gfx/primitives/polygon-3D.h"

namespace gfx
{
    const PolygonMesh& Polygon3D::get_mesh() const
    {
        return _mesh_data;
    }

    void Polygon3D::set_mesh(const PolygonMesh& mesh) const
    {
        _mesh_data = mesh;
    }
}
