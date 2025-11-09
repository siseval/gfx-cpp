#ifndef PRIMITIVE_2D_H
#define PRIMITIVE_2D_H

#include <algorithm>
#include <gfx/core/types/pixel.h>
#include <gfx/core/types/color4.h>
#include <gfx/core/types/obb-2D.h>
#include <gfx/core/shader-2D.h>
#include <gfx/core/render-surface.h>
#include <gfx/utils/uuid.h>
#include <gfx/math/box2.h>
#include <gfx/math/matrix.h>

namespace gfx::core
{

class Primitive2D
{

public:

    Primitive2D() : id(gfx::utils::UUID::generate()) {}
        
    virtual void rasterize(const gfx::math::Matrix3x3d &transform, const std::function<void(const types::Pixel&)> emit_pixel) const = 0;

    gfx::core::types::OBB2D get_oriented_bounding_box(const gfx::math::Matrix3x3d &transform) const;
    virtual gfx::math::Box2d get_geometry_size() const = 0;
    virtual gfx::math::Box2d get_axis_aligned_bounding_box(const gfx::math::Matrix3x3d &transform) const;

    gfx::math::Vec2d get_uv(const gfx::math::Vec2d point) const;

    inline bool is_obb_dirty() const { return obb_dirty; }
    inline void set_obb_dirty() { obb_dirty = true; }

    inline void set_shader(const std::shared_ptr<gfx::core::Shader2D> &shd) { shader = shd; }
    inline std::shared_ptr<gfx::core::Shader2D> get_shader() const { return shader; }

    inline void set_use_shader(const bool use) { use_shader = use; }
    inline bool get_use_shader() const { return use_shader; }

    virtual bool point_collides(const gfx::math::Vec2d point, const gfx::math::Matrix3x3d &transform) const = 0;
    inline bool point_collides(const double x, const double y, const gfx::math::Matrix3x3d &transform) const
    {
        return point_collides(gfx::math::Vec2d { x, y }, transform);
    }

    gfx::math::Matrix3x3d get_transform() const;

    inline gfx::utils::UUID get_id() const { return id; }

    inline types::Color4 get_color() const { return color; }
    inline void set_color(const types::Color4 col) { color = col; }
    inline void set_color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a = 255) { color = types::Color4 { r, g, b, a }; }
    inline void set_color(const double r, const double g, const double b, const double a = 1.0) 
    { 
        color = types::Color4 { 
            static_cast<uint8_t>(std::clamp(r * 255.0, 0.0, 255.0)), 
            static_cast<uint8_t>(std::clamp(g * 255.0, 0.0, 255.0)), 
            static_cast<uint8_t>(std::clamp(b * 255.0, 0.0, 255.0)), 
            static_cast<uint8_t>(std::clamp(a * 255.0, 0.0, 255.0)) 
        }; 
    }

    inline gfx::math::Box2d get_bounds() const { return bounds; }
    inline gfx::math::Vec2d get_bounds_size() const { return bounds.size(); }

    inline gfx::math::Vec2d get_anchor() const { return anchor; }
    inline void set_anchor(const gfx::math::Vec2d pos) 
    { 
        anchor = pos; 
        increment_transform_version(); 
        set_obb_dirty();
        set_transform_dirty();
    }
    inline void set_anchor(const double x, const double y) 
    { 
        anchor = gfx::math::Vec2d { x, y }; 
        increment_transform_version(); 
        set_obb_dirty();
        set_transform_dirty();
    }

    inline int get_depth() const { return depth; }
    inline void set_depth(const int d) { depth = d; }

    inline gfx::math::Vec2d get_position() const { return position; }
    inline void set_position(const gfx::math::Vec2d pos) 
    { 
        position = pos; 
        increment_transform_version(); 
        set_obb_dirty(); 
        set_transform_dirty();
    }
    inline void set_position(const double x, const double y) 
    { 
        position = gfx::math::Vec2d { x, y }; 
        increment_transform_version(); 
        set_obb_dirty(); 
        set_transform_dirty();
    }
    
    inline gfx::math::Vec2f get_scale() const { return scale; }
    inline void set_scale(const gfx::math::Vec2d s) 
    { 
        scale = s; 
        increment_transform_version(); 
        set_obb_dirty(); 
        set_transform_dirty();
    }
    inline void set_scale(const double sx, const double sy) 
    { 
        scale = gfx::math::Vec2d { sx, sy }; 
        increment_transform_version(); 
        set_obb_dirty(); 
        set_transform_dirty();
    }
    inline void set_scale(const double s) 
    { 
        scale = gfx::math::Vec2d { s, s }; 
        increment_transform_version(); 
        set_obb_dirty(); 
        set_transform_dirty();
    }

    inline double get_rotation() const { return rotation; }
    inline void set_rotation(const double r) 
    { 
        rotation = r; 
        increment_transform_version(); 
        set_obb_dirty(); 
        set_transform_dirty();
    }

    inline double get_rotation_degrees() const { return rotation * 180 / std::numbers::pi; }
    inline void set_rotation_degrees(const double r) 
    { 
        rotation = r * std::numbers::pi / 180; 
        increment_transform_version(); 
        set_obb_dirty(); 
        set_transform_dirty();
    }

    inline bool is_visible() const { return visible; }
    inline void set_visible(const bool v) { visible = v; }

    inline int64_t get_transform_version() const { return transform_version; }
    inline void increment_transform_version() { transform_version++; }

    inline bool is_transform_dirty() const { return transform_dirty; }
    inline void set_transform_dirty() { transform_dirty = true; }

    static int count;

protected:

    gfx::utils::UUID id;
    std::shared_ptr<gfx::core::Shader2D> shader;
    bool use_shader = false;

    types::Color4 color;

    gfx::math::Box2d bounds;
    gfx::math::Vec2d position;
    gfx::math::Vec2d anchor { 0.0, 0.0 };
    gfx::math::Vec2d scale { 1.0, 1.0 };

    bool fill = false;
    bool visible = true;
    double rotation = 0.0;
    int depth = 0;

    mutable types::OBB2D cached_obb;
    mutable bool obb_dirty = true;

    mutable gfx::math::Matrix3x3d cached_transform;
    mutable bool transform_dirty = true;
    int64_t transform_version = -1;

    // bool should_fill_pixel(std::shared_ptr<GfxContext2D> context, const gfx::math::Vec2d pixel) const;

};

};

template <>
struct std::hash<gfx::core::Primitive2D>
{
    size_t operator()(const gfx::core::Primitive2D& item) const
    {
        int64_t hash = std::hash<gfx::math::Vec2d>()(item.get_position());
        hash ^= (std::hash<gfx::math::Vec2d>()(item.get_scale()) << 1);
        hash ^= (std::hash<double>()(item.get_rotation()) << 1);
        hash ^= (std::hash<int>()(item.get_depth()) << 1);

        return hash;
    }
};


#endif // PRIMITIVE_2D_H
