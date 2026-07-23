#pragma once

// #include <iostream>
#include <glfw-render-surface.h>
#include <gfx/core/render-engine.h>

#include "common/core/demo-player.h"

namespace demos
{
    class GLFWDemoPlayer : public DemoPlayer
    {
    public:

        GLFWDemoPlayer()
        {
            const gfx::Vec2i resolution = { 1280, 720 };

            auto surface = std::make_shared<gfx::GLFWRenderSurface>(resolution);
            renderer = std::make_shared<gfx::RenderEngine>(surface);

            glfwSetWindowUserPointer(surface->get_window(), this);

            glfwSetInputMode(surface->get_window(), GLFW_STICKY_KEYS, GLFW_TRUE);
            glfwSetInputMode(surface->get_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

            glfwSetMouseButtonCallback(surface->get_window(), mouse_callback);
            glfwSetKeyCallback(surface->get_window(), key_callback);
            glfwSetScrollCallback(surface->get_window(), scroll_callback);
            glfwSetCursorPosCallback(surface->get_window(), cursor_callback);
            glfwSetCharCallback(surface->get_window(), char_callback);
        }

        gfx::Vec2i get_screen_size() override
        {
            return renderer->get_resolution();
            // int w, h;
            // glfwGetFramebufferSize(get_window(), &w, &h);
            // return { w, h };
        }

        int get_input() override
        {
            glfwPollEvents();
            return 0;
        }

        void draw_info() override
        {
            // auto surface = std::dynamic_pointer_cast<gfx::GLFWRenderSurface>
            //     (renderer->get_render_surface());
            // gfx::Vec2i resolution = renderer->get_render_surface()->get_resolution();
            // std::string fps = std::to_string(static_cast<int>(demos[current_demo]->get_fps()));
            // while (fps.size() < 4)
            // {
            //     fps = " " + fps;
            // }
            // int x, y;
            // glfwGetWindowSize(surface->get_window(), &x, &y);
            // surface->set_window_title(
            //     "Demo [" + std::to_string(current_demo + 1) + "/" + std::to_string(demos.size()) + "]" +
            //     " :: " + std::to_string(resolution.x) + "x" + std::to_string(resolution.y) +
            //     " :: " + fps + " FPS" +
            //     " :: " + std::to_string(renderer->num_items()) + " items" +
            //     "   [" + std::to_string(x) + "x" + std::to_string(y) + "]"
            //
            // );
            //
            // std::cout << "\033[2J\033[1;1H";
            // std::vector<std::string> debug_lines = demos[current_demo]->debug_text();
            // for (size_t i = 0; i < debug_lines.size(); ++i)
            // {
            //     std::cout << debug_lines[i] << std::endl;
            // }
        }

    private:

        static void mouse_callback(GLFWwindow* win, const int button, const int action, int mods)
        {
            const auto* self = static_cast<GLFWDemoPlayer*>(glfwGetWindowUserPointer(win));
            if (!self)
            {
                return;
            }

            MouseEvent event;
            if (button == GLFW_MOUSE_BUTTON_LEFT)
            {
                event.type = action == GLFW_PRESS ? MouseEventType::LEFT_DOWN : MouseEventType::LEFT_UP;
            }
            else if (button == GLFW_MOUSE_BUTTON_RIGHT)
            {
                event.type = action == GLFW_PRESS ? MouseEventType::RIGHT_DOWN : MouseEventType::RIGHT_UP;
            }

            double x, y;
            glfwGetCursorPos(win, &x, &y);

            int width, height;
            glfwGetWindowSize(win, &width, &height);

            const gfx::Vec2d pos {
                gfx::Vec2d { x, y } /
                gfx::Vec2d { static_cast<double>(width), static_cast<double>(height) }
            };
            event.position = pos;

            self->demos[self->current_demo]->report_mouse(event);
        }

        static void scroll_callback(GLFWwindow* win, double x_offset, const double y_offset)
        {
            const auto* self = static_cast<GLFWDemoPlayer*>(glfwGetWindowUserPointer(win));
            if (!self)
                return;

            MouseEvent event;
            event.type = y_offset > 0 ? MouseEventType::SCROLL_UP : MouseEventType::SCROLL_DOWN;

            double x, y;
            glfwGetCursorPos(win, &x, &y);
            event.position = static_cast<gfx::Vec2<double>>(gfx::Vec2i { static_cast<int>(x), static_cast<int>(y) } / self->renderer->get_render_2D()->get_viewport_scaling());

            self->demos[self->current_demo]->report_mouse(event);
        }

        static void cursor_callback(GLFWwindow* win, const double xpos, const double ypos)
        {
            const auto* self = static_cast<GLFWDemoPlayer*>(glfwGetWindowUserPointer(win));
            if (!self)
                return;

            MouseEvent event;
            event.type = MouseEventType::MOVE;

            int width, height;
            glfwGetWindowSize(win, &width, &height);
            const gfx::Vec2i window_size { width, height };

            // gfx::Vec2d position { gfx::Vec2d { xpos, ypos } / self->renderer->get_viewport_scaling() };
            const gfx::Vec2d position { gfx::Vec2d { xpos, ypos } / window_size };
            // gfx::Vec2d pos {
            //     2.0 * (position.x / static_cast<double>(width)) - 1.0,
            //     1.0 - 2.0 * (position.y / static_cast<double>(height))
            // };
            event.position = position;

            self->demos[self->current_demo]->report_mouse(event);
        }

        static void char_callback(GLFWwindow* win, const unsigned int key)
        {
            auto* self = static_cast<GLFWDemoPlayer*>(glfwGetWindowUserPointer(win));
            if (!self)
            {
                return;
            }
            self->handle_input(key);
            self->demos[self->current_demo]->handle_char(key);
        }

        static Key glfw_key_to_key(const int glfw_key)
        {
            switch (glfw_key)
            {
            case GLFW_KEY_UP:
                return Key::UP;
            case GLFW_KEY_DOWN:
                return Key::DOWN;
            case GLFW_KEY_LEFT:
                return Key::LEFT;
            case GLFW_KEY_RIGHT:
                return Key::RIGHT;
            case GLFW_KEY_W:
                return Key::W;
            case GLFW_KEY_A:
                return Key::A;
            case GLFW_KEY_S:
                return Key::S;
            case GLFW_KEY_D:
                return Key::D;
            case GLFW_KEY_Q:
                return Key::Q;
            case GLFW_KEY_E:
                return Key::E;
            case GLFW_KEY_LEFT_SHIFT:
            case GLFW_KEY_RIGHT_SHIFT:
                return Key::SHIFT;
            case GLFW_KEY_LEFT_CONTROL:
            case GLFW_KEY_RIGHT_CONTROL:
                return Key::CTRL;
            case GLFW_KEY_SPACE:
                return Key::SPACE;
            default:
                return Key::UNKNOWN;
            }
        }

        static void key_callback(GLFWwindow* win, const int key, int scancode, const int action, int mods)
        {
            const auto* self = static_cast<GLFWDemoPlayer*>(glfwGetWindowUserPointer(win));
            if (!self)
            {
                return;
            }
            const KeyEvent event {
                .type = [&]()
                {
                    switch (action)
                    {
                    case GLFW_PRESS:
                        return KeyEventType::KEY_PRESS;
                    case GLFW_RELEASE:
                        return KeyEventType::KEY_RELEASE;
                    case GLFW_REPEAT:
                        return KeyEventType::KEY_REPEAT;
                    default:
                        return KeyEventType::KEY_RELEASE;
                    }
                }(),
                .key = glfw_key_to_key(key)
            };
            self->demos[self->current_demo]->report_key(event);
        }

        GLFWwindow* get_window() const
        {
            return std::static_pointer_cast<gfx::GLFWRenderSurface>(renderer->get_render_surface())->get_window();
        }

        static gfx::Vec2d get_scaling(GLFWwindow* win)
        {
            const auto* self = static_cast<GLFWDemoPlayer*>(glfwGetWindowUserPointer(win));
            int width, height;
            glfwGetWindowSize(self->get_window(), &width, &height);

            int fb_width, fb_height;
            glfwGetFramebufferSize(self->get_window(), &fb_width, &fb_height);
            return gfx::Vec2d {
                static_cast<double>(fb_width) / static_cast<double>(width),
                static_cast<double>(fb_height) / static_cast<double>(height)
            };
        }
    };
}
