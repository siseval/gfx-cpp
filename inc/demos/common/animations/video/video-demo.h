#ifndef VIDEO_DEMO_H
#define VIDEO_DEMO_H

#include <gfx/core/render-2D.h>
#include <demos/common/core/gfx-demo.h>

namespace demos::common::animations::video
{

class VideoDemo : public demos::common::core::GfxDemo
{

enum class Video
{
    P5,
    BAD_APPLE
};

public:

    VideoDemo(const std::shared_ptr<gfx::core::Render2D> renderer)
        : GfxDemo(renderer) {}

    void init() override;
    void render_frame(const double dt) override;
    void end() override;
    void handle_input(const int input) override;
    void report_mouse(const demos::common::core::MouseEvent event) override {}

    std::vector<std::string> debug_text() override
    {
        return { 
            "frame: " + std::to_string(frame_number),
            "pos: [" + std::to_string(bitmap->get_position().x) + ", " + std::to_string(bitmap->get_position().y) + "]",
        };
    }

private:

    void cycle_video(const int direction);
    void load_video(const Video video);
    void load_bad_apple();
    void load_p5();

    std::vector<gfx::core::types::Color4> palette;
    std::vector<Video> videos { Video::BAD_APPLE, Video::P5 };
    int current_video { 0 };
    std::string video_name;

    std::shared_ptr<gfx::primitives::Bitmap2D> bitmap { renderer->create_bitmap({ 0, 0 }, { 800, 600 }) };
    double time_since_last_frame { 0.0 };
    int frame_number { 1 };
    bool paused { false };

};

}

#endif // VIDEO_DEMO_H
