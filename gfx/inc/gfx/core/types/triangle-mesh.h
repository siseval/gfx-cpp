#pragma once

#include "gfx/core/types/bounding-sphere.h"
#include "gfx/core/types/color4.h"
#include "gfx/math/box3.h"
#include "gfx/math/vec2.h"
#include "gfx/math/vec3.h"

#include <vector>

namespace gfx
{
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

        TriangleMesh();

        Box3d get_aabb() const;
        BoundingSphere get_bounding_sphere() const;

        void set_vertices(const std::vector<Vec3d>& verts);
        void set_normals(const std::vector<Vec3d>& norms);
        void set_uvs(const std::vector<Vec2d>& uvs);
        void set_colors(const std::vector<Color4>& cols);
        void set_faces(const std::vector<Face>& fcs);

        const std::vector<Vec3d>& get_vertices() const;
        const std::vector<Vec3d>& get_normals() const;
        const std::vector<Vec2d>& get_uvs() const;
        const std::vector<Color4>& get_colors() const;
        const std::vector<Face>& get_faces() const;

        size_t num_vertices() const;

        void clear();

    private:

        std::vector<Vec3d> _vertices;
        std::vector<Vec3d> _normals;
        std::vector<Vec2d> _uv_coords;
        std::vector<Color4> _colors;
        std::vector<Face> _faces;

        mutable bool _aabb_dirty;
        mutable bool _bounding_sphere_dirty;
        mutable Box3d _aabb;
        mutable BoundingSphere _bounding_sphere;
    };
}
