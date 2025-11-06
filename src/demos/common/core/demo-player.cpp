#include <demos/common/core/demo-player.h>
#include <demos/common/animations/star/star-demo.h>
#include <demos/common/animations/snake/snake-demo.h>
#include <demos/common/animations/fireworks/fireworks-demo.h>
#include <demos/common/animations/space/space-demo.h>
#include <demos/common/animations/video/video-demo.h>
#include <demos/common/animations/fractal/fractal-demo.h>
#include <demos/common/animations/boids/boids-demo.h>
#include <demos/common/animations/shader/shader-demo.h>
#include <demos/common/animations/text/text-demo.h>

namespace demos::common::core
{

using namespace gfx::core;
using namespace gfx::core::types;
using namespace gfx::primitives;
using namespace gfx::math;
using namespace demos::common::animations;
using namespace demos::common;

void DemoPlayer::init()
{
    demos.emplace_back(std::make_shared<text::TextDemo>(renderer));
    // demos.emplace_back(std::make_shared<video::VideoDemo>(renderer));
    demos.emplace_back(std::make_shared<star::StarDemo>(renderer));
    demos.emplace_back(std::make_shared<snake::SnakeDemo>(renderer));
    demos.emplace_back(std::make_shared<boids::BoidsDemo>(renderer));
    demos.emplace_back(std::make_shared<fractal::FractalDemo>(renderer));
    demos.emplace_back(std::make_shared<shader::ShaderDemo>(renderer));
    demos.emplace_back(std::make_shared<fireworks::FireworksDemo>(renderer));
    demos.emplace_back(std::make_shared<space::SpaceDemo>(renderer));

    resize(get_screen_size());
}

void DemoPlayer::run()
{
    double last_frame_timestamp_us { utils::time_ms() };
    while (running)
    {
        if (screen_size_changed())
        {
            resize(get_screen_size());
        }

        double now_us { utils::time_us() };
        double dt_sec { (now_us - last_frame_timestamp_us) / 1000000.0 };

        demos[current_demo]->render_frame(dt_sec);
        demos[current_demo]->set_last_frame_us(now_us - last_frame_timestamp_us);
        last_frame_timestamp_us = now_us;

        if (show_info)
        {
            draw_info();
        }

        int input { get_input() };
        handle_input(input);
        demos[current_demo]->handle_input(input);
    }
}

void DemoPlayer::resize(const gfx::math::Vec2i new_resolution)
{
    renderer->set_resolution(new_resolution);
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
        case 'N':
            cycle_demo(1);
            break;
        case 'P':
            cycle_demo(-1);
            break;
    }

}

std::vector<std::string> DemoPlayer::get_info()
{
    std::vector<std::string> info { demos[current_demo]->info_text() };

    if (show_debug)
    {
        info.push_back("");
        for (const auto &line : demos[current_demo]->debug_text())
        {
            info.push_back(line);
        }
    }

    info.push_back("");
    info.push_back("[p/n] to cycle (" + std::to_string(current_demo + 1) + "/" + std::to_string(demos.size()) + ")");
    info.push_back("[q] to quit");

    return info;
}

void DemoPlayer::cycle_demo(const int direction)
{
    demos[current_demo]->end();
    current_demo = (current_demo + direction + demos.size()) % demos.size();
    renderer->clear_items();
    demos[current_demo]->init();
}

}
