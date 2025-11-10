#ifndef TEXT_2D_H
#define TEXT_2D_H

#include <gfx/core/primitive-2D.h>
#include <gfx/text/font-ttf.h>

namespace gfx::primitives
{

class Text2D : public gfx::core::Primitive2D
{

public:

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

private:

    void rasterize_glyph(std::vector<gfx::text::ContourEdge> glyph, const std::function<void(const gfx::core::types::Pixel&)> emit_pixel) const;

    void set_edges_dirty() { edges_dirty = true; }
    void set_size_dirty() { size_dirty = true; }

    gfx::math::Vec2d text_box { -1.0, -1.0 };

    mutable bool edges_dirty = true;
    mutable bool size_dirty = true;

    mutable gfx::math::Box2d cached_geometry_size;
    mutable std::unordered_map<uint32_t, std::vector<gfx::text::ContourEdge>> cached_glyph_edges;

    std::string text;
    std::shared_ptr<gfx::text::FontTTF> font;
    double font_size;
};

}

#endif // TEXT_2D_H
