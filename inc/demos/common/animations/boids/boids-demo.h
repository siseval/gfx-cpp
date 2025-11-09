#ifndef BOIDS_DEMO_H
#define BOIDS_DEMO_H

#include <gfx/core/render-2D.h>
#include <demos/common/core/gfx-demo.h>

namespace demos::common::animations::boids
{

struct Boid
{
    gfx::math::Vec2d position;
    gfx::math::Vec2d velocity;

    std::shared_ptr<gfx::primitives::Polyline2D> shape;
};

class BoidsDemo : public demos::common::core::GfxDemo
{

public:

    BoidsDemo(const std::shared_ptr<gfx::core::Render2D> renderer)
        : GfxDemo(renderer) {}

    void init() override;
    void render_frame(const double dt) override;
    void end() override;
    void handle_input(const int input) override;
    void report_mouse(const demos::common::core::MouseEvent event) override;

    std::vector<std::string> debug_text() override
    {
        return { 
            "num boids: " + std::to_string(boids.size()),
            "pos: " + (boids.size() > 0 ? "[" + std::to_string(boids[0]->position.x) + ", " + std::to_string(boids[0]->position.y) + "]" : "N/A"),
        };
    }

private: 

    gfx::math::Vec2d alignment(const std::shared_ptr<Boid> boid, const std::vector<std::shared_ptr<Boid>> &neighbors);
    gfx::math::Vec2d cohesion(const std::shared_ptr<Boid> boid, const std::vector<std::shared_ptr<Boid>> &neighbors);
    gfx::math::Vec2d separation(const std::shared_ptr<Boid> boid, const std::vector<std::shared_ptr<Boid>> &neighbors);
    gfx::math::Vec2d centering(const std::shared_ptr<Boid> boid);
    gfx::math::Vec2d mouse_avoidance(const std::shared_ptr<Boid> boid);
    void apply_behaviors(const std::shared_ptr<Boid> boid, const std::vector<std::shared_ptr<Boid>> &neighbors, const double dt);

    void wrap_position(const std::shared_ptr<Boid> boid);

    std::vector<std::shared_ptr<Boid>> get_neighbors(const std::shared_ptr<Boid> boid);

    void process_boid(const std::shared_ptr<Boid> boid, const double dt);
    void process_boids(const double dt);
    void render_boids();

    void spawn_boid(const gfx::math::Vec2d position, const gfx::math::Vec2d velocity);
    void spawn_boid();
    void remove_boid(const std::shared_ptr<Boid> boid);

    gfx::math::Vec2d mouse_position;
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

    gfx::core::types::Color4 slow_color { 0.5, 0.8, 1.0 };
    gfx::core::types::Color4 fast_color { 1.0, 0.6, 0.6 };
    std::vector<gfx::core::types::Color4> boid_palette;
    int num_boid_colors = 120;
};

}

#endif // BOIDS_DEMO_H
