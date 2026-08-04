#pragma once

#include <gfx/core/render-2D.h>

#include "common/core/gfx-demo.h"

namespace demos
{
    class PhysicsDemo : public GfxDemo
    {
    public:

        PhysicsDemo(const std::shared_ptr<gfx::RenderEngine> renderer)
            : GfxDemo(renderer)
        {
        }

        void init() override;
        void render_frame(double dt) override;
        void end() override;
        void handle_char(int input) override;

        void report_mouse(const MouseEvent event) override
        {
        }

    private:

        int num_polylines = 32;
        std::vector<std::shared_ptr<gfx::Polyline2D>> polylines;
    };
}
