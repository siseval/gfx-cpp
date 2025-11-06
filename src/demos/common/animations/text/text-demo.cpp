#include <gfx/text/font-manager-ttf.h>
#include <gfx/geometry/flatten.h>
#include <demos/common/animations/text/text-demo.h>
#include <demos/common/core/demo-utils.h>

namespace demos::common::animations::text
{

using namespace gfx::core::types;
using namespace gfx::math;
using namespace demos::common::core;



void TextDemo::init()
{
    renderer->clear_items();
    Vec2d center { renderer->center() };

    std::vector<Vec2d> poly_points {
        { 30, 30 }, 
        { 90, 30 }, 
        { 90, 90 }, 
        { 30, 90 }, 
    };
    std::vector<Vec2d> hole_points {
        { 50, 50 },
        { 70, 55 },
        { 70, 75 },
        { 50, 70 },
    };

    poly = renderer->create_polygon(center, poly_points, Color4(1.0));
    poly->add_hole_vertices(hole_points);
    renderer->add_item(poly);

    auto font_manager = renderer->get_font_manager();
    auto font = font_manager->load_from_file("/Users/sigurdsevaldrud/documents/code/c++/gfx/assets/fonts/ARIAL.ttf");

    if (!font)
    {
        throw std::runtime_error("Failed to load font.");
    }

    std::shared_ptr<gfx::text::GlyphTTF> glyph;
    // for (auto g : font->get_glyphs())
    // {
    //     if (!g.second->contours.empty())
    //     {
    //         glyph = g.second;
    //         break;
    //     }
    // }
    std::string text { "DOLBA" };
    for (int i = 0; i < text.length(); i++)
    {
        double kerning { 15.0 };
        Vec2d offset { -(text.length() * kerning / 2) + i * kerning, 0 };

        auto polygon = renderer->create_polygon(center + offset, {}, { 255, 255, 255 });
        polygon->set_rotation_degrees(180);
        // polyline->set_anchor({ 0.5, 0.5 });
        polygon->set_scale({ 0.015, 0.0035 });
        renderer->add_item(polygon);

        glyph = font->get_glyph(text[i]);
        int contour_index = 0;
        for (auto contour : glyph->contours)
        {
            std::vector<std::pair<Vec2d, bool>> cont;
            for (auto point : contour)
            {
                cont.push_back({ { point.x, point.y }, point.on_curve });
            }

            std::vector<Vec2d> points;
            for (auto point : gfx::geometry::flatten_contour(cont))
            {
                points.push_back({ point.x, point.y });
                num_points++;
            }

            if (is_clockwise(points))
            {
                polygon->add_hole_vertices(points, contour_index);
            }

            else 
            {
                polygon->add_vertices(points, contour_index);
                contour_index++;
            }
            
        }
    }
}

bool TextDemo::is_clockwise(std::vector<gfx::math::Vec2d> vertices)
{
    double sum = 0.0;
    for (int i = 0; i < vertices.size(); ++i)
    {
        Vec2d p0 { vertices[i] };
        Vec2d p1 { vertices[(i + 1) % vertices.size()] };
        sum += (p1.x - p0.x) * (p1.y + p0.y);
    }
    return sum < 0.0;
}

void TextDemo::render_frame(const double dt)
{
    double t0 { utils::time_us() };
    double time_ms { t0 / 1000.0 };

    renderer->draw_frame();

    last_frame_us = utils::time_us() - t0;
}

void TextDemo::handle_input(const int input)
{
}

void TextDemo::end()
{
    renderer->clear_items();
}

}
