#pragma once

#include <memory>

#include "gfx/core/material/fragment-shader.h"
#include "gfx/core/material/texture.h"

namespace gfx
{
    class Material
    {
    public:

        Material();
        explicit Material(const std::shared_ptr<FragmentShader>& frag_shader);
        Material(const std::shared_ptr<FragmentShader>& frag_shader, const std::shared_ptr<Texture>& tex);

        int get_id() const;

        void set_texture(const std::shared_ptr<Texture>& tex);
        void set_fragment_shader(const std::shared_ptr<FragmentShader>& shader);

        std::shared_ptr<Texture> get_texture() const;
        std::shared_ptr<FragmentShader> get_fragment_shader() const;

        virtual ~Material() = default;

    private:

        static int next_id();

        std::shared_ptr<Texture> _texture;
        std::shared_ptr<FragmentShader> _fragment_shader;

        int _id;
    };
}
