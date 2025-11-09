#ifndef FONT_TTF_H
#define FONT_TTF_H

#include <vector>
#include <unordered_map>
#include <gfx/text/font.h>
#include <gfx/math/box2.h>

namespace gfx::text
{

struct Point
{
    double x;
    double y;
    bool on_curve;
};

struct GlyphTTF
{
    gfx::math::Box2d bbox;
    std::vector<std::vector<Point>> contours;
};

struct ContourEdge
{
    gfx::math::Vec2d v0;
    gfx::math::Vec2d v1;
};

class FontTTF
{

public:

    FontTTF(int units_per_em, double ascent, double descent, double line_gap, int num_glyphs)
        : units_per_em(units_per_em), ascent(ascent), descent(descent), line_gap(line_gap), num_glyphs(num_glyphs) {}

    std::shared_ptr<GlyphTTF> get_glyph(const uint32_t codepoint) const;
    std::shared_ptr<GlyphTTF> get_glyph(const char character) const;

    std::vector<ContourEdge> get_glyph_edges(const uint32_t codepoint) const;

    void set_kerning(const char left, const char right, const double &offset)
    {
        kerning_table[{static_cast<uint32_t>(static_cast<uint8_t>(left)), static_cast<uint32_t>(static_cast<uint8_t>(right))}] = offset;
    }
    double get_kerning(const char left, const char right) const
    {
        return kerning_table.count({static_cast<uint32_t>(static_cast<uint8_t>(left)), static_cast<uint32_t>(static_cast<uint8_t>(right))}) ?
            kerning_table.at({static_cast<uint32_t>(static_cast<uint8_t>(left)), static_cast<uint32_t>(static_cast<uint8_t>(right))}) : 0.0;
    }

    double get_ascent() const { return ascent; }
    double get_descent() const { return descent; }
    double get_line_gap() const { return line_gap; }
    double get_line_height() const { return ascent - descent + line_gap; }
    double get_units_per_em() const { return units_per_em; }

    void set_glyphs(const std::unordered_map<uint32_t, std::shared_ptr<GlyphTTF>> &g) { glyphs = g; }
    std::unordered_map<uint32_t, std::shared_ptr<GlyphTTF>> get_glyphs() const { return glyphs; }

private:

    std::vector<ContourEdge> flatten_glyph(const std::shared_ptr<GlyphTTF> glyph) const;

    int units_per_em;
    double ascent;
    double descent;
    double line_gap;
    int num_glyphs;

    struct PairHash {
        std::size_t operator()(const std::pair<int,int>& p) const 
        {
            return std::hash<int>()(p.first) ^ (std::hash<int>()(p.second) << 1);
        }
    };

    std::unordered_map<uint32_t, std::shared_ptr<GlyphTTF>> glyphs;
    std::unordered_map<std::pair<uint32_t, uint32_t>, double, PairHash> kerning_table;
    mutable std::unordered_map<uint32_t, std::vector<ContourEdge>> vertex_cache;

};

}

#endif // FONT_TTF_H
