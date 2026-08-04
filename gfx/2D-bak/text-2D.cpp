#include "text-2D.h"

#include <utility>

#include "rasterize.h"
#include "gfx/geometry/transform-2D.h"
#include "triangulate.h"
#include "gfx/geometry/types/polygon.h"
#include "utf-8.h"


namespace gfx
{
    Primitive2D::RasterizeOutput Text2D::rasterize(const Matrix3x3d& transform) const
    {
        RasterizeOutput output;

        const double scale = _font_size / _font->get_units_per_em();
        const double ascent = _font->get_ascent() * scale;
        const double line_gap = _font->get_line_gap() * scale;

        const double line_height = line_gap > 0.0 ? _font_size + line_gap : _font_size * _line_height_multiplier;

        Vec2d pen { Vec2d::zero() };

        auto min { Vec2d(std::numeric_limits<double>::max()) };
        auto max { Vec2d(std::numeric_limits<double>::lowest()) };

        std::vector line_widths { 0.0 };
        int line_index = 0;

        size_t i = 0;
        while (i < _text.size())
        {
            uint32_t codepoint;
            size_t bytes;

            if (!decode_utf8(_text, i, codepoint, bytes))
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
                decode_utf8(_text, i - 1, prev_cp, prev_bytes);
                pen.x += _font->get_kerning(prev_cp, codepoint) * scale;
                line_widths[line_index] = pen.x;
            }

            for (auto edges = _font->get_glyph_edges(codepoint); auto& edge : edges)
            {
                Vec2d v0 = edge.v0 * scale;
                Vec2d v1 = edge.v1 * scale;

                v0.x += pen.x;
                v1.x += pen.x;
                v0.y = -v0.y + ascent + pen.y;
                v1.y = -v1.y + ascent + pen.y;

                min.x = std::min({ min.x, v0.x, v1.x });
                min.y = std::min({ min.y, v0.y, v1.y });

                max.x = std::max({ max.x, v0.x, v1.x });
                max.y = std::max({ max.y, v0.y, v1.y });
            }

            pen.x += _font->get_glyph_advance(codepoint) * scale;
            line_widths[line_index] = pen.x;
            i += bytes;
        }

        pen = Vec2d { 0.0, 0.0 };
        line_index = 0;
        i = 0;
        while (i < _text.size())
        {
            uint32_t codepoint;
            size_t bytes;

            if (!decode_utf8(_text, i, codepoint, bytes))
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
                decode_utf8(_text, i - 1, prev_cp, prev_bytes);
                pen.x += _font->get_kerning(prev_cp, codepoint) * scale;
            }

            const double offset_x {
                [&] {
                    switch (_alignment)
                    {
                    case TextAlignment::LEFT:
                        return 0.0;
                    case TextAlignment::RIGHT:
                        return max.x - line_widths[line_index];
                    case TextAlignment::CENTER:
                        return (max.x - line_widths[line_index]) / 2.0;
                    }
                    std::unreachable();
                }()
            };

            // auto edges = _font->get_glyph_edges_multi_contour(codepoint);
            // for (auto& contour : edges)
            // {
            //     for (auto& [v0, v1] : contour)
            //     {
            //         v0 = v0 * scale;
            //         v1 = v1 * scale;
            //
            //         v0.x += pen.x - min.x + offset_x;
            //         v1.x += pen.x - min.x + offset_x;
            //
            //         v0.y = -v0.y + ascent + pen.y - min.y;
            //         v1.y = -v1.y + ascent + pen.y - min.y;
            //
            //         v0 = Transform2D::transform_point(v0, transform);
            //         v1 = Transform2D::transform_point(v1, transform);
            //     }
            // }

            // rasterize_glyph_triangulation(edges, output.pixels);
            auto edges = _font->get_glyph_edges(codepoint);
            for (auto& [v0, v1] : edges)
            {
                v0 = v0 * scale;
                v1 = v1 * scale;
            
                v0.x += pen.x - min.x + offset_x;
                v1.x += pen.x - min.x + offset_x;
            
                v0.y = -v0.y + ascent + pen.y - min.y;
                v1.y = -v1.y + ascent + pen.y - min.y;
            
                v0 = Transform2D::transform_point(v0, transform);
                v1 = Transform2D::transform_point(v1, transform);
            }

            if (_smoothing_radius > 0.0)
            {
                rasterize_glyph_sdf(edges, output);
            }
            else
            {
                rasterize_glyph(edges, output.pixels);
            }

