#ifndef TEXT_2D_H
#define TEXT_2D_H

#include <string>
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

    void set_text(const std::string &new_text) { text = new_text; }
    void set_font(const std::shared_ptr<gfx::text::FontTTF> new_font) { font = new_font; }
    void set_font_size(const double new_font_size) { font_size = new_font_size; }

private:

    void rasterize_glyph(std::vector<gfx::text::ContourEdge> glyph, const std::function<void(const gfx::core::types::Pixel&)> emit_pixel) const;

    std::string text;
    std::shared_ptr<gfx::text::FontTTF> font;
    double font_size;
};

}

#endif // TEXT_2D_H
