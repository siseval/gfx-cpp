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
            { .v0 = 0, .v1 = 1, .v2 = 2 },
            { .v0 = 3, .v1 = 4, .v2 = 5 },
            { .v0 = 6, .v1 = 7, .v2 = 8 },
            { .v0 = 9, .v1 = 10, .v2 = 11 },
            { .v0 = 12, .v1 = 13, .v2 = 14 },
            { .v0 = 15, .v1 = 16, .v2 = 17 },
            { .v0 = 18, .v1 = 19, .v2 = 20 },
            { .v0 = 21, .v1 = 22, .v2 = 23 },
            { .v0 = 24, .v1 = 25, .v2 = 26 },
            { .v0 = 27, .v1 = 28, .v2 = 29 },
            { .v0 = 30, .v1 = 31, .v2 = 32 },
            { .v0 = 33, .v1 = 34, .v2 = 35 }
        };

        _mesh_data.set_vertices(std::move(vertices));
        _mesh_data.set_normals(std::move(normals));
        _mesh_data.set_uvs(std::move(uvs));
        _mesh_data.set_faces(std::move(faces));
        _mesh_data.set_colors(std::vector(_mesh_data.num_vertices(), get_color()));

        set_mesh_dirty(false);
    }
}
