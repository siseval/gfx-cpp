#include "common/animations/space/body.h"
#include "common/animations/space/units.h"

namespace demos
{
    void Body::update_position(const double dt)
    {
        position += velocity * dt;
    }

    void Body::apply_gravity(const std::shared_ptr<Body> other, const double dt)
    {
        gfx::Vec2d direction { other->position - position };
        const double distance { std::max(direction.length(), get_radius() + other->get_radius()) };
        if (distance <= 0.0)
        {
            return;
        }

        direction = direction.normalize();

        const double force_magnitude { units::G * mass * other->mass / (distance * distance) };
        const gfx::Vec2d force { direction * force_magnitude };

        const gfx::Vec2d acceleration { force / mass };
        velocity += acceleration * dt;
    }
}
