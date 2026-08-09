#pragma once

#include <map>
#include <memory>
#include <mutex>

#include "gfx/core/primitive.h"
#include "gfx/core/render-surface.h"
#include "gfx/core/scene-graph.h"
#include "gfx/core/thread-pool.h"
#include "gfx/core/view/projection.h"
#include "gfx/core/view/view.h"

namespace gfx
{
    template <typename VectorType>
    class RenderLayer
    {
    public:

        struct Settings
        {
            bool multicore_vertex_transformation { true };
            bool multicore_rasterization { true };

            bool draw_wireframes { false };
            Color4 wireframe_color { Color4::white() };

            Texture::FilteringMode texture_filtering_mode { Texture::FilteringMode::NEAREST };
        };

        using MatrixType = Transform<VectorType>::MatrixType;

        explicit RenderLayer(const Viewport& viewport);

        void draw_frame(
            RenderSurface& render_surface,
            const View<VectorType>& view,
            const Projection<VectorType>& projection
        ) const;

        std::shared_ptr<SceneGraph<VectorType>> get_scene_graph() const;
        Viewport& get_viewport();

        int get_num_triangles() const;

        void add_fullscreen_shader(std::shared_ptr<FragmentShader> shader);
        void remove_fullscreen_shader(std::shared_ptr<FragmentShader> shader);
        void clear_fullscreen_shaders();

        /* Immediate mode drawing */

        void draw_line(
            Vec2d a,
            Vec2d b,
            RenderSurface& surface,
            Color4 color = Color4::white(),
            double depth = 0.0
        ) const;


        void draw_circle(
            Vec2d center,
            double radius,
            RenderSurface& surface,
            Color4 color = Color4::white(),
            double depth = 0.0
        ) const;


        Settings settings;

    private:

        using DrawQueue = std::vector<std::pair<std::shared_ptr<Primitive<VectorType>>, Transform<VectorType>>>;
        using VertexIn = VertexShader<VectorType>::Input;
        using VertexOut = VertexShader<VectorType>::Output;
        using VertexUniforms = VertexShader<VectorType>::Uniforms;

        static constexpr int TILE_SIZE = 32;

        struct VertexBufferIndexFace
        {
            size_t v0;
            size_t v1;
            size_t v2;
        };

        struct ClipVertex
        {
            VertexOut vertex_out;
            Vec2d uv;
            Color4 color = Color4::white();
        };

        struct ClipTriangle
        {
            ClipVertex v0;
            ClipVertex v1;
            ClipVertex v2;
        };

        struct ScreenVertex
        {
            Vec2d pos;
            Vec3d normal;
            Vec2d uv;
            Color4 color;
            double z_over_w;
            double inv_w;
        };

        struct ScreenTriangle
        {
            ScreenVertex v0;
            ScreenVertex v1;
            ScreenVertex v2;
            int material_id;
        };

        struct Tile
        {
            Vec2i screen_pos;
            std::map<int, std::vector<ScreenTriangle>> material_batches;
            std::array<int, TILE_SIZE * TILE_SIZE> triangle_index_buffer;
            std::array<int, TILE_SIZE * TILE_SIZE> material_id_buffer;
            std::array<double, TILE_SIZE * TILE_SIZE> depth_buffer;

            explicit Tile(const Vec2i screen_pos)
                : screen_pos(screen_pos)
                , triangle_index_buffer()
                , material_id_buffer()
            {
                depth_buffer.fill(std::numeric_limits<float>::infinity());
            }
        };


        void build_material_map(const DrawQueue& draw_queue) const;

        void flatten_vertices(const DrawQueue& draw_queue) const;
        void transform_vertices(const DrawQueue& draw_queue, const MatrixType& vp_matrix) const;
        void construct_screen_triangles(const DrawQueue& draw_queue) const;

        ScreenVertex clip_to_screen(const ClipVertex& clip_vertex, double z) const;

        void generate_tiles() const;
        void bin_triangles() const;

        void render_tile(Tile& tile, RenderSurface& render_surface) const;
        static void rasterize_triangle_in_tile(const ScreenTriangle& triangle, int tri_index, Tile& tile);

        VertexShader<VectorType>::Output default_vertex_shader(
            const VertexShader<VectorType>::Input& input,
            const VertexShader<VectorType>::Uniforms& uniforms
        ) const;

        void draw_wireframes(RenderSurface& render_surface) const;

        /* 3D specific culling */

        int num_inside_near_plane(const VertexBufferIndexFace& clip_triangle) const;

        int clip_against_near_plane(
            const VertexBufferIndexFace& clip_triangle,
            std::array<ClipTriangle, 2>& new_triangles
        ) const requires std::same_as<VectorType, Vec3d>;

        static bool is_backface(const ScreenTriangle& tri);
        Viewport _viewport;

        std::shared_ptr<SceneGraph<VectorType>> _scene_graph;
        std::shared_ptr<ThreadPool> _thread_pool;
        std::vector<std::shared_ptr<FragmentShader>> _fullscreen_shaders;

        mutable double _frame_start_timestamp { 0.0 };

        mutable std::unordered_map<size_t, std::shared_ptr<Material>> _material_map;

