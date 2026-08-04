#pragma once
#include "gfx/math/vec2.h"

namespace gfx
{
    struct Viewport
    {
        Viewport() = default;
        explicit Viewport(Vec2i size);
        Viewport(Vec2i size, Vec2i offset);

        Vec2i size;
        Vec2i offset;

        double get_aspect_ratio() const;
    };

    inline Viewport::Viewport(Vec2i size)
        : size(size) {}

    inline Viewport::Viewport(Vec2i size, Vec2i offset)
        : size(size)
        , offset(offset) {}

    inline double Viewport::get_aspect_ratio() const
    {
        return static_cast<double>(size.x) / static_cast<double>(size.y);
    }
}
