#pragma once

#include "gfx/math/matrix.h"
#include "gfx/math/vec2.h"
#include "gfx/math/vec3.h"

namespace gfx
{
    template <typename VectorType>
    class VertexShader
    {
        using MatrixType = std::conditional_t<std::same_as<VectorType, Vec2d>, Matrix3x3d, Matrix4x4d>;
        
    public:

        struct Uniforms
        {
            double t;
            MatrixType mvp_matrix;
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

    };
}
