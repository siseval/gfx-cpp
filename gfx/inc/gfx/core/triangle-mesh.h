#pragma once

#include "../geometry/types/bounding-ball.h"
#include "gfx/core/types/color4.h"
#include "../geometry/types/aligned-box.h"
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
            size_t v0;
            size_t v1;
            size_t v2;
            size_t material_index { 0 };
        };

        AlignedBox<VectorType> get_aabb() const;
        BoundingBall<VectorType> get_bounding_sphere() const;

        void set_vertices(const std::vector<VectorType>& vertices);
        void set_normals(const std::vector<Vec3d>& normals);
        void set_uvs(const std::vector<Vec2d>& uvs);
        void set_colors(const std::vector<Color4>& colors);
        void set_faces(const std::vector<Face>& faces);

        const std::vector<VectorType>& get_vertices() const;
        const std::vector<Vec3d>& get_normals() const;
        const std::vector<Vec2d>& get_uvs() const;
        const std::vector<Color4>& get_colors() const;
        const std::vector<Face>& get_faces() const;
        
        void calculate_uvs() requires std::same_as<VectorType, Vec2d>;

        size_t num_vertices() const;

        void clear();

    private:

        std::vector<VectorType> _vertices;
        std::vector<Vec3d> _normals;
        std::vector<Vec2d> _uvs;
        std::vector<Color4> _colors;
        std::vector<Face> _faces;

        mutable AlignedBox<VectorType> _aabb;
        mutable BoundingBall<VectorType> _bounding_ball;

        mutable bool _aabb_dirty { true };
        mutable bool _bounding_sphere_dirty { true };
    };

    template <typename VectorType>
    AlignedBox<VectorType> TriangleMesh<VectorType>::get_aabb() const
    {
        if (!_aabb_dirty)
        {
            return _aabb;
        }

        if (_vertices.empty())
        {
            return AlignedBox<VectorType> {
                .min = VectorType::zero(),
                .max = VectorType::zero()
            };
        }

        AlignedBox<VectorType> extent {
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
            return _bounding_ball;
        }

        const AlignedBox<VectorType> aabb { get_aabb() };
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

        _bounding_ball = BoundingBall(center, radius);
        _bounding_sphere_dirty = false;
        return _bounding_ball;
    }

    template <typename VectorType>
    void TriangleMesh<VectorType>::set_vertices(const std::vector<VectorType>& vertices)
    {
        _vertices = vertices;
        _aabb_dirty = true;
        _bounding_sphere_dirty = true;
    }

    template <typename VectorType>
    void TriangleMesh<VectorType>::set_normals(const std::vector<Vec3d>& normals)
    {
        _normals = normals;
    }

    template <typename VectorType>
    void TriangleMesh<VectorType>::set_uvs(const std::vector<Vec2d>& uvs)
    {
        _uvs = uvs;
    }

    template <typename VectorType>
    void TriangleMesh<VectorType>::set_colors(const std::vector<Color4>& colors)
    {
        _colors = colors;
    }

    template <typename VectorType>
    void TriangleMesh<VectorType>::set_faces(const std::vector<Face>& faces)
    {
        _faces = faces;
    }

    template <typename VectorType>
    const std::vector<VectorType>& TriangleMesh<VectorType>::get_vertices() const
    {
        return _vertices;
    }

    template <typename VectorType>
    const std::vector<Vec3d>& TriangleMesh<VectorType>::get_normals() const
    {
        return _normals;
    }

    template <typename VectorType>
    const std::vector<Vec2d>& TriangleMesh<VectorType>::get_uvs() const
    {
        return _uvs;
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
    void TriangleMesh<VectorType>::calculate_uvs() requires std::same_as<VectorType, Vec2d>
    {
        const AlignedBox<Vec2d> aabb { get_aabb() };
        
        _uvs.resize(_vertices.size());
        for (size_t i = 0; i < _vertices.size(); ++i)
        {
            _uvs[i] = aabb.get_sample_coords(_vertices[i]);
        }
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
        _uvs.clear();
        _colors.clear();
        _faces.clear();
    }
}
