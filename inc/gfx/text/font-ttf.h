#ifndef FONT_TTF_H
#define FONT_TTF_H

#include <string>
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

struct GlyphMetrics
{
    int advance_width;
    int left_side_bearing;
};

class FontTTF
{

public:

    FontTTF(int units_per_em, double ascent, double descent, double line_gap, int num_glyphs)
        : units_per_em(units_per_em), ascent(ascent), descent(descent), line_gap(line_gap), num_glyphs(num_glyphs) {}

    std::shared_ptr<GlyphTTF> get_glyph(const uint32_t codepoint) const;

    std::vector<ContourEdge> get_glyph_edges(const uint32_t codepoint) const;

    void set_kerning(const char left, const char right, const int offset)
    {
        kerning_table[{static_cast<uint32_t>(static_cast<uint8_t>(left)), static_cast<uint32_t>(static_cast<uint8_t>(right))}] = offset;
    }
    int get_kerning(const char left, const char right) const
    {
        return kerning_table.count({static_cast<uint32_t>(static_cast<uint8_t>(left)), static_cast<uint32_t>(static_cast<uint8_t>(right))}) ?
            kerning_table.at({static_cast<uint32_t>(static_cast<uint8_t>(left)), static_cast<uint32_t>(static_cast<uint8_t>(right))}) : 0;
    }

    void set_metrics(const uint32_t codepoint, const GlyphMetrics &metrics)
    {
        glyph_metrics[codepoint] = metrics;
    }

    int get_glyph_advance(const uint32_t codepoint) const
    {
        return glyph_metrics.count(codepoint) ? glyph_metrics.at(codepoint).advance_width : 0;
    }
    int get_glyph_left_side_bearing(const uint32_t codepoint) const
    {
        return glyph_metrics.count(codepoint) ? glyph_metrics.at(codepoint).left_side_bearing : 0;
    }

    double get_ascent() const { return ascent; }
    double get_descent() const { return descent; }
    double get_line_gap() const { return line_gap; }
    double get_line_height() const { return ascent - descent + line_gap; }
    double get_units_per_em() const { return units_per_em; }

    inline void set_name(const std::string &n) { name = n; }
    inline std::string get_name() const { return name; }

    void set_glyphs(const std::unordered_map<uint32_t, std::shared_ptr<GlyphTTF>> &g) { glyphs = g; }
    std::unordered_map<uint32_t, std::shared_ptr<GlyphTTF>> get_glyphs() const { return glyphs; }

private:

    std::vector<ContourEdge> flatten_glyph(const std::shared_ptr<GlyphTTF> glyph) const;
    bool decode_utf8(const std::string &s, size_t pos, uint32_t &out_codepoint, size_t &bytes) const;

    std::string name;

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
    std::unordered_map<uint32_t, GlyphMetrics> glyph_metrics;
    std::unordered_map<std::pair<uint32_t, uint32_t>, int, PairHash> kerning_table;

    mutable std::unordered_map<uint32_t, std::vector<ContourEdge>> vertex_cache;

};

}

#endif // FONT_TTF_H
