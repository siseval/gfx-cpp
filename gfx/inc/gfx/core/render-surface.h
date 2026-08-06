#pragma once

#include "gfx/core/types/color4.h"
#include "view/viewport.h"
#include "gfx/math/vec2.h"

namespace gfx
{
    class RenderSurface
    {
    public:
        
        enum class BlendMode
        {
            OPAQUE,
            ALPHA,
        };

        explicit RenderSurface(Vec2i resolution);
        virtual ~RenderSurface() = default;

        virtual int init() = 0;

        void write_pixel(Vec2i viewport_offset, Vec2i viewport_pixel, Color4 color, double depth = 0, BlendMode blend_mode = BlendMode::OPAQUE);
        Color4 read_pixel(Vec2i viewport_offset, Vec2i viewport_pixel) const;

        double get_depth(Vec2i viewport_offset, Vec2i viewport_pixel) const;

        virtual void present() = 0;
        virtual void clear_screen() const = 0;

        void clear_frame_buffer();
        void clear_depth_buffer();
        
        void clear_frame_buffer(const Viewport& viewport);
        void clear_depth_buffer(const Viewport& viewport);
        
        void clean();

        virtual void resize_window(Vec2i new_resolution) = 0;

        void set_resolution(Vec2i new_resolution);
        Vec2i get_resolution() const;
        double get_aspect_ratio() const;

        virtual void set_clear_color(Color4 color);
        virtual Color4 get_clear_color() const;

    protected:

        Vec2i _resolution;
        Color4 _clear_color { 0.2, 0.2, 0.2, 1.0 };
        std::vector<int32_t> _frame_buffer;
        std::vector<double> _depth_buffer;
    };
}