        mutable std::vector<typename VertexShader<VectorType>::Input> _vertex_in_buffer;
        mutable std::vector<ClipVertex> _vertex_out_buffer;

        mutable std::vector<VertexBufferIndexFace> _index_faces;
        mutable std::vector<size_t> _index_face_to_material_id_map;

        mutable std::vector<size_t> _vertex_to_item_map;
        mutable std::vector<size_t> _vertex_to_index_map;
        mutable std::vector<size_t> _item_to_vertex_chunk_start_map;

        mutable std::vector<VertexUniforms> _vertex_uniforms_buffer;

        mutable std::vector<ScreenTriangle> _screen_triangles;

        mutable Vec2i _last_resolution { 0, 0 };
        mutable std::vector<Tile> _tiles;
    };

    template <typename VectorType>
    RenderLayer<VectorType>::RenderLayer(const Viewport& viewport)
        : _viewport(viewport)
        , _scene_graph(std::make_shared<SceneGraph<VectorType>>())
        , _thread_pool(std::make_shared<ThreadPool>(std::thread::hardware_concurrency())) {}

    template <typename VectorType>
    void RenderLayer<VectorType>::draw_frame(
        RenderSurface& render_surface,
        const View<VectorType>& view,
        const Projection<VectorType>& projection
    ) const
    {
        _frame_start_timestamp =
            std::chrono::duration<double, std::milli>(
                std::chrono::high_resolution_clock::now().time_since_epoch()
            ).count() / 1000.0;

        const DrawQueue& draw_queue {
            _scene_graph->get_draw_queue(
                projection.get_view_bounds(view, _viewport)
            )
        };
        const MatrixType vp_matrix {
            projection.get_matrix(_viewport.get_aspect_ratio()) *
            view.get_matrix()
        };

        build_material_map(draw_queue);

        flatten_vertices(draw_queue);
        transform_vertices(draw_queue, vp_matrix);
        construct_screen_triangles(draw_queue);

        generate_tiles();
        bin_triangles();

        if (settings.multicore_rasterization)
        {
            _thread_pool->run(
                static_cast<int>(_tiles.size()),
                [&](const int tile_index) {
                    render_tile(_tiles[tile_index], render_surface);
                }
            );
        }
        else
        {
            for (auto& tile : _tiles)
            {
                render_tile(tile, render_surface);
            }
        }

        if (settings.draw_wireframes)
        {
            draw_wireframes(render_surface);
        }
    }

    template <typename VectorType>
    void RenderLayer<VectorType>::build_material_map(const DrawQueue& draw_queue) const
    {
        _material_map.clear();

        for (const auto& primitive : draw_queue | std::views::keys)
        {
            for (const auto& material : primitive->get_materials())
            {
                if (material)
                {
                    _material_map[material->get_id()] = material;
                }
            }
        }
    }

    template <typename VectorType>
    void RenderLayer<VectorType>::flatten_vertices(const DrawQueue& draw_queue) const
    {
        _vertex_in_buffer.clear();
        _vertex_out_buffer.clear();
        _vertex_to_item_map.clear();
        _item_to_vertex_chunk_start_map.clear();
        _index_faces.clear();
        _index_face_to_material_id_map.clear();

        for (size_t i = 0; i < draw_queue.size(); ++i)
        {
            const size_t item_chunk_start { _vertex_in_buffer.size() };
            _item_to_vertex_chunk_start_map.emplace_back(item_chunk_start);

            const TriangleMesh<VectorType>& mesh { draw_queue[i].first->get_mesh() };

            const std::vector<VectorType>& vertices { mesh.get_vertices() };
            const std::vector<Vec3d>& normals { mesh.get_normals() };
            const std::vector<Vec2d>& uvs { mesh.get_uvs() };
            const std::vector<Color4>& colors { mesh.get_colors() };

            const std::vector<typename TriangleMesh<VectorType>::Face>& faces { mesh.get_faces() };

            for (size_t j = 0; j < vertices.size(); ++j)
            {
                _vertex_in_buffer.emplace_back(
                    vertices[j],
                    normals.size() > j ? normals[j] : Vec3d::zero()
                );
                _vertex_to_item_map.emplace_back(i);
            }

            if (_vertex_out_buffer.size() < _vertex_in_buffer.size())
            {
                _vertex_out_buffer.resize(_vertex_in_buffer.size());
            }

            const size_t uv_count = std::min(uvs.size(), vertices.size());
            for (size_t j = 0; j < uv_count; ++j)
            {
                _vertex_out_buffer[j + item_chunk_start].uv = uvs[j];
            }

            const size_t color_count = std::min(colors.size(), vertices.size());
            for (size_t j = 0; j < color_count; ++j)
            {
                _vertex_out_buffer[j + item_chunk_start].color = colors[j];
            }

            for (const auto& face : faces)
            {
                _index_faces.emplace_back(
                    face.v0 + item_chunk_start,
                    face.v1 + item_chunk_start,
                    face.v2 + item_chunk_start
                );
                _index_face_to_material_id_map.emplace_back(face.material_index);
            }
        }
    }

