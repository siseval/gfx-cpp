#pragma once

#include "gfx/core/material/material.h"
#include "gfx/core/types/triangle-mesh.h"
#include "gfx/core/types/uuid.h"
#include "gfx/math/box.h"
#include "gfx/math/matrix.h"
#include "gfx/core/material/vertex-shader.h"

namespace gfx
{
    class Primitive3D
    {
    public:

        virtual ~Primitive3D() = default;

        Primitive3D();

        virtual const TriangleMesh& get_mesh() const = 0;

        void set_position(const Vec3d& pos);
        void set_position(double x, double y, double z);
        void set_scale(const Vec3d& s);
        void set_scale(double x, double y, double z);
        void set_rotation(const Vec3d& rot);
        void set_rotation(double x, double y, double z);
        void set_rotation_degrees(const Vec3d& rot);
        void set_rotation_degrees(double x, double y, double z);
        void set_color(const Color4& col);
        void set_color(double r, double g, double b, double a = 1.0);
        void set_anchor(const Vec3d& a);
        void set_material(const std::shared_ptr<Material>& mat, size_t slot = 0);
        void set_vertex_shader(const std::shared_ptr<VertexShader>& shader);

        Vec3d get_position() const;
        Vec3d get_scale() const;
        Vec3d get_rotation() const;
        Vec3d get_rotation_degrees() const;
        Color4 get_color() const;
        Vec3d get_anchor() const;
        std::shared_ptr<Material> get_material(size_t slot = 0) const;
        std::vector<std::shared_ptr<Material>> get_materials() const;
        std::shared_ptr<VertexShader> get_vertex_shader() const;

        virtual Box3d get_aabb() const;
        virtual BoundingBall get_bounding_sphere() const;

        UUID get_id() const;
        Matrix4x4d get_transform() const;
        int64_t get_transform_version() const;

    protected:

        void set_mesh_dirty(bool dirty) const;
        void set_transform_dirty(bool dirty) const;

        bool is_mesh_dirty() const;
        bool is_transform_dirty() const;

        void increment_transform_version();

        UUID _id;
        std::vector<std::shared_ptr<Material>> _materials;
        std::shared_ptr<VertexShader> _vertex_shader;
        Color4 _color;

        Vec3d _position;
        Vec3d _rotation;
        Vec3d _scale { 1.0, 1.0, 1.0 };
        Vec3d _anchor;

        mutable TriangleMesh _mesh_data;
        mutable bool _mesh_dirty { true };

        mutable Matrix4x4d _cached_transform;
        mutable bool _transform_dirty { true };
        int64_t _transform_version { 0 };
    };
}
