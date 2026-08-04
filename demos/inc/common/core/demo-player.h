#pragma once

#include "common/core/gfx-demo.h"

namespace demos
{
    class DemoPlayer
    {
    public:

        virtual ~DemoPlayer() = default;
        
        void init();
        void run();

        void resize(gfx::Vec2i new_resolution) const;

        bool screen_size_changed()
        {
            return _previous_screen_size != get_screen_size();
        }

    protected:

        void cycle_demo(int direction);
        void handle_input(int input);
        std::vector<std::string> get_info() const;

        virtual gfx::Vec2i get_screen_size() = 0;

        virtual int get_input() = 0;
        virtual void draw_info() = 0;

        mutable gfx::Vec2i _previous_screen_size;
        
        std::shared_ptr<gfx::RenderLayer<gfx::Vec3d>> renderer;
        std::shared_ptr<gfx::RenderSurface> surface;
        
        std::vector<std::shared_ptr<GfxDemo<gfx::Vec3d>>> demos;
        int current_demo = 0;

        bool show_info = true;
        bool show_debug = true;

        bool running = true;
    };
}
