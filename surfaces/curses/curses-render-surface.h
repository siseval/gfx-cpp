#pragma once

#include <string_view>

#include "gfx/core/render-surface.h"

namespace gfx
{
class CursesRenderSurface final : public RenderSurface
{

public:

    explicit CursesRenderSurface(const Vec2i resolution)
        : RenderSurface(resolution) {}

    int init() override;

    void present() override;
    void clear_screen() const override;

    void resize_window(Vec2i new_resolution) override;

    static constexpr int PALETTE_START_INDEX = 127;
    static constexpr int MAX_COLORS = 128;

private:

    static void set_color(Color4 color);

    static int color_to_curses_color_index(Color4 color);
    static void init_curses_color_palette();

    int _color_index = 0;


    static constexpr std::string_view pixel_tree[2][2][2][2] {
        {
            // TOP LEFT 0
            {
                //TOP RIGHT 0
                {
                    // BOT LEFT 0
                    {
                        // BOT RIGHT 0
                        " \0"
                    },
                    {
                        // BOT RIGHT 1
                        "⠠\0"
                    }
                },
                {
                    // BOT LEFT 1
                    {
                        // BOT RIGHT 0
                        "⠄\0"
                    },
                    {
                        // BOT RIGHT 1
                        "⠤\0"
                    }
                }
            },
            {
                // TOP RIGHT 1
                {
                    // BOT LEFT 0
                    {
                        // BOT RIGHT 0
                        "⠈\0"
                    },
                    {
                        // BOT RIGHT 1
                        "⠨\0"
                    }
                },
                {
                    // BOT LEFT 1
                    {
                        // BOT RIGHT 0
                        "⠌\0"
                    },
                    {
                        // BOT RIGHT 1
                        "⠬\0"
                    },
                }
            }
        },
        {
            // TOP LEFT 1
            {
                // TOP RIGHT 0
                {
                    // BOT LEFT 0
                    {
                        // BOT RIGHT 0
                        "⠁\0"
                    },
                    {
                        // BOT RIGHT 1
                        "⠡\0"
                    }
                },
                {
                    // BOT LEFT 1
                    {
                        // BOT RIGHT 0
                        "⠅\0"
                    },
                    {
                        // BOT RIGHT 1
                        "⠥\0"
                    }
                }
            },
            {
                // TOP RIGHT 1
                {
                    // BOT LEFT 0
                    {
                        // BOT RIGHT 0
                        "⠉\0"
                    },
                    {
                        // BOT RIGHT 1
                        "⠩\0"
                    }
                },
                {
                    // BOT LEFT 1
                    {
                        // BOT RIGHT 0
                        "⠍\0"
                    },
                    {
                        // BOT RIGHT 1
                        "⠭\0"
                    }
                }
            }
        }
    };
};
}
