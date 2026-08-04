#pragma once

#include <string>

#include "glad.h"
#include "gfx/core/render-surface.h"

#include <GLFW/glfw3.h>

namespace gfx
{
class GLFWRenderSurface final : public RenderSurface
{

public:

    explicit GLFWRenderSurface(const Vec2i resolution)
        : RenderSurface(resolution)
    {
        init();
    }

    int init() override;

    void present() override;
    void clear_screen() const override;

    void resize_window(Vec2i new_resolution) override;

    void print_frame_buffer() const;

    GLFWwindow* get_window() const
    {
        return _window;
    }

    void set_window_title(const std::string title)
    {
        _window_title = title;
        if (_window)
        {
            glfwSetWindowTitle(_window, _window_title.c_str());
        }
    }

    void set_clear_color(const Color4 color) override
    {
        _clear_color = color;
        glClearColor(
            _clear_color.r_double(),
            _clear_color.g_double(),
            _clear_color.b_double(),
            _clear_color.a_double()
        );
    }

private:

    void setup_texture();
    void setup_quad();
    void setup_shader();

    GLFWwindow* _window;

    Vec2i _gl_window_size { 800, 600 };
    double _refresh_rate_hz = 60.0;

    GLuint _texture = 0;
    GLuint _vao = 0;
    GLuint _vbo = 0;
    GLuint _shader_program = 0;

    std::string _window_title = "GLFW Render Surface";
};
}
