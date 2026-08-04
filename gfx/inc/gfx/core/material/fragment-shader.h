#pragma once

#include "gfx/core/types/color4.h"
#include "gfx/math/vec2.h"
#include "gfx/math/vec3.h"

namespace gfx
{
    class FragmentShader
    {
    public:

        struct Uniforms
        {
            double t;
            Vec3d light_dir;
            double ambient_intensity;
        };

        struct Input
        {
            Vec2d uv;
            double depth;
            Vec3d normal;
            Color4 color;
        };

        virtual ~FragmentShader() = default;

        virtual Color4 frag(const Input& input, const Uniforms& uniforms) const = 0;
    };
}
