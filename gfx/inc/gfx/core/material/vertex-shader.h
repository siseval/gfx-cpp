#pragma once

#include "gfx/math/matrix.h"
#include "gfx/math/vec3.h"

namespace gfx
{
    class VertexShader
    {
    public:

        struct Uniforms
        {
            double t;
            Matrix4x4d model_matrix;
            Matrix4x4d view_matrix;
            Matrix4x4d projection_matrix;
            Matrix4x4d mvp_matrix;
        };

        struct Input
        {
            Vec3d pos;
            Vec3d normal;
        };

        struct Output
        {
            Vec3d xyz;
            double w;
            Vec3d normal;
        };

        virtual ~VertexShader() = default;

        virtual Output vert(const Input& input, const Uniforms& uniforms) const = 0;

    protected:
    };
}
