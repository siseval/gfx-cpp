#pragma once

#include "gfx/core/types/color4.h"
#include "gfx/math/vec2.h"

#include <algorithm>

namespace gfx
{
    struct ShaderInput2D
    {
        std::vector<Vec2d> uv;
        double t;
        Color4 color;
    };

    class Shader2D
    {
    public:

        virtual ~Shader2D() = default;

        virtual std::vector<Color4> frag(const ShaderInput2D& input) const = 0;

        static Color4 mix(const Color4& a, const Color4& b, const double factor)
        {
            const double clamped_factor = std::clamp(factor, 0.0, 1.0);
            return Color4(
                static_cast<uint8_t>(a.r * (1.0 - clamped_factor) + b.r * clamped_factor),
                static_cast<uint8_t>(a.g * (1.0 - clamped_factor) + b.g * clamped_factor),
                static_cast<uint8_t>(a.b * (1.0 - clamped_factor) + b.b * clamped_factor),
                static_cast<uint8_t>(a.a * (1.0 - clamped_factor) + b.a * clamped_factor)
            );
        }
    };
}
