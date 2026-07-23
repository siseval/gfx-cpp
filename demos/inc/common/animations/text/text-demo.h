#pragma once

#include <gfx/core/render-2D.h>

#include "common/core/gfx-demo.h"

namespace demos
{
    class TextDemo : public GfxDemo
    {
    public:

        explicit TextDemo(const std::shared_ptr<gfx::RenderEngine> renderer, const std::shared_ptr<gfx::DebugViewer> debug_viewer = nullptr)
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

        std::vector<std::string> debug_text() override
        {
            return {
                "num points: " + std::to_string(num_points),
                // "holes: " + std::to_string(polygon->get_hole_vertices().size())
            };
        }

        gfx::Color4 get_clear_color() const override
        {
            return gfx::Color4(0.0, 0.0, 0.0, 1.0);
        }

    private:

        static bool is_clockwise(std::vector<gfx::Vec2d> vertices);

        std::shared_ptr<gfx::Render2D> render2D;

        std::shared_ptr<gfx::Text2D> text_item;
        int num_points = 0;
    };
}
