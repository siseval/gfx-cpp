#include <gfx/text/font-ttf.h>
#include <gfx/geometry/flatten.h>

namespace gfx::text
{

using namespace gfx::math;
using namespace gfx::geometry;


std::shared_ptr<GlyphTTF> FontTTF::get_glyph(const uint32_t codepoint) const
{
    auto it = glyphs.find(codepoint);
    if (it != glyphs.end())
    {
        return it->second;
    }
    return nullptr;
}

std::vector<ContourEdge> FontTTF::get_glyph_edges(const uint32_t codepoint) const
{
    auto it = vertex_cache.find(codepoint);
    if (it != vertex_cache.end())
    {
        return it->second;
    }

    std::vector<ContourEdge> vertices = flatten_glyph(get_glyph(codepoint));
    vertex_cache[codepoint] = vertices;
    return vertices;
}

std::vector<ContourEdge> FontTTF::flatten_glyph(const std::shared_ptr<GlyphTTF> glyph) const
{
    std::vector<ContourEdge> edges;
    for (const auto &contour : glyph->contours)
    {
        std::vector<std::pair<Vec2d, bool>> points_on_off_curve;
        for (const auto &point : contour)
        {
            points_on_off_curve.push_back({ Vec2d { point.x, point.y }, point.on_curve });
        }

        std::vector<Vec2d> flattened_contour = flatten_contour(points_on_off_curve);
        int num_points = flattened_contour.size();
        for (int i = 0; i < num_points; ++i)
        {
            ContourEdge edge {
                flattened_contour[i],
                flattened_contour[(i + 1) % num_points]
            };
            edges.push_back(edge);
        }
    }
    return edges;
}

}
