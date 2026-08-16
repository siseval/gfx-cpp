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
            MatrixType model_matrix;
            MatrixType mvp_matrix;
        };

        struct Input
        {
            VectorType pos;
            Vec3d normal;
        };

        struct Output
        {
            VectorType pos;
            double w;
            Vec3d normal;
        };

        Output transform_position(VectorType input, const Uniforms& uniforms) const;
        Output transform_normal(Vec3d input, const Uniforms& uniforms) const;
    };

    template <typename VectorType>
    VertexShader<VectorType>::Output VertexShader<VectorType>::transform_position(
        const VectorType input,
        const Uniforms& uniforms
    ) const {}

    template <typename VectorType>
    VertexShader<VectorType>::Output VertexShader<VectorType>::transform_normal(
        const Vec3d input,
        const Uniforms& uniforms
    ) const {}
}
