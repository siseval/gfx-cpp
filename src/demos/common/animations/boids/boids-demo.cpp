#include <demos/common/animations/boids/boids-demo.h>
#include <demos/common/core/demo-utils.h>

namespace demos::common::animations::boids
{

using namespace gfx::core;
using namespace gfx::core::types;
using namespace gfx::primitives;
using namespace gfx::math;
using namespace demos::common::core;


void BoidsDemo::init()
{
    Vec2i resolution { renderer->get_resolution() };

    renderer->clear_items();
    renderer->get_render_surface()->clear_palette();
    boids.clear();

    perception_radius = resolution.x * 0.05;
    max_speed = resolution.x * 0.25;
    max_force = max_speed * 0.2;
    desired_separation = perception_radius * 0.5;

    mouse_influence_radius = perception_radius * 2.0;

    for (int i = 0; i < num_boids; ++i)
    {
        spawn_boid();
    }

    for (int i = 0; i < static_cast<int>(max_speed); i++)
    {
        double t { i / max_speed };
        boid_palette.emplace_back(Color4::lerp(slow_color, fast_color, t));
    }
}

void BoidsDemo::spawn_boid(const Vec2d position, const Vec2d velocity)
{
    std::vector<Vec2d> boid_shape {
        { 0, -5 },
        { 10, 0 },
        { 0, 5 },
        { 2, 0 },
    };
    auto boid_primitive { renderer->create_polyline(position, boid_shape, Color4(1.0, 1.0, 1.0, 1.0), 1.0) };
    std::shared_ptr<Boid> boid { std::make_shared<Boid>() };
    boid->velocity = velocity;
    boid->position = position;
    boid->shape = boid_primitive;
    boid->shape->set_anchor({ 0.2, 0.5 });
    boid->shape->set_fill(true);
    boid->shape->set_scale(renderer->get_resolution().x * 0.001 * boid_scale);
    renderer->add_item(boid_primitive);
    boids.push_back(boid);
}

void BoidsDemo::spawn_boid()
{
    Vec2d resolution { get_resolution() };
    Vec2d position {
        utils::random_double(0.0, resolution.x),
        utils::random_double(0.0, resolution.y)
    };
    Vec2d velocity {
        utils::random_double(-50.0, 50.0),
        utils::random_double(-50.0, 50.0)
    };
    spawn_boid(position, velocity);
}

void BoidsDemo::remove_boid(const std::shared_ptr<Boid> boid)
{
    renderer->remove_item(boid->shape);
    boids.erase(std::remove(boids.begin(), boids.end(), boid), boids.end());
}

std::vector<std::shared_ptr<Boid>> BoidsDemo::get_neighbors(const std::shared_ptr<Boid> boid)
{
    std::vector<std::shared_ptr<Boid>> neighbors;
    for (auto &other_boid : boids)
    {
        if (other_boid == boid)
        {
            continue;
        }
        double distance { Vec2d::distance(boid->position, other_boid->position) };
        if (distance < perception_radius)
        {
            neighbors.push_back(other_boid);
        }
    }
    return neighbors;
}

Vec2d BoidsDemo::alignment(const std::shared_ptr<Boid> boid, const std::vector<std::shared_ptr<Boid>> &neighbors)
{
    if (neighbors.empty())
    {
        return Vec2d::zero();
    }

    Vec2d avg_velocity { 0.0, 0.0 };
    for (auto &neighbor : neighbors)
    {
        avg_velocity += neighbor->velocity;
    }
    avg_velocity /= static_cast<double>(neighbors.size());
    avg_velocity = avg_velocity.normalize() * max_speed;

    Vec2d steering { (avg_velocity - boid->velocity).limit(max_force) };
    return steering;
}

Vec2d BoidsDemo::cohesion(const std::shared_ptr<Boid> boid, const std::vector<std::shared_ptr<Boid>> &neighbors)
{
    if (neighbors.empty())
    {
        return Vec2d::zero();
    }

    Vec2d center_of_mass { 0.0, 0.0 };
    for (auto &neighbor : neighbors)
    {
        center_of_mass += neighbor->position;
    }
    center_of_mass /= static_cast<double>(neighbors.size());

    Vec2d desired { center_of_mass - boid->position };
    Vec2d steering { desired.normalize() * max_speed - boid->velocity };
    return steering.limit(max_force);
}

Vec2d BoidsDemo::separation(const std::shared_ptr<Boid> boid, const std::vector<std::shared_ptr<Boid>> &neighbors)
{
    if (neighbors.empty())
    {
        return Vec2d::zero();
    }

    Vec2d steering { 0.0, 0.0 };
    int total { 0 };
    for (auto &neighbor : neighbors)
    {
        double distance { Vec2d::distance(boid->position, neighbor->position) };
        if (distance < desired_separation && distance > 0)
        {
            Vec2d diff { boid->position - neighbor->position };
            diff = diff.normalize() / distance;
            steering += diff;
            total++;
        }
    }
    if (total > 0)
    {
        steering /= static_cast<double>(total);
        steering = steering.normalize() * max_speed - boid->velocity;
        return steering.limit(max_force);
    }
    return Vec2d::zero();
}

void BoidsDemo::wrap_position(const std::shared_ptr<Boid> boid)
{
    Vec2d resolution { get_resolution() };

    if (boid->position.x < 0) 
    {
        boid->position.x += resolution.x;
    }
    else if (boid->position.x > resolution.x) 
    {
        boid->position.x -= resolution.x;
    }

    if (boid->position.y < 0) 
    {
        boid->position.y += resolution.y;
    }
    else if (boid->position.y > resolution.y) 
    {
        boid->position.y -= resolution.y;
    }
}

Vec2d BoidsDemo::centering(const std::shared_ptr<Boid> boid)
{
    Vec2d resolution { get_resolution() };
    Vec2d bounds_margin { 
        resolution.x * bounds_margin_fraction, 
        resolution.y * bounds_margin_fraction 
    };

    if (boid->position.x < bounds_margin.x || boid->position.x > resolution.x - bounds_margin.x
        || boid->position.y < bounds_margin.y || boid->position.y > resolution.y - bounds_margin.y)
    {
        Vec2d center { resolution.x / 2.0, resolution.y / 2.0 };
        Vec2d desired { center - boid->position };
        Vec2d steering { desired.normalize() * max_speed - boid->velocity };
        return steering.limit(max_force);
    }
    return Vec2d::zero();
}

Vec2d BoidsDemo::mouse_avoidance(const std::shared_ptr<Boid> boid)
{
    double distance { Vec2d::distance(boid->position, mouse_position) };
    if (distance < mouse_influence_radius && distance > 0)
    {
        Vec2d diff { boid->position - mouse_position };
        diff = diff.normalize() / distance;
        Vec2d desired { diff.normalize() * max_speed };
        Vec2d steering { desired - boid->velocity };
        return steering.limit(max_force);
    }
    return Vec2d::zero();
}

void BoidsDemo::apply_behaviors(const std::shared_ptr<Boid> boid, const std::vector<std::shared_ptr<Boid>> &neighbors, const double dt)
{
    Vec2d separate = separation(boid, neighbors) * separation_weight;
    Vec2d align = alignment(boid, neighbors) * alignment_weight;
    Vec2d cohere = cohesion(boid, neighbors) * cohesion_weight;
    Vec2d center = centering(boid) * centering_weight;
    Vec2d mouse_avoid { mouse_active ? mouse_avoidance(boid) * mouse_avoid_weigth : Vec2d::zero() };

    boid->velocity += (align + cohere + separate + center + mouse_avoid) * dt;

    // wrap_position(boid);
}

void BoidsDemo::process_boid(const std::shared_ptr<Boid> boid, const double dt)
{
    boid->position += boid->velocity * dt;
}

void BoidsDemo::process_boids(const double dt)
{
    for (auto &boid : boids)
    {
        std::vector<std::shared_ptr<Boid>> neighbors { get_neighbors(boid) };
        apply_behaviors(boid, neighbors, dt);
        process_boid(boid, dt);
    }
}

void BoidsDemo::render_boids()
{
    for (auto &boid : boids)
    {
        boid->shape->set_position(boid->position);
        double angle { std::atan2(boid->velocity.y, boid->velocity.x) };
        boid->shape->set_rotation(angle);
        int color_index { static_cast<int>(boid->velocity.length()) };
        boid->shape->set_color(boid_palette[std::clamp(color_index, 0, static_cast<int>(boid_palette.size()) - 1)]);
    }
}

void BoidsDemo::render_frame(const double dt)
{
    process_boids(dt);
    render_boids();

    renderer->draw_frame();
}

void BoidsDemo::report_mouse(const demos::common::core::MouseEvent event)
{
    if (event.type == demos::common::core::MouseEventType::MOVE)
    {
        Vec2i resolution { renderer->get_resolution() };
        mouse_position = Vec2d {
            event.position.x * static_cast<double>(resolution.x),
            event.position.y * static_cast<double>(resolution.y)
        };
    }
    else if (event.type == demos::common::core::MouseEventType::LEFT_DOWN)
    {
        mouse_active = !mouse_active;
    }
}

void BoidsDemo::handle_input(const int input)
{
}

void BoidsDemo::end()
{
}

}
