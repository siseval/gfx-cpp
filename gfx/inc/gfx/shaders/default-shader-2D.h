#pragma once

#include "gfx/core/shader-2D.h"

namespace gfx
{
    class DefaultShader2D final : public Shader2D
    {
    public:

        std::vector<Color4> frag(const ShaderInput2D& input) const override
        {
            return std::vector(input.uv.size(), input.color);
        }
    };
}
