#pragma once

#include <memory>

#include "gfx/core/shader-2D.h"
#include "gfx/core/types/color4.h"
#include "gfx/core/types/obb-2D.h"
#include "gfx/core/types/uuid.h"
#include "gfx/math/box2.h"
#include "gfx/math/matrix.h"
#include "gfx/shaders/default-shader-2D.h"

namespace gfx
{
    class Primitive2D
    {
    public:

        virtual ~Primitive2D() = default;

        struct RasterizeOutput
        {
            std::vector<Vec2i> pixels;
            bool use_alphas = false;
            std::vector<double> alphas;
        };

        Primitive2D();

        virtual RasterizeOutput rasterize(const Matrix3x3d& transform) const = 0;

        virtual Box2d get_geometry_size() const = 0;
        virtual Box2d get_axis_aligned_bounding_box(const Matrix3x3d& transform) const;

        OBB2D get_oriented_bounding_box(const Matrix3x3d& transform) const;

        void set_position(Vec2d pos);
        void set_position(double x, double y);
        void set_scale(Vec2d s);
        void set_scale(double sx, double sy);
        void set_scale(double s);
        void set_rotation(double r);
        void set_rotation_degrees(double r);
        void set_color(Color4 col);
        void set_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
        void set_color(double r, double g, double b, double a = 1.0);
        void set_anchor(Vec2d pos);
        void set_anchor(double x, double y);
        void set_depth(int d);
        void set_visible(bool v);
        void set_shader(const std::shared_ptr<Shader2D>& shd);

        Vec2d get_position() const;
        Vec2d get_scale() const;
        double get_rotation() const;
        double get_rotation_degrees() const;
        Color4 get_color() const;
        Vec2d get_anchor() const;
        int get_depth() const;
        bool is_visible() const;
        std::shared_ptr<Shader2D> get_shader() const;

        UUID get_id() const;
        Vec2d get_uv(Vec2d point) const;
        Matrix3x3d get_transform() const;

        int64_t get_transform_version() const;
        void increment_transform_version();

    protected:

        void set_obb_dirty() const;
        void set_transform_dirty() const;

        bool is_transform_dirty() const;
        bool is_obb_dirty() const;

        UUID _id;
        std::shared_ptr<Shader2D> _shader = std::make_shared<DefaultShader2D>();

        Color4 _color { Color4::white() };

        Vec2d _position { 0.0, 0.0 };
        Vec2d _anchor { 0.0, 0.0 };
        Vec2d _scale { 1.0, 1.0 };

        bool _fill = false;
        bool _visible = true;
        double _rotation = 0.0;
        int _depth = 0;

        mutable OBB2D _cached_obb;
        mutable bool _obb_dirty = true;

        mutable Matrix3x3d _cached_transform;
        mutable bool _transform_dirty = true;
        int64_t _transform_version = 0;
    };
}

template <>
struct std::hash<gfx::Primitive2D>
{
    size_t operator()(const gfx::Primitive2D& item) const noexcept
    {
        int64_t hash = std::hash<gfx::Vec2d>()(item.get_position());
        hash ^= std::hash<gfx::Vec2d>()(item.get_scale()) << 1;
        hash ^= std::hash<double>()(item.get_rotation()) << 1;
        hash ^= std::hash<int>()(item.get_depth()) << 1;

        return hash;
    }
};
