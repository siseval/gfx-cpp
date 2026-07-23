#pragma once

#include <gfx/core/render-2D.h>

#include "common/core/gfx-demo.h"

namespace demos
{
    class SnakeDemo : public GfxDemo
    {
    public:

        SnakeDemo(const std::shared_ptr<gfx::RenderEngine> renderer, const std::shared_ptr<gfx::DebugViewer> debug_viewer = nullptr)
            : GfxDemo(renderer, debug_viewer)
        {
            render2D = renderer->get_render_2D();
        }

        void init() override;
        void render_frame(double dt) override;
        void end() override;
        void handle_char(int input) override;

        void report_mouse(const MouseEvent event) override
        {
        }

        virtual std::vector<std::string> debug_text() override
        {
            return {
                "dt: " + std::to_string(last_frame_us / CLOCKS_PER_SEC),
            };
        }

    private:

        void move_target(double dt);
        void move_head(double dt);
        void move_segments() const;

        void spawn();
        void die();
        void do_dead(double dt);

        void remove_food();
        void add_food();
        gfx::Vec2d closest_food() const;

        void update_segments();
        void add_segment();
        void remove_segment();
        void update_scale(double s);

        std::shared_ptr<gfx::Render2D> render2D;

        bool control = false;
        bool dead = true;
        double explode_speed = 100;
        double dead_time = 0.0;

        bool first_init = true;

        int num_segments = 64;
        double scale = 1.0;

        double segment_length = 2.0;
        double segment_width = 1.0;
        double segment_spacing = 1.0;

        gfx::Vec2d head_target;
        gfx::Vec2d head_dir;
        gfx::Vec2d head_dimensions { 5, 2.5 };

        double speed = 30;
        double turn_speed = 5;

        double target_speed = 30;
        double target_turn_speed = 10;
        gfx::Vec2d target_direction;
        gfx::Vec2d target_bounds_margin;
        bool target_visible = false;

        double food_radius = 0.5;
        std::vector<std::shared_ptr<gfx::Circle2D>> food;

        std::shared_ptr<gfx::Polyline2D> tongue;
        std::shared_ptr<gfx::Polyline2D> head;
        std::vector<std::shared_ptr<gfx::Ellipse2D>> segments;
        std::shared_ptr<gfx::Circle2D> target_marker;

        std::vector<gfx::Color4> segment_palette;
        int num_colors = 120;
    };
}
