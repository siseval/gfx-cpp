#ifndef RENDER_SURFACE_H
#define RENDER_SURFACE_H

#include <gfx/core/types/color4.h>
#include <gfx/math/vec2.h>

namespace gfx::core
{

class RenderSurface 
{

public:

    RenderSurface(const gfx::math::Vec2i resolution)
        : resolution(resolution) {}

    virtual int init() = 0;

    virtual void present() = 0;
    virtual void clear() const = 0;

    virtual void clear_frame_buffer() = 0;
    virtual void clear_palette() = 0;

    virtual void write_pixel(const gfx::math::Vec2i pos, const types::Color4 color, const int depth = 0) = 0;
    inline void write_pixels(const std::vector<gfx::math::Vec2i> &positions, const types::Color4 color, const int depth = 0)
    {
        for (auto pos : positions)
        {
            write_pixel(pos, color, depth);
        }
    }

    virtual void resize(const gfx::math::Vec2i new_resolution) = 0;

    inline void set_resolution(const gfx::math::Vec2i new_resolution) { resolution = new_resolution; }
    inline gfx::math::Vec2i get_resolution() const { return resolution; }

    inline virtual void set_clear_color(const types::Color4 color) { clear_color = color; }
    inline virtual types::Color4 get_clear_color() const { return clear_color; }

protected:

    gfx::math::Vec2i resolution;
    gfx::core::types::Color4 clear_color = gfx::core::types::Color4(0.2, 0.2, 0.2, 1.0);
};

}

#endif // RENDER_SURFACE_H
