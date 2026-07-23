#pragma once

#include "gfx/core/material/fragment-shader.h"

namespace gfx
{
    class DefaultFragmentShader final : public FragmentShader
    {
    public:

        Color4 frag(const Input& input, const Uniforms& uniforms) const override
        {
            return input.color;
        }
    };
}