    template <typename VectorType>
    void RenderLayer<VectorType>::transform_vertices(const DrawQueue& draw_queue, const MatrixType& vp_matrix) const
    {
        _vertex_uniforms_buffer.clear();

        for (size_t i = 0; i < draw_queue.size(); ++i)
        {
            const MatrixType& model_matrix { draw_queue[i].second.get_matrix() };

            _vertex_uniforms_buffer.emplace_back(
                _frame_start_timestamp,
                model_matrix,
                vp_matrix * model_matrix
            );
        }

        for (size_t i = 0; i < _vertex_in_buffer.size(); ++i)
        {
            const size_t draw_queue_index { _vertex_to_item_map[i] };

            const Primitive<VectorType>& item { *draw_queue[draw_queue_index].first };
            const std::shared_ptr<VertexShader<VectorType>> shader { item.get_vertex_shader() };
            const VertexUniforms& uniforms { _vertex_uniforms_buffer[draw_queue_index] };

            _vertex_out_buffer[i].vertex_out =
                shader ?
                shader->vert(_vertex_in_buffer[i], uniforms) :
                default_vertex_shader(_vertex_in_buffer[i], uniforms);
        }
    }

    template <typename VectorType>
    void RenderLayer<VectorType>::construct_screen_triangles(const DrawQueue& draw_queue) const
    {
        _screen_triangles.clear();

        for (size_t j = 0; j < _index_faces.size(); ++j)
        {
            const VertexBufferIndexFace& index_face = _index_faces[j];
            const size_t item_index { _vertex_to_item_map[index_face.v0] };
            const Primitive<VectorType>& item { *draw_queue[item_index].first };

            if constexpr (std::same_as<VectorType, Vec2d>)
            {
                _screen_triangles.emplace_back(
                    clip_to_screen(_vertex_out_buffer[index_face.v0], item.get_depth()),
                    clip_to_screen(_vertex_out_buffer[index_face.v1], item.get_depth()),
                    clip_to_screen(_vertex_out_buffer[index_face.v2], item.get_depth()),
                    item.get_material(_index_face_to_material_id_map[j])->get_id()
                );
            }
            else
            {
                const int num_inside { num_inside_near_plane(index_face) };

                if (num_inside == 0)
                {
                    continue;
                }

                if (num_inside == 3)
                {
                    const ClipVertex& v0 = _vertex_out_buffer[index_face.v0];
                    const ClipVertex& v1 = _vertex_out_buffer[index_face.v1];
                    const ClipVertex& v2 = _vertex_out_buffer[index_face.v2];

                    _screen_triangles.emplace_back(
                        clip_to_screen(v0, v0.vertex_out.pos.z),
                        clip_to_screen(v1, v1.vertex_out.pos.z),
                        clip_to_screen(v2, v2.vertex_out.pos.z),
                        item.get_material(_index_face_to_material_id_map[j])->get_id()
                    );
                    if (is_backface(_screen_triangles.back()))
                    {
                        _screen_triangles.pop_back();
                    }
                    continue;
                }

                std::array<ClipTriangle, 2> new_triangles;
                const int num_returned { clip_against_near_plane(index_face, new_triangles) };

                _screen_triangles.emplace_back(
                    clip_to_screen(new_triangles[0].v0, new_triangles[0].v0.vertex_out.pos.z),
                    clip_to_screen(new_triangles[0].v1, new_triangles[0].v1.vertex_out.pos.z),
                    clip_to_screen(new_triangles[0].v2, new_triangles[0].v2.vertex_out.pos.z),
                    item.get_material(_index_face_to_material_id_map[j])->get_id()
                );
                if (is_backface(_screen_triangles.back()))
                {
                    _screen_triangles.pop_back();
                }

                if (num_returned > 1)
                {
                    _screen_triangles.emplace_back(
                        clip_to_screen(new_triangles[1].v0, new_triangles[1].v0.vertex_out.pos.z),
                        clip_to_screen(new_triangles[1].v1, new_triangles[1].v1.vertex_out.pos.z),
                        clip_to_screen(new_triangles[1].v2, new_triangles[1].v2.vertex_out.pos.z),
                        item.get_material(_index_face_to_material_id_map[j])->get_id()
                    );
                    if (is_backface(_screen_triangles.back()))
                    {
                        _screen_triangles.pop_back();
                    }
                }
            }
        }
    }

    template <typename VectorType>
    RenderLayer<VectorType>::ScreenVertex RenderLayer<VectorType>::clip_to_screen(
        const ClipVertex& clip_vertex,
        const double z
    ) const
    {
        const double inv_w { 1.0 / clip_vertex.vertex_out.w };

        return ScreenVertex {
            .pos = {
                (clip_vertex.vertex_out.pos.x * inv_w * 0.5 + 0.5) * _viewport.size.x,
                (clip_vertex.vertex_out.pos.y * inv_w * 0.5 + 0.5) * _viewport.size.y
            },
            .normal   = clip_vertex.vertex_out.normal,
            .uv       = clip_vertex.uv,
            .color    = clip_vertex.color,
            .z_over_w = z * inv_w,
            .inv_w    = inv_w
        };
    }

