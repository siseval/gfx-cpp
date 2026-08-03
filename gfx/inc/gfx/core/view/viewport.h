#pragma once
#include "gfx/math/vec2.h"

namespace gfx
{
    struct Viewport
    {
        Vec2i offset;
        Vec2i size;
        
        double get_aspect_ratio() const;
    };
    
    inline double Viewport::get_aspect_ratio() const
    {
        return static_cast<double>(size.x) / static_cast<double>(size.y);
    }
}
