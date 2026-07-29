#include "gfx/primitives/cuboid-3D.h"

namespace gfx
{
    const TriangleMesh& Cuboid3D::get_mesh() const
    {
        if (!is_mesh_dirty())
        {
            return _mesh_data;
        }

        _mesh_data.clear();

        const double w { _size.x * 0.5 };
        const double h { _size.y * 0.5 };
        const double d { _size.z * 0.5 };

        const std::vector<Vec3d> vertices {
            { -w, -h, -d },
            { -w, h, -d },
            { w, h, -d },
            { -w, -h, -d },
            { w, h, -d },
            { w, -h, -d },
            { -w, -h, d },
            { w, -h, d },
            { w, h, d },
            { -w, -h, d },
            { w, h, d },
            { -w, h, d },
            { -w, -h, d },
            { -w, h, d },
            { -w, h, -d },
            { -w, -h, d },
            { -w, h, -d },
            { -w, -h, -d },
            { w, -h, -d },
            { w, h, -d },
            { w, h, d },
            { w, -h, -d },
            { w, h, d },
            { w, -h, d },
            { -w, h, -d },
            { -w, h, d },
            { w, h, d },
            { -w, h, -d },
            { w, h, d },
            { w, h, -d },
            { -w, -h, d },
            { -w, -h, -d },
            { w, -h, -d },
            { -w, -h, d },
            { w, -h, -d },
            { w, -h, d }
        };

        const std::vector<Vec3d> normals {
            { 0, 0, -1 },
            { 0, 0, -1 },
            { 0, 0, -1 },
            { 0, 0, -1 },
            { 0, 0, -1 },
            { 0, 0, -1 },
            { 0, 0, 1 },
            { 0, 0, 1 },
            { 0, 0, 1 },
            { 0, 0, 1 },
            { 0, 0, 1 },
            { 0, 0, 1 },
            { -1, 0, 0 },
            { -1, 0, 0 },
            { -1, 0, 0 },
            { -1, 0, 0 },
            { -1, 0, 0 },
            { -1, 0, 0 },
            { 1, 0, 0 },
            { 1, 0, 0 },
            { 1, 0, 0 },
            { 1, 0, 0 },
            { 1, 0, 0 },
            { 1, 0, 0 },
            { 0, 1, 0 },
            { 0, 1, 0 },
            { 0, 1, 0 },
            { 0, 1, 0 },
            { 0, 1, 0 },
            { 0, 1, 0 },
            { 0, -1, 0 },
            { 0, -1, 0 },
            { 0, -1, 0 },
            { 0, -1, 0 },
            { 0, -1, 0 },
            { 0, -1, 0 }
        };

        const std::vector<Vec2d> uvs {
            { 0.0, 0.0 },
            { 0.0, 1.0 },
            { 1.0, 1.0 },
            { 0.0, 0.0 },
            { 1.0, 1.0 },
            { 1.0, 0.0 },
            { 0.0, 0.0 },
            { 1.0, 0.0 },
            { 1.0, 1.0 },
            { 0.0, 0.0 },
            { 1.0, 1.0 },
            { 0.0, 1.0 },
            { 0.0, 0.0 },
            { 0.0, 1.0 },
            { 1.0, 1.0 },
            { 0.0, 0.0 },
            { 1.0, 1.0 },
            { 1.0, 0.0 },
            { 0.0, 0.0 },
            { 0.0, 1.0 },
            { 1.0, 1.0 },
            { 0.0, 0.0 },
            { 1.0, 1.0 },
            { 1.0, 0.0 },
            { 0.0, 0.0 },
            { 0.0, 1.0 },
            { 1.0, 1.0 },
            { 0.0, 0.0 },
            { 1.0, 1.0 },
            { 1.0, 0.0 },
            { 0.0, 0.0 },
            { 0.0, 1.0 },
            { 1.0, 1.0 },
            { 0.0, 0.0 },
            { 1.0, 1.0 },
            { 1.0, 0.0 }
        };

        const std::vector<TriangleMesh::Face> faces {
            { 0, 1, 2 },
            { 3, 4, 5 },
            { 6, 7, 8 },
            { 9, 10, 11 },
            { 12, 13, 14 },
            { 15, 16, 17 },
            { 18, 19, 20 },
            { 21, 22, 23 },
            { 24, 25, 26 },
            { 27, 28, 29 },
            { 30, 31, 32 },
            { 33, 34, 35 }
        };

        _mesh_data.set_vertices(std::move(vertices));
        _mesh_data.set_normals(std::move(normals));
        _mesh_data.set_uvs(std::move(uvs));
        _mesh_data.set_faces(std::move(faces));
        _mesh_data.set_colors(std::vector(_mesh_data.num_vertices(), get_color()));

        set_mesh_dirty(false);

        return _mesh_data;
    }


    void Cuboid3D::set_size(const Vec3d& new_size)
    {
        _size = new_size;
        set_mesh_dirty(true);
    }

    void Cuboid3D::set_size(const double width, const double height, const double depth)
    {
        _size = Vec3d { width, height, depth };
        set_mesh_dirty(true);
    }

    Vec3d Cuboid3D::get_size() const
    {
        return _size;
    }
}