    template <typename VectorType>
    void RenderLayer<VectorType>::draw_line(
        const Vec2d a,
        const Vec2d b,
        RenderSurface& surface,
        const Color4 color,
        const double depth
    ) const
    {
        int x0 { static_cast<int>(a.x) };
        int y0 { static_cast<int>(a.y) };
        
        const int x1 { static_cast<int>(b.x) };
        const int y1 { static_cast<int>(b.y) };

        const int dx { std::abs(x1 - x0) };
        const int dy { std::abs(y1 - y0) };

        const int sx { x0 < x1 ? 1 : -1 };
        const int sy { y0 < y1 ? 1 : -1 };

        int err { dx - dy };

        while (true)
        {
            surface.write_pixel(_viewport.offset, Vec2i { x0, y0 }, color, depth);

            if (x0 == x1 && y0 == y1)
            {
                break;
            }

            const int e2 { 2 * err };

            if (e2 > -dy)
            {
                err -= dy;
                x0 += sx;
            }

            if (e2 < dx)
            {
                err += dx;
                y0 += sy;
            }
        }
    }

    template <typename VectorType>
    void RenderLayer<VectorType>::draw_circle(
        const Vec2d center,
        const double radius,
        RenderSurface& surface,
        const Color4 color,
        const double depth
    ) const
    {
        int x { 0 };
        int y { static_cast<int>(radius) };
        int d { 3 - 2 * static_cast<int>(radius) };

        auto draw_symmetric_points = [&](const int cx, const int cy, const int px, const int py) {
            surface.write_pixel(_viewport.offset, Vec2i { cx + px, cy + py }, color, depth);
            surface.write_pixel(_viewport.offset, Vec2i { cx - px, cy + py }, color, depth);
            surface.write_pixel(_viewport.offset, Vec2i { cx + px, cy - py }, color, depth);
            surface.write_pixel(_viewport.offset, Vec2i { cx - px, cy - py }, color, depth);
            surface.write_pixel(_viewport.offset, Vec2i { cx + py, cy + px }, color, depth);
            surface.write_pixel(_viewport.offset, Vec2i { cx - py, cy + px }, color, depth);
            surface.write_pixel(_viewport.offset, Vec2i { cx + py, cy - px }, color, depth);
            surface.write_pixel(_viewport.offset, Vec2i { cx - py, cy - px }, color, depth);
        };

        while (y >= x)
        {
            draw_symmetric_points(center.x, center.y, x, y);

            x++;

            if (d > 0)
            {
                y--;
                d = d + 4 * (x - y) + 10;
            }
            else
            {
                d = d + 4 * x + 6;
            }
        }
    }

    template <typename VectorType>
    std::shared_ptr<SceneGraph<VectorType>> RenderLayer<VectorType>::get_scene_graph() const
    {
        return _scene_graph;
    }

    template <typename VectorType>
    Viewport& RenderLayer<VectorType>::get_viewport()
    {
        return _viewport;
    }

    template <typename VectorType>
    int RenderLayer<VectorType>::get_num_triangles() const
    {
        return _screen_triangles.size();
    }

    template <typename VectorType>
    void RenderLayer<VectorType>::add_fullscreen_shader(const std::shared_ptr<FragmentShader> shader)
    {
        _fullscreen_shaders.push_back(shader);
    }

    template <typename VectorType>
    void RenderLayer<VectorType>::remove_fullscreen_shader(const std::shared_ptr<FragmentShader> shader)
    {
        std::erase(_fullscreen_shaders, shader);
    }

    template <typename VectorType>
    void RenderLayer<VectorType>::clear_fullscreen_shaders()
    {
        _fullscreen_shaders.clear();
    }

