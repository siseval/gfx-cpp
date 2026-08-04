#include "gfx/debug/debug-viewer.h"

#include <numeric>

namespace gfx
{
    void DebugViewer::update(std::shared_ptr<RenderEngine> render_engine)
    {
        clear(render_engine);
        if (!_enabled)
        {
            return;
        }
        populate(render_engine);
    }

    void DebugViewer::set_enabled(const bool enable)
    {
        _enabled = enable;
    }

    void DebugViewer::set_font(const std::shared_ptr<FontTTF> fnt)
    {
        _font = fnt;
    }

    void DebugViewer::set_font_size(const double size)
    {
        _font_size = size;
    }

    void DebugViewer::set_text_color(const Color4 color)
    {
        _text_color = color;
    }

    void DebugViewer::set_show_fps(const bool show)
    {
        _show_fps = show;
    }

    void DebugViewer::set_show_num_items(const bool show)
    {
        _show_num_items = show;
    }

    void DebugViewer::set_text_position(const Vec2d position)
    {
        _text_position = position;
    }

    void DebugViewer::add_debug_line(const std::string& line, const int line_number)
    {
        if (line_number >= 0 && line_number < static_cast<int>(_debug_lines.size()))
        {
            _debug_lines[line_number] = line;
        }
        else
        {
            _debug_lines.push_back(line);
        }
    }

    bool DebugViewer::get_enabled() const
    {
        return _enabled;
    }

    std::shared_ptr<FontTTF> DebugViewer::get_font() const
    {
        return _font;
    }

    double DebugViewer::get_font_size() const
    {
        return _font_size;
    }

    Color4 DebugViewer::get_text_color() const
    {
        return _text_color;
    }

    bool DebugViewer::get_show_fps() const
    {
        return _show_fps;
    }

    bool DebugViewer::get_show_num_items() const
    {
        return _show_num_items;
    }

    Vec2d DebugViewer::get_text_position() const
    {
        return _text_position;
    }

    void DebugViewer::populate(std::shared_ptr<RenderEngine> render_engine)
    {
        const RendererInfo info { gather_renderer_info(render_engine) };

        if (_font)
        {
            update_text(render_engine, info);
        }
    }

    void DebugViewer::clear(std::shared_ptr<RenderEngine> render_engine) const
    {
        if (_text&& render_engine 
        ->
        contains_primitive(_text)
        )
        {
            render_engine->remove_primitive(_text);
        }
    }

    DebugViewer::RendererInfo DebugViewer::gather_renderer_info(const std::shared_ptr<RenderEngine> render_engine)
    {
        const double current_time_ms {
            std::chrono::duration<double, std::milli>(
                std::chrono::high_resolution_clock::now().time_since_epoch()
            ).count()
        };

        _fps_history.push_back(current_time_ms - _last_frame_timestamp_ms);
        if (_fps_history.size() > _fps_history_max_size)
        {
            _fps_history.pop_front();
        }
        const double acc { std::accumulate(_fps_history.begin(), _fps_history.end(), 0.0) };
        const double fps { acc == 0.0 ? 0.0 : _fps_history.size() / (acc / 1000.0) };

        const RendererInfo info { .fps = fps, .num_items = render_engine->num_primitives() };

        _last_frame_timestamp_ms = current_time_ms;
        return info;
    }

    void DebugViewer::update_text(std::shared_ptr<RenderEngine> render_engine, const RendererInfo& info)
    {
        std::string debug_text;

        if (_show_fps)
        {
            debug_text += "fps: " + std::to_string(static_cast<int>(info.fps)) + "\n";
        }

        if (_show_num_items)
        {
            debug_text += "items: " + std::to_string(info.num_items) + "\n";
        }

        for (const auto& line : _debug_lines)
        {
            debug_text += line + "\n";
        }

        if (!_text)
        {
            _text = render_engine->create_text(_text_position, debug_text, _font, _font_size, _text_color);
            _text->set_smoothing_radius(0.7);
            _text->set_depth(-256);
        }
        else
        {
            _text->set_text(debug_text);
            _text->set_font(_font);
            _text->set_font_size(_font_size);
            _text->set_color(_text_color);
        }

        if (!render_engine->contains_primitive(_text))
        {
            render_engine->add_primitive(_text);
        }
    }

    int DebugViewer::num_debug_items() const
    {
        return _show_fps || _show_num_items;
    }
}
