#pragma once

#include <curses-render-surface.h>
#include <gfx/core/render-2D.h>

#include "common/core/demo-player.h"
#include "curses/curses-utils.h"

namespace demos
{
    class CursesDemoPlayer : public DemoPlayer
    {
    public:

        CursesDemoPlayer()
            : DemoPlayer()
        {
            auto surface { std::make_shared<gfx::CursesRenderSurface>(demos::get_screen_size() * 2) };
            surface->init();
            renderer = std::make_shared<gfx::RenderEngine>(surface);
        }

        gfx::Vec2i get_screen_size() override
        {
            return demos::get_screen_size();
        }

        int get_input() override
        {
            const int input { demos::get_input() };
            if (input == KEY_MOUSE)
            {
                MEVENT e;
                if (getmouse(&e) == OK)
                {
                    const MouseEvent event { curses_to_mouse_event(e) };
                    demos[current_demo]->report_mouse(event);
                }
                refresh();
                return 0;
            }
            return input;
        }

        void draw_info() override
        {
            set_bold(true);
            set_color(default_color::WHITE);
            const std::vector<std::string> info = get_info();
            for (int i = 0; i < info.size(); ++i)
            {
                add_str({ 0, i }, info[i]);
            }
        }

    private:

        MouseEvent curses_to_mouse_event(const MEVENT e) const
        {
            MouseEvent event;
            switch (e.bstate)
            {
            case BUTTON1_PRESSED:
                event.type = MouseEventType::LEFT_DOWN;
                break;

            case BUTTON3_PRESSED:
                event.type = MouseEventType::RIGHT_DOWN;
                break;

            case BUTTON4_PRESSED:
                event.type = MouseEventType::SCROLL_UP;
                break;

            case BUTTON5_PRESSED:
                event.type = MouseEventType::SCROLL_DOWN;
                break;

            case REPORT_MOUSE_POSITION:
                event.type = MouseEventType::MOVE;
                break;

            default:
                break;
            }
            const auto position = static_cast<gfx::Vec2d>(gfx::Vec2i { e.x, e.y } * 2 / renderer->get_render_2D()->get_viewport_scaling());
            event.position = position / renderer->get_resolution();
            return event;
        }
    };
}
