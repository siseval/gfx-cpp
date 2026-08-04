#pragma once

#include <gfx/math/vec2.h>

namespace demos
{
    class Camera
    {
    public:

        enum class State
        {
            Free,
            Transitioning,
            Tracking
        };

        void process(double dt);

        void handle_input(int input);

        void smooth_pan(gfx::Vec2d direction);
        void smooth_zoom(double factor);

        void do_transition(double dt);
        void do_track(double dt);
        void do_free(double dt);

        State state { State::Free };

        double pan_speed { 0.1 };
        double zoom_velocity { 0.0 };
        double track_time { 0.0 };

        gfx::Vec2d velocity { 0.0, 0.0 };
        gfx::Vec2d zoom_out_size { 0.0, 0.0 };

        gfx::Vec2d start_pos { 0.0, 0.0 };
        gfx::Vec2d size0 { 0.0, 0.0 };
        gfx::Vec2d end_pos { 0.0, 0.0 };
        gfx::Vec2d size1 { 0.0, 0.0 };

        gfx::Vec2d cur_pos { 0.0, 0.0 };
        gfx::Vec2d size_cur { 0.0, 0.0 };

        double track_duration { 2.5 };
    };
}
