#pragma once

#include "gfx/core/render-surface.h"
#include "gfx/core/scene-graph-2D.h"
#include "gfx/core/types/color4.h"
// #include "gfx/core/types/bitmap.h"
#include "gfx/primitives/circle-2D.h"
#include "gfx/primitives/ellipse-2D.h"
#include "gfx/primitives/polygon-2D.h"
#include "gfx/primitives/polyline-2D.h"
#include "gfx/primitives/text-2D.h"
#include "gfx/text/font-manager-ttf.h"

namespace gfx
{
    class Render2D
    {
    public:

        explicit Render2D(std::shared_ptr<RenderSurface> surface, Vec2d viewport_scaling = Vec2d { 1, 1 });

        void draw_frame() const;
        void clear_frame() const;
        void present_frame() const;

        void add_item(std::shared_ptr<Primitive2D> item) const;
        void add_item(std::shared_ptr<Primitive2D> item, std::shared_ptr<Primitive2D> parent) const;
        void remove_item(std::shared_ptr<Primitive2D> item) const;
        void clear_items() const;

        int num_items() const;
        bool contains_item(std::shared_ptr<Primitive2D> item) const;

        void set_resolution(Vec2i new_resolution) const;
        void set_resolution(int width, int height) const;
        void set_viewport_scaling(Vec2d scaling);
        void set_viewport_scaling(double x, double y);
        void set_clear_color(Color4 color) const;
        Color4 get_clear_color() const;
        void set_blend_mode(RenderSurface::BlendMode mode);
        Matrix3x3d get_global_transform() const;

        void set_font_directory(const std::filesystem::path& path) const;
        void load_font_directory(const std::filesystem::path& path = "") const;

        void set_render_surface(std::shared_ptr<RenderSurface> new_surface);

        Vec2i get_resolution() const;
        Vec2d center() const;
        Vec2d get_aspect_ratio() const;
        Vec2d get_viewport_scaling() const;
        RenderSurface::BlendMode get_blend_mode() const;

        std::filesystem::path get_font_directory() const;
        bool is_font_loaded(const std::string& name) const;
        bool fonts_empty() const;
        std::shared_ptr<FontTTF> get_font(const std::string& name) const;

        std::shared_ptr<SceneGraph2D> get_scene_graph() const;
        std::shared_ptr<RenderSurface> get_render_surface() const;
        std::shared_ptr<FontManagerTTF> get_font_manager() const;

        static std::shared_ptr<Circle2D> create_circle(
            Vec2d position,
            double radius,
            Color4 color,
            double line_thickness = 1.0
        );

        static std::shared_ptr<Circle2D> create_circle(
            double x,
            double y,
            double radius,
            Color4 color,
            double line_thickness = 1.0
        );

        static std::shared_ptr<Ellipse2D> create_ellipse(
            Vec2d position,
            Vec2d radius,
            Color4 color,
            double line_thickness = 1.0
        );

        static std::shared_ptr<Ellipse2D> create_ellipse(
            double x,
            double y,
            double radius_x,
            double radius_y,
            Color4 color,
            double line_thickness = 1.0
        );

        static std::shared_ptr<Polyline2D> create_polyline(
            Vec2d position,
            const std::vector<Vec2d>& points,
            Color4 color,
            double line_thickness = 1.0
        );

        static std::shared_ptr<Polyline2D> create_polyline(
            double x,
            double y,
            const std::vector<Vec2d>& points,
            Color4 color,
            double line_thickness = 1.0
        );

        static std::shared_ptr<Polygon2D> create_polygon(
            Vec2d position,
            const std::vector<Vec2d>& points,
            Color4 fill_color
        );

        static std::shared_ptr<Polygon2D> create_polygon(
            double x,
            double y,
            const std::vector<Vec2d>& points,
            Color4 fill_color
        );

        static std::shared_ptr<Text2D> create_text(
            Vec2d position,
            const std::string& text,
            std::shared_ptr<FontTTF> font,
            double font_size,
            Color4 color
        );

        static std::shared_ptr<Text2D> create_text(
            double x,
            double y,
            const std::string& text,
            std::shared_ptr<FontTTF> font,
            double font_size,
            Color4 color
        );

    private:

        std::vector<std::pair<std::shared_ptr<Primitive2D>, Matrix3x3d>> get_draw_queue() const;

        std::shared_ptr<RenderSurface> _surface;
        std::shared_ptr<SceneGraph2D> _scene_graph;
        std::shared_ptr<FontManagerTTF> _font_manager;

        std::vector<std::pair<std::shared_ptr<Primitive2D>, Matrix3x3d>> _draw_queue;

        RenderSurface::BlendMode _blend_mode = RenderSurface::BlendMode::ALPHA;

        mutable double _last_frame_time_us = 0.0;

        Vec2d _viewport_scaling;
    };
}
