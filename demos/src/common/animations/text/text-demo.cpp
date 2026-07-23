#include "common/animations/text/text-demo.h"
#include "common/core/demo-utils.h"

namespace demos
{
    using namespace gfx;

    class TextShader final : public Shader2D
    {
        std::vector<Color4> frag(const ShaderInput2D& input) const override
        {
            std::vector<Color4> out;
            out.reserve(input.uv.size());

            for (const auto& uv : input.uv)
            {
                const double t { input.t };

                double diagonal { uv.x + uv.y + t };
                diagonal = std::fmod(diagonal, std::numbers::pi);

                double r { std::sin(diagonal) };
                double g { 1 - std::sin(diagonal) };
                double b = 0.5;

                out.emplace_back(r, g, b);
            }

            return out;
        }
    };


    void TextDemo::init()
    {
        render2D->clear_items();
        render2D->set_clear_color(Color4(0.2, 0.2, 0.2, 1.0));
        // renderer->set_clear_color(Color4::black());

        const Vec2d center { render2D->center() };

        if (render2D->fonts_empty())
        {
            render2D->load_font_directory("/Users/sigurdsevaldrud/documents/code/c++/sigfx/assets/fonts");
        }

        const auto font_1 { render2D->get_font("eva-classic") };
        auto font_2 { render2D->get_font("minecraft") };

        const Vec2d top_left { 10.0, center.y - 50.0 };

        const std::string top_text { "NEON\nGENESIS" };
        const auto top_item = render2D->create_text(
            top_left,
            top_text,
            font_1,
            14.0,
            Color4::white()
        );

        top_item->set_scale(1.5, 2.0);
        top_item->set_anchor(0.0, 0.5);
        top_item->set_line_height_multiplier(0.9);

        const double y_offset { 14.0 * top_item->get_scale().y * 2.0 * 0.9 };

        const std::string bottom_text { "EVANGELION" };
        const auto bottom_item = render2D->create_text(
            top_left + Vec2d { 0.0, y_offset },
            bottom_text,
            font_1,
            14.0,
            Color4::white()
        );
        bottom_item->set_scale(2.0, 3.0);
        bottom_item->set_anchor(0.0, 0.5);

        const double font_size = get_resolution().x * 0.05;

        text_item = render2D->create_text(
            center,
            ".abcdefgh\nijklmno\npqrstuvwxyz",
            font_2,
            font_size,
            Color4::white()
        );

        text_item->set_shader(std::make_shared<TextShader>());
        text_item->set_anchor(0.5, 0.5);
        text_item->set_alignment(Text2D::TextAlignment::CENTER);

        render2D->add_item(text_item);

        // renderer->add_item(top_item);
        // renderer->add_item(bottom_item);
    }

    void TextDemo::render_frame(const double dt)
    {
        const double t0 { time_us() };
        const double time_ms { t0 / 1000.0 };
        const double t { time_ms / 1000.0 };

        const double scale = 3.0 + 1.2 * std::sin(time_ms * 0.002);

        // Color4 color {
        //     0.5 + 0.5 * std::sin(t),
        //     0.5 + 0.5 * std::sin(t + 2.0),
        //     0.5 + 0.5 * std::sin(t + 4.0),
        //     1.0
        // };

        text_item->set_scale(scale);
        // text_item->set_color(color);
        text_item->set_rotation_degrees(std::sin(t * 3.0) * 5.0);

        render2D->clear_frame();
        render2D->draw_frame();
        render2D->present_frame();

        last_frame_us = time_us() - t0;
    }

    void TextDemo::end()
    {
        render2D->clear_items();
    }

    void TextDemo::handle_char(const int input)
    {
    }

    bool TextDemo::is_clockwise(std::vector<Vec2d> vertices)
    {
        double sum = 0.0;
        for (int i = 0; i < vertices.size(); ++i)
        {
            const Vec2d p0 { vertices[i] };
            const Vec2d p1 { vertices[(i + 1) % vertices.size()] };
            sum += (p1.x - p0.x) * (p1.y + p0.y);
        }
        return sum < 0.0;
    }
}
