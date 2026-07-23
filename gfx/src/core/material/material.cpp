#include "gfx/core/material/material.h"

namespace gfx
{
    Material::Material()
        : _id(next_id()) {}

    Material::Material(const std::shared_ptr<FragmentShader>& frag_shader)
        : _fragment_shader(frag_shader)
        , _id(next_id()) {}

    Material::Material(const std::shared_ptr<FragmentShader>& frag_shader, const std::shared_ptr<Texture>& tex)
        : _texture(tex)
        , _fragment_shader(frag_shader)
        , _id(next_id()) {}

    int Material::get_id() const
    {
        return _id;
    }

    void Material::set_texture(const std::shared_ptr<Texture>& tex)
    {
        _texture = tex;
    }

    void Material::set_fragment_shader(const std::shared_ptr<FragmentShader>& shader)
    {
        _fragment_shader = shader;
    }

    std::shared_ptr<Texture> Material::get_texture() const
    {
        return _texture;
    }

    std::shared_ptr<FragmentShader> Material::get_fragment_shader() const
    {
        return _fragment_shader;
    }

    int Material::next_id()
    {
        static int next_id = 0;
        return next_id++;
    }
}
