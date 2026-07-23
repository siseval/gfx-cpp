#include "gfx/core/primitive-2D.h"

#include <algorithm>

#include "gfx/core/types/uuid.h"
#include "gfx/geometry/transform-2D.h"


namespace gfx
{
    Primitive2D::Primitive2D()
        : _id(UUID::generate()) {}

    Box2d Primitive2D::get_axis_aligned_bounding_box(const Matrix3x3d& transform) const
    {
        const auto [min, max] { get_geometry_size() };
        Vec2d top_left { min };
        Vec2d bot_right { max };

        const std::vector<Vec2d> corners {
            { top_left.x, top_left.y },
            { bot_right.x, top_left.y },
            { top_left.x, bot_right.y },
            { bot_right.x, bot_right.y },
        };
        const std::vector transformed_corners { Transform2D::transform_points(corners, transform) };

        Box2d bounds { transformed_corners[0], transformed_corners[0] };
        bounds.expand(transformed_corners);

        return bounds;
    }

    OBB2D Primitive2D::get_oriented_bounding_box(const Matrix3x3d& transform) const
    {
        if (!_obb_dirty)
        {
            return _cached_obb;
        }

        const auto [min, max] { get_geometry_size() };

        OBB2D bounds { min, { max.x - min.x, 0 }, { 0, max.y - min.y } };

        bounds.origin = Transform2D::transform_point(bounds.origin, transform);
        bounds.side_x = Transform2D::transform_vector(bounds.side_x, transform);
        bounds.side_y = Transform2D::transform_vector(bounds.side_y, transform);

        _cached_obb = bounds;
        _obb_dirty = false;

        return bounds;
    }

    void Primitive2D::set_position(const Vec2d pos)
    {
        _position = pos;
        increment_transform_version();
        set_obb_dirty();
        set_transform_dirty();
    }

    void Primitive2D::set_position(const double x, const double y)
    {
        _position = Vec2d { x, y };
        increment_transform_version();
        set_obb_dirty();
        set_transform_dirty();
    }

    void Primitive2D::set_scale(const Vec2d s)
    {
        _scale = s;
        increment_transform_version();
        set_obb_dirty();
        set_transform_dirty();
    }

    void Primitive2D::set_scale(const double sx, const double sy)
    {
        _scale = Vec2d { sx, sy };
        increment_transform_version();
        set_obb_dirty();
        set_transform_dirty();
    }

    void Primitive2D::set_scale(const double s)
    {
        _scale = Vec2d { s, s };
        increment_transform_version();
        set_obb_dirty();
        set_transform_dirty();
    }

    void Primitive2D::set_rotation(const double r)
    {
        _rotation = r;
        increment_transform_version();
        set_obb_dirty();
        set_transform_dirty();
    }

    void Primitive2D::set_rotation_degrees(const double r)
    {
        _rotation = r * std::numbers::pi / 180;
        increment_transform_version();
        set_obb_dirty();
        set_transform_dirty();
    }

    void Primitive2D::set_color(const Color4 col)
    {
        _color = col;
    }

    void Primitive2D::set_color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a)
    {
        _color = Color4 { r, g, b, a };
    }

    void Primitive2D::set_color(const double r, const double g, const double b, const double a)
    {
        _color = Color4 {
            static_cast<uint8_t>(std::clamp(r * 255.0, 0.0, 255.0)),
            static_cast<uint8_t>(std::clamp(g * 255.0, 0.0, 255.0)),
            static_cast<uint8_t>(std::clamp(b * 255.0, 0.0, 255.0)),
            static_cast<uint8_t>(std::clamp(a * 255.0, 0.0, 255.0))
        };
    }

    void Primitive2D::set_anchor(const Vec2d pos)
    {
        _anchor = pos;
        increment_transform_version();
        set_obb_dirty();
        set_transform_dirty();
    }

    void Primitive2D::set_anchor(const double x, const double y)
    {
        _anchor = Vec2d { x, y };
        increment_transform_version();
        set_obb_dirty();
        set_transform_dirty();
    }

    void Primitive2D::set_depth(const int d)
    {
        _depth = d;
    }

    void Primitive2D::set_visible(const bool v)
    {
        _visible = v;
    }

    void Primitive2D::set_shader(const std::shared_ptr<Shader2D>& shd)
    {
        _shader = shd;
    }

    Vec2d Primitive2D::get_position() const
    {
        return _position;
    }

    Vec2d Primitive2D::get_scale() const
    {
        return _scale;
    }

    double Primitive2D::get_rotation() const
    {
        return _rotation;
    }

    double Primitive2D::get_rotation_degrees() const
    {
        return _rotation * 180 / std::numbers::pi;
    }

    Color4 Primitive2D::get_color() const
    {
        return _color;
    }

    Vec2d Primitive2D::get_anchor() const
    {
        return _anchor;
    }

    int Primitive2D::get_depth() const
    {
        return _depth;
    }

    bool Primitive2D::is_visible() const
    {
        return _visible;
    }

    std::shared_ptr<Shader2D> Primitive2D::get_shader() const
    {
        return _shader;
    }

    UUID Primitive2D::get_id() const
    {
        return _id;
    }

    Vec2d Primitive2D::get_uv(const Vec2d point) const
    {
        const OBB2D obb { get_oriented_bounding_box(get_transform()) };
        return obb.get_uv(point);
    }

    Matrix3x3d Primitive2D::get_transform() const
    {
        if (!_transform_dirty)
        {
            return _cached_transform;
        }

        const Vec2d size { get_geometry_size().size() };
        const Vec2d anchor_offset { get_anchor() * size };

        const Matrix3x3d anchor_translation_matrix { Transform2D::translate(-anchor_offset) };
        const Matrix3x3d scale_matrix { Transform2D::scale(_scale) };
        const Matrix3x3d rotation_matrix { Transform2D::rotate(_rotation) };
        const Matrix3x3d position_translation_matrix { Transform2D::translate(get_position()) };

        _cached_transform = position_translation_matrix * rotation_matrix * scale_matrix * anchor_translation_matrix;

        _cached_transform = position_translation_matrix * rotation_matrix * scale_matrix * anchor_translation_matrix;
        _transform_dirty = false;

        return _cached_transform;
    }

    int64_t Primitive2D::get_transform_version() const
    {
        return _transform_version;
    }

    void Primitive2D::increment_transform_version()
    {
        _transform_version++;
    }

    void Primitive2D::set_obb_dirty() const
    {
        _obb_dirty = true;
    }

    void Primitive2D::set_transform_dirty() const
    {
        _transform_dirty = true;
    }

    bool Primitive2D::is_transform_dirty() const
    {
        return _transform_dirty;
    }

    bool Primitive2D::is_obb_dirty() const
    {
        return _obb_dirty;
    }
}
