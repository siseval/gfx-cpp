#include "gfx/primitives/cuboid-3D.h"

namespace gfx
{
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

    void Cuboid3D::generate_mesh() const
    {
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

        const std::vector<TriangleMesh<Vec3d>::Face> faces {
            { .v1 = 0, .v2 = 1, .v3 = 2 },
            { .v1 = 3, .v2 = 4, .v3 = 5 },
            { .v1 = 6, .v2 = 7, .v3 = 8 },
            { .v1 = 9, .v2 = 10, .v3 = 11 },
            { .v1 = 12, .v2 = 13, .v3 = 14 },
            { .v1 = 15, .v2 = 16, .v3 = 17 },
            { .v1 = 18, .v2 = 19, .v3 = 20 },
            { .v1 = 21, .v2 = 22, .v3 = 23 },
            { .v1 = 24, .v2 = 25, .v3 = 26 },
            { .v1 = 27, .v2 = 28, .v3 = 29 },
            { .v1 = 30, .v2 = 31, .v3 = 32 },
            { .v1 = 33, .v2 = 34, .v3 = 35 }
        };

        _mesh_data.set_vertices(std::move(vertices));
        _mesh_data.set_normals(std::move(normals));
        _mesh_data.set_uvs(std::move(uvs));
        _mesh_data.set_faces(std::move(faces));
        _mesh_data.set_colors(std::vector(_mesh_data.num_vertices(), get_color()));

        set_mesh_dirty(false);
    }
}
