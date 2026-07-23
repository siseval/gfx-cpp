#pragma once

#include "gfx/core/types/color4.h"
#include "gfx/math/vec2.h"

namespace gfx
{
    class RenderSurface
    {
    public:

        virtual ~RenderSurface() = default;

        enum class BlendMode
        {
            OPAQUE,
            ALPHA,
        };

        explicit RenderSurface(Vec2i resolution);

        virtual int init() = 0;

        void write_pixel(Vec2i pos, Color4 color, double depth = 0, BlendMode blend_mode = BlendMode::OPAQUE);
        Color4 read_pixel(Vec2i pos) const;

        double get_depth(Vec2i pos) const;

        virtual void present() = 0;
        virtual void clear_screen() const = 0;

        void clear_frame_buffer();

        virtual void resize(Vec2i new_resolution) = 0;

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
