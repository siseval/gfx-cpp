#pragma once

#include "gfx/core/material/material.h"
#include "gfx/core/material/vertex-shader.h"
#include "transform.h"
#include "gfx/core/types/triangle-mesh.h"
#include "gfx/core/types/uuid.h"
#include "gfx/math/box.h"

namespace gfx
{
    template <typename VectorType>
    class Primitive
    {
    public:

        using RotationType = Transform<VectorType>::RotationType;

        Primitive();

        virtual ~Primitive() = default;

        virtual const TriangleMesh<VectorType>& get_mesh() const = 0;

        void set_position(VectorType pos);
        void set_scale(VectorType s);
        void set_rotation(RotationType rot);
        void set_color(const Color4& col);
        void set_color(double r, double g, double b, double a = 1.0);
        void set_anchor(VectorType a);
        void set_material(const std::shared_ptr<Material>& mat, size_t slot = 0);
        void set_vertex_shader(const std::shared_ptr<VertexShader<VectorType>>& shader);

        VectorType get_position() const;
        VectorType get_scale() const;
        RotationType get_rotation() const;
        Color4 get_color() const;
        VectorType get_anchor() const;
        std::shared_ptr<Material> get_material(size_t slot = 0) const;
        std::vector<std::shared_ptr<Material>> get_materials() const;
        std::shared_ptr<VertexShader<VectorType>> get_vertex_shader() const;

        virtual Box<VectorType> get_aabb() const;
        virtual BoundingBall<VectorType> get_bounding_sphere() const;

        UUID get_id() const;
        Transform<VectorType> get_transform() const;
        int64_t get_transform_generation() const;


        /* 2D Specific helpers */

        void set_position(double x, double y) requires std::same_as<VectorType, Vec2d>;
        void set_scale(double x, double y) requires std::same_as<VectorType, Vec2d>;
        void set_rotation_degrees(double angle) requires std::same_as<RotationType, double>;
        void set_anchor(double x, double y) requires std::same_as<VectorType, Vec2d>;


        /* 3D Specific helpers */

        void set_position(double x, double y, double z) requires std::same_as<VectorType, Vec3d>;
        void set_scale(double x, double y, double z) requires std::same_as<VectorType, Vec3d>;
        void set_rotation(double x, double y, double z) requires std::same_as<RotationType, Vec3d>;
        void set_rotation_degrees(Vec3d rot) requires std::same_as<RotationType, Vec3d>;
        void set_rotation_degrees(double x, double y, double z) requires std::same_as<RotationType, Vec3d>;
        void set_anchor(double x, double y, double z) requires std::same_as<VectorType, Vec3d>;

    protected:

        void set_mesh_dirty(bool dirty) const;
        void set_transform_dirty(bool dirty) const;

        bool is_mesh_dirty() const;
        bool is_transform_dirty() const;

        void increment_transform_generation();
        void transform_updated();

        UUID _id;

        std::vector<std::shared_ptr<Material>> _materials;
        std::shared_ptr<VertexShader<VectorType>> _vertex_shader;

        Color4 _color;

        VectorType _position { VectorType::zero() };
        RotationType _rotation { RotationType::zero() };
        VectorType _scale { VectorType::one() };
        VectorType _anchor { VectorType::zero() };

        mutable TriangleMesh<VectorType> _mesh_data;
        mutable Transform<VectorType> _cached_transform;

        mutable bool _mesh_dirty { true };
        mutable bool _transform_dirty { true };

        int64_t _transform_generation { 0 };
    };

    template <typename VectorType>
    Primitive<VectorType>::Primitive()
        : _id(UUID::generate()) {}

    template <typename VectorType>
    void Primitive<VectorType>::set_position(const VectorType pos)
    {
        _position = pos;
        transform_updated();
    }

    template <typename VectorType>
    void Primitive<VectorType>::set_scale(const VectorType s)
    {
        _scale = s;
        transform_updated();
    }

    template <typename VectorType>
    void Primitive<VectorType>::set_rotation(const RotationType rot)
    {
        _rotation = rot;
        transform_updated();
    }

    template <typename VectorType>
    void Primitive<VectorType>::set_color(const Color4& col)
    {
        _color = col;
    }

    template <typename VectorType>
    void Primitive<VectorType>::set_color(const double r, const double g, const double b, const double a)
    {
        _color = Color4(r, g, b, a);
    }

    template <typename VectorType>
    void Primitive<VectorType>::set_anchor(const VectorType a)
    {
        _anchor = a;
        transform_updated();
    }

    template <typename VectorType>
    void Primitive<VectorType>::set_material(const std::shared_ptr<Material>& mat, const size_t slot)
    {
        _materials.resize(std::max(slot + 1, _materials.size()));
        _materials[slot] = mat;
    }

    template <typename VectorType>
    void Primitive<VectorType>::set_vertex_shader(const std::shared_ptr<VertexShader<VectorType>>& shader)
    {
        _vertex_shader = shader;
    }

    template <typename VectorType>
    VectorType Primitive<VectorType>::get_position() const
    {
        return _position;
    }

    template <typename VectorType>
    VectorType Primitive<VectorType>::get_scale() const
    {
        return _scale;
    }

    template <typename VectorType>
    Primitive<VectorType>::RotationType Primitive<VectorType>::get_rotation() const
    {
        return _rotation;
    }

