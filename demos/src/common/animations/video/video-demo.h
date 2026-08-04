// #pragma once
//
// #include <gfx/core/render-2D.h>
//
// #include "common/core/gfx-demo.h"
//
// namespace demos
// {
//
// class VideoDemo : public demos::GfxDemo
// {
//
// enum class Video
// {
//     P5,
//     BAD_APPLE
// };
//
// public:
//
//     VideoDemo(const std::shared_ptr<gfx::RenderEngine> renderer)
//         : GfxDemo(renderer) 
//     {
//         render2D = renderer->get_render_2D();
//     }
//
//     void init() override;
//     void render_frame(const double dt) override;
//     void end() override;
//     void handle_char(const int input) override;
//     void report_mouse(const demos::MouseEvent event) override {}
//
//     std::vector<std::string> debug_text() override
//     {
//         return { 
//             "frame: " + std::to_string(frame_number),
//             "pos: [" + std::to_string(bitmap->get_position().x) + ", " + std::to_string(bitmap->get_position().y) + "]",
//         };
//     }
//
// private:
//
//     void cycle_video(const int direction);
//     void load_video(const Video video);
//     void load_bad_apple();
//     void load_p5();
//
//     std::shared_ptr<gfx::Render2D> render2D;
//
//     std::vector<gfx::Color4> palette;
//     std::vector<Video> videos { Video::BAD_APPLE, Video::P5 };
//     int current_video { 0 };
//     std::string video_name;
//
//     std::shared_ptr<gfx::Bitmap2D> bitmap { render2D->create_bitmap({ 0, 0 }, { 800, 600 }) };
//     double time_since_last_frame { 0.0 };
//     int frame_number { 1 };
//     bool paused { false };
//
// };
//
// }
