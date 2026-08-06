#include "gfx/primitives/text-2D.h"

#include <ranges>
#include <utility>

#include "gfx/geometry/triangulate.h"
#include "gfx/geometry/types/polygon.h"
#include "gfx/text/utf-8.h"


namespace gfx
{
    void Text2D::set_text(const std::string& new_text)
    {
        _text = new_text;
        _mesh_dirty = true;
    }

    void Text2D::set_font(const std::shared_ptr<FontTTF> new_font)
    {
        _font = new_font;
        _mesh_dirty = true;
    }

    void Text2D::set_font_size(const double new_font_size)
    {
        _font_size = new_font_size;
        _mesh_dirty = true;
    }

    void Text2D::set_alignment(const TextAlignment new_alignment)
    {
        _alignment = new_alignment;
        _mesh_dirty = true;
    }

    Text2D::TextAlignment Text2D::get_alignment() const
    {
        return _alignment;
    }

    void Text2D::set_line_height_multiplier(const double multiplier)
    {
        _line_height_multiplier = multiplier;
        _mesh_dirty = true;
    }

    double Text2D::get_line_height_multiplier() const
    {
        return _line_height_multiplier;
    }

    void Text2D::generate_mesh() const
    {
        std::vector<Vec2d> vertices;
        std::vector<TriangleMesh<Vec2d>::Face> faces;

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

            auto edges = _font->get_glyph_edges_multi_contour(codepoint);
            for (auto& contour : edges)
            {
                for (auto& [v0, v1] : contour)
                {
                    v0 = v0 * scale;
                    v1 = v1 * scale;

                    v0.x += pen.x - min.x + offset_x;
                    v1.x += pen.x - min.x + offset_x;

                    v0.y = -v0.y + ascent + pen.y - min.y;
                    v1.y = -v1.y + ascent + pen.y - min.y;
                }
            }

            triangulate_glyph(edges, vertices, faces);

            pen.x += _font->get_glyph_advance(codepoint) * scale;
            i += bytes;
        }

        _mesh_data = TriangleMesh<Vec2d>();
        _mesh_data.set_vertices(std::move(vertices));
        _mesh_data.set_faces(std::move(faces));
        _mesh_data.calculate_uvs();

        _mesh_dirty = false;
    }

    static bool is_clockwise(const std::vector<Vec2d>& vertices)
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

    void Text2D::triangulate_glyph(
        const std::vector<std::vector<FontTTF::ContourEdge>>& glyph,
        std::vector<Vec2d>& vertices,
        std::vector<TriangleMesh<Vec2d>::Face>& faces
    )
    {
        std::vector<Polygon<double>> polygons;
        std::vector<std::vector<Vec2d>> holes;
        std::vector<Box2d> polygon_bounds;
        std::vector<Box2d> holes_bounds;

        for (const auto& contour : glyph)
        {
            std::vector<Vec2d> points;
            Box2d bounds;
            for (const auto& [v0, v1] : contour)
            {
                points.push_back(v0);
                bounds.expand(v0);
            }
            if (!is_clockwise(points))
            {
                holes.push_back(std::move(points));
                holes_bounds.push_back(std::move(bounds));
            }
            else
            {
                polygons.push_back(Polygon<double>(std::move(points), true));
                polygon_bounds.push_back(std::move(bounds));
            }
        }

        for (size_t i = 0; i < holes.size(); ++i)
        {
            for (size_t j = 0; j < polygons.size(); ++j)
            {
                if (holes_bounds[i].intersects(polygon_bounds[j]))
                {
                    polygons[j].holes.push_back(Polygon<double>::Contour(std::move(holes[i]), false));
                    break;
                }
            }
        }

        for (const auto& polygon : polygons)
        {
            const auto& mesh = Triangulate::triangulate_polygon(polygon);

            const size_t index_offset = vertices.size();

            vertices.insert(
                vertices.end(),
                std::make_move_iterator(mesh.get_vertices().begin()),
                std::make_move_iterator(mesh.get_vertices().end())
            );

            for (const auto& face : mesh.get_faces())
            {
                faces.emplace_back(
                    face.v0 + index_offset,
                    face.v1 + index_offset,
                    face.v2 + index_offset
                );
            }
        }
    }
}
