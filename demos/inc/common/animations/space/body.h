#pragma once

#include <memory>
#include <string>

#include <gfx/core/types/color4.h>
#include <gfx/math/vec2.h>


namespace demos
{
    class Body
    {
    public:

        Body(
            std::string name,
            const gfx::Vec2d position,
            const gfx::Vec2d velocity,
            const double radius,
            const double mass,
            const bool locked,
            const gfx::Color4 color
        )
            : name(name),
              position(position),
              velocity(velocity),
              radius(radius),
              mass(mass),
              locked(locked),
              color(color)
        {
        }

        void update_position(double dt);
        void apply_gravity(std::shared_ptr<Body> other, double dt);

        void set_name(const std::string n)
        {
            name = n;
        }

        std::string get_name() const
        {
            return name;
        }

        void set_position(const gfx::Vec2d pos)
        {
            position = pos;
        }

        gfx::Vec2d get_position() const
        {
            return position;
        }

        void set_velocity(const gfx::Vec2d vel)
        {
            velocity = vel;
        }

        gfx::Vec2d get_velocity() const
        {
            return velocity;
        }

        void set_radius(const double r)
        {
            radius = r;
        }

        double get_radius() const
        {
            return radius;
        }

        void set_mass(const double m)
        {
            mass = m;
        }

        double get_mass() const
        {
            return mass;
        }

        bool is_locked() const
        {
            return locked;
        }

        void set_color(const gfx::Color4 col)
        {
            color = col;
        }

        gfx::Color4 get_color() const
        {
            return color;
        }

    protected:

        std::string name;

        gfx::Vec2d position;
        gfx::Vec2d velocity;

        double radius;
        double mass;

        bool locked { false };

        gfx::Color4 color;
    };
}
