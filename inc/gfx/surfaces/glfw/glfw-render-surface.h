#ifndef GLFW_RENDER_SURFACE_H
#define GLFW_RENDER_SURFACE_H

#include <string>
#include <vector>
#include <memory>
#include <gfx/core/render-surface.h>
#include <gfx/surfaces/glfw/glad.h>
#include <GLFW/glfw3.h>

namespace gfx::surfaces
{

class GLFWRenderSurface : public gfx::core::RenderSurface
{

public:

    GLFWRenderSurface(const gfx::math::Vec2i resolution) 
        : RenderSurface(resolution), 
        frame_buffer(std::make_unique<std::vector<int32_t>>(resolution.x * resolution.y, 0))
        {};

    int init() override;

    void present() override;
    void clear() const override;

    void clear_palette() override {};
    void clear_frame_buffer() override;
    void write_pixel(const gfx::math::Vec2i pos, const gfx::core::types::Color4 color, const int depth = 0) override;

    void resize(const gfx::math::Vec2i new_resolution) override;

    void print_frame_buffer();

    inline GLFWwindow* get_window() const { return window; }

    inline void set_window_title(const std::string title)
    {
        window_title = title;
        if (window)
        {
            glfwSetWindowTitle(window, window_title.c_str());
        }
    }

    inline void set_clear_color(const gfx::core::types::Color4 color) override
    {
        clear_color = color;
        glClearColor(
            clear_color.r_double(),
            clear_color.g_double(),
            clear_color.b_double(),
            clear_color.a_double()
        );
    }

private:

    void render_multithreaded();

    void setup_texture();
    void setup_quad();
    void setup_shader();

    GLFWwindow* window;
    std::unique_ptr<std::vector<int32_t>> frame_buffer;

    gfx::math::Vec2i gl_window_size { 800, 600 };
    double refresh_rate_hz = 60.0;

    GLuint texture = 0;
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint shader_program = 0;

    std::string window_title = "GLFW Render Surface";
};

}

#endif // GLFW_RENDER_SURFACE_H
