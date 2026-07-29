#include "gfx/primitives/sphere-3D.h"

namespace gfx
{
    const TriangleMesh& Sphere3D::get_mesh() const
    {
        if (!is_mesh_dirty())
        {
            return _mesh_data;
        }

        _mesh_data.clear();

        std::vector<Vec3d> vertices;
        std::vector<Vec3d> normals;
        std::vector<Vec2d> uvs;
        std::vector<TriangleMesh::Face> faces;

        for (int r = 0; r < _lat_segments; ++r)
        {
            for (int s = 0; s < _lon_segments; ++s)
            {
                const double phi0 = std::numbers::pi * r / _lat_segments;
                const double phi1 = std::numbers::pi * (r + 1) / _lat_segments;
                const double theta0 = 2.0 * std::numbers::pi * s / _lon_segments;
                const double theta1 = 2.0 * std::numbers::pi * (s + 1) / _lon_segments;

                auto get_pos = [&](const double phi, const double theta) {
                    const double x = _radius * std::sin(phi) * std::cos(theta);
                    const double y = _radius * std::cos(phi);
                    const double z = _radius * std::sin(phi) * std::sin(theta);

                    Vec3d pos { x, y, z };
                    Vec3d normal = pos.normalize();
                    return std::make_pair(pos, normal);
                };

                auto get_uv = [&](const double phi, const double theta) {
                    const double u = theta / (2.0 * std::numbers::pi);
                    const double v = phi / std::numbers::pi;
                    return Vec2d { u, v };
                };

                auto v0 = get_pos(phi0, theta0);
                auto v1 = get_pos(phi0, theta1);
                auto v2 = get_pos(phi1, theta1);
                auto v3 = get_pos(phi1, theta0);

                auto uv0 = get_uv(phi0, theta0);
                auto uv1 = get_uv(phi0, theta1);
                auto uv2 = get_uv(phi1, theta1);
                auto uv3 = get_uv(phi1, theta0);

                auto emit = [&](
                    const auto& a,
                    const auto& b,
                    const auto& c,
                    const auto& uva,
                    const auto& uvb,
                    const auto& uvc
                ) {
                    const size_t base = vertices.size();

                    vertices.push_back(a.first);
                    vertices.push_back(b.first);
                    vertices.push_back(c.first);

                    normals.push_back(a.second);
                    normals.push_back(b.second);
                    normals.push_back(c.second);

                    uvs.push_back(uva);
                    uvs.push_back(uvb);
                    uvs.push_back(uvc);

                    faces.push_back(TriangleMesh::Face { base + 0, base + 1, base + 2 });
                };

                if (r == 0)
                {
                    emit(v0, v2, v3, uv0, uv2, uv3);
                }
                else if (r == _lat_segments - 1)
                {
                    emit(v0, v1, v2, uv0, uv1, uv2);
                }
                else
                {
                    emit(v0, v1, v2, uv0, uv1, uv2);
                    emit(v0, v2, v3, uv0, uv2, uv3);
                }
            }
        }

        _mesh_data.set_vertices(std::move(vertices));
        _mesh_data.set_normals(std::move(normals));
        _mesh_data.set_uvs(std::move(uvs));
        _mesh_data.set_faces(std::move(faces));
        _mesh_data.set_colors(std::vector(_mesh_data.num_vertices(), get_color()));

        set_mesh_dirty(false);

        return _mesh_data;
    }


    void Sphere3D::set_radius(const double r)
    {
        _radius = r;
        set_mesh_dirty(true);
    }

    double Sphere3D::get_radius() const
    {
        return _radius;
    }

    void Sphere3D::set_num_lat_segments(const int segments)
    {
        _lat_segments = segments;
        set_mesh_dirty(true);
    }

    void Sphere3D::set_num_lon_segments(const int segments)
    {
        _lon_segments = segments;
        set_mesh_dirty(true);
    }

    void Sphere3D::set_num_segments(const int lat, const int lon)
    {
        _lat_segments = lat;
        _lon_segments = lon;
        set_mesh_dirty(true);
    }

    void Sphere3D::set_num_segments(const int segments)
    {
        _lat_segments = segments;
        _lon_segments = segments;
        set_mesh_dirty(true);
    }
}
