#pragma once

#include "gfx/core/primitive-2D.h"
#include "gfx/math/box2.h"
#include "gfx/math/matrix.h"
#include "gfx/math/vec2.h"
#include "../../../2D-bak/font-ttf.h"

namespace gfx
{
    class Text2D final : public Primitive2D
    {
    public:

        enum class TextAlignment
        {
            LEFT,
            CENTER,
            RIGHT
        };

        RasterizeOutput rasterize(const Matrix3x3d& transform) const override;

        Box2d get_geometry_size() const override;

        void set_text(const std::string& new_text);
        void set_font(std::shared_ptr<FontTTF> new_font);
        void set_font_size(double new_font_size);
        void set_alignment(TextAlignment new_alignment);

        TextAlignment get_alignment() const;

        void set_smoothing_radius(double radius);
        double get_smoothing_radius() const;

        void set_line_height_multiplier(double multiplier);
        inline double get_line_height_multiplier() const;

        static void rasterize_glyph_triangulation(
            const std::vector<std::vector<FontTTF::ContourEdge>>& glyph,
            std::vector<Vec2i>& pixels
        );
        void rasterize_glyph_sdf(const std::vector<FontTTF::ContourEdge>& glyph, RasterizeOutput& output) const;
        static void rasterize_glyph(const std::vector<FontTTF::ContourEdge>& glyph, std::vector<Vec2i>& pixels);

    private:

        struct EdgeData
        {
            Vec2d p0;
            Vec2d p1;
            Vec2d dir;
            Vec2d normal;
            double inv_length_sq;
        };

        struct ETEntry
        {
            double x;
            double dx;
            int y_max;
        };

        static std::vector<EdgeData> preprocess_edges(const std::vector<FontTTF::ContourEdge>& edges);
        static std::vector<std::vector<ETEntry>> build_edge_table(
            const std::vector<FontTTF::ContourEdge>& edges,
            const Box2i& bounds
        );

        static bool point_inside_glyph(const std::vector<ETEntry>& edges, Vec2d point);
        static double dist_to_segment(const EdgeData& edge_data, Vec2d point);
        static double signed_distance_to_glyph(const std::vector<EdgeData>& edges, const Vec2d& point, bool inside);

        double coverage_from_sdf(double signed_distance) const;
        static double coverage_to_alpha(double coverage);

        void set_edges_dirty() const;
        void set_size_dirty() const;

        TextAlignment _alignment = TextAlignment::LEFT;

        Vec2d _text_box { -1.0, -1.0 };

        std::string _text;
        std::shared_ptr<FontTTF> _font;

        double _font_size { 0 };
        double _line_height_multiplier { 1.2 };

        double _smoothing_radius { 0.0 };

        mutable bool _edges_dirty { true };
        mutable bool _size_dirty { true };

        mutable Box2d _cached_geometry_size;
        mutable std::unordered_map<uint32_t, std::vector<FontTTF::ContourEdge>> _cached_glyph_edges;
    };
}
