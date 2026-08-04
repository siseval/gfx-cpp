#include "curses-render-surface.h"

#include <algorithm>
#include <locale.h>
#include <ncurses.h>
#include <string>
#include <unistd.h>

namespace gfx
{
    int CursesRenderSurface::init()
    {
        setlocale(LC_ALL, "");

        initscr();
        timeout(0);
        cbreak();
        noecho();
        curs_set(0);
        start_color();
        use_default_colors();

        init_curses_color_palette();

        _clear_color = { 0, 0, 0, 0 };

        return 0;
    }


    static void append_ansi_color(std::string& out, const int r, const int g, const int b)
    {
        out += "\x1b[38;2;";
        out += std::to_string(r);
        out += ";";
        out += std::to_string(g);
        out += ";";
        out += std::to_string(b);
        out += "m";
    }

    void CursesRenderSurface::present()
    {
        std::string out;
        out.reserve(_resolution.x * _resolution.y * 12);

        out += "\x1b[H";

        for (int y = 0; y < _resolution.y; ++y)
        {
            int last_r = -1, last_g = -1, last_b = -1;

            for (int x = 0; x < _resolution.x; ++x)
            {
                Color4 c = read_pixel({ x, y });

                const int r = static_cast<int>(c.r_double() * 255.0);
                const int g = static_cast<int>(c.g_double() * 255.0);
                const int b = static_cast<int>(c.b_double() * 255.0);

                if (r != last_r || g != last_g || b != last_b)
                {
                    append_ansi_color(out, r, g, b);
                    last_r = r;
                    last_g = g;
                    last_b = b;
                }

                out += "██";
            }

            out += "\x1b[0m\n\r";
        }

        out += "\x1b[0m";

        write(STDOUT_FILENO, out.data(), out.size());
    }

    // void CursesRenderSurface::present()
    // {
    //     for (int y = 0; y < resolution.y; ++y)
    //     {
    //         for (int x = 0; x < resolution.x; ++x)
    //         {
    //             set_color(read_pixel({ x, y }));
    //             mvaddstr(y, x * 2, "█");
    //             mvaddstr(y, x * 2 + 1, "█");
    //         }
    //     }
    // }

    void CursesRenderSurface::clear_screen() const
    {
        erase();
    }

    void CursesRenderSurface::resize_window(const Vec2i new_resolution)
    {
        _resolution = new_resolution;
        _frame_buffer.resize(_resolution.x * _resolution.y, 0);
        _depth_buffer.resize(_resolution.x * _resolution.y, std::numeric_limits<float>::infinity());
    }

    void CursesRenderSurface::set_color(const Color4 color)
    {
        attron(COLOR_PAIR(color_to_curses_color_index(color)));
    }

    int CursesRenderSurface::color_to_curses_color_index(const Color4 color)
    {
        const double brightness {
            0.2126 * color.r_double() +
            0.7152 * color.g_double() +
            0.0722 * color.b_double()
        };

        const int index = std::clamp(
            static_cast<int>(brightness * (MAX_COLORS - 1) + 0.5),
            0,
            MAX_COLORS - 1
        );

        return PALETTE_START_INDEX + index;
    }

    void CursesRenderSurface::init_curses_color_palette()
    {
        for (int i = 0; i < MAX_COLORS; ++i)
        {
            const int brightness = static_cast<int>(i / static_cast<double>(MAX_COLORS) * 1000);
            init_color(PALETTE_START_INDEX + i, brightness, brightness, brightness);
            init_pair(PALETTE_START_INDEX + i, PALETTE_START_INDEX + i, -1);
        }
    }
}
