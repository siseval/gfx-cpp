#include "gfx/core/primitive-3D.h"

#include "gfx/geometry/transform-3D.h"

namespace gfx
{
    Primitive3D::Primitive3D()
        : _id(UUID::generate()) {}

    void Primitive3D::set_position(const Vec3d& pos)
    {
        _position = pos;
        set_transform_dirty(true);
        increment_transform_version();
    }

    void Primitive3D::set_position(const double x, const double y, const double z)
    {
        _position = Vec3d { x, y, z };
        set_transform_dirty(true);
        increment_transform_version();
    }

    void Primitive3D::set_scale(const Vec3d& s)
    {
        _scale = s;
        set_transform_dirty(true);
        increment_transform_version();
    }

    void Primitive3D::set_scale(const double x, const double y, const double z)
    {
        _scale = Vec3d { x, y, z };
        set_transform_dirty(true);
        increment_transform_version();
    }

    void Primitive3D::set_rotation(const Vec3d& rot)
    {
        _rotation = rot;
        set_transform_dirty(true);
        increment_transform_version();
    }

    void Primitive3D::set_rotation(const double x, const double y, const double z)
    {
        _rotation = Vec3d { x, y, z };
        set_transform_dirty(true);
        increment_transform_version();
    }

    void Primitive3D::set_rotation_degrees(const Vec3d& rot)
    {
        _rotation = Vec3d {
            rot.x * std::numbers::pi / 180,
            rot.y * std::numbers::pi / 180,
            rot.z * std::numbers::pi / 180
        };
        set_transform_dirty(true);
        increment_transform_version();
    }

    void Primitive3D::set_rotation_degrees(const double x, const double y, const double z)
    {
        _rotation = Vec3d { x * std::numbers::pi / 180, y * std::numbers::pi / 180, z * std::numbers::pi / 180 };
        set_transform_dirty(true);
        increment_transform_version();
    }

    void Primitive3D::set_color(const Color4& col)
    {
        _color = col;
    }

    void Primitive3D::set_color(const double r, const double g, const double b, const double a)
    {
        _color = Color4(r, g, b, a);
    }

    void Primitive3D::set_anchor(const Vec3d& a)
    {
        _anchor = a;
        set_transform_dirty(true);
        increment_transform_version();
    }

    void Primitive3D::set_material(const std::shared_ptr<Material>& mat, const size_t slot)
    {
        _materials.resize(std::max(slot + 1, _materials.size()));
        _materials[slot] = mat;
    }

    void Primitive3D::set_vertex_shader(const std::shared_ptr<VertexShader>& shader)
    {
        _vertex_shader = shader;
    }

    Vec3d Primitive3D::get_position() const
    {
        return _position;
    }

    Vec3d Primitive3D::get_scale() const
    {
        return _scale;
    }

    Vec3d Primitive3D::get_rotation() const
    {
        return _rotation;
    }

    Vec3d Primitive3D::get_rotation_degrees() const
    {
        return Vec3d {
            _rotation.x * 180 / std::numbers::pi,
            _rotation.y * 180 / std::numbers::pi,
            _rotation.z * 180 / std::numbers::pi
        };
    }

    Color4 Primitive3D::get_color() const
    {
        return _color;
    }

    Vec3d Primitive3D::get_anchor() const
    {
        return _anchor;
    }

    std::shared_ptr<Material> Primitive3D::get_material(const size_t slot) const
    {
        if (slot >= _materials.size())
        {
            return nullptr;
        }
        return _materials[slot];
    }

    std::vector<std::shared_ptr<Material>> Primitive3D::get_materials() const
    {
        return _materials;
    }

    std::shared_ptr<VertexShader> Primitive3D::get_vertex_shader() const
    {
        return _vertex_shader;
    }

    Box3d Primitive3D::get_aabb() const
    {
        return _mesh_data.get_aabb();
    }

    BoundingSphere Primitive3D::get_bounding_sphere() const
    {
        return _mesh_data.get_bounding_sphere();
    }

    UUID Primitive3D::get_id() const
    {
        return _id;
    }

    Matrix4x4d Primitive3D::get_transform() const
    {
        if (!_transform_dirty)
        {
            return _cached_transform;
        }

        const Vec3d anchor_offset { get_anchor() * get_aabb().size() };

        const Matrix4x4d anchor_translation = Transform3D::translate(
            { -anchor_offset.x, -anchor_offset.y, -anchor_offset.z }
        );
        const Matrix4x4d translation = Transform3D::translate({ _position.x, _position.y, _position.z });

        const Matrix4x4d rotation_x = Transform3D::rotate_x(_rotation.x);
        const Matrix4x4d rotation_y = Transform3D::rotate_y(_rotation.y);
        const Matrix4x4d rotation_z = Transform3D::rotate_z(_rotation.z);
        const Matrix4x4d rotation = rotation_z * rotation_y * rotation_x;

        const Matrix4x4d scaling = Transform3D::scale({ _scale.x, _scale.y, _scale.z });

        _cached_transform = translation * rotation * scaling * anchor_translation;
        _transform_dirty = false;

        return _cached_transform;
    }

    int64_t Primitive3D::get_transform_version() const
    {
        return _transform_version;
    }

    void Primitive3D::set_mesh_dirty(const bool dirty) const
    {
        _mesh_dirty = dirty;
    }

    void Primitive3D::set_transform_dirty(const bool dirty) const
    {
        _transform_dirty = dirty;
    }

    bool Primitive3D::is_mesh_dirty() const
    {
        return _mesh_dirty;
    }

    bool Primitive3D::is_transform_dirty() const
    {
        return _transform_dirty;
    }

    void Primitive3D::increment_transform_version()
    {
        _transform_version++;
    }
}