    template <typename VectorType>
    void RenderLayer<VectorType>::rasterize_triangle_in_tile(
        const ScreenTriangle& triangle,
        const int tri_index,
        Tile& tile
    )
    {
        Box2d bounds {
            static_cast<Vec2d>(Vec2d {
                std::min({ triangle.v0.pos.x, triangle.v1.pos.x, triangle.v2.pos.x }),
                std::min({ triangle.v0.pos.y, triangle.v1.pos.y, triangle.v2.pos.y })
            }.round_to_int()),
            static_cast<Vec2d>(Vec2d {
                std::max({ triangle.v0.pos.x, triangle.v1.pos.x, triangle.v2.pos.x }),
                std::max({ triangle.v0.pos.y, triangle.v1.pos.y, triangle.v2.pos.y })
            }.round_to_int())
        };

        const Box2d clip_bounds {
            static_cast<Vec2d>(tile.screen_pos),
            static_cast<Vec2d>(tile.screen_pos + Vec2i(TILE_SIZE))
        };

        bounds.min.x = std::max(bounds.min.x, clip_bounds.min.x);
        bounds.min.y = std::max(bounds.min.y, clip_bounds.min.y);
        bounds.max.x = std::min(bounds.max.x, clip_bounds.max.x);
        bounds.max.y = std::min(bounds.max.y, clip_bounds.max.y);

        auto is_top_left = [](const Vec2d& a, const Vec2d& b) {
            return a.y < b.y || (a.y == b.y && a.x > b.x);
        };

        const double a { triangle.v1.pos.y - triangle.v2.pos.y };
        const double b { triangle.v2.pos.x - triangle.v1.pos.x };
        const double c { triangle.v1.pos.x * triangle.v2.pos.y - triangle.v1.pos.y * triangle.v2.pos.x };

        const double d { triangle.v2.pos.y - triangle.v0.pos.y };
        const double e { triangle.v0.pos.x - triangle.v2.pos.x };
        const double f { triangle.v2.pos.x * triangle.v0.pos.y - triangle.v2.pos.y * triangle.v0.pos.x };

        const double g { triangle.v0.pos.y - triangle.v1.pos.y };
        const double h { triangle.v1.pos.x - triangle.v0.pos.x };
        const double i { triangle.v0.pos.x * triangle.v1.pos.y - triangle.v0.pos.y * triangle.v1.pos.x };

        const bool tl0 = is_top_left(triangle.v1.pos, triangle.v2.pos);
        const bool tl1 = is_top_left(triangle.v2.pos, triangle.v0.pos);
        const bool tl2 = is_top_left(triangle.v0.pos, triangle.v1.pos);

        const double area { a * triangle.v0.pos.x + b * triangle.v0.pos.y + c };
        const double inv_area { 1.0f / area };

        if (area == 0.0)
        {
            return;
        }

        Vec3d w_row {
            a * (bounds.min.x + 0.5) + b * (bounds.min.y + 0.5) + c,
            d * (bounds.min.x + 0.5) + e * (bounds.min.y + 0.5) + f,
            g * (bounds.min.x + 0.5) + h * (bounds.min.y + 0.5) + i
        };

        for (int y = std::floor(bounds.min.y); y < std::ceil(bounds.max.y); ++y)
        {
            Vec3d w { w_row };

            for (int x = std::floor(bounds.min.x); x < std::ceil(bounds.max.x); ++x)
            {
                if ((w.x > 0 || (w.x == 0 && tl0)) && (w.y > 0 || (w.y == 0 && tl1)) && (w.z > 0 || (w.z == 0 && tl2)))
                {
                    const double depth {
                        (w.x * triangle.v0.z_over_w + w.y * triangle.v1.z_over_w + w.z * triangle.v2.z_over_w) *
                        inv_area
                    };

                    const int buffer_index { (y - tile.screen_pos.y) * TILE_SIZE + (x - tile.screen_pos.x) };

                    if (depth < tile.depth_buffer[buffer_index])
                    {
                        tile.triangle_index_buffer[buffer_index] = tri_index;
                        tile.material_id_buffer[buffer_index] = triangle.material_id;
                        tile.depth_buffer[buffer_index] = depth;
                    }
                }

                w.x += a;
                w.y += d;
                w.z += g;
            }

            w_row.x += b;
            w_row.y += e;
            w_row.z += h;
        }
    }

    template <typename VectorType>
    void RenderLayer<VectorType>::generate_tiles() const
    {
        const Vec2i resolution { _viewport.size };
        if (resolution == _last_resolution)
        {
            for (auto& tile : _tiles)
            {
                tile.material_batches.clear();
                tile.triangle_index_buffer.fill(0);
                tile.material_id_buffer.fill(0);
                tile.depth_buffer.fill(std::numeric_limits<float>::infinity());
            }
            return;
        }

        _tiles.clear();

        for (int ty = 0; ty < resolution.y; ty += TILE_SIZE)
        {
            for (int tx = 0; tx < resolution.x; tx += TILE_SIZE)
            {
                _tiles.emplace_back(Vec2i { tx, ty });
            }
        }

        _last_resolution = resolution;
    }

    template <typename VectorType>
    void RenderLayer<VectorType>::bin_triangles() const
    {
        const Vec2i resolution { _viewport.size };

        for (const auto& triangle : _screen_triangles)
        {
            const Vec2i v0_tile {
                static_cast<int>(std::floor(triangle.v0.pos.x)) / TILE_SIZE,
                static_cast<int>(std::floor(triangle.v0.pos.y)) / TILE_SIZE
            };
            const Vec2i v1_tile {
                static_cast<int>(std::floor(triangle.v1.pos.x)) / TILE_SIZE,
                static_cast<int>(std::floor(triangle.v1.pos.y)) / TILE_SIZE
            };
            const Vec2i v2_tile {
                static_cast<int>(std::floor(triangle.v2.pos.x)) / TILE_SIZE,
                static_cast<int>(std::floor(triangle.v2.pos.y)) / TILE_SIZE
            };

            const Box2i tile_bounds {
                Vec2i {
                    std::max(0, std::min({ v0_tile.x, v1_tile.x, v2_tile.x })),
                    std::max(0, std::min({ v0_tile.y, v1_tile.y, v2_tile.y }))
                },
                Vec2i {
                    std::min(
                        (resolution.x + TILE_SIZE - 2) / TILE_SIZE - 1,
                        std::max({ v0_tile.x, v1_tile.x, v2_tile.x })
                    ),
                    std::min(
                        (resolution.y + TILE_SIZE - 2) / TILE_SIZE - 1,
                        std::max({ v0_tile.y, v1_tile.y, v2_tile.y })
                    )
                }
            };

            for (int ty = tile_bounds.min.y; ty <= tile_bounds.max.y; ++ty)
            {
                for (int tx = tile_bounds.min.x; tx <= tile_bounds.max.x; ++tx)
                {
                    const int tile_index { ty * ((resolution.x + TILE_SIZE - 1) / TILE_SIZE) + tx };

                    _tiles[tile_index].material_batches[triangle.material_id].push_back(triangle);
                }
            }
        }
    }

