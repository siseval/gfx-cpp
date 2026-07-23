#pragma once

#include "gfx/core/types/color4.h"
#include "gfx/math/vec2.h"

namespace gfx
{
    class Texture
    {
    public:

        enum class FilteringMode
        {
            NEAREST,
            BILINEAR
        };

        Texture() = default;
        explicit Texture(Vec2i resolution);
        Texture(Vec2i resolution, const std::vector<Color4>& data);

        void resize(Vec2i new_resolution);
        Vec2i get_resolution() const;

        void set_pixel(Vec2i pos, const Color4& color);
        void set_pixel(int x, int y, const Color4& color);

        Color4 get_pixel(Vec2i pos) const;

        Color4 sample(Vec2d uv, FilteringMode mode) const;
        Color4 sample_nearest(Vec2d uv) const;
        Color4 sample_bilinear(Vec2d uv) const;

    private:

        Vec2i _resolution;
        std::vector<Color4> _data;
    };
}
