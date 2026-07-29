#include "gfx/primitives/plane-3D.h"

namespace gfx
{
    const TriangleMesh& Plane3D::get_mesh() const
    {
        if (!is_mesh_dirty())
        {
            return _mesh_data;
        }

        _mesh_data.clear();

        const double w { _size.x * 0.5 };
        const double d { _size.y * 0.5 };

        const std::vector<Vec3d> vertices = { { -w, 0, d }, { w, 0, d }, { w, 0, -d }, { -w, 0, -d } };

        const std::vector<Vec3d> normals = { { 0, 1, 0 }, { 0, 1, 0 }, { 0, 1, 0 }, { 0, 1, 0 } };

        const std::vector<Vec2d> uvs = { { 0.0, 1.0 }, { 1.0, 1.0 }, { 1.0, 0.0 }, { 0.0, 0.0 } };

        const std::vector<TriangleMesh::Face> faces = { { 0, 1, 2 }, { 0, 2, 3 }, { 0, 2, 1 }, { 0, 3, 2 } };

        _mesh_data.set_vertices(std::move(vertices));
        _mesh_data.set_normals(std::move(normals));
        _mesh_data.set_uvs(std::move(uvs));
        _mesh_data.set_faces(std::move(faces));
        _mesh_data.set_colors(std::vector(4, get_color()));

        set_mesh_dirty(false);

        return _mesh_data;
    }


    void Plane3D::set_size(const Vec2d new_size)
    {
        _size = new_size;
    }

    void Plane3D::set_size(const double width, const double height)
    {
        _size = Vec2d { width, height };
    }

    Vec2d Plane3D::get_size() const
    {
        return _size;
    }
}
