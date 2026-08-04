#include "../../inc/gfx/text/font-ttf.h"
#include "gfx/geometry/flatten.h"

namespace gfx
{
    FontTTF::FontTTF(
        const int units_per_em,
        const double ascent,
        const double descent,
        const double line_gap,
        const int num_glyphs
    )
        : _units_per_em(units_per_em)
        , _ascent(ascent)
        , _descent(descent)
        , _line_gap(line_gap)
        , _num_glyphs(num_glyphs) {}

    std::shared_ptr<FontTTF::GlyphTTF> FontTTF::get_glyph(const uint32_t codepoint) const
    {
        const auto it = _glyphs.find(codepoint);
        if (it != _glyphs.end())
        {
            return it->second;
        }
        return nullptr;
    }

    std::vector<std::vector<FontTTF::ContourEdge>> FontTTF::get_glyph_edges_multi_contour(
        const uint32_t codepoint
    ) const
    {
        // const auto it = _vertex_cache.find(codepoint);
        // if (it != _vertex_cache.end())
        // {
        //     return it->second;
        // }

        std::vector<std::vector<ContourEdge>> vertices = flatten_glyph_multi_contour(get_glyph(codepoint));
        // _vertex_cache[codepoint] = vertices;
        return vertices;
    }

    std::vector<FontTTF::ContourEdge> FontTTF::get_glyph_edges(const uint32_t codepoint) const
    {
        const auto it = _vertex_cache.find(codepoint);
        if (it != _vertex_cache.end())
        {
            return it->second;
        }

        std::vector<ContourEdge> vertices = flatten_glyph(get_glyph(codepoint));
        _vertex_cache[codepoint] = vertices;
        return vertices;
    }

    void FontTTF::set_kerning(const char left, const char right, const int offset)
    {
        _kerning_table[{
            static_cast<uint32_t>(static_cast<uint8_t>(left)),
            static_cast<uint32_t>(static_cast<uint8_t>(right))
        }] = offset;
    }

    int FontTTF::get_kerning(const char left, const char right) const
    {
        return _kerning_table.contains(
                   {
                       static_cast<uint32_t>(static_cast<uint8_t>(left)),
                       static_cast<uint32_t>(static_cast<uint8_t>(right))
                   }
               ) ?
               _kerning_table.at(
                   {
                       static_cast<uint32_t>(static_cast<uint8_t>(left)),
                       static_cast<uint32_t>(static_cast<uint8_t>(right))
                   }
               ) :
               0;
    }

    void FontTTF::set_metrics(const uint32_t codepoint, const GlyphMetrics& metrics)
    {
        _glyph_metrics[codepoint] = metrics;
    }

    int FontTTF::get_glyph_advance(const uint32_t codepoint) const
    {
        return _glyph_metrics.contains(codepoint) ? _glyph_metrics.at(codepoint).advance_width : 0;
    }

    int FontTTF::get_glyph_left_side_bearing(const uint32_t codepoint) const
    {
        return _glyph_metrics.contains(codepoint) ? _glyph_metrics.at(codepoint).left_side_bearing : 0;
    }

    double FontTTF::get_ascent() const
    {
        return _ascent;
    }

    double FontTTF::get_descent() const
    {
        return _descent;
    }

    double FontTTF::get_line_gap() const
    {
        return _line_gap;
    }

    double FontTTF::get_line_height() const
    {
        return _ascent - _descent + _line_gap;
    }

    double FontTTF::get_units_per_em() const
    {
        return _units_per_em;
    }

    void FontTTF::set_name(const std::string& n)
    {
        _name = n;
    }

    std::string FontTTF::get_name() const
    {
        return _name;
    }

    void FontTTF::set_glyphs(const std::unordered_map<uint32_t, std::shared_ptr<GlyphTTF>>& g)
    {
        _glyphs = g;
    }

    std::unordered_map<uint32_t, std::shared_ptr<FontTTF::GlyphTTF>> FontTTF::get_glyphs() const
    {
        return _glyphs;
    }

    std::vector<std::vector<FontTTF::ContourEdge>> FontTTF::flatten_glyph_multi_contour(
        const std::shared_ptr<GlyphTTF> glyph
    ) const
    {
        std::vector<std::vector<ContourEdge>> edges;
        if (!glyph)
        {
            return edges;
        }

        for (const auto& contour : glyph->contours)
        {
            std::vector<ContourEdge> contour_edges;
            std::vector<std::pair<Vec2d, bool>> points_on_off_curve;
            for (const auto& [x, y, on_curve] : contour)
            {
                points_on_off_curve.push_back({ Vec2d { x, y }, on_curve });
            }

            std::vector<Vec2d> flattened_contour = Flatten::flatten_contour(points_on_off_curve);
            const int num_points = flattened_contour.size();
            for (int i = 0; i < num_points; ++i)
            {
                ContourEdge edge { flattened_contour[i], flattened_contour[(i + 1) % num_points] };
                contour_edges.push_back(edge);
            }

            edges.push_back(contour_edges);
        }
        return edges;
    }

    std::vector<FontTTF::ContourEdge> FontTTF::flatten_glyph(const std::shared_ptr<GlyphTTF> glyph)
    {
        std::vector<ContourEdge> edges;
        if (!glyph)
        {
            return edges;
        }

        for (const auto& contour : glyph->contours)
        {
            std::vector<std::pair<Vec2d, bool>> points_on_off_curve;
            for (const auto& [x, y, on_curve] : contour)
            {
                points_on_off_curve.push_back({ Vec2d { x, y }, on_curve });
            }

            std::vector<Vec2d> flattened_contour = Flatten::flatten_contour(points_on_off_curve);
            const int num_points = flattened_contour.size();
            for (int i = 0; i < num_points; ++i)
            {
                ContourEdge edge { flattened_contour[i], flattened_contour[(i + 1) % num_points] };
                edges.push_back(edge);
            }
        }
        return edges;
    }
}
