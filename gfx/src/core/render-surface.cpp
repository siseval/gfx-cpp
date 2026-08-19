#include "gfx/core/render-surface.h"

#include <algorithm>
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
        std::ranges::fill(_frame_buffer, std::byteswap(_clear_color.to_i32()));
    }
    
    void RenderSurface::clear_depth_buffer()
    {
        std::ranges::fill(_depth_buffer, std::numeric_limits<double>::infinity());
    }
    
    void RenderSurface::clear_frame_buffer(const Viewport& viewport)
    {
        if (viewport.offset == Vec2i::zero() && viewport.size == _resolution)
        {
            clear_frame_buffer();
        }
        
        const Vec2i bottom_right { viewport.offset + viewport.size };
        
        for (size_t i = viewport.offset.x; i < bottom_right.x; ++i)
        {
            for (size_t j = viewport.offset.y; j < bottom_right.y; ++j)
            {
                const size_t index { j * _resolution.x + i };
                _frame_buffer.data()[index] = std::byteswap(_clear_color.to_i32());
            }
        }
    }
    
    void RenderSurface::clear_depth_buffer(const Viewport& viewport)
    {
        if (viewport.offset == Vec2i::zero() && viewport.size == _resolution)
        {
            clear_depth_buffer();
        }
        
        const Vec2i bottom_right { viewport.offset + viewport.size };
        
        for (size_t i = viewport.offset.x; i < bottom_right.x; ++i)
        {
            for (size_t j = viewport.offset.y; j < bottom_right.y; ++j)
            {
                const size_t index { j * _resolution.x + i };
                _depth_buffer.at(index) = std::numeric_limits<double>::infinity();
            }
        }
    }
    
    void RenderSurface::clean()
    {
        clear_frame_buffer();
        clear_depth_buffer();
        clear_screen();
    }

    void RenderSurface::set_resolution(const Vec2i new_resolution)
    {
        _resolution = new_resolution;
        _frame_buffer.resize(_resolution.x * _resolution.y, 0);
        _depth_buffer.resize(_resolution.x * _resolution.y, std::numeric_limits<float>::infinity());
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
