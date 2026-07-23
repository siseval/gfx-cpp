#pragma once

#include "gfx/core/material/vertex-shader.h"

namespace gfx
{
    class DefaultVertexShader final : public VertexShader
    {
    public:

        VertexShader::Output
        vert(const VertexShader::Input& input, const VertexShader::Uniforms& uniforms) const override
        {
            const Matrix4x1d pos_h { { input.pos.x }, { input.pos.y }, { input.pos.z }, { 1.0 } };

            const Matrix4x1d normal_h { { input.normal.x }, { input.normal.y }, { input.normal.z }, { 0.0 } };

            const Matrix4x1d pos_clip = uniforms.mvp_matrix * pos_h;
            const Matrix4x1d normal_clip = uniforms.model_matrix * normal_h;

            return VertexShader::Output {
                .xyz    = { pos_clip(0, 0), pos_clip(1, 0), pos_clip(2, 0) },
                .w      = pos_clip(3, 0),
                .normal = { normal_clip(0, 0), normal_clip(1, 0), normal_clip(2, 0) }
            };
        }
    };
}
