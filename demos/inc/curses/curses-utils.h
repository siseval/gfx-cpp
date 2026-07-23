#pragma once

#include <locale.h>
#include <ncurses.h>
#include <string>
#include <gfx/math/vec2.h>

#ifndef BUTTON5_PRESSED
#define BUTTON5_PRESSED NCURSES_MOUSE_MASK(5, NCURSES_BUTTON_PRESSED)
#endif

#ifndef BUTTON5_RELEASED
#define BUTTON5_RELEASED NCURSES_MOUSE_MASK(5, NCURSES_BUTTON_RELEASED)
#endif

namespace demos
{
    enum class default_color
    {
        BLACK = 1,
        RED,
        GREEN,
        YELLOW,
        BLUE,
        MAGENTA,
        CYAN,
        WHITE,
    };


    inline gfx::Vec2i get_screen_size()
    {
        int width, height;
        getmaxyx(stdscr, height, width);
        return { width, height };
    }

    inline int get_input()
    {
        return getch();
    }

    inline void set_bold(const bool enable)
    {
        if (enable)
        {
            attron(A_BOLD);
        }
        else
        {
            attroff(A_BOLD);
        }
    }

    inline void set_color(const enum default_color color)
    {
        attron(COLOR_PAIR(color));
    }

    inline void add_str(const gfx::Vec2i pos, const std::string pixel)
    {
        mvaddstr(pos.y, pos.x, pixel.data());
    }

    inline void init()
    {
        setlocale(LC_ALL, "");

        initscr();
        timeout(0);
        cbreak();
        noecho();
        curs_set(0);

        keypad(stdscr, TRUE);
        mouseinterval(0);
        mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, nullptr);
        printf("\033[?1003h\n");
        fflush(stdout);

        start_color();
        use_default_colors();

        init_pair(1, COLOR_BLACK, -1);
        init_pair(2, COLOR_RED, -1);
        init_pair(3, COLOR_GREEN, -1);
        init_pair(4, COLOR_YELLOW, -1);
        init_pair(5, COLOR_BLUE, -1);
        init_pair(6, COLOR_MAGENTA, -1);
        init_pair(7, COLOR_CYAN, -1);
        init_pair(8, COLOR_WHITE, -1);
    }

    inline void end()
    {
        printf("\033[?1003l\n");
        endwin();
        exit(0);
    }
}