    template <typename VectorType>
    Color4 Primitive<VectorType>::get_color() const
    {
        return _color;
    }

    template <typename VectorType>
    VectorType Primitive<VectorType>::get_anchor() const
    {
        return _anchor;
    }

    template <typename VectorType>
    std::shared_ptr<Material> Primitive<VectorType>::get_material(const size_t slot) const
    {
        if (slot >= _materials.size())
        {
            return nullptr;
        }
        return _materials[slot];
    }

    template <typename VectorType>
    std::vector<std::shared_ptr<Material>> Primitive<VectorType>::get_materials() const
    {
        return _materials;
    }

    template <typename VectorType>
    std::shared_ptr<VertexShader<VectorType>> Primitive<VectorType>::get_vertex_shader() const
    {
        return _vertex_shader;
    }

    template <typename VectorType>
    Box<VectorType> Primitive<VectorType>::get_aabb() const
    {
        return _mesh_data.get_aabb();
    }

    template <typename VectorType>
    BoundingBall<VectorType> Primitive<VectorType>::get_bounding_sphere() const
    {
        return _mesh_data.get_bounding_sphere();
    }

    template <typename VectorType>
    UUID Primitive<VectorType>::get_id() const
    {
        return _id;
    }

    template <typename VectorType>
    Transform<VectorType> Primitive<VectorType>::get_transform() const
    {
        if (!_transform_dirty)
        {
            return _cached_transform;
        }

        const VectorType anchor_offset { get_anchor() * get_aabb().size() };

        _cached_transform.set_transform(_position - anchor_offset, _rotation, _scale);

        _transform_dirty = false;

        return _cached_transform;
    }

    template <typename VectorType>
    int64_t Primitive<VectorType>::get_transform_generation() const
    {
        return _transform_generation;
    }

    template <typename VectorType>
    void Primitive<VectorType>::set_position(const double x, const double y) requires std::same_as<VectorType, Vec2d>
    {
        _position = Vec2d { x, y };
        transform_updated();
    }

    template <typename VectorType>
    void Primitive<VectorType>::set_scale(const double x, const double y) requires std::same_as<VectorType, Vec2d>
    {
        _scale = Vec2d { x, y };
        transform_updated();
    }

    template <typename VectorType>
    void Primitive<VectorType>::set_rotation_degrees(const double angle) requires std::same_as<RotationType, double>
    {
        _rotation = angle * std::numbers::pi / 180;
        transform_updated();
    }

    template <typename VectorType>
    void Primitive<VectorType>::set_anchor(const double x, const double y) requires std::same_as<VectorType, Vec2d>
    {
        _anchor = Vec2d { x, y };
        transform_updated();
    }

    template <typename VectorType>
    void Primitive<VectorType>::set_position(const double x, const double y, const double z) requires std::same_as<
        VectorType, Vec3d>
    {
        _position = Vec3d { x, y, z };
        transform_updated();
    }

    template <typename VectorType>
    void Primitive<VectorType>::set_scale(const double x, const double y, const double z) requires std::same_as<
        VectorType, Vec3d>
    {
        _scale = Vec3d { x, y, z };
        transform_updated();
    }

    template <typename VectorType>
    void Primitive<VectorType>::set_rotation(const double x, const double y, const double z) requires std::same_as<
        RotationType, Vec3d>
    {
        _rotation = Vec3d { x, y, z };
        transform_updated();
    }

    template <typename VectorType>
    void Primitive<VectorType>::set_rotation_degrees(const Vec3d rot) requires std::same_as<RotationType, Vec3d>
    {
        _rotation = Vec3d {
            rot.x * std::numbers::pi / 180,
            rot.y * std::numbers::pi / 180,
            rot.z * std::numbers::pi / 180
        };
        transform_updated();
    }

    template <typename VectorType>
    void Primitive<VectorType>::set_rotation_degrees(const double x, const double y, const double z) requires
        std::same_as<RotationType, Vec3d>
    {
        _rotation = Vec3d {
            x * std::numbers::pi / 180,
            y * std::numbers::pi / 180,
            z * std::numbers::pi / 180
        };
        transform_updated();
    }

    template <typename VectorType>
    void Primitive<VectorType>::set_anchor(const double x, const double y, const double z) requires std::same_as<
        VectorType, Vec3d>
    {
        _anchor = Vec3d { x, y, z };
        transform_updated();
    }

    template <typename VectorType>
    void Primitive<VectorType>::set_mesh_dirty(const bool dirty) const
    {
        _mesh_dirty = dirty;
    }

    template <typename VectorType>
    void Primitive<VectorType>::set_transform_dirty(const bool dirty) const
    {
        _transform_dirty = dirty;
    }

    template <typename VectorType>
    bool Primitive<VectorType>::is_mesh_dirty() const
    {
        return _mesh_dirty;
    }

    template <typename VectorType>
    bool Primitive<VectorType>::is_transform_dirty() const
    {
        return _transform_dirty;
    }

    template <typename VectorType>
    void Primitive<VectorType>::increment_transform_generation()
    {
        _transform_generation++;
    }

    template <typename VectorType>
    void Primitive<VectorType>::transform_updated()
    {
        set_transform_dirty(true);
        increment_transform_generation();
    }
}
