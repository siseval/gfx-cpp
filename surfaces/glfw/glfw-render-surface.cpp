#include <iostream>

#include "glfw-render-surface.h"

namespace gfx
{

int GLFWRenderSurface::init()
{
    _clear_color = Color4(0.2, 0.2, 0.2, 1.0);

    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_RED_BITS, 8);
    glfwWindowHint(GLFW_GREEN_BITS, 8);
    glfwWindowHint(GLFW_BLUE_BITS, 8);
    glfwWindowHint(GLFW_ALPHA_BITS, 8);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    if (!monitor)
    {
        glfwTerminate();
        std::cerr << "Failed to get primary monitor";
        return -1;
    }

    GLFWvidmode const* mode = glfwGetVideoMode(monitor);
    if (!mode)
    {
        glfwTerminate();
        std::cerr << "Failed to get video mode";
        return -1;
    }

    _gl_window_size = { mode->width, mode->height };
    _refresh_rate_hz = mode->refreshRate;

    _window = glfwCreateWindow(_resolution.x, _resolution.y, _window_title.c_str(), nullptr, nullptr);
    if (!_window)
    {
        glfwTerminate();
        std::cerr << "Failed to create GLFW window";
        return -1;
    }
    glfwMakeContextCurrent(_window);
    glfwSwapInterval(0);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        glfwDestroyWindow(_window);
        glfwTerminate();
        std::cerr << "Failed to initialize GLAD";
        return -1;
    }

    setup_texture();
    setup_quad();
    setup_shader();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glViewport(0, 0, _resolution.x, _resolution.y);

    glClearColor(
        _clear_color.r_double(),
        _clear_color.g_double(),
        _clear_color.b_double(),
        _clear_color.a_double()
    );

    return 0;
}

void GLFWRenderSurface::present()
{
    glBindTexture(GL_TEXTURE_2D, _texture);
    glTexSubImage2D(
        GL_TEXTURE_2D,
        0,
        0,
        0,
        _resolution.x,
        _resolution.y,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        _frame_buffer.data()
    );

    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(_shader_program);
    glBindVertexArray(_vao);
    glBindTexture(GL_TEXTURE_2D, _texture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
    int width, height;
    glfwGetFramebufferSize(_window, &width, &height);
    glViewport(0, 0, width, height);

    glfwSwapBuffers(_window);
    glfwPollEvents();
}

void GLFWRenderSurface::clear_screen() const
{
    glClear(GL_COLOR_BUFFER_BIT);
}

void GLFWRenderSurface::resize(const Vec2i new_resolution)
{
    _resolution = new_resolution;
    _frame_buffer.resize(_resolution.x * _resolution.y, 0);
    _depth_buffer.resize(_resolution.x * _resolution.y, std::numeric_limits<float>::infinity());
    if (_window)
    {
        glfwSetWindowSize(_window, new_resolution.x, new_resolution.y);
    }
    glBindTexture(GL_TEXTURE_2D, _texture);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        _resolution.x,
        _resolution.y,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        _frame_buffer.data()
    );
    glViewport(0, 0, _resolution.x, _resolution.y);
}

void GLFWRenderSurface::print_frame_buffer() const
{
    for (int y = 0; y < _resolution.y; y += 32)
    {
        for (int x = 0; x < _resolution.x; x += 32)
        {
            if (_frame_buffer.at(y * _resolution.x + x) != 0)
            {
                std::cout << "*";
            }
            else
            {
                std::cout << " ";
            }
        }
        std::cout << std::endl;
    }
}

void GLFWRenderSurface::setup_texture()
{
    glGenTextures(1, &_texture);
    glBindTexture(GL_TEXTURE_2D, _texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        _resolution.x,
        _resolution.y,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        _frame_buffer.data()
    );
}

void GLFWRenderSurface::setup_quad()
{
    constexpr float vertices[] = {
        -1.f,
        -1.f,
        0.f,
        1.f,
        1.f,
        -1.f,
        1.f,
        1.f,
        -1.f,
        1.f,
        0.f,
        0.f,
        1.f,
        1.f,
        1.f,
        0.f
    };

    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_vbo);
    glBindVertexArray(_vao);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), static_cast<void*>(nullptr));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));
}

void GLFWRenderSurface::setup_shader()
{
    const auto vert_src = R"(
    #version 330 core
    layout(location = 0) in vec2 aPos;
    layout(location = 1) in vec2 aTex;
    out vec2 TexCoord;
    void main() { TexCoord = aTex; gl_Position = vec4(aPos, 0.0, 1.0); }
    )";

    const auto frag_src = R"(
    #version 330 core
    in vec2 TexCoord;
    out vec4 FragColor;
    uniform sampler2D screenTexture;
    void main() { FragColor = texture(screenTexture, TexCoord); }
    )";

    const GLuint vert = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert, 1, &vert_src, nullptr);
    glCompileShader(vert);

    const GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag, 1, &frag_src, nullptr);
    glCompileShader(frag);

    _shader_program = glCreateProgram();
    glAttachShader(_shader_program, vert);
    glAttachShader(_shader_program, frag);
    glLinkProgram(_shader_program);

    glDeleteShader(vert);
    glDeleteShader(frag);
}


}
