#include <gfx/primitives/text-2D.h>
#include <gfx/utils/transform.h>

namespace gfx::primitives
{

using namespace gfx::math;
using namespace gfx::core::types;
using namespace gfx::text;

Box2d Text2D::get_geometry_size() const
{
    Box2d bounds { Vec2d::zero(), Vec2d::zero() };
    double scale = font_size / font->get_units_per_em();

    double ascent { font->get_ascent() };
    double baseline_offset { ascent * scale };

    double advance_x = 0.0;

    for (int i = 0; i < text.length(); i++)
    {
        advance_x += i > 0 ? font->get_glyph(text[i - 1])->bbox.max.x : 0.0;
        advance_x += font->get_kerning(i > 0 ? text[i - 1] : '\0', text[i]);

        Vec2d pen_offset { advance_x * scale, 0.0 };

        std::vector<ContourEdge> edges = font->get_glyph_edges(text[i]);
        for (const auto &edge : edges)
        {
            bounds.expand(edge.v0 * scale + pen_offset);
            bounds.expand(edge.v1 * scale + pen_offset);
        }
    }
    return bounds;
}


void Text2D::rasterize_glyph(std::vector<ContourEdge> glyph, const std::function<void(const Pixel&)> emit_pixel) const
{
    if (glyph.empty())
    {
        return;
    }

    Box2i bounds {
        glyph[0].v0.round(),
        glyph[0].v0.round()
    };

    for (auto &edge : glyph)
    {
        bounds.expand(edge.v0);
        bounds.expand(edge.v1);
    }

    for (int y = bounds.min.y; y <= bounds.max.y; ++y)
    {
        for (int x = bounds.min.x; x <= bounds.max.x; ++x)
        {
            int crossings = 0;

            for (auto &edge : glyph)
            {
                Vec2d v0 { edge.v0 };
                Vec2d v1 { edge.v1 };
                if (v0.y == v1.y)
                {
                    continue;
                }

                bool intersects_y { ((v0.y > y) != (v1.y > y)) };
                if (!intersects_y)
                {
                    continue;
                }

                double intersects_x { v0.x + (v1.x - v0.x) * (y - v0.y) / (v1.y - v0.y) };
                if (intersects_x > x)
                {
                    crossings++;
                }
            }

            if (crossings % 2 == 1)
            {
                emit_pixel({ { x, y }, color });
            }
        }
    }
}


void Text2D::rasterize(const Matrix3x3d &transform, const std::function<void(const Pixel&)> emit_pixel) const
{
    double units_per_em { font->get_units_per_em() };
    double scale { font_size / units_per_em };

    double ascent { font->get_ascent() };
    double baseline_offset { ascent * scale };

    double pen_x = 0.0;

    for (int i = 0; i < text.length(); ++i)
    {
        std::vector<ContourEdge> edges = font->get_glyph_edges(text[i]);

        double advance_width { i > 0 ? font->get_glyph(text[i - 1])->bbox.max.x : 0.0 };
        double kerning { font->get_kerning(i > 0 ? text[i - 1] : '\0', text[i]) };

        pen_x += kerning + advance_width;

        for (auto &edge : edges)
        {
            edge.v0.x += pen_x;
            edge.v1.x += pen_x;

            edge.v0.y = -(edge.v0.y + baseline_offset);
            edge.v1.y = -(edge.v1.y + baseline_offset);

            edge.v0 = gfx::utils::transform_point(edge.v0 * scale, transform);
            edge.v1 = gfx::utils::transform_point(edge.v1 * scale, transform);
        }
        rasterize_glyph(edges, emit_pixel);
    }
}

}
