#pragma once

#include "gfx/core/render-2D.h"

#include <common/core/gfx-demo.h>

namespace demos
{
    struct Ripple
    {
        gfx::Vec2d center;
        double start_time;
    };

    class WaterSurfaceShader : public gfx::Shader2D
    {
    public:

        gfx::Vec2d mouse_uv;
        std::vector<std::shared_ptr<Ripple>> ripples;
        double ripple_lifetime_sec = 5.0;
        gfx::Color4 base_color = gfx::Color4(0.0, 0.3, 0.5);

        mutable gfx::Color4 prev_color;

        std::vector<gfx::Color4> frag(const gfx::ShaderInput2D& input) const override;
    };


    class ShaderDemo : public GfxDemo
    {
    public:

        ShaderDemo(const std::shared_ptr<gfx::RenderEngine> renderer, const std::shared_ptr<gfx::DebugViewer> debug_viewer = nullptr)
            : GfxDemo(renderer, debug_viewer)
        {
            render2D = renderer->get_render_2D();
        }

        void init() override;
        void render_frame(double dt) override;
        void end() override;
        void handle_char(int input) override;
        void report_mouse(MouseEvent event) override;

        std::vector<std::string> debug_text() override
        {
            const gfx::Vec2d mouse_uv { quad->get_uv(mouse_position) };
            const WaterSurfaceShader* water_shader = shader.get();
            return {
                "uv: (" + std::to_string(mouse_uv.x) + ", " + std::to_string(mouse_uv.y) + ")"
                "\nnum_ripples: " + std::to_string(water_shader->ripples.size()) +
                "\nprev_color: " + std::to_string(water_shader->prev_color.r_double()) + ", " + std::to_string(
                    water_shader->prev_color.g_double()
                ) + ", " + std::to_string(water_shader->prev_color.b_double())
            };
        }

    private:

        void update_primitives(double dt);
        void wrap_position(gfx::Vec2d& position);
        void spawn_ripple(gfx::Vec2d position) const;
        void update_ripples(double dt);
        gfx::Vec2d get_random_position() const;

        std::shared_ptr<gfx::Render2D> render2D;

        std::shared_ptr<WaterSurfaceShader> shader;

        std::shared_ptr<gfx::Polygon2D> quad;

        gfx::Vec2d mouse_position;
        double spawn_timer;
        double spawn_interval_sec = 0.2;
        bool random_spawn = false;
        double random_spawn_interval_sec = 0.2;
        double random_spawn_timer = 0.0;
    };
}
