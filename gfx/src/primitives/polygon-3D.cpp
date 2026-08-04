#include "gfx/primitives/polygon-3D.h"

namespace gfx
{
    void Polygon3D::set_mesh(const TriangleMesh<Vec3d>& mesh) const
    {
        _mesh_data = mesh;
    }

    void Polygon3D::generate_mesh() const {}
}
