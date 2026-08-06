#include "common/core/demo-player.h"
// #include "common/animations/fireworks/fireworks-demo.h"
// #include "common/animations/snake/snake-demo.h"
// #include "common/animations/space/space-demo.h"
// #include "common/animations/star/star-demo.h"
#include "common/animations/test-3D/test-3D-demo.h"
#include "common/core/demo-utils.h"
// #include "common/animations/video/video-demo.h"
// #include "common/animations/fractal/fractal-demo.h"
// #include "common/animations/boids/boids-demo.h"
#include "common/animations/construct/construct-demo.h"
// #include "common/animations/shader/shader-demo.h"
// #include "common/animations/text/text-demo.h"

namespace demos
{
    using namespace gfx;

    void DemoPlayer::init()
    {
        // renderer->load_font_directory("/home/sisev/projects/code/cpp/sigfx/assets/fonts");

        // demos.emplace_back(std::make_shared<TextDemo>(renderer, debug_viewer));
        demos.emplace_back(std::make_shared<ConstructDemo>(renderer, surface));
        demos.emplace_back(std::make_shared<Test3DDemo>(renderer, surface));
        // demos.emplace_back(std::make_shared<StarDemo>(renderer, debug_viewer));
        // demos.emplace_back(std::make_shared<VideoDemo>(renderer, debug_viewer));
        // demos.emplace_back(std::make_shared<SnakeDemo>(renderer, debug_viewer));
        // demos.emplace_back(std::make_shared<BoidsDemo>(renderer, debug_viewer));
        // demos.emplace_back(std::make_shared<FractalDemo>(renderer, debug_viewer));
        // demos.emplace_back(std::make_shared<ShaderDemo>(renderer, debug_viewer));
        // demos.emplace_back(std::make_shared<FireworksDemo>(renderer, debug_viewer));
        // demos.emplace_back(std::make_shared<SpaceDemo>(renderer, debug_viewer));

        // debug_viewer->set_font(renderer->get_font("gohu-regular"));

        cycle_demo(0);
    }

    void DemoPlayer::run()
    {
        double last_frame_timestamp_us { time_us() };
        cycle_demo(1);
        cycle_demo(-1);

        while (running)
        {
            // if (screen_size_changed())
            // {
            //     resize(get_screen_size());
            //     demos[current_demo]->init();
            // }

            const double now_us { time_us() };
            const double dt_sec { (now_us - last_frame_timestamp_us) / 1000000.0 };

            demos[current_demo]->render_frame(dt_sec);
            // demos[current_demo]->set_last_frame_us(now_us - last_frame_timestamp_us);
            last_frame_timestamp_us = now_us;

            // debug_viewer->update(renderer);

            // if (show_info)
            // {
            //     draw_info();
            // }

            const int input { get_input() };
            handle_input(input);
            demos[current_demo]->handle_char(input);
        }
    }

    void DemoPlayer::resize(const Vec2i new_resolution) const
    {
        renderer->get_viewport().size = new_resolution;
        surface->set_resolution(new_resolution);
        // surface->resize(new_resolution);
        _previous_screen_size = new_resolution;
    }

    void DemoPlayer::cycle_demo(const int direction)
    {
        demos[current_demo]->end();
        current_demo = (current_demo + direction + demos.size()) % demos.size();
        renderer->get_scene_graph()->clear();

        if (demos[current_demo]->get_clear_color() != surface->get_clear_color())
        {
            surface->set_clear_color(demos[current_demo]->get_clear_color());
        }

        demos[current_demo]->init();
    }

    void DemoPlayer::handle_input(const int input)
    {
        switch (input)
        {
        case 'q':
            running = false;
            break;
        case 'U':
            show_info = !show_info;
            break;
        case '3':
            show_debug = !show_debug;
            break;
        case '4':
            // debug_viewer->set_enabled(!debug_viewer->get_enabled());
            break;
        // case '5':
        //     renderer->debug_viewer_show_aabb(!renderer->is_debug_viewer_showing_aabb());
        //     break;
        // case '6':
        //     renderer->debug_viewer_show_obb(!renderer->is_debug_viewer_showing_obb());
        //     break;
        // case '7':
        //     renderer->debug_viewer_show_anchor(!renderer->is_debug_viewer_showing_anchor());
        //     break;
        case 'N':
            cycle_demo(1);
            break;
        case 'P':
            cycle_demo(-1);
            break;
        default:
            break;
        }
    }

    std::vector<std::string> DemoPlayer::get_info() const
    {
        std::vector<std::string> info;// { demos[current_demo]->info_text() };

        if (show_debug)
        {
            info.push_back("");
            for (const auto& line : demos[current_demo]->debug_text())
            {
                info.push_back(line);
            }
        }

        info.push_back("");
        info.push_back(
            "[p/n] to cycle (" + std::to_string(current_demo + 1) + "/" + std::to_string(demos.size()) + ")"
        );
        info.push_back("[q] to quit");

        return info;
    }
}
