#include "gfx/core/render-surface.h"

#include <bit>

namespace gfx
{
    RenderSurface::RenderSurface(const Vec2i resolution)
        : _resolution(resolution)
        , _frame_buffer(std::vector(resolution.x * resolution.y, 0))
        , _depth_buffer(std::vector(resolution.x * resolution.y, std::numeric_limits<double>::infinity())) {}

    void RenderSurface::write_pixel(const Vec2i viewport_offset, const Vec2i viewport_pixel, const Color4 color, const double depth, const BlendMode blend_mode)
    {
        const Vec2i pos { viewport_offset + viewport_pixel };
        
        if (pos.x < 0 || pos.y < 0 || pos.x >= _resolution.x || pos.y >= _resolution.y)
        {
            return;
        }

        const int index = pos.y * _resolution.x + pos.x;
        if (depth > _depth_buffer.at(index))
        {
            return;
        }
        _depth_buffer.at(index) = depth;

        switch (blend_mode)
        {
        case BlendMode::OPAQUE:
            {
                _frame_buffer.data()[index] = std::byteswap(color.to_i32());
                return;
            }
        case BlendMode::ALPHA:
            {
                const Color4 dst = Color4::from_i32(std::byteswap(_frame_buffer.data()[index]));

                const double a = color.a_double();
                const double ia = 1.0 - a;

                const Color4 out {
                    static_cast<uint8_t>((color.r_double() * a + dst.r_double() * ia) * 255.0),
                    static_cast<uint8_t>((color.g_double() * a + dst.g_double() * ia) * 255.0),
                    static_cast<uint8_t>((color.b_double() * a + dst.b_double() * ia) * 255.0),
                    static_cast<uint8_t>((a + dst.a_double() * ia) * 255.0)
                };

                _frame_buffer.data()[index] = std::byteswap(out.to_i32());
            }
        }
    }

    Color4 RenderSurface::read_pixel(const Vec2i viewport_offset, const Vec2i viewport_pixel) const
    {
        const Vec2i pos { viewport_offset + viewport_pixel };
        
        if (pos.x < 0 || pos.y < 0 || pos.x >= _resolution.x || pos.y >= _resolution.y)
        {
            return Color4 { 0, 0, 0, 0 };
        }

        const int index = pos.y * _resolution.x + pos.x;
        return Color4::from_i32(std::byteswap(_frame_buffer.data()[index]));
    }

    double RenderSurface::get_depth(const Vec2i viewport_offset, const Vec2i viewport_pixel) const
    {
        const Vec2i pos { viewport_offset + viewport_pixel };
        
        if (pos.x < 0 || pos.y < 0 || pos.x >= _resolution.x || pos.y >= _resolution.y)
        {
            return std::numeric_limits<double>::infinity();
        }

        const int index = pos.y * _resolution.x + pos.x;
        return _depth_buffer.at(index);
    }

    void RenderSurface::clear_frame_buffer()
    {
        for (size_t i = 0; i < _frame_buffer.size(); ++i)
        {
            _frame_buffer.data()[i] = std::byteswap(_clear_color.to_i32());
            _depth_buffer.at(i) = std::numeric_limits<double>::infinity();
        }
    }

    void RenderSurface::set_resolution(const Vec2i new_resolution)
    {
        _resolution = new_resolution;
    }

    Vec2i RenderSurface::get_resolution() const
    {
        return _resolution;
    }

    double RenderSurface::get_aspect_ratio() const
    {
        return static_cast<double>(_resolution.x) / static_cast<double>(_resolution.y);
    }

    void RenderSurface::set_clear_color(const Color4 color)
    {
        _clear_color = color;
    }

    Color4 RenderSurface::get_clear_color() const
    {
        return _clear_color;
    }
}
