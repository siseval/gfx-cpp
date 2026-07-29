#include "gfx/core/types/triangle-mesh.h"

namespace gfx
{
    TriangleMesh::TriangleMesh()
        : _aabb_dirty(true)
        , _bounding_sphere_dirty(true) {}

    Box3d TriangleMesh::get_aabb() const
    {
        if (!_aabb_dirty)
        {
            return _aabb;
        }

        if (_vertices.empty())
        {
            return Box3d { Vec3d { 0, 0, 0 }, Vec3d { 0, 0, 0 } };
        }

        Box3d extent { _vertices[0], _vertices[0] };

        for (const auto& vertex : _vertices)
        {
            extent.expand(vertex);
        }

        _aabb = extent;
        _aabb_dirty = false;
        return _aabb;
    }

    BoundingSphere TriangleMesh::get_bounding_sphere() const
    {
        if (!_bounding_sphere_dirty)
        {
            return _bounding_sphere;
        }

        const Box3d aabb { get_aabb() };
        const Vec3d center { aabb.min + (aabb.max - aabb.min) * 0.5 };

        double radius { 0 };

        for (const auto& vertex : _vertices)
        {
            const double dist { (vertex - center).length() };
            if (dist > radius)
            {
                radius = dist;
            }
        }

        _bounding_sphere = BoundingSphere(center, radius);
        _bounding_sphere_dirty = false;
        return _bounding_sphere;
    }

    void TriangleMesh::set_vertices(const std::vector<Vec3d>& verts)
    {
        _vertices = verts;
        _aabb_dirty = true;
        _bounding_sphere_dirty = true;
    }

    void TriangleMesh::set_normals(const std::vector<Vec3d>& norms)
    {
        _normals = norms;
    }

    void TriangleMesh::set_uvs(const std::vector<Vec2d>& uvs)
    {
        _uv_coords = uvs;
    }

    void TriangleMesh::set_colors(const std::vector<Color4>& cols)
    {
        _colors = cols;
    }

    void TriangleMesh::set_faces(const std::vector<Face>& fcs)
    {
        _faces = fcs;
    }

    const std::vector<Vec3d>& TriangleMesh::get_vertices() const
    {
        return _vertices;
    }

    const std::vector<Vec3d>& TriangleMesh::get_normals() const
    {
        return _normals;
    }

    const std::vector<Vec2d>& TriangleMesh::get_uvs() const
    {
        return _uv_coords;
    }

    const std::vector<Color4>& TriangleMesh::get_colors() const
    {
        return _colors;
    }

    const std::vector<TriangleMesh::Face>& TriangleMesh::get_faces() const
    {
        return _faces;
    }

    size_t TriangleMesh::num_vertices() const
    {
        return _vertices.size();
    }

    void TriangleMesh::clear()
    {
        _vertices.clear();
        _normals.clear();
        _uv_coords.clear();
        _colors.clear();
        _faces.clear();
    }
}
