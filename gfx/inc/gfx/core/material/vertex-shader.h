#pragma once

#include "gfx/core/transform.h"

namespace gfx
{
    template <typename VectorType>
    class VertexShader
    {
        using MatrixType = Transform<VectorType>::MatrixType;
        
    public:

        struct Uniforms
        {
            double t;
            MatrixType mvp_matrix;
        };

        struct Input
        {
            VectorType pos;
            VectorType normal;
        };

        struct Output
        {
            VectorType pos;
            double w;
            VectorType normal;
        };

        virtual ~VertexShader() = default;

        virtual Output vert(const Input& input, const Uniforms& uniforms) const = 0;

    };
}
