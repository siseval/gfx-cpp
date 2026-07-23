#include "common/animations/star/star-demo.h"
#include "common/core/demo-utils.h"

namespace demos
{
    using namespace gfx;

    void StarDemo::init()
    {
        const Vec2d resolution { get_resolution() };
        const Vec2d center { resolution / 2 };
        constexpr double distance_ratio = 16;

        render2D->clear_items();
        polylines.clear();
        colors.clear();

        for (int i = 0; i < num_colors; ++i)
        {
            const double progress { static_cast<double>(i) / num_colors };

            Color4 inner { 1.0, 0.8, 0.1 };
            Color4 outer { 0.9, 0.5, 1.0 };

            Color4 color { Color4::lerp(inner, outer, progress) };
            colors.push_back(color);
        }

        for (int i = 0; i < num_polylines; ++i)
        {
            const double progress { static_cast<double>(i) / num_polylines };
            const int color_index { static_cast<int>(progress * (colors.size() - 1)) };

            const Color4 color { colors[color_index] };

            std::vector<Vec2d> points {
                { 0, -resolution.y / distance_ratio },
                { resolution.x / distance_ratio, 0 },
                { 0, resolution.y / distance_ratio },
                { -resolution.x / distance_ratio, 0 },
            };

            auto polyline { render2D->create_polyline(center, points, color) };
            polyline->set_close(true);
            polyline->set_fill(true);
            polyline->set_rounded_corners(true);
            polyline->set_anchor({ 0.5, 0.5 });
            polyline->set_depth(i);

            polylines.push_back(polyline);
            render2D->add_item(polyline);
        }
    }

    void StarDemo::render_frame(const double dt)
    {
        if (!paused)
        {
            time += dt * speed;
        }
        const double sin_t { std::sin(time) };
        const double cos_t { std::cos(time * 2) };

        for (int i = 0; i < num_polylines; ++i)
        {
            const double progress { static_cast<double>(i) / num_polylines };
            const auto polyline { polylines[i] };

            polyline->set_scale({ 1.0 + progress * (0.8 + sin_t * 0.4), 1.0 + progress * (1 + cos_t * 0.8) });
            polyline->set_rotation(time * (1.0 + progress));
            polyline->set_line_thickness(1 + progress * 1 + (1 + sin_t) * 1);
        }

        render2D->clear_frame();
        render2D->draw_frame();
        render2D->present_frame();
    }

    void StarDemo::end()
    {
        render2D->clear_items();
        polylines.clear();
    }

    void StarDemo::handle_char(const int input)
    {
        switch (input)
        {
        case 'k':
            num_polylines += 1;
            init();
            break;

        case 'j':
            if (num_polylines > 1)
            {
                num_polylines -= 1;
                init();
            }
            break;

        case 'l':
            speed += 0.1;
            break;

        case 'h':
            speed = std::max(0.1, speed - 0.1);
            break;

        case ' ':
            paused = !paused;
            break;

        default:
            break;
        }
    }
}
