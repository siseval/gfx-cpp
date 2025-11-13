#ifndef GFX_DEMO_H
#define GFX_DEMO_H

#include <string>
#include <gfx/core/render-2D.h>

namespace demos::common::core
{

enum class MouseEventType
{
    LEFT_DOWN,
    LEFT_UP,
    RIGHT_DOWN,
    RIGHT_UP,
    SCROLL_UP,
    SCROLL_DOWN,
    MOVE,
};

struct MouseEvent
{
    MouseEventType type;
    gfx::math::Vec2d position;
};

class GfxDemo
{

public:

    GfxDemo(const std::shared_ptr<gfx::core::Render2D> renderer)
        : renderer(renderer) {}

    virtual void init() = 0;
    virtual void render_frame(const double dt) = 0;
    virtual void handle_input(const int input) = 0;
    virtual void report_mouse(const MouseEvent event) {}
    virtual void end() = 0;

    virtual std::vector<std::string> debug_text() { return {}; }

    inline std::shared_ptr<gfx::core::Render2D> get_renderer() const { return renderer; }
    inline gfx::math::Vec2i get_resolution() const { return renderer->get_resolution(); }

    inline double get_fps() const 
    { 
        return 1000000 / last_frame_us; 
    }

    inline std::vector<std::string> info_text()
    {
        std::vector<std::string> info;
        info.push_back("resolution: " + std::to_string(renderer->get_resolution().round().x) + "x" + std::to_string(renderer->get_resolution().round().y));
        info.push_back("fps: " + std::to_string(static_cast<int>(get_fps())));
        info.push_back("items: " + std::to_string(renderer->num_items()));

        return info;
    }

    inline void set_last_frame_us(const double us) { last_frame_us = us; }
    inline virtual gfx::core::types::Color4 get_clear_color() const { return gfx::core::types::Color4(0.2, 0.2, 0.2); }

protected:


    std::shared_ptr<gfx::core::Render2D> renderer;
    double last_frame_us = 0.0;
};

}

#endif // GFX_DEMO_H