    template <typename VectorType>
    void RenderLayer<VectorType>::render_tile(Tile& tile, RenderSurface& render_surface) const
    {
        const double t { _frame_start_timestamp };

        const Vec2i resolution { _viewport.size };
        constexpr int num_pixels { TILE_SIZE * TILE_SIZE };

        for (const auto& tri : tile.material_batches | std::views::values)
        {
            for (int i = 0; i < tri.size(); ++i)
            {
                rasterize_triangle_in_tile(tri[i], i, tile);
            }
        }

        const FragmentShader::Uniforms uniforms {
            .t                 = t,
            .light_dir         = Vec3d { 1.0, 1.0, 1.0 },
            .ambient_intensity = 0.4,
        };

        for (const auto& [material_id, tri] : tile.material_batches)
        {
            const auto& material { _material_map.at(material_id) };
            const auto& texture { material->get_texture() };
            const auto& frag_shader { material->get_fragment_shader() };

            for (int i = 0; i < num_pixels; ++i)
            {
                const double depth { tile.depth_buffer[i] };
                if (depth == std::numeric_limits<float>::infinity())
                {
                    continue;
                }

                if (tile.material_id_buffer[i] != material_id)
                {
                    continue;
                }

                const Vec2d pixel_pos {
                    tile.screen_pos.x + i % TILE_SIZE + 0.5,
                    tile.screen_pos.y + i / TILE_SIZE + 0.5
                };

                if (pixel_pos.x >= resolution.x || pixel_pos.y >= resolution.y)
                {
                    continue;
                }

                const ScreenTriangle& triangle { tri[tile.triangle_index_buffer[i]] };

                const double area {
                    (triangle.v1.pos.x - triangle.v0.pos.x) * (triangle.v2.pos.y - triangle.v0.pos.y) -
                    (triangle.v1.pos.y - triangle.v0.pos.y) * (triangle.v2.pos.x - triangle.v0.pos.x)
                };

                if (area == 0.0)
                {
                    continue;
                }

                const Vec3d w {
                    ((triangle.v1.pos.y - triangle.v2.pos.y) * (pixel_pos.x - triangle.v2.pos.x) +
                     (triangle.v2.pos.x - triangle.v1.pos.x) * (pixel_pos.y - triangle.v2.pos.y)) / area,
                    ((triangle.v2.pos.y - triangle.v0.pos.y) * (pixel_pos.x - triangle.v2.pos.x) +
                     (triangle.v0.pos.x - triangle.v2.pos.x) * (pixel_pos.y - triangle.v2.pos.y)) / area,
                    ((triangle.v0.pos.y - triangle.v1.pos.y) * (pixel_pos.x - triangle.v1.pos.x) +
                     (triangle.v1.pos.x - triangle.v0.pos.x) * (pixel_pos.y - triangle.v1.pos.y)) / area
                };

                const double inv_w {
                    triangle.v0.inv_w * w.x +
                    triangle.v1.inv_w * w.y +
                    triangle.v2.inv_w * w.z
                };

                if (inv_w == 0.0)
                {
                    continue;
                }

                const Vec3d normal_interp {
                    Vec3d(
                        (triangle.v0.normal * w.x * triangle.v0.inv_w +
                         triangle.v1.normal * w.y * triangle.v1.inv_w +
                         triangle.v2.normal * w.z * triangle.v2.inv_w) / inv_w
                    ).normalize()
                };

                const auto color_interp {
                    [inv_w](const ScreenTriangle& screen_triangle, const Vec3d& w) -> Color4 {
                        return screen_triangle.v0.color == screen_triangle.v1.color &&
                               screen_triangle.v1.color == screen_triangle.v2.color ?

                               screen_triangle.v0.color :

                               Color4::trilinear_interp(
                                   screen_triangle.v0.color,
                                   screen_triangle.v1.color,
                                   screen_triangle.v2.color,
                                   w.x * screen_triangle.v0.inv_w,
                                   w.y * screen_triangle.v1.inv_w,
                                   w.z * screen_triangle.v2.inv_w
                               ) / inv_w;
                    }
                };

                const auto uv_interp {
                    [inv_w](const ScreenTriangle& screen_triangle, const Vec3d& w) -> Vec2d {
                        return screen_triangle.v0.uv == screen_triangle.v1.uv &&
                               screen_triangle.v1.uv == screen_triangle.v2.uv ?

                               screen_triangle.v0.uv :

                               Vec2d {
                                   (screen_triangle.v0.uv.x * w.x * screen_triangle.v0.inv_w + screen_triangle.v1.uv.x *
                                    w.y *
                                    screen_triangle.v1.inv_w + screen_triangle.v2.uv.x * w.z * screen_triangle.v2.inv_w)
                                   /
                                   inv_w,
                                   (screen_triangle.v0.uv.y * w.x * screen_triangle.v0.inv_w + screen_triangle.v1.uv.y *
                                    w.y *
                                    screen_triangle.v1.inv_w + screen_triangle.v2.uv.y * w.z * screen_triangle.v2.inv_w)
                                   /
                                   inv_w
                               };
                    }
                };

                const Vec2d uv { uv_interp(triangle, w) };

                const Color4 color {
                    texture ? texture->sample(uv, settings.texture_filtering_mode) : color_interp(triangle, w)
                };

                const FragmentShader::Input frag_in {
                    .uv     = uv,
                    .depth  = depth,
                    .normal = normal_interp,
                    .color  = color
                };

                const Color4 out { frag_shader->frag(frag_in, uniforms) };

                render_surface.write_pixel(
                    _viewport.offset,
                    static_cast<Vec2i>(pixel_pos),
                    out,
                    depth,
                    RenderSurface::BlendMode::ALPHA
                );
            }
        }

        for (const auto& shader : _fullscreen_shaders)
        {
            for (int i = 0; i < num_pixels; ++i)
            {
                const Vec2i pixel_pos { tile.screen_pos.x + i % TILE_SIZE, tile.screen_pos.y + i / TILE_SIZE };

                const Color4 color {
                    shader->frag(
                        FragmentShader::Input {
                            .uv = Vec2d {
                                static_cast<double>(pixel_pos.x) / static_cast<double>(resolution.x),
                                static_cast<double>(pixel_pos.y) / static_cast<double>(resolution.y)
                            },
                            .depth  = tile.depth_buffer[i],
                            .normal = Vec3d { 0.0, 0.0, 1.0 },
                            .color  = render_surface.read_pixel(_viewport.offset, pixel_pos)
                        },
                        uniforms
                    )
                };

                render_surface.write_pixel(
                    _viewport.offset,
                    pixel_pos,
                    color,
                    tile.depth_buffer[i],
                    RenderSurface::BlendMode::ALPHA
                );
            }
        }
    }

