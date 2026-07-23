#pragma once

#include <gfx/core/render-2D.h>

#include "common/core/gfx-demo.h"
#include "gfx/core/render-engine.h"
#include "gfx/debug/debug-viewer.h"

namespace demos
{
    struct Boid
    {
        gfx::Vec2d position;
        gfx::Vec2d velocity;

        std::shared_ptr<gfx::Polyline2D> shape;
    };

    class BoidsDemo : public GfxDemo
    {
    public:

        BoidsDemo(const std::shared_ptr<gfx::RenderEngine> renderer, const std::shared_ptr<gfx::DebugViewer> debug_viewer = nullptr)
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
            return {
                "num boids: " + std::to_string(boids.size()),
                "pos: " + (boids.size() > 0
                               ? "[" + std::to_string(boids[0]->position.x) + ", " + std::to_string(boids[0]->position.y)
                               + "]"
                               : "N/A"),
            };
        }

    private:

        gfx::Vec2d alignment(
            std::shared_ptr<Boid> boid,
            const std::vector<std::shared_ptr<Boid>>& neighbors
        ) const;
        gfx::Vec2d cohesion(
            std::shared_ptr<Boid> boid,
            const std::vector<std::shared_ptr<Boid>>& neighbors
        ) const;
        gfx::Vec2d separation(
            std::shared_ptr<Boid> boid,
            const std::vector<std::shared_ptr<Boid>>& neighbors
        ) const;
        gfx::Vec2d centering(std::shared_ptr<Boid> boid) const;
        gfx::Vec2d mouse_avoidance(std::shared_ptr<Boid> boid) const;
        void apply_behaviors(
            std::shared_ptr<Boid> boid,
            const std::vector<std::shared_ptr<Boid>>& neighbors,
            double dt
        ) const;

        void wrap_position(std::shared_ptr<Boid> boid) const;

        std::vector<std::shared_ptr<Boid>> get_neighbors(std::shared_ptr<Boid> boid) const;

        static void process_boid(std::shared_ptr<Boid> boid, double dt);
        void process_boids(double dt);
        void render_boids() const;

        void spawn_boid(gfx::Vec2d position, gfx::Vec2d velocity);
        void spawn_boid();
        void remove_boid(std::shared_ptr<Boid> boid);

        std::shared_ptr<gfx::Render2D> render2D;

        gfx::Vec2d mouse_position;
        bool mouse_active = false;
        double mouse_influence_radius = 100.0;

        int num_boids = 500;
        std::vector<std::shared_ptr<Boid>> boids;

        double boid_scale = 0.8;

        double perception_radius;
        double max_speed;
        double max_force;
        double desired_separation;

        double separation_weight = 1.5;
        double alignment_weight = 1.0;
        double cohesion_weight = 1.0;
        double centering_weight = 2.0;
        double mouse_avoid_weigth = 10.0;

        double bounds_margin_fraction = 0.2;

        gfx::Color4 slow_color { 0.5, 0.8, 1.0 };
        gfx::Color4 fast_color { 1.0, 0.6, 0.6 };
        std::vector<gfx::Color4> boid_palette;
        int num_boid_colors = 120;
    };
}
