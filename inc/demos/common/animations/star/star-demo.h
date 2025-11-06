#ifndef STAR_DEMO_H
#define STAR_DEMO_H

#include <gfx/core/render-2D.h>
#include <demos/common/core/gfx-demo.h>

namespace demos::common::animations::star
{

class StarDemo : public demos::common::core::GfxDemo
{

public:

    StarDemo(const std::shared_ptr<gfx::core::Render2D> renderer)
        : GfxDemo(renderer) {}

    void init() override;
    void render_frame(const double dt) override;
    void end() override;
    void handle_input(const int input) override;
    void report_mouse(const demos::common::core::MouseEvent event) override {}

private:

    int num_polylines = 32;
    std::vector<std::shared_ptr<gfx::primitives::Polyline2D>> polylines;
    std::vector<gfx::core::types::Color4> colors;
    int num_colors = 64;

    double speed = 2.0;
    double time = 0.0;
    bool paused = false;

};

}

#endif // STAR_DEMO_H