    template <typename VectorType>
    int RenderLayer<VectorType>::num_inside_near_plane(const VertexBufferIndexFace& clip_triangle) const
    {
        constexpr double NEAR_PLANE = 0.0;

        const std::array inside {
            _vertex_out_buffer[clip_triangle.v0].vertex_out.pos.z >= NEAR_PLANE,
            _vertex_out_buffer[clip_triangle.v1].vertex_out.pos.z >= NEAR_PLANE,
            _vertex_out_buffer[clip_triangle.v2].vertex_out.pos.z >= NEAR_PLANE
        };

        const int num_inside {
            (inside[0] ? 1 : 0) +
            (inside[1] ? 1 : 0) +
            (inside[2] ? 1 : 0)
        };

        return num_inside;
    }

    template <typename VectorType>
    int RenderLayer<VectorType>::clip_against_near_plane(
        const VertexBufferIndexFace& clip_triangle,
        std::array<ClipTriangle, 2>& new_triangles
    ) const requires std::same_as<VectorType, Vec3d>
    {
        constexpr double NEAR_PLANE = 0.0;

        const std::array inside {
            _vertex_out_buffer[clip_triangle.v0].vertex_out.pos.z >= NEAR_PLANE,
            _vertex_out_buffer[clip_triangle.v1].vertex_out.pos.z >= NEAR_PLANE,
            _vertex_out_buffer[clip_triangle.v2].vertex_out.pos.z >= NEAR_PLANE
        };

        const int num_inside {
            (inside[0] ? 1 : 0) +
            (inside[1] ? 1 : 0) +
            (inside[2] ? 1 : 0)
        };

        const ClipVertex& v0 = _vertex_out_buffer[clip_triangle.v0];
        const ClipVertex& v1 = _vertex_out_buffer[clip_triangle.v1];
        const ClipVertex& v2 = _vertex_out_buffer[clip_triangle.v2];

        const auto intersect {
            [&](const ClipVertex& vert_in, const ClipVertex& vert_out) -> ClipVertex {
                const double t = (NEAR_PLANE - vert_in.vertex_out.pos.z) /
                                 (vert_out.vertex_out.pos.z - vert_in.vertex_out.pos.z);

                const Vec3d intersect_pos {
                    vert_in.vertex_out.pos.x + (vert_out.vertex_out.pos.x - vert_in.vertex_out.pos.x) * t,
                    vert_in.vertex_out.pos.y + (vert_out.vertex_out.pos.y - vert_in.vertex_out.pos.y) * t,
                    NEAR_PLANE
                };
                const Vec3d intersect_normal {
                    vert_in.vertex_out.normal.x + t * (vert_out.vertex_out.normal.x - vert_in.vertex_out.normal.x),
                    vert_in.vertex_out.normal.y + t * (vert_out.vertex_out.normal.y - vert_in.vertex_out.normal.y),
                    vert_in.vertex_out.normal.z + t * (vert_out.vertex_out.normal.z - vert_in.vertex_out.normal.z)
                };
                const Vec2d intersect_uv {
                    vert_in.uv.x + t * (vert_out.uv.x - vert_in.uv.x),
                    vert_in.uv.y + t * (vert_out.uv.y - vert_in.uv.y)
                };
                const Color4 intersect_color {
                    static_cast<uint8_t>(vert_in.color.r + t * (vert_out.color.r - vert_in.color.r)),
                    static_cast<uint8_t>(vert_in.color.g + t * (vert_out.color.g - vert_in.color.g)),
                    static_cast<uint8_t>(vert_in.color.b + t * (vert_out.color.b - vert_in.color.b)),
                    static_cast<uint8_t>(vert_in.color.a + t * (vert_out.color.a - vert_in.color.a))
                };

                return ClipVertex {
                    .vertex_out = {
                        .pos    = intersect_pos,
                        .w      = vert_in.vertex_out.w + (vert_out.vertex_out.w - vert_in.vertex_out.w) * t,
                        .normal = intersect_normal
                    },
                    .uv    = intersect_uv,
                    .color = intersect_color,
                };
            }
        };

        if (num_inside == 1)
        {
            if (inside[0])
            {
                new_triangles[0] = ClipTriangle {
                    .v0 = v0,
                    .v1 = intersect(v0, v1),
                    .v2 = intersect(v0, v2)
                };
            }
            else if (inside[1])
            {
                new_triangles[0] = ClipTriangle {
                    .v0 = intersect(v1, v0),
                    .v1 = v1,
                    .v2 = intersect(v1, v2)
                };
            }
            else
            {
                new_triangles[0] = ClipTriangle {
                    .v0 = intersect(v2, v0),
                    .v1 = intersect(v2, v1),
                    .v2 = v2
                };
            }
            return 1;
        }

        if (num_inside == 2)
        {
            if (!inside[0])
            {
                const ClipVertex v10 = intersect(v1, v0);
                const ClipVertex v20 = intersect(v2, v0);
                new_triangles[0] = ClipTriangle(v10, v1, v2);
                new_triangles[1] = ClipTriangle(v10, v2, v20);
            }
            else if (!inside[1])
            {
                const ClipVertex v01 = intersect(v0, v1);
                const ClipVertex v21 = intersect(v2, v1);
                new_triangles[0] = ClipTriangle(v0, v01, v2);
                new_triangles[1] = ClipTriangle(v01, v21, v2);
            }
            else
            {
                const ClipVertex v02 = intersect(v0, v2);
                const ClipVertex v12 = intersect(v1, v2);
                new_triangles[0] = ClipTriangle(v0, v1, v02);
                new_triangles[1] = ClipTriangle(v02, v1, v12);
            }
            return 2;
        }
        return 0;
    }