            pen.x += _font->get_glyph_advance(codepoint) * scale;
            i += bytes;
        }

        if (_smoothing_radius > 0.0)
        {
            output.use_alphas = true;
        }

        return output;
    }

    Box2d Text2D::get_geometry_size() const
    {
        const double scale = _font_size / _font->get_units_per_em();
        const double ascent = _font->get_ascent() * scale;
        const double line_gap = _font->get_line_gap() * scale;

        const double line_height = line_gap > 0.0 ? _font_size + line_gap : _font_size * _line_height_multiplier;

        Box2d bounds { Vec2d(std::numeric_limits<double>::max()), Vec2d(std::numeric_limits<double>::lowest()) };

        Vec2d pen { 0.0, 0.0 };

        size_t i = 0;
        uint32_t prev_codepoint = 0;

        while (i < _text.size())
        {
            uint32_t codepoint;
            size_t bytes;

            if (!decode_utf8(_text, i, codepoint, bytes))
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
                pen.x += _font->get_kerning(prev_codepoint, codepoint) * scale;
            }

            for (auto edges = _font->get_glyph_edges(codepoint); auto& edge : edges)
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

            pen.x += _font->get_glyph_advance(codepoint) * scale;
            prev_codepoint = codepoint;
            i += bytes;
        }

        return Box2d { Vec2d::zero(), bounds.size() };
    }


    void Text2D::set_text(const std::string& new_text)
    {
        _text = new_text;
        set_edges_dirty();
        set_size_dirty();
    }

    void Text2D::set_font(const std::shared_ptr<FontTTF> new_font)
    {
        _font = new_font;
        set_edges_dirty();
        set_size_dirty();
    }

    void Text2D::set_font_size(const double new_font_size)
    {
        _font_size = new_font_size;
        set_edges_dirty();
        set_size_dirty();
    }

    void Text2D::set_alignment(const TextAlignment new_alignment)
    {
        _alignment = new_alignment;
        set_size_dirty();
    }

    Text2D::TextAlignment Text2D::get_alignment() const
    {
        return _alignment;
    }


    void Text2D::set_smoothing_radius(const double radius)
    {
        _smoothing_radius = radius;
    }

    double Text2D::get_smoothing_radius() const
    {
        return _smoothing_radius;
    }

    void Text2D::set_line_height_multiplier(const double multiplier)
    {
        _line_height_multiplier = multiplier;
        set_size_dirty();
    }

    double Text2D::get_line_height_multiplier() const
    {
        return _line_height_multiplier;
    }

    static bool is_clockwise(std::vector<Vec2d> vertices)
    {
        double sum = 0.0;
        for (int i = 0; i < vertices.size(); ++i)
        {
            const Vec2d p0 { vertices[i] };
            const Vec2d p1 { vertices[(i + 1) % vertices.size()] };
            sum += (p1.x - p0.x) * (p1.y + p0.y);
        }
        return sum < 0.0;
    }

    static void rasterize_edge(const Vec2d start, const Vec2d end, std::vector<Vec2i>& pixels)
    {
        const double line_extent { 0.5 };
        const Vec2d normal { (end - start).normal().normalize() };

        const Vec2d offset { normal * line_extent };

        const Vec2d v0 { start + offset };
        const Vec2d v1 { start - offset };
        const Vec2d v2 { end + offset };
        const Vec2d v3 { end - offset };

        Rasterize::rasterize_filled_triangle({ v0, v1, v2 }, pixels);
        Rasterize::rasterize_filled_triangle({ v1, v3, v2 }, pixels);
    }

    void Text2D::rasterize_glyph_triangulation(
        const std::vector<std::vector<FontTTF::ContourEdge>>& glyph,
        std::vector<Vec2i>& pixels
    )
    {
        std::vector<Polygon<double>> polygons;
        std::vector<std::vector<Vec2d>> holes;
        std::vector<Box2d> polygon_bounds;
        std::vector<Box2d> holes_bounds;

        for (const auto& contour : glyph)
        {
            // std::vector<Vec2d> vertices;
            // for (const auto& vertex : contour)
            // {
            //     vertices.push_back(vertex.v0);
            //     vertices.push_back(vertex.v1);
            // }
            // polygons.push_back(vertices);

            std::vector<Vec2d> points;
            Box2d bounds;
            for (const auto& [v0, v1] : contour)
            {
                points.push_back(v0);
                points.push_back(v1);
                bounds.expand(v0);
                bounds.expand(v1);
            }
            if (!is_clockwise(points))
            {
                holes.push_back(points);
                holes_bounds.push_back(bounds);
                polygons.back().holes.push_back(Polygon<double>::Contour(points, false));
            }
            else
            {
                polygons.push_back(Polygon<double>(points, true));
                polygon_bounds.push_back(bounds);
            }
        }
        for (const auto& polygon : polygons)
        {
            const auto& triangles = Triangulate::triangulate_polygon(polygon);
            
            for (int i = 0; i < polygon.contour.vertices.size() - 1; ++i)
            {
                // rasterize_edge(polygon.contour.vertices[i], polygon.contour.vertices[i + 1], pixels);
            }

            for (const auto& tri : triangles)
            {
                // rasterize_edge(tri.v0, tri.v1, pixels);
                // rasterize_edge(tri.v1, tri.v2, pixels);
                // rasterize_edge(tri.v2, tri.v0, pixels);
                Rasterize::rasterize_filled_triangle(tri, pixels);
            }
        }

        // std::vector<std::vector<std::vector<Vec2d>>> poly_to_holes(polygons.size());
        //
        // for (int i = 0; i < holes.size(); ++i)
        // {
        //     for (int j = 0; j < polygons.size(); ++j)
        //     {
        //         if (polygon_bounds[j].contains(holes_bounds[i]))
        //         {
        //             poly_to_holes[j].push_back(holes[i]);
        //             break;
        //         }
        //     }
        // }
        //
        // for (int i = 0; i < poly_to_holes.size(); ++i)
        // {
        //     std::vector<Polygon<double>::Contour> poly_holes;
        //     for (int j = 0; j < poly_to_holes[i].size(); ++j)
        //     {
        //         poly_holes.push_back(Polygon<double>::Contour(poly_to_holes[i][j], false));
        //     }
        //     auto component = Polygon<double>(polygons[i], true, poly_holes);
        //     auto triangles = Triangulate::triangulate_polygon(component);
        //
        //     for (int j = 0; j < triangles.size(); ++j)
        //     {
        //         Rasterize::rasterize_filled_triangle(triangles[i], pixels);
        //     }
        // }
    }

    void Text2D::rasterize_glyph_sdf(const std::vector<FontTTF::ContourEdge>& glyph, RasterizeOutput& output) const
    {
        if (glyph.empty())
        {
            return;
        }

        const std::vector<EdgeData> edges = preprocess_edges(glyph);

        auto bounds = Box2i::unexpanded();
        for (const auto& [v0, v1] : glyph)
        {
            bounds.expand(static_cast<Vec2<int>>(v0));
            bounds.expand(static_cast<Vec2<int>>(v1));
        }

        for (int y = bounds.min.y - 1; y <= bounds.max.y + 1; ++y)
        {
            for (int x = bounds.min.x - 1; x <= bounds.max.x + 1; ++x)
            {
                Vec2d p { x + 0.5, y + 0.5 };

                int winding = 0;
                for (const auto& [p0, p1, dir, normal, inv_length_sq] : edges)
                {
                    const bool intersects_y { p0.y > p.y != p1.y > p.y };
                    if (intersects_y)
                    {
                        const double t = (p.y - p0.y) / (p1.y - p0.y);
                        const double x_cross = p0.x + t * (p1.x - p0.x);
                        if (p.x < x_cross)
                        {
                            winding += p1.y > p0.y ? 1 : -1;
                        }
                    }
                }
                const bool inside = winding != 0;

                const double signed_distance { signed_distance_to_glyph(edges, p, inside) };

                const double coverage = coverage_from_sdf(signed_distance);

                if (coverage > 0.0)
                {
                    output.pixels.push_back(Vec2i { x, y });
                    output.alphas.push_back(coverage_to_alpha(coverage));
                }
            }
        }
    }

    void Text2D::rasterize_glyph(const std::vector<FontTTF::ContourEdge>& glyph, std::vector<Vec2i>& pixels)
    {
        if (glyph.empty())
        {
            return;
        }

        std::vector<EdgeData> edges = preprocess_edges(glyph);

        auto bounds { Box2i::unexpanded() };

        for (const auto& [v0, v1] : glyph)
        {
            bounds.expand(static_cast<Vec2<int>>(v0));
            bounds.expand(static_cast<Vec2<int>>(v1));
        }

        const std::vector edge_table { build_edge_table(glyph, bounds) };

        std::vector<ETEntry> active_table;

        for (int y = bounds.min.y; y <= bounds.max.y; ++y)
        {
            if (y - bounds.min.y < edge_table.size())
            {
                for (const auto& edge : edge_table[y - bounds.min.y])
                {
                    active_table.push_back(edge);
                }
            }

            std::erase_if(
                active_table,
                [y](const ETEntry& e) {
                    return y >= e.y_max;
                }
            );

            std::ranges::sort(
                active_table,
                [](const ETEntry& a, const ETEntry& b) {
                    return a.x < b.x;
                }
            );

            for (int i = 0; i + 1 < active_table.size(); i += 2)
            {
                const int x0 = static_cast<int>(std::ceil(active_table[i].x));
                const int x1 = static_cast<int>(std::floor(active_table[i + 1].x));

                for (int x = x0; x <= x1; ++x)
                {
                    pixels.push_back(Vec2i { x, y });
                }
            }

            for (auto& edge : active_table)
            {
                edge.x += edge.dx;
            }
        }
    }

    std::vector<Text2D::EdgeData> Text2D::preprocess_edges(const std::vector<FontTTF::ContourEdge>& edges)
    {
        std::vector<EdgeData> data;
        data.reserve(edges.size());

        for (const auto& [v0, v1] : edges)
        {
            Vec2d v { v1 - v0 };
            const double len2 { Vec2d::dot(v, v) };
            double inv_len { 1.0 / std::sqrt(len2) };

            const Vec2d dir { v * inv_len };
            const Vec2d normal { -dir.y, dir.x };

            data.emplace_back(EdgeData { v0, v1, dir, normal, 1.0 / len2 });
        }

        return data;
    }

    std::vector<std::vector<Text2D::ETEntry>> Text2D::build_edge_table(
        const std::vector<FontTTF::ContourEdge>& edges,
        const Box2i& bounds
    )
    {
        std::vector<std::vector<ETEntry>> edge_table(bounds.max.y - bounds.min.y + 1);

        for (const auto& [v0, v1] : edges)
        {
            double y0 = v0.y;
            double y1 = v1.y;
            double x0 = v0.x;
            double x1 = v1.x;

            if (y0 == y1)
            {
                continue;
            }

            if (y0 > y1)
            {
                std::swap(y0, y1);
                std::swap(x0, x1);
            }

            int y_start = static_cast<int>(std::ceil(y0));
            int y_end = static_cast<int>(std::ceil(y1));

            if (y_end <= bounds.min.y || y_start > bounds.max.y)
            {
                continue;
            }

            y_start = std::max(y_start, bounds.min.y);
            y_end = std::min(y_end, bounds.max.y + 1);

            const double dy = y1 - y0;
            const double dx = (x1 - x0) / dy;

            const double intersection = x0 + (y_start - y0) * dx;

            ETEntry entry { .x = intersection, .dx = dx, .y_max = y_end };

            edge_table[y_start - bounds.min.y].push_back(entry);
        }

        for (auto& entries : edge_table)
        {
            std::ranges::sort(
                entries,
                [](const ETEntry& a, const ETEntry& b) {
                    return a.x < b.x;
                }
            );
        }

        return edge_table;
    }

    bool Text2D::point_inside_glyph(const std::vector<ETEntry>& edges, const Vec2d point)
    {
        if (edges.empty())
        {
            return false;
        }

        bool inside { false };
        for (size_t i = 0; i < edges.size(); i += 2)
        {
            if (point.x >= edges[i].x && point.x < edges[i + 1].x)
            {
                inside = true;
                break;
            }
        }

        return inside;
    }

    double Text2D::dist_to_segment(const EdgeData& edge_data, const Vec2d point)
    {
        const Vec2d w { point - edge_data.p0 };
        const double t { std::clamp(Vec2d::dot(w, edge_data.dir), 0.0, (edge_data.p1 - edge_data.p0).length()) };

        const Vec2d dist { point - (edge_data.p0 + edge_data.dir * t) };

        return std::sqrt(Vec2d::dot(dist, dist));
    }

    double Text2D::signed_distance_to_glyph(const std::vector<EdgeData>& edges, const Vec2d& point, const bool inside)
    {
        double dist { std::numeric_limits<double>::max() };

        for (const auto& edge : edges)
        {
            dist = std::min(dist, dist_to_segment(edge, point));
        }

        return inside ? -dist : dist;
    }

    double Text2D::coverage_from_sdf(const double signed_distance) const
    {
        const double a { _smoothing_radius - signed_distance / (2.0 * _smoothing_radius) };
        return std::clamp(a, 0.0, 1.0);
    }

    double Text2D::coverage_to_alpha(const double coverage)
    {
        // return coverage;
        // return std::pow(coverage, 1.5);
        return coverage * coverage * (3.0 - 2.0 * coverage);
    }

    void Text2D::set_edges_dirty() const
    {
        _edges_dirty = true;
    }

    void Text2D::set_size_dirty() const
    {
        _size_dirty = true;
    }
}
