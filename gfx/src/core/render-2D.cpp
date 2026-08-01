#include "gfx/core/render-2D.h"
#include "gfx/geometry/transform-2D.h"

namespace gfx
{
    Render2D::Render2D(std::shared_ptr<RenderSurface> surface, Vec2d viewport_scaling)
        : _surface(surface)
        , _scene_graph(std::make_shared<SceneGraph2D>())
        , _font_manager(std::make_shared<FontManagerTTF>())
        , _viewport_scaling(viewport_scaling) {}

    void Render2D::draw_frame() const
    {
        const double t {
            std::chrono::duration<double, std::micro>(
                std::chrono::high_resolution_clock::now().time_since_epoch()
            ).count()
        };

        _last_frame_time_us = t;

        std::vector draw_queue { get_draw_queue() };

        for (const auto& [primitive, transform] : draw_queue)
        {
            if (!primitive->is_visible())
            {
                continue;
            }

            Primitive2D::RasterizeOutput output { primitive->rasterize(transform) };

            ShaderInput2D input { .uv = std::vector<Vec2d>(), .t = t / 1000000.0, .color = primitive->get_color() };

            for (const auto& pixel_pos : output.pixels)
            {
                input.uv.push_back(primitive->get_uv(static_cast<Vec2d>(pixel_pos)));
            }

            std::vector colors { primitive->get_shader()->frag(input) };

            if (output.use_alphas)
            {
                for (size_t i = 0; i < colors.size(); ++i)
                {
                    colors[i].a = static_cast<uint8_t>(std::clamp(
                        static_cast<int>(colors[i].a * output.alphas[i]),
                        0,
                        255
                    ));
                }
            }

            for (size_t i = 0; i < output.pixels.size(); ++i)
            
                _surface->write_pixel(
                    output.pixels[i],
                    colors[i],
                    primitive->get_depth(),
                    RenderSurface::BlendMode::ALPHA
                );
        }
    }

    void Render2D::clear_frame() const
    {
        _surface->clear_frame_buffer();
        _surface->clear_screen();
    }

    void Render2D::present_frame() const
    {
        _surface->present();
    }

    void Render2D::add_item(const std::shared_ptr<Primitive2D> item) const
    {
        _scene_graph->add_item(item);
    }

    void Render2D::add_item(const std::shared_ptr<Primitive2D> item, const std::shared_ptr<Primitive2D> parent) const
    {
        _scene_graph->add_item(item, parent);
    }

    void Render2D::remove_item(const std::shared_ptr<Primitive2D> item) const
    {
        _scene_graph->remove_item(item);
    }

    void Render2D::clear_items() const
    {
        _scene_graph->clear();
    }

    int Render2D::num_items() const
    {
        return _scene_graph->num_items();
    }

    bool Render2D::contains_item(const std::shared_ptr<Primitive2D> item) const
    {
        return _scene_graph->contains_item(item);
    }

    void Render2D::set_resolution(const Vec2i new_resolution) const
    {
        _surface->resize(new_resolution);
    }

    void Render2D::set_resolution(const int width, const int height) const
    {
        _surface->resize(Vec2i { width, height });
    }

    void Render2D::set_viewport_scaling(const Vec2d scaling)
    {
        _viewport_scaling = scaling;
    }

    void Render2D::set_viewport_scaling(const double x, const double y)
    {
        _viewport_scaling = Vec2d { x, y };
    }

    void Render2D::set_clear_color(const Color4 color) const
    {
        _surface->set_clear_color(color);
    }

    Color4 Render2D::get_clear_color() const
    {
        return _surface->get_clear_color();
    }

    void Render2D::set_blend_mode(const RenderSurface::BlendMode mode)
    {
        _blend_mode = mode;
    }

    Matrix3x3d Render2D::get_global_transform() const
    {
        const Matrix3x3d scale { Transform2D::scale(_viewport_scaling) };
        return scale;
    }

    void Render2D::set_font_directory(const std::filesystem::path& path) const
    {
        _font_manager->set_font_directory_path(path);
    }

    void Render2D::load_font_directory(const std::filesystem::path& path) const
    {
        _font_manager->load_font_directory(path);
    }

    void Render2D::set_render_surface(const std::shared_ptr<RenderSurface> new_surface)
    {
        _surface = new_surface;
    }

    Vec2i Render2D::get_resolution() const
    {
        return _surface->get_resolution() / get_viewport_scaling();
    }

    Vec2d Render2D::center() const
    {
        return static_cast<Vec2d>(get_resolution() / 2);
    }

    Vec2d Render2D::get_aspect_ratio() const
    {
        return Vec2d { static_cast<double>(get_resolution().x) / get_resolution().y, 1.0 };
    }

    Vec2d Render2D::get_viewport_scaling() const
    {
        return _viewport_scaling;
    }

    RenderSurface::BlendMode Render2D::get_blend_mode() const
    {
        return _blend_mode;
    }

    std::filesystem::path Render2D::get_font_directory() const
    {
        return _font_manager->get_font_directory_path();
    }

    bool Render2D::is_font_loaded(const std::string& name) const
    {
        return _font_manager->is_font_loaded(name);
    }

