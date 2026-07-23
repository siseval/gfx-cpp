#include "common/animations/fireworks/fireworks-demo.h"
#include "common/core/demo-utils.h"

namespace demos
{
    using namespace gfx;

    void FireworksDemo::init()
    {
        const Vec2i resolution { get_resolution() };
        spawn_margins = static_cast<Vec2<double>>(resolution * 0.1);
        firework_speed = std::sqrt(9.81 * resolution.y);

        options.size = { resolution.x * 0.005, resolution.x * 0.0005 };

        options.max_particles = 70;
        options.particle_size = resolution.x * 0.001;
        options.particle_lifespan_ms = 2000.0;
        options.particle_speed = firework_speed * 0.6;

        options.smoke_size = resolution.x * 0.001;
        options.smoke_speed = firework_speed * 0.4;
        options.smoke_trail_interval_ms = 25;
    }

    void FireworksDemo::render_frame(const double dt)
    {
        const double t0 { time_us() };
        const double time_ms { t0 / 1000.0 };

        if (fireworks.size() < max_fireworks && (fireworks.empty() || time_ms - last_spawn_time_ms >=
            spawn_interval_ms))
        {
            spawn_firework();
            last_spawn_time_ms = time_ms;
        }

        std::vector<int> to_remove;
        for (int i = 0; i < fireworks.size(); ++i)
        {
            fireworks[i].process(dt);
            const auto& firework { fireworks[i] };
            if (firework.state == Firework::State::Done)
            {
                to_remove.push_back(i);
            }
        }
        for (const auto index : to_remove)
        {
            fireworks.erase(fireworks.begin() + index);
        }

        render2D->clear_frame();
        render2D->draw_frame();
        render2D->present_frame();
    }

    void FireworksDemo::end()
    {
        fireworks.clear();
        render2D->clear_items();
    }

    void FireworksDemo::handle_char(const int input)
    {
        switch (input)
        {
        case 'f':
            spawn_firework();
            break;
        case '+':
            max_fireworks += 1;
            break;
        case '-':
            if (max_fireworks > 1)
            {
                max_fireworks -= 1;
            }
            break;
        default:
            break;
        }
    }


    void FireworksDemo::spawn_firework()
    {
        Vec2d position {
            static_cast<double>(random_int(spawn_margins.x, get_resolution().x - spawn_margins.x)),
            static_cast<double>(get_resolution().y)
        };

        const double angle { random_double(-angle_variation, angle_variation) - 90 };
        Vec2d velocity {
            Vec2d::from_angle_degrees(angle, random_double(firework_speed * 0.75, firework_speed * 1.25))
        };

        std::vector colors { color_combinations[rand() % color_combinations.size()] };
        fireworks.emplace_back(render2D, position, velocity, options, colors);
    }
}