    template <typename VectorType>
    bool RenderLayer<VectorType>::is_backface(const ScreenTriangle& tri)
    {
        const double area = (tri.v1.pos.x - tri.v0.pos.x) * (tri.v2.pos.y - tri.v0.pos.y) -
                            (tri.v1.pos.y - tri.v0.pos.y) * (tri.v2.pos.x - tri.v0.pos.x);
        return area <= 0.0;
    }

    template <typename VectorType>
    VertexShader<VectorType>::Output RenderLayer<VectorType>::default_vertex_shader(
        const typename VertexShader<VectorType>::Input& input,
        const typename VertexShader<VectorType>::Uniforms& uniforms
    ) const
    {
        if constexpr (std::same_as<VectorType, Vec3d>)
        {
            const Matrix4x1d pos_h { { input.pos.x }, { input.pos.y }, { input.pos.z }, { 1.0 } };
            const Matrix4x1d normal_h { { input.normal.x }, { input.normal.y }, { input.normal.z }, { 0.0 } };

            const Matrix4x1d pos_clip = uniforms.mvp_matrix * pos_h;
            const Matrix4x1d normal_clip = uniforms.model_matrix * normal_h;

            return typename VertexShader<VectorType>::Output {
                .pos    = { pos_clip(0, 0), pos_clip(1, 0), pos_clip(2, 0) },
                .w      = pos_clip(3, 0),
                .normal = { normal_clip(0, 0), normal_clip(1, 0), normal_clip(2, 0) }
            };
        }
        else
        {
            const Matrix3x1d pos_h { { input.pos.x }, { input.pos.y }, { 1.0 } };
            const Matrix3x1d normal_h { { input.normal.x }, { input.normal.y }, { 0.0 } };

            const Matrix3x1d pos_clip = uniforms.mvp_matrix * pos_h;
            const Matrix3x1d normal_clip = uniforms.model_matrix * normal_h;

            return typename VertexShader<VectorType>::Output {
                .pos    = { pos_clip(0, 0), pos_clip(1, 0), },
                .w      = pos_clip(2, 0),
                .normal = { normal_clip(0, 0), normal_clip(1, 0), normal_clip(2, 0) }
            };
        }
    }

    template <typename VectorType>
    void RenderLayer<VectorType>::draw_wireframes(RenderSurface& render_surface) const
    {
        for (auto& tri : _screen_triangles)
        {
            draw_line(tri.v0.pos, tri.v1.pos, render_surface, settings.wireframe_color);
            draw_line(tri.v1.pos, tri.v2.pos, render_surface, settings.wireframe_color);
            draw_line(tri.v2.pos, tri.v0.pos, render_surface, settings.wireframe_color);
        }
    }
}
