#include <gfx/primitives/text-2D.h>
#include <gfx/utils/transform.h>
#include <gfx/text/utf-8.h>

namespace gfx::primitives
{

using namespace gfx::math;
using namespace gfx::core::types;
using namespace gfx::text;


Box2d Text2D::get_geometry_size() const
{
    double scale = font_size / font->get_units_per_em();
    double ascent = font->get_ascent() * scale;
    double line_gap = font->get_line_gap() * scale;

    double line_height = (line_gap > 0.0) ? font_size + line_gap : font_size * line_height_multiplier;

    Box2d bounds {
        Vec2d(std::numeric_limits<double>::max()),
        Vec2d(std::numeric_limits<double>::lowest())
    };

    Vec2d pen {0.0, 0.0};

    size_t i = 0;
    uint32_t prev_codepoint = 0;

    while (i < text.size())
    {
        uint32_t codepoint;
        size_t bytes;

        if (!decode_utf8(text, i, codepoint, bytes))
        {
            ++i;
            continue;
        }

        if (codepoint == '\n')
        {
            pen.x = 0.0;
            pen.y += line_height;
            prev_codepoint = 0;
            i += bytes;
            continue;
        }

        if (prev_codepoint != 0)
        {
            pen.x += font->get_kerning(prev_codepoint, codepoint) * scale;
        }

        auto edges = font->get_glyph_edges(codepoint);
        for (auto &edge : edges)
        {
            Vec2d v0 = edge.v0 * scale;
            Vec2d v1 = edge.v1 * scale;

            v0.x += pen.x;
            v1.x += pen.x;
            v0.y = -v0.y + ascent + pen.y;
            v1.y = -v1.y + ascent + pen.y;

            bounds.expand(v0);
            bounds.expand(v1);
        }

        pen.x += font->get_glyph_advance(codepoint) * scale;
        prev_codepoint = codepoint;
        i += bytes;
    }

    return Box2d { Vec2d::zero(), bounds.size() };
}


void Text2D::rasterize_glyph(std::vector<ContourEdge> glyph, const std::function<void(const Pixel&)> emit_pixel) const
{
    if (glyph.empty()) 
    {
        return;
    }

    Box2i bounds { glyph[0].v0.round(), glyph[0].v0.round() };

    for (auto &edge : glyph) 
    {
        bounds.expand(edge.v0);
        bounds.expand(edge.v1);
    }

    const int height = bounds.max.y - bounds.min.y + 1;

    std::vector<std::vector<size_t>> edge_table(height);

    for (size_t i = 0; i < glyph.size(); ++i) 
    {
        const auto &edge = glyph[i];

        if (edge.v0.y == edge.v1.y) 
        {
            continue;
        }

        int y0 = static_cast<int>(std::floor(std::min(edge.v0.y, edge.v1.y)));
        int y1 = static_cast<int>(std::ceil(std::max(edge.v0.y, edge.v1.y)));

        for (int y = y0; y < y1; ++y)
        {
            if (y >= bounds.min.y && y <= bounds.max.y)
            {
                edge_table[y - bounds.min.y].push_back(i);
            }
        }
    }

    for (int y = bounds.min.y; y <= bounds.max.y; ++y)
    {
        const auto &row_edges = edge_table[y - bounds.min.y];

        if (row_edges.empty()) 
        {
            continue;
        }

        std::vector<double> intersections;
        intersections.reserve(row_edges.size());

        for (size_t index : row_edges)
        {
            const auto &edge = glyph[index];

            double y0 = edge.v0.y;
            double y1 = edge.v1.y;

            if ((y < y0 && y < y1) || (y >= y0 && y >= y1)) 
            {
                continue;
            }

            double t = (y - y0) / (y1 - y0);
            intersections.push_back(edge.v0.x + t * (edge.v1.x - edge.v0.x));
        }

        std::sort(intersections.begin(), intersections.end());

        for (size_t i = 0; i + 1 < intersections.size(); i += 2)
        {
            int x0 = static_cast<int>(std::ceil(intersections[i]));
            int x1 = static_cast<int>(std::floor(intersections[i + 1]));
            for (int x = x0; x <= x1; ++x)
            {
                emit_pixel( { { x, y }, color });
            }
        }
    }
}



void Text2D::rasterize(const Matrix3x3d &transform, const std::function<void(const Pixel&)> emit_pixel) const
{
    double scale = font_size / font->get_units_per_em();
    double ascent = font->get_ascent() * scale;
    double line_gap = font->get_line_gap() * scale;

    double line_height = (line_gap > 0.0) ? 
        font_size + line_gap : 
        font_size * line_height_multiplier;

    Vec2d pen { Vec2d::zero() };

    Vec2d min { Vec2d(std::numeric_limits<double>::max()) };
    Vec2d max { Vec2d(std::numeric_limits<double>::lowest()) };

    std::vector<double> line_widths { 0.0 };
    int line_index = 0;

    size_t i = 0;
    while (i < text.size())
    {
        uint32_t codepoint;
        size_t bytes;

        if (!decode_utf8(text, i, codepoint, bytes))
        {
            ++i;
            continue;
        }

        if (codepoint == '\n')
        {
            pen.x = 0.0;
            pen.y += line_height;
            i += bytes;
            line_index++;
            line_widths.push_back(0.0);
            continue;
        }

        if (i > 0)
        {
            size_t prev_bytes;
            uint32_t prev_cp;
            decode_utf8(text, i - 1, prev_cp, prev_bytes);
            pen.x += font->get_kerning(prev_cp, codepoint) * scale;
            line_widths[line_index] = pen.x;
        }

        auto edges = font->get_glyph_edges(codepoint);
        for (auto &edge : edges)
        {
            Vec2d v0 = edge.v0 * scale;
            Vec2d v1 = edge.v1 * scale;

            v0.x += pen.x; v1.x += pen.x;
            v0.y = -v0.y + ascent + pen.y;
            v1.y = -v1.y + ascent + pen.y;

            min.x = std::min({min.x, v0.x, v1.x});
            min.y = std::min({min.y, v0.y, v1.y});

            max.x = std::max({max.x, v0.x, v1.x});
            max.y = std::max({max.y, v0.y, v1.y});
        }

        pen.x += font->get_glyph_advance(codepoint) * scale;
        line_widths[line_index] = pen.x;
        i += bytes;
    }

    pen = Vec2d { 0.0, 0.0 };
    line_index = 0;
    i = 0;
    while (i < text.size())
    {
        uint32_t codepoint;
        size_t bytes;

        if (!decode_utf8(text, i, codepoint, bytes))
        {
            ++i;
            continue;
        }

        if (codepoint == '\n')
        {
            pen.x = 0.0;
            pen.y += line_height;
            i += bytes;
            line_index++;
            continue;
        }

        if (i > 0)
        {
            size_t prev_bytes;
            uint32_t prev_cp;
            decode_utf8(text, i - 1, prev_cp, prev_bytes);
            pen.x += font->get_kerning(prev_cp, codepoint) * scale;
        }

        double offset_x { [&] { switch (alignment) 
            {
                case TextAlignment::LEFT: return 0.0;
                case TextAlignment::RIGHT: return max.x - line_widths[line_index];
                case TextAlignment::CENTER: return (max.x - line_widths[line_index]) / 2.0;
            }
            std::unreachable();
        }()};

        auto edges = font->get_glyph_edges(codepoint);
        for (auto &edge : edges)
        {
            edge.v0 = edge.v0 * scale;
            edge.v1 = edge.v1 * scale;

            edge.v0.x += pen.x - min.x + offset_x;
            edge.v1.x += pen.x - min.x + offset_x;

            edge.v0.y = -edge.v0.y + ascent + pen.y - min.y;
            edge.v1.y = -edge.v1.y + ascent + pen.y - min.y;

            edge.v0 = utils::transform_point(edge.v0, transform);
            edge.v1 = utils::transform_point(edge.v1, transform);
        }

        rasterize_glyph(edges, emit_pixel);
        pen.x += font->get_glyph_advance(codepoint) * scale;
        i += bytes;
    }
}


}
