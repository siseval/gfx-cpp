#include "common/animations/shader/shader-demo.h"

#include "common/core/demo-utils.h"

namespace demos
{
    using namespace gfx;

    std::vector<Color4> WaterSurfaceShader::frag(const ShaderInput2D& input) const
    {
        std::vector<Color4> out;

        constexpr double wave_freq = 20.0;
        constexpr double wave_speed = 3.0;
        constexpr double dist_falloff = 4.0;
        constexpr double amp_scale = 0.3;
        constexpr double fade_decay = 7.0;
        constexpr double base_brightness = 2.0;

        for (const auto& uv : input.uv)
        {
            const double global_time_sec { time_ms() / 1000.0 };

            double ripple_amount = base_brightness;
            for (const auto& ripple : ripples)
            {
                const double distance = Vec2d::distance(uv, ripple->center);

                const double ripple_time_sec { global_time_sec - ripple->start_time / 1000.0 };
                const double t { ripple_time_sec / ripple_lifetime_sec };

                const double fade_in { smoothstep(std::clamp(t / 0.1, 0.0, 1.0)) };

                const double wave { std::sin(wave_freq * distance - wave_speed * ripple_time_sec) };
                const double falloff { std::exp(-dist_falloff * distance - fade_decay * t) };

                ripple_amount += wave * falloff * amp_scale * fade_in;
            }

            ripple_amount = inv_lerp(-0.05, 0.05, ripple_amount);
            ripple_amount = fmod(ripple_amount, 1.0);

            out.emplace_back(base_color + Color4(std::fmod(ripple_amount, 1.0)));
        }

        return out;
    }


    void ShaderDemo::init()
    {
        Vec2d resolution { get_resolution() };
        const Vec2d center { resolution / 2 };

        shader = std::make_shared<WaterSurfaceShader>();

        quad = render2D->create_polygon(
            center,
            {
                { 0, 0 },
                { resolution.x, 0 },
                { resolution.x, resolution.y },
                { 0, resolution.y }
            },
            Color4(255, 255, 255)
        );

        quad->set_anchor(0.5, 0.5);
        quad->set_shader(shader);

        render2D->clear_items();
        render2D->add_item(quad);
    }

    void ShaderDemo::render_frame(const double dt)
    {
        const double t0 { time_us() };

        update_ripples(dt);

        random_spawn_timer += dt;
        if (random_spawn&& random_spawn_timer 
        >
        random_spawn_interval_sec
        )
        {
            spawn_ripple(get_random_position());
            random_spawn_timer = 0.0;
        }

        shader->mouse_uv = quad->get_uv(mouse_position);

        const double t { t0 / 1000000.0 };

        shader->base_color = Color4(
            std::sin(t * 0.5) * 0.1 + 0.2,
            std::sin(t * 0.3 + 2.0) * 0.1 + 0.2,
            std::sin(t * 0.7 + 4.0) * 0.1 + 0.2
        );

        render2D->clear_frame();
        render2D->draw_frame();
        render2D->present_frame();
    }

    void ShaderDemo::end()
    {
        render2D->clear_items();
    }

    void ShaderDemo::handle_char(const int input)
    {
        switch (input)
        {
        case 'r':
            random_spawn = !random_spawn;
            break;
        case 'k':
            spawn_interval_sec = std::max(0.05, spawn_interval_sec - 0.05);
            break;
        case 'j':
            spawn_interval_sec += 0.05;
            break;
        default:
            break;
        }
    }

    void ShaderDemo::report_mouse(const MouseEvent event)
    {
        switch (event.type)
        {
        case MouseEventType::LEFT_DOWN:
            {
                spawn_ripple(event.position * render2D->get_resolution());
            }
            break;
        case MouseEventType::MOVE:
            {
                mouse_position = event.position * render2D->get_resolution();
                if (spawn_timer > spawn_interval_sec)
                {
                    spawn_ripple(event.position * render2D->get_resolution());
                    spawn_timer = 0.0;
                }
            }
            break;
        default:
            break;
        }
    }

    void ShaderDemo::spawn_ripple(const Vec2d position) const
    {
        const auto ripple = std::make_shared<Ripple>();
        ripple->center = quad->get_uv(position);
        ripple->start_time = time_ms();
        shader->ripples.push_back(ripple);
    }

    void ShaderDemo::update_ripples(const double dt)
    {
        spawn_timer += dt;
        std::vector<std::shared_ptr<Ripple>> to_remove;
        for (auto& ripple_center : shader->ripples)
        {
            if (time_ms() - ripple_center->start_time > shader->ripple_lifetime_sec * 1000.0)
            {
                to_remove.push_back(ripple_center);
            }
        }
        for (const auto& ripple : to_remove)
        {
            auto it = std::ranges::find(shader->ripples, ripple);
            if (it != shader->ripples.end())
            {
                shader->ripples.erase(it);
            }
        }
    }

    Vec2d ShaderDemo::get_random_position() const
    {
        const Vec2d resolution { get_resolution() };
        const double x = random_double(0.0, resolution.x);
        const double y = random_double(0.0, resolution.y);
        return Vec2d { x, y };
    }
}
