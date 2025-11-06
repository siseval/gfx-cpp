#include <demos/common/animations/video/video-demo.h>
#include <demos/common/core/demo-utils.h>
#include <gfx/primitives/bitmap-2D.h>
#include <gfx/core/types/bitmap.h>

namespace demos::common::animations::video
{

using namespace gfx::math;
using namespace gfx::primitives;
using namespace gfx::core::types;
using namespace demos::common::core;

void VideoDemo::init()
{
    load_video(videos[current_video]);

    renderer->clear_items();
    Vec2i resolution { renderer->get_resolution() };
    Vec2d center { renderer->center() };

    auto bitmap { renderer->create_bitmap(center, { 800, 600 }) };
    bitmap->set_anchor({ 0.5, 0.5 });
    bitmap->set_scale(0.2);
}

void VideoDemo::render_frame(const double dt)
{
    double t0 { utils::time_us() };
    double time_ms { t0 / 1000.0 };

    double fps = 60.0;
    int num_frames = 6573;
    renderer->clear_items();

    if (!paused)
    {
        time_since_last_frame += dt;
    }
    if (time_since_last_frame >= (1.0 / fps))
    {
        frame_number++;
        time_since_last_frame = 0.0;
    }

    if (frame_number >= num_frames)
    {
        frame_number = 1;
    }

    Vec2i resolution { renderer->get_resolution() };
    Bitmap bm { Bitmap::decode_bmp("/Users/sigurdsevaldrud/documents/code/c++/gfx/assets/" + video_name + "/" + std::to_string(frame_number) + ".bmp") };

    bm.compress_colors(palette);
    bitmap->load_bitmap(bm);
    bitmap->set_scale(static_cast<double>(resolution.x) / static_cast<double>(bm.resolution.x));
    renderer->add_item(bitmap);

    renderer->draw_frame();
}

void VideoDemo::cycle_video(const int direction)
{
    current_video += direction;
    if (current_video < 0)
    {
        current_video = static_cast<int>(videos.size()) - 1;
    }
    else if (current_video >= static_cast<int>(videos.size()))
    {
        current_video = 0;
    }
    load_video(videos[current_video]);
}

void VideoDemo::load_video(const Video video)
{
    switch (video)
    {
        case Video::P5:
            load_p5();
            break;
        case Video::BAD_APPLE:
            load_bad_apple();
            break;
    }
}

void VideoDemo::load_bad_apple()
{
    video_name = "badapple";
    frame_number = 1;
    time_since_last_frame = 0.0;
    paused = false;
    palette.clear();
    Color4 black { 0, 0, 0 , 0 };
    Color4 white { 255, 255, 255 };
    int num_colors { 2 };
    for (int i = 0; i < num_colors; ++i)
    {
        double t = static_cast<double>(i) / static_cast<double>(num_colors - 1);
        palette.push_back(Color4::lerp(black, white, t));
    }
}

void VideoDemo::load_p5()
{
    video_name = "p5";
    frame_number = 1;
    time_since_last_frame = 0.0;
    paused = false;
    palette = {
        { 1.0, 0.0, 0.0 },
        { 1.0, 1.0, 1.0 },
        { 0.8, 0.8, 0.8 },
        { 0.4, 0.4, 0.4 },
        { 0.0, 0.0, 0.0 },
        { 0.6, 0.6, 0.6 },
    };
}

void VideoDemo::handle_input(const int input)
{
    switch (input)
    {
        case ' ':
            paused = !paused;
            break;
        case 'n':
            cycle_video(1);
            break;
        case 'p':
            cycle_video(-1);
            break;
    }
}

void VideoDemo::end()
{
    renderer->clear_items();
}

}
