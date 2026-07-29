#pragma once

#include "common/core/gfx-demo.h"

#include "gfx/primitives/ellipse-2D.h"
#include "gfx/primitives/polygon-3D.h"

#include <map>

namespace demos
{
    class ConstructDemo : public GfxDemo
    {
    public:

        ConstructDemo(const std::shared_ptr<gfx::RenderEngine> renderer, const std::shared_ptr<gfx::DebugViewer> debug_viewer = nullptr)
            : GfxDemo(renderer, debug_viewer)
        {
        }

        void init() override;
        void render_frame(double dt) override;
        void end() override;
        void handle_char(int input) override;
        void report_key(KeyEvent event) override;
        void report_mouse(MouseEvent event) override;

        std::vector<std::string> debug_text() override
        {
            return {
            };
        }

    private:

        void update_camera(double dt);
        void set_camera_velocity(gfx::Vec3d velocity);
        void camera_movement(Key key, double dt);
        void poll_held_keys(double dt);

        std::vector<std::shared_ptr<gfx::Primitive3D>> scene_items;

        std::shared_ptr<gfx::Ellipse2D> crosshair;
        std::shared_ptr<gfx::Polygon3D> map;

        gfx::Vec2d prev_mouse_pos { 0.0, 0.0 };

        gfx::Camera camera;
        gfx::Vec3d camera_velocity { 0.0, 0.0, 0.0 };
        double camera_acceleration = 100.0;
        double camera_damping = 0.85;
        double max_camera_speed = 300.0;

        double last_frame_ms = 0.0;

        std::map<Key, bool> key_states {
            { Key::W, false },
            { Key::A, false },
            { Key::S, false },
            { Key::D, false },
            { Key::SPACE, false },
            { Key::CTRL, false },
            { Key::SHIFT, false },
        };
    };
}
