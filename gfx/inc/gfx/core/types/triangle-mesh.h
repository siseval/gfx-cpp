#pragma once

#include "gfx/core/types/bounding-ball.h"
#include "gfx/core/types/color4.h"
#include "gfx/math/box.h"
#include "gfx/math/vec2.h"
#include "gfx/math/vec3.h"

#include <vector>

namespace gfx
{
    template <typename VectorType>
    class TriangleMesh
    {
    public:

        struct Face
        {
            size_t v1;
            size_t v2;
            size_t v3;
            size_t material_index { 0 };
        };

        Box<VectorType> get_aabb() const;
        BoundingBall<VectorType> get_bounding_sphere() const;

        void set_vertices(const std::vector<VectorType>& verts);
        void set_normals(const std::vector<VectorType>& norms);
        void set_uvs(const std::vector<Vec2d>& uvs);
        void set_colors(const std::vector<Color4>& cols);
        void set_faces(const std::vector<Face>& fcs);

        const std::vector<VectorType>& get_vertices() const;
        const std::vector<VectorType>& get_normals() const;
        const std::vector<Vec2d>& get_uvs() const;
        const std::vector<Color4>& get_colors() const;
        const std::vector<Face>& get_faces() const;

        size_t num_vertices() const;

        void clear();

    private:

        std::vector<VectorType> _vertices;
        std::vector<VectorType> _normals;
        std::vector<Vec2d> _uv_coords;
        std::vector<Color4> _colors;
        std::vector<Face> _faces;

        mutable Box<VectorType> _aabb;
        mutable BoundingBall<VectorType> _bounding_sphere;

        mutable bool _aabb_dirty { true };
        mutable bool _bounding_sphere_dirty { true };
    };

    template <typename VectorType>
    Box<VectorType> TriangleMesh<VectorType>::get_aabb() const
    {
        if (!_aabb_dirty)
        {
            return _aabb;
        }

        if (_vertices.empty())
        {
            return Box3d {
                .min = Vec3d { 0, 0, 0 }, 
                .max = Vec3d { 0, 0, 0 }
            };
        }

        Box3d extent {
            .min = _vertices[0],
            .max = _vertices[0]
        };

        for (const auto& vertex : _vertices)
        {
            extent.expand(vertex);
        }

        _aabb = extent;
        _aabb_dirty = false;
        return _aabb;
    }

    template <typename VectorType>
    BoundingBall<VectorType> TriangleMesh<VectorType>::get_bounding_sphere() const
    {
        if (!_bounding_sphere_dirty)
        {
            return _bounding_sphere;
        }

        const Box<VectorType> aabb { get_aabb() };
        const VectorType center { aabb.min + (aabb.max - aabb.min) * 0.5 };

        double radius { 0 };

        for (const auto& vertex : _vertices)
        {
            const double dist { (vertex - center).length() };
            if (dist > radius)
            {
                radius = dist;
            }
        }

        _bounding_sphere = BoundingBall(center, radius);
        _bounding_sphere_dirty = false;
        return _bounding_sphere;
    }

    template <typename VectorType>
    void TriangleMesh<VectorType>::set_vertices(const std::vector<VectorType>& verts)
    {
        _vertices = verts;
        _aabb_dirty = true;
        _bounding_sphere_dirty = true;
    }

    template <typename VectorType>
    void TriangleMesh<VectorType>::set_normals(const std::vector<VectorType>& norms)
    {
        _normals = norms;
    }

    template <typename VectorType>
    void TriangleMesh<VectorType>::set_uvs(const std::vector<Vec2d>& uvs)
    {
        _uv_coords = uvs;
    }

    template <typename VectorType>
    void TriangleMesh<VectorType>::set_colors(const std::vector<Color4>& cols)
    {
        _colors = cols;
    }

    template <typename VectorType>
    void TriangleMesh<VectorType>::set_faces(const std::vector<Face>& fcs)
    {
        _faces = fcs;
    }

    template <typename VectorType>
    const std::vector<VectorType>& TriangleMesh<VectorType>::get_vertices() const
    {
        return _vertices;
    }

    template <typename VectorType>
    const std::vector<VectorType>& TriangleMesh<VectorType>::get_normals() const
    {
        return _normals;
    }

    template <typename VectorType>
    const std::vector<Vec2d>& TriangleMesh<VectorType>::get_uvs() const
    {
        return _uv_coords;
    }

    template <typename VectorType>
    const std::vector<Color4>& TriangleMesh<VectorType>::get_colors() const
    {
        return _colors;
    }

    template <typename VectorType>
    const std::vector<typename TriangleMesh<VectorType>::Face>& TriangleMesh<VectorType>::get_faces() const
    {
        return _faces;
    }

    template <typename VectorType>
    size_t TriangleMesh<VectorType>::num_vertices() const
    {
        return _vertices.size();
    }

    template <typename VectorType>
    void TriangleMesh<VectorType>::clear()
    {
        _vertices.clear();
        _normals.clear();
        _uv_coords.clear();
        _colors.clear();
        _faces.clear();
    }
}
