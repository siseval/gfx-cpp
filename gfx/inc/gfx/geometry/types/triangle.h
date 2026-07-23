#pragma once

#include "gfx/math/vec2.h"

namespace gfx
{
    template <typename T>
    struct Triangle
    {
        Vec2<T> v0;
        Vec2<T> v1;
        Vec2<T> v2;
    };
}


