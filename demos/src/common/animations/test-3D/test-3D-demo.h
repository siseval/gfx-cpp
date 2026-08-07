#pragma once

#include "common/core/gfx-demo.h"
#include "gfx/primitives/cone-3D.h"
#include "gfx/primitives/cuboid-3D.h"
#include "gfx/primitives/plane-3D.h"
#include "gfx/primitives/polygon-3D.h"
#include "gfx/primitives/sphere-3D.h"

#include <map>

#include "gfx/primitives/text-2D.h"
#include "gfx/projections/perspective-projection.h"
#include "gfx/text/font-manager-ttf.h"

namespace demos
{
    class Test3DDemo : public GfxDemo<gfx::Vec3d>
    {
    public:

        explicit Test3DDemo(
            const std::shared_ptr<gfx::RenderLayer<gfx::Vec3d>> renderer,
            const std::shared_ptr<gfx::RenderSurface> surface
        )
            : GfxDemo(renderer, surface) {}

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

        struct Ripple
        {
            gfx::Vec2d center;
            double start_time;
        };

        class WaterSurfaceShader : public gfx::FragmentShader
        {
        public:

            gfx::Vec2d mouse_uv;
            std::vector<std::shared_ptr<Ripple>> ripples;
            double ripple_lifetime_sec = 5.0;
            gfx::Color4 base_color = gfx::Color4(0.0, 0.3, 0.5);

            mutable gfx::Color4 prev_color;

            gfx::Color4 frag(const Input& input, const Uniforms& uniforms) const override;
        };

    private:

        void update_camera(double dt);
        void camera_movement(Key key, double dt);
        void poll_held_keys(double dt);

        std::shared_ptr<gfx::Cuboid3D> cube;
        std::shared_ptr<gfx::Sphere3D> sphere;
        std::shared_ptr<gfx::Plane3D> plane;
        std::shared_ptr<gfx::Cone3D> cone;
        std::shared_ptr<gfx::Polygon3D> teapot;
        
        std::shared_ptr<gfx::FontManagerTTF> font_manager;
        std::shared_ptr<gfx::RenderLayer<gfx::Vec2d>> render2D;
        std::shared_ptr<gfx::Text2D> text_item;

        void spawn_ripple(gfx::Vec2d position) const;
        gfx::Vec2d get_random_position() const;
        void update_ripples(double dt) const;

        std::shared_ptr<WaterSurfaceShader> water_shader;
        std::shared_ptr<gfx::Plane3D> quad;
        bool random_spawn = true;
        double random_spawn_interval_sec = 0.2;
        double random_spawn_timer = 0.0;
        
        double smoothed_fps { 0.0 };

        gfx::View<gfx::Vec3d> view;
        gfx::PerspectiveProjection projection;

        std::vector<std::shared_ptr<gfx::Primitive<gfx::Vec3d>>> scene_items;

        // std::shared_ptr<gfx::Ellipse2D> crosshair;

        gfx::Vec2d prev_mouse_pos { 0.0, 0.0 };

        gfx::Vec3d camera_velocity { 0.0, 0.0, 0.0 };
        double camera_acceleration = 500.0;
        double camera_damping = 0.85;
        double max_camera_speed = 30.0;

        double last_frame_ms = 0.0;

        std::map<Key, bool> key_states {
            { Key::W, false },
            { Key::A, false },
            { Key::S, false },
            { Key::D, false },
            { Key::SPACE, false },
            { Key::CTRL, false },
        };
    };
}
