#ifndef RENDER_2D_H
#define RENDER_2D_H

#include <iostream>
#include <gfx/core/scene-graph-2D.h>
#include <gfx/core/render-surface.h>
#include <gfx/core/types/color4.h>
#include <gfx/core/types/bitmap.h>
#include <gfx/primitives/circle-2D.h>
#include <gfx/primitives/ellipse-2D.h>
#include <gfx/primitives/polyline-2D.h>
#include <gfx/primitives/polygon-2D.h>
#include <gfx/primitives/text-2D.h>
#include <gfx/primitives/bitmap-2D.h>
#include <gfx/text/font-manager-ttf.h>
#include <gfx/debug/debug-viewer.h>

namespace gfx::core
{

class Render2D
{

public:

    Render2D(std::shared_ptr<RenderSurface> surface, gfx::math::Vec2d viewport_scaling = gfx::math::Vec2d { 1, 1 }, std::string default_font_path = "") : 
        surface(surface), 
        scene_graph(std::make_shared<SceneGraph2D>()), 
        font_manager(std::make_shared<gfx::text::FontManagerTTF>()),
        debug_viewer(std::make_shared<gfx::debug::DebugViewer>()),
        viewport_scaling(viewport_scaling) 
    {
        if (!default_font_path.empty())
        {
            try
            {
                default_font = font_manager->load_from_file(default_font_path);
            }
            catch (const std::exception &e)
            {
                std::cerr << "Render2D: Failed to load default font from '" << default_font_path << "': " << e.what() << std::endl;
            }
        }
        set_debug_font(default_font);
        surface->init();
    }

    void draw_frame() const;

    gfx::math::Matrix3x3d get_global_transform() const;

    std::shared_ptr<gfx::primitives::Circle2D> create_circle(const gfx::math::Vec2d position, const double radius, const types::Color4 color, const double line_thickness = 1.0) const;
    std::shared_ptr<gfx::primitives::Circle2D> create_circle(const double x, const double y, const double radius, const types::Color4 color, const double line_thickness = 1.0) const 
    {
        return create_circle(gfx::math::Vec2d { x, y }, radius, color, line_thickness);
    };

    std::shared_ptr<gfx::primitives::Ellipse2D> create_ellipse(const gfx::math::Vec2d position, const gfx::math::Vec2d radius, const types::Color4 color, const double line_thickness = 1.0) const;
    std::shared_ptr<gfx::primitives::Ellipse2D> create_ellipse(const double x, const double y, const double radius_x, const double radius_y, const types::Color4 color, const double line_thickness = 1.0) const 
    {
        return create_ellipse(gfx::math::Vec2d { x, y }, gfx::math::Vec2d { radius_x, radius_y }, color, line_thickness);
    };

    std::shared_ptr<gfx::primitives::Polyline2D> create_polyline(const gfx::math::Vec2d position, const std::vector<gfx::math::Vec2d> &points, const types::Color4 color, const double line_thickness = 1.0) const;
    std::shared_ptr<gfx::primitives::Polyline2D> create_polyline(const double x, const double y, const std::vector<gfx::math::Vec2d> &points, const types::Color4 color, const double line_thickness = 1.0) const 
    {
        return create_polyline(gfx::math::Vec2d { x, y }, points, color, line_thickness);
    };

    std::shared_ptr<gfx::primitives::Polygon2D> create_polygon(const gfx::math::Vec2d position, const std::vector<gfx::math::Vec2d> &points, const types::Color4 fill_color) const;
    std::shared_ptr<gfx::primitives::Polygon2D> create_polygon(const double x, const double y, const std::vector<gfx::math::Vec2d> &points, const types::Color4 fill_color) const 
    {
        return create_polygon(gfx::math::Vec2d { x, y }, points, fill_color);
    };

    std::shared_ptr<gfx::primitives::Bitmap2D> create_bitmap(const gfx::math::Vec2d position, const types::Bitmap &bm) const;
    std::shared_ptr<gfx::primitives::Bitmap2D> create_bitmap(const double x, const double y, const types::Bitmap &bm) const 
    {
        return create_bitmap(gfx::math::Vec2d { x, y }, bm);
    };

    std::shared_ptr<gfx::primitives::Bitmap2D> create_bitmap(const gfx::math::Vec2d position, const gfx::math::Vec2i resolution) const;
    std::shared_ptr<gfx::primitives::Bitmap2D> create_bitmap(const double x, const double y, const gfx::math::Vec2i resolution) const 
    {
        return create_bitmap(gfx::math::Vec2d { x, y }, resolution);
    };

    std::shared_ptr<gfx::primitives::Text2D> create_text(const gfx::math::Vec2d position, const std::string &text, const std::shared_ptr<gfx::text::FontTTF> font, const double font_size, const types::Color4 color) const;
    std::shared_ptr<gfx::primitives::Text2D> create_text(const double x, const double y, const std::string &text, const std::shared_ptr<gfx::text::FontTTF> font, const double font_size, const types::Color4 color) const 
    {
        return create_text(gfx::math::Vec2d { x, y }, text, font, font_size, color);
    };

