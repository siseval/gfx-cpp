#include "common/animations/fireworks/particle.h"

namespace demos
{
    using namespace gfx;

    void Particle::process(const double dt)
    {
        shape->set_rotation_degrees(velocity.angle_degrees());

        const double t { (time_ms() - creation_time_ms) / lifespan_ms };
        if (colors.size() > 1)
        {
            shape->set_color(Color4::lerp(colors[0], colors[1], t));
        }

        if (t < 0.2)
        {
            shape->set_scale(Vec2d::lerp({ 0.001, 0.001 }, { 1, 1 }, t * 5));
        }
        else
        {
            const double t2 { (t - 0.2) * 1.2 };
            shape->set_scale(Vec2d::lerp({ 1, 1 }, { 0.001, 0.001 }, t2));
            Color4 transparent_color = shape->get_color();
            transparent_color.a = 0.0;
            shape->set_color(Color4::lerp(shape->get_color(), transparent_color, t2 * t2 * 0.02));
        }

        update_position(dt);
        apply_gravity(dt);

        if (time_ms() - creation_time_ms >= lifespan_ms)
        {
            renderer->remove_item(shape);
            done = true;
        }
    }

    void Particle::update_position(const double dt)
    {
        position += velocity * dt;
        shape->set_position(position);
    }

    void Particle::apply_gravity(const double dt)
    {
        constexpr double gravity = 5.0;
        velocity.y += gravity * dt;
    }
}
