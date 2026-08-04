#pragma once

#include <gfx/core/render-2D.h>
#include <gfx/core/types/color4.h>
#include <gfx/math/vec2.h>
#include <gfx/primitives/ellipse-2D.h>

#include "common/core/demo-utils.h"

namespace demos
{
    class Particle
    {
    public:

        Particle(
            std::shared_ptr<gfx::Render2D> renderer,
            const gfx::Vec2d position,
            const gfx::Vec2d velocity,
            const gfx::Vec2d size,
            const std::vector<gfx::Color4>& colors,
            const double lifespan
        )
            : renderer(renderer),
              size(size),
              position(position),
              velocity(velocity),
              colors(colors),
              lifespan_ms(lifespan)
        {
            shape = renderer->create_ellipse(position, size, colors[0]);
            shape->set_filled(true);
            renderer->add_item(shape);
            creation_time_ms = time_ms();
        }

        void process(double dt);
        bool done = false;

    private:

        void update_position(double dt);
        void apply_gravity(double dt);

        std::shared_ptr<gfx::Render2D> renderer;
        std::shared_ptr<gfx::Ellipse2D> shape;
        gfx::Vec2d size;
        gfx::Vec2d position;
        gfx::Vec2d velocity;
        std::vector<gfx::Color4> colors;

        double lifespan_ms = 0.0;
        double creation_time_ms = 0.0;
    };
}
