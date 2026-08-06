#pragma once

#include "gfx/core/primitive.h"
#include "gfx/math/vec2.h"
#include "gfx/text/font-ttf.h"

namespace gfx
{
    class Text2D final : public Primitive<Vec2d>
    {
    public:

        enum class TextAlignment
        {
            LEFT,
            CENTER,
            RIGHT
        };

        void set_text(const std::string& new_text);
        void set_font(std::shared_ptr<FontTTF> new_font);
        void set_font_size(double new_font_size);
        void set_alignment(TextAlignment new_alignment);

        TextAlignment get_alignment() const;

        void set_line_height_multiplier(double multiplier);
        double get_line_height_multiplier() const;

    protected:

        void generate_mesh() const override;

    private:

        static void triangulate_glyph(
            const std::vector<std::vector<FontTTF::ContourEdge>>& glyph,
            std::vector<Vec2d>& vertices,
            std::vector<TriangleMesh<Vec2d>::Face>& faces
        );

        TextAlignment _alignment = TextAlignment::LEFT;

        Vec2d _text_box { -1.0, -1.0 };

        std::string _text;
        std::shared_ptr<FontTTF> _font;

        double _font_size { 0 };
        double _line_height_multiplier { 1.2 };

        double _smoothing_radius { 0.0 };
    };
}
