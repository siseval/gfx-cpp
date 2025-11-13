#ifndef TEXT_2D_H
#define TEXT_2D_H

#include <gfx/core/primitive-2D.h>
#include <gfx/text/font-ttf.h>

namespace gfx::primitives
{

class Text2D : public gfx::core::Primitive2D
{

public:

    enum class TextAlignment
    {
        LEFT,
        CENTER,
        RIGHT
    };

    void rasterize(const gfx::math::Matrix3x3d &transform, const std::function<void(const gfx::core::types::Pixel&)> emit_pixel) const override;
    gfx::math::Box2d get_geometry_size() const override;
    bool point_collides(const gfx::math::Vec2d point, const gfx::math::Matrix3x3d &transform) const override { return false; }

    void set_text(const std::string &new_text) 
    { 
        text = new_text; 
        set_edges_dirty();
        set_size_dirty();
    }

    void set_font(const std::shared_ptr<gfx::text::FontTTF> new_font) 
    { 
        font = new_font; 
        set_edges_dirty();
        set_size_dirty();
    }

    void set_font_size(const double new_font_size) 
    { 
        font_size = new_font_size; 
        set_edges_dirty();
        set_size_dirty();
    }

    void set_alignment(const TextAlignment new_alignment) 
    { 
        alignment = new_alignment; 
        set_size_dirty();
    }

    TextAlignment get_alignment() const { return alignment; }

    inline void set_line_height_multiplier(const double multiplier) 
    { 
        line_height_multiplier = multiplier; 
        set_size_dirty();
    }

    inline double get_line_height_multiplier() const { return line_height_multiplier; }

private:

    void rasterize_glyph(std::vector<gfx::text::ContourEdge> glyph, const std::function<void(const gfx::core::types::Pixel&)> emit_pixel) const;

    void set_edges_dirty() { edges_dirty = true; }
    void set_size_dirty() { size_dirty = true; }

    TextAlignment alignment = TextAlignment::LEFT;

    gfx::math::Vec2d text_box { -1.0, -1.0 };

    std::string text;
    std::shared_ptr<gfx::text::FontTTF> font;

    double font_size;
    double line_height_multiplier = 1.2;

    mutable bool edges_dirty = true;
    mutable bool size_dirty = true;

    mutable gfx::math::Box2d cached_geometry_size;
    mutable std::unordered_map<uint32_t, std::vector<gfx::text::ContourEdge>> cached_glyph_edges;
 };

}

#endif // TEXT_2D_H