    bool Render2D::fonts_empty() const
    {
        return _font_manager->get_loaded_fonts().empty();
    }

    std::shared_ptr<FontTTF> Render2D::get_font(const std::string& name) const
    {
        return _font_manager->get_font(name);
    }

    std::shared_ptr<SceneGraph2D> Render2D::get_scene_graph() const
    {
        return _scene_graph;
    }

    std::shared_ptr<RenderSurface> Render2D::get_render_surface() const
    {
        return _surface;
    }

    std::shared_ptr<FontManagerTTF> Render2D::get_font_manager() const
    {
        return _font_manager;
    }

    std::shared_ptr<Circle2D> Render2D::create_circle(
        const Vec2d position,
        const double radius,
        const Color4 color,
        const double line_thickness
    )
    {
        auto circle { std::make_shared<Circle2D>() };

        circle->set_position(position);
        circle->set_radius(radius);
        circle->set_line_thickness(line_thickness);
        circle->set_color(color);

        return circle;
    }


    std::shared_ptr<Circle2D> Render2D::create_circle(
        const double x,
        const double y,
        const double radius,
        const Color4 color,
        const double line_thickness
    )
    {
        return create_circle(Vec2d { x, y }, radius, color, line_thickness);
    }

    std::shared_ptr<Ellipse2D> Render2D::create_ellipse(
        const Vec2d position,
        const Vec2d radius,
        const Color4 color,
        const double line_thickness
    )
    {
        auto ellipse { std::make_shared<Ellipse2D>() };

        ellipse->set_position(position);
        ellipse->set_radius(radius);
        ellipse->set_line_thickness(line_thickness);
        ellipse->set_color(color);

        return ellipse;
    }


    std::shared_ptr<Ellipse2D> Render2D::create_ellipse(
        const double x,
        const double y,
        const double radius_x,
        const double radius_y,
        const Color4 color,
        const double line_thickness
    )
    {
        return create_ellipse(Vec2d { x, y }, Vec2d { radius_x, radius_y }, color, line_thickness);
    }

    std::shared_ptr<Polyline2D> Render2D::create_polyline(
        const Vec2d position,
        const std::vector<Vec2d>& points,
        const Color4 color,
        const double line_thickness
    )
    {
        auto polyline { std::make_shared<Polyline2D>() };

        polyline->set_position(position);
        polyline->set_points(points);
        polyline->set_line_thickness(line_thickness);
        polyline->set_color(color);

        return polyline;
    }

    std::shared_ptr<Polyline2D> Render2D::create_polyline(
        const double x,
        const double y,
        const std::vector<Vec2d>& points,
        const Color4 color,
        const double line_thickness
    )
    {
        return create_polyline(Vec2d { x, y }, points, color, line_thickness);
    }

    std::shared_ptr<Polygon2D> Render2D::create_polygon(
        const Vec2d position,
        const std::vector<Vec2d>& points,
        const Color4 color
    )
    {
        auto polygon { std::make_shared<Polygon2D>() };

        polygon->set_position(position);
        polygon->set_vertices(points);
        polygon->set_color(color);

        return polygon;
    }

    std::shared_ptr<Polygon2D> Render2D::create_polygon(
        const double x,
        const double y,
        const std::vector<Vec2d>& points,
        const Color4 fill_color
    )
    {
        return create_polygon(Vec2d { x, y }, points, fill_color);
    }

    std::vector<std::pair<std::shared_ptr<Primitive2D>, Matrix3x3d>> Render2D::get_draw_queue() const
    {
        _scene_graph->set_root_transform(get_global_transform());
        auto queue { _scene_graph->get_draw_queue() };

        return queue;
    }


    // std::shared_ptr<Bitmap2D> Render2D::create_bitmap(const Vec2d position, const Bitmap &bm) const
    // {
    //     auto bitmap { std::make_shared<Bitmap2D>() };
    //
    //     bitmap->set_position(position);
    //     bitmap->load_bitmap(bm);
    //
    //     return bitmap;
    // }
    //
    // std::shared_ptr<Bitmap2D> Render2D::create_bitmap(const Vec2d position, const Vec2i resolution) const
    // {
    //     auto bitmap { std::make_shared<Bitmap2D>() };
    //
    //     bitmap->set_position(position);
    //     bitmap->set_resolution(resolution);
    //
    //     return bitmap;
    // }

    std::shared_ptr<Text2D> Render2D::create_text(
        const Vec2d position,
        const std::string& text,
        const std::shared_ptr<FontTTF> font,
        const double font_size,
        const Color4 color
    )
    {
        auto text_primitive { std::make_shared<Text2D>() };

        text_primitive->set_position(position);
        text_primitive->set_font(font);
        text_primitive->set_text(text);
        text_primitive->set_font_size(font_size);
        text_primitive->set_color(color);

        return text_primitive;
    }

    std::shared_ptr<Text2D> Render2D::create_text(
        const double x,
        const double y,
        const std::string& text,
        const std::shared_ptr<FontTTF> font,
        const double font_size,
        const Color4 color
    )
    {
        return create_text(Vec2d { x, y }, text, font, font_size, color);
    }
}
