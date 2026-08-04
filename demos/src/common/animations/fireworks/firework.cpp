#include "common/animations/fireworks/firework.h"
#include "common/core/demo-utils.h"

namespace demos
{
    using namespace gfx;

    void Firework::process(const double dt)
    {
        switch (state)
        {
        case State::Ascending:
            do_ascending(dt);
            break;

        case State::Exploding:
            do_exploding(dt);
            break;

        default:
            break;
        }
    }

    void Firework::explode()
    {
        state = State::Exploding;
        renderer->remove_item(shape);

        const int num_particles { random_int(max_particles / 2, max_particles) };
        for (int i = 0; i < num_particles; ++i)
        {
            const double angle { random_double(0, 360) };
            const double speed { random_double(particle_speed * 0.1, particle_speed * 1.25) };
            Vec2d velocity { Vec2d::from_angle_degrees(angle, speed) - Vec2d { 0, particle_speed / 10 } };

            const double size { random_double(particle_size * 0.5, particle_size * 1.5) };
            double lifespan { random_double(particle_lifespan_ms * 0.75, particle_lifespan_ms * 1.25) };

            std::vector color { colors[rand() % colors.size()] };
            particles.emplace_back(
                renderer,
                position,
                velocity,
                Vec2d { size * particle_x_factor, size / particle_x_factor },
                color,
                lifespan
            );
        }
    }

    void Firework::do_ascending(const double dt)
    {
        shape->set_rotation(velocity.angle());
        update_position(dt);
        apply_gravity(dt);

        do_smoke(dt);

        for (auto& particle : smoke_particles)
        {
            particle.process(dt);
        }

        if (velocity.y >= 0.0)
        {
            explode();
        }
    }

    void Firework::do_exploding(const double dt)
    {
        for (auto& particle : particles)
        {
            particle.process(dt);
        }
        for (auto& particle : smoke_particles)
        {
            particle.process(dt);
        }

        std::erase_if(
            particles,
            [](const Particle& p)
            {
                return p.done;
            }
        );

        std::erase_if(
            smoke_particles,
            [](const Particle& p)
            {
                return p.done;
            }
        );

        if (particles.empty() && smoke_particles.empty())
        {
            state = State::Done;
        }
    }

    void Firework::do_smoke(const double dt)
    {
        const double slowdown = inv_lerp(start_velocity.y, 0, velocity.y);
        const double interval = lerp(smoke_trail_interval_ms, smoke_trail_interval_ms * 3, slowdown);
        if (time_ms() - last_smoke_time_ms > interval)
        {
            const double angle {
                velocity.angle_degrees() + random_double(-smoke_angle_variation_degrees, smoke_angle_variation_degrees)
            };
            const double speed { random_double(smoke_speed * 0.75, smoke_speed * 1.25) };
            Vec2d velocity { Vec2d::from_angle_degrees(angle, -speed) };

            const double avg_size = lerp(smoke_size, smoke_size * 0.1, slowdown);
            double lifespan { random_double(particle_lifespan_ms, particle_lifespan_ms * 2.0) };

            smoke_particles.emplace_back(
                renderer,
                position + Vec2d { 0, avg_size },
                velocity,
                Vec2d { avg_size * smoke_x_factor, avg_size / smoke_x_factor },
                std::vector { smoke_color },
                lifespan
            );
            last_smoke_time_ms = time_ms();
        }
    }

    void Firework::update_position(const double dt)
    {
        position += velocity * dt;
        shape->set_position(position);
    }

    void Firework::apply_gravity(const double dt)
    {
        constexpr double gravity = 9.81;
        velocity.y += gravity * dt;
    }
}
