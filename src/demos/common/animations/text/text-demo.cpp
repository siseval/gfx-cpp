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

    auto font_manager = renderer->get_font_manager();
    auto font = font_manager->load_from_file("/Users/sigurdsevaldrud/documents/code/c++/gfx/assets/fonts/comic-sans.ttf");

    if (!font)
    {
        throw std::runtime_error("Failed to load font.");
    }

    std::string text { "vro" };
    text_item = renderer->create_text(
        center,
        text,
        font,
        14.0,
        Color4(1.0, 0.0, 0.0, 1.0)
    );
    text_item->set_scale(3.0);
    text_item->set_anchor({ 0.5, 0.5 });
    renderer->add_item(text_item);
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

    double scale = 3.0 + 2.0 * std::sin(time_ms * 0.002);
    double rotation = std::fmod(time_ms * 0.05, 360.0);

    Color4 color {
        0.5 + 0.5 * std::sin(time_ms * 0.001),
        0.5 + 0.5 * std::sin(time_ms * 0.001 + 2.0),
        0.5 + 0.5 * std::sin(time_ms * 0.001 + 4.0),
        1.0
    };

    text_item->set_scale(scale);
    text_item->set_color(color);
    // text_item->set_rotation_degrees(rotation);

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