    bool collides(const gfx::math::Vec2d point, const std::shared_ptr<Primitive2D>) const;
    bool collides(const double x, const double y, const std::shared_ptr<Primitive2D> primitive) const
    {
        return collides(gfx::math::Vec2d { x, y }, primitive);
    };

    inline void set_resolution(const gfx::math::Vec2i new_resolution) { surface->resize(new_resolution); }
    inline void set_resolution(const int width, const int height) { surface->resize(gfx::math::Vec2i { width, height }); }
    inline gfx::math::Vec2i get_resolution() const { return surface->get_resolution() / get_viewport_scaling(); }
    inline gfx::math::Vec2d center() const { return get_resolution() / 2; };
    inline gfx::math::Vec2d get_aspect_ratio() const { return gfx::math::Vec2d { static_cast<double>(get_resolution().x) / get_resolution().y, 1.0 }; }

    inline std::shared_ptr<SceneGraph2D> get_scene_graph() const { return scene_graph; };
    inline std::shared_ptr<RenderSurface> get_render_surface() const { return surface; };
    inline std::shared_ptr<gfx::text::FontManagerTTF> get_font_manager() const { return font_manager; };

    inline void set_enable_debug_viewer(const bool enable) { debug_viewer->set_enabled(enable); }
    inline bool get_enable_debug_viewer() const { return debug_viewer->is_enabled(); }

    inline void set_debug_viewer_show_aabb(const bool show) { debug_viewer->set_show_aabb(show); }
    inline bool get_debug_viewer_show_aabb() const { return debug_viewer->get_show_aabb(); }

    inline void set_debug_viewer_show_obb(const bool show) { debug_viewer->set_show_obb(show); }
    inline bool get_debug_viewer_show_obb() const { return debug_viewer->get_show_obb(); }

    inline void set_debug_viewer_show_anchor(const bool show) { debug_viewer->set_show_anchor(show); }
    inline bool get_debug_viewer_show_anchor() const { return debug_viewer->get_show_anchor(); }

    inline void set_debug_font(const std::shared_ptr<gfx::text::FontTTF> font) { debug_viewer->set_font(font); }

    void add_debug_items() const;

    inline void add_item(const std::shared_ptr<Primitive2D> item) { scene_graph->add_item(item); }
    inline void add_item(const std::shared_ptr<Primitive2D> item, const std::shared_ptr<Primitive2D> parent) { scene_graph->add_item(item, parent); }
    inline void remove_item(const std::shared_ptr<Primitive2D> item) { scene_graph->remove_item(item); }
    inline void clear_items() { scene_graph->clear(); }

    inline int num_items() const { return scene_graph->num_items(); }
    inline bool contains_item(const std::shared_ptr<Primitive2D> item) const { return scene_graph->contains_item(item); }

    inline gfx::math::Vec2d get_viewport_scaling() const { return viewport_scaling; }
    inline void set_viewport_scaling(const gfx::math::Vec2d scaling) { viewport_scaling = scaling; }
    inline void set_viewport_scaling(const double x, const double y) { viewport_scaling = gfx::math::Vec2d { x, y }; }

    inline void set_clear_color(const types::Color4 color) { surface->set_clear_color(color); }
    inline types::Color4 get_clear_color() const { return surface->get_clear_color(); }

    inline int get_transform_recalculation_count() { return scene_graph->get_transform_recalculation_count(); }

    inline void set_font_directory(const std::filesystem::path &path) { font_manager->set_font_directory_path(path); }
    inline std::filesystem::path get_font_directory() const { return font_manager->get_font_directory_path(); }

    inline bool is_font_loaded(const std::string &name) const { return font_manager->is_font_loaded(name); }
    inline bool fonts_empty() const { return font_manager->get_loaded_fonts().empty(); }

    inline void load_font_directory(const std::filesystem::path &path = "") { font_manager->load_font_directory(path); }

    inline std::shared_ptr<gfx::text::FontTTF> get_font(const std::string &name) const { return font_manager->get_font(name); }

private:

    std::vector<std::pair<std::shared_ptr<Primitive2D>, gfx::math::Matrix3x3d>> get_draw_queue() const;

    std::shared_ptr<RenderSurface> surface;
    std::shared_ptr<SceneGraph2D> scene_graph;
    std::shared_ptr<gfx::text::FontManagerTTF> font_manager;
    std::shared_ptr<gfx::debug::DebugViewer> debug_viewer;

    std::shared_ptr<gfx::text::FontTTF> default_font;

    std::vector<std::pair<std::shared_ptr<Primitive2D>, gfx::math::Matrix3x3d>> draw_queue;

    mutable double last_frame_time_us = 0.0;

    math::Vec2d viewport_scaling;
};

}

#endif // RENDER_2D_H
