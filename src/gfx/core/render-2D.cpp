#include <gfx/core/render-2D.h>
#include <gfx/utils/transform.h>

namespace gfx::core
{

using namespace gfx::core::types;
using namespace gfx::primitives;
using namespace gfx::math;


void Render2D::draw_frame() const
{
    surface->clear_frame_buffer();

    double t { std::chrono::duration<double, std::micro>(
        std::chrono::high_resolution_clock::now().time_since_epoch()
    ).count() };

    if (debug_viewer->is_enabled())
    {
        debug_viewer->populate({
            scene_graph->get_global_transforms(),
            1000000 / (t - last_frame_time_us),
            scene_graph->num_items(),
            get_resolution()
        });
    }

    last_frame_time_us = t;

    std::vector<std::pair<std::shared_ptr<Primitive2D>, Matrix3x3d>> draw_queue { 
        get_draw_queue() 
    };

    for (const auto& [primitive, transform] : draw_queue)
    {
        if (!primitive->is_visible())
        {
            continue;
        }

        std::function<void(const Pixel&)> emit_pixel = primitive->get_use_shader() ?
            std::function([this, primitive, t](const Pixel &pixel) {
                Vec2d uv { primitive->get_uv(pixel.position) };
                ShaderInput2D input { uv, t / 1000000.0 };
                Color4 shaded_color { primitive->get_shader()->frag(input) };
                surface->write_pixel(pixel.position, shaded_color);
            }) :
            std::function([this](const Pixel &pixel) {
                surface->write_pixel(pixel.position, pixel.color);
            });

        primitive->rasterize(transform, emit_pixel);
    }

    surface->clear();
    surface->present();
}

std::vector<std::pair<std::shared_ptr<Primitive2D>, Matrix3x3d>> Render2D::get_draw_queue() const
{
    scene_graph->set_root_transform(get_global_transform());
    auto queue { scene_graph->get_draw_queue() };

    if (debug_viewer->is_enabled())
    {
        auto debug_items { debug_viewer->get_debug_items() };
        for (const auto &item : debug_items)
        {
            queue.emplace_back(std::make_pair(item, item->get_transform()));
        }
    }

    return queue;
}

gfx::math::Matrix3x3d Render2D::get_global_transform() const
{
    Matrix3x3d scale { utils::scale(viewport_scaling) };
    return scale;
}

void Render2D::add_debug_items() const
{
}

std::shared_ptr<Circle2D> Render2D::create_circle(const Vec2d position, const double radius, const Color4 color, const double line_thickness) const
{
    auto circle { std::make_shared<Circle2D>() };

    circle->set_position(position);
    circle->set_radius(radius);
    circle->set_line_thickness(line_thickness);
    circle->set_color(color);

    return circle;
}

std::shared_ptr<Ellipse2D> Render2D::create_ellipse(const Vec2d position, const Vec2d radius, const Color4 color, const double line_thickness) const
{
    auto ellipse { std::make_shared<Ellipse2D>() };

    ellipse->set_position(position);
    ellipse->set_radius(radius);
    ellipse->set_line_thickness(line_thickness);
    ellipse->set_color(color);

    return ellipse;
}

std::shared_ptr<Polyline2D> Render2D::create_polyline(const Vec2d position, const std::vector<Vec2d> &points, const Color4 color, const double line_thickness) const
{
    auto polyline { std::make_shared<Polyline2D>() };

    polyline->set_position(position);
    polyline->set_points(points);
    polyline->set_line_thickness(line_thickness);
    polyline->set_color(color);

    return polyline;
}

std::shared_ptr<Polygon2D> Render2D::create_polygon(const Vec2d position, const std::vector<Vec2d> &points, const Color4 color) const
{
    auto polygon { std::make_shared<Polygon2D>() };

    polygon->set_position(position);
    polygon->set_vertices(points);
    polygon->set_color(color);

    return polygon;
}

std::shared_ptr<Bitmap2D> Render2D::create_bitmap(const Vec2d position, const gfx::core::types::Bitmap &bm) const
{
    auto bitmap { std::make_shared<Bitmap2D>() };

    bitmap->set_position(position);
    bitmap->load_bitmap(bm);

    return bitmap;
}

std::shared_ptr<Bitmap2D> Render2D::create_bitmap(const Vec2d position, const Vec2i resolution) const
{
    auto bitmap { std::make_shared<Bitmap2D>() };

    bitmap->set_position(position);
    bitmap->set_resolution(resolution);

    return bitmap;
}

std::shared_ptr<Text2D> Render2D::create_text(const Vec2d position, const std::string &text, const std::shared_ptr<gfx::text::FontTTF> font, const double font_size, const Color4 color) const
{
    auto text_primitive { std::make_shared<Text2D>() };

    text_primitive->set_position(position);
    text_primitive->set_font(font);
    text_primitive->set_text(text);
    text_primitive->set_font_size(font_size);
    text_primitive->set_color(color);

    return text_primitive;
}

bool Render2D::collides(const Vec2d point, const std::shared_ptr<Primitive2D> primitive) const
{
    Matrix3x3d global_transform { get_global_transform() * primitive->get_transform() };
    return primitive->point_collides(point, global_transform);
}


}
