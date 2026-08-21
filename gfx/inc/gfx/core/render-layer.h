#pragma once

#include <map>
#include <memory>
#include <mutex>

#include "gfx/core/primitive.h"
#include "gfx/core/render-surface.h"
#include "gfx/core/scene-graph.h"
#include "gfx/core/thread-pool.h"
#include "gfx/core/types/homogenous-coordinate.h"
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

        explicit RenderLayer(
            const Viewport& viewport,
            std::shared_ptr<ThreadPool> thread_pool = ThreadPool::default_thread_pool()
        );

        void draw_frame(
            RenderSurface& render_surface,
            const View<VectorType>& view,
            const Projection<VectorType>& projection
        );

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

        static constexpr int VERTEX_CHUNKING_FACTOR = 16;
        static constexpr int TILE_SIZE = 32;

        struct MeshRange
        {
            size_t vertex_offset;
            size_t vertex_count;

            size_t triangle_offset;
            size_t triangle_count;

            MatrixType mvp_matrix;
            MatrixType model_matrix;

            const TriangleMesh<VectorType>& mesh;
            const std::vector<std::shared_ptr<Material>>& materials;

            double depth;
        };

        struct VertexBufferIndexFace
        {
            const size_t v0;
            const size_t v1;
            const size_t v2;
        };

        struct ClipVertex
        {
            HomogenousCoordinate<VectorType> coordinate;
            Vec3d normal;
            Vec2d uv;
            Color4 color;
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
            double inv_w;
            double z_over_w;
        };

        struct ScreenTriangle
        {
            ScreenVertex v0;
            ScreenVertex v1;
            ScreenVertex v2;
            size_t material_id;
        };

        struct ThreadState
        {
            std::vector<ScreenTriangle> local_triangles;
            size_t num_triangles;
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

        void build_material_map(const DrawQueue& draw_queue);
        void generate_mesh_ranges(const DrawQueue& draw_queue, const MatrixType& vp_matrix);

        void transform_vertices();
        void process_vertex_chunk(size_t chunk_start, size_t chunk_end);

        void generate_screen_triangles();

        void process_triangle_chunk(
            size_t chunk_start,
            size_t chunk_end,
            ThreadState& thread_state
        );

        ClipVertex create_clip_vertex(
            const HomogenousCoordinate<VectorType>& coordinate,
            const TriangleMesh<VectorType>& mesh,
            size_t attribute_index
        ) const;

        ScreenVertex clip_to_screen(
            const HomogenousCoordinate<VectorType>& coordinate,
            double z,
            const MatrixType& normal_transform,
            const TriangleMesh<VectorType>& mesh,
            size_t attribute_index
        ) const;

        ScreenVertex clip_to_screen(const ClipVertex& vertex, double z) const;

        void generate_tiles();
        void bin_triangles();

        void render_tile(Tile& tile, RenderSurface& render_surface) const;
        static void rasterize_triangle_in_tile(const ScreenTriangle& triangle, int tri_index, Tile& tile);

        void draw_wireframes(RenderSurface& render_surface) const;

        /* 3D specific culling */

        int num_inside_near_plane(
            const HomogenousCoordinate<VectorType>& v0,
            const HomogenousCoordinate<VectorType>& v1,
            const HomogenousCoordinate<VectorType>& v2
        ) const;

        int clip_against_near_plane(
            const ClipTriangle& clip_triangle,
            std::array<ClipTriangle, 2>& new_triangles
        ) const requires std::same_as<VectorType, Vec3d>;

        static bool is_backface(
            const HomogenousCoordinate<VectorType>& v0,
            const HomogenousCoordinate<VectorType>& v1,
            const HomogenousCoordinate<VectorType>& v2
        );

        Viewport _viewport;

        std::shared_ptr<SceneGraph<VectorType>> _scene_graph;
        std::shared_ptr<ThreadPool> _thread_pool;
        std::vector<std::shared_ptr<FragmentShader>> _fullscreen_shaders;

        std::vector<ThreadState> _thread_states;

        double _frame_start_timestamp { 0.0 };

        size_t _total_vertices { 0 };
        size_t _total_triangles { 0 };
        size_t _num_screen_triangles { 0 };

        std::vector<MeshRange> _mesh_ranges;
        std::vector<size_t> _mesh_vertex_chunk_offsets;
        std::vector<size_t> _mesh_triangle_chunk_offsets;

        std::unordered_map<size_t, std::shared_ptr<Material>> _material_map;

        std::vector<HomogenousCoordinate<VectorType>> _vertex_out_buffer;
        std::vector<VertexBufferIndexFace> _index_faces;

        std::vector<size_t> _index_face_to_material_id_map;

        std::vector<ScreenTriangle> _screen_triangles;

        Vec2i _last_resolution { 0, 0 };
        std::vector<Tile> _tiles;
    };

    template <typename VectorType>
    RenderLayer<VectorType>::RenderLayer(const Viewport& viewport, std::shared_ptr<ThreadPool> thread_pool)
        : _viewport(viewport)
        , _scene_graph(std::make_shared<SceneGraph<VectorType>>())
        , _thread_pool(std::move(thread_pool))
        , _thread_states(std::vector<ThreadState>(std::thread::hardware_concurrency())) {}

    template <typename VectorType>
    void RenderLayer<VectorType>::draw_frame(
        RenderSurface& render_surface,
        const View<VectorType>& view,
        const Projection<VectorType>& projection
    )
    {
        _frame_start_timestamp =
            std::chrono::duration<double, std::milli>(
                std::chrono::high_resolution_clock::now().time_since_epoch()
            ).count() / 1000.0;

        const DrawQueue& draw_queue { _scene_graph->get_draw_queue(projection.get_view_bounds(view, _viewport)) };
        const MatrixType vp_matrix { projection.get_matrix(_viewport.get_aspect_ratio()) * view.get_matrix() };

        build_material_map(draw_queue);
        generate_mesh_ranges(draw_queue, vp_matrix);

        transform_vertices();
        generate_screen_triangles();

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
        return _num_screen_triangles;
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
    void RenderLayer<VectorType>::build_material_map(const DrawQueue& draw_queue)
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
    void RenderLayer<VectorType>::generate_mesh_ranges(const DrawQueue& draw_queue, const MatrixType& vp_matrix)
    {
        _mesh_ranges.clear();
        _mesh_vertex_chunk_offsets.clear();
        _mesh_triangle_chunk_offsets.clear();

        _index_faces.clear();
        _index_face_to_material_id_map.clear();

        _total_vertices = 0;
        _total_triangles = 0;

        for (const auto& [item, transform] : draw_queue)
        {
            const TriangleMesh<VectorType>& mesh { item->get_mesh() };

            _mesh_ranges.emplace_back(
                _total_vertices,
                mesh.get_vertices().size(),

                _total_triangles,
                mesh.get_faces().size(),

                vp_matrix * transform.get_matrix(),
                transform.get_matrix(),

                mesh,
                item->get_materials()
            );

            if constexpr (std::same_as<VectorType, Vec2d>)
            {
                _mesh_ranges.back().depth = item->get_depth();
            }

            _mesh_vertex_chunk_offsets.emplace_back(_total_vertices);
            _mesh_triangle_chunk_offsets.emplace_back(_total_triangles);

            for (const typename TriangleMesh<VectorType>::Face& face : mesh.get_faces())
            {
                _index_faces.emplace_back(
                    face.v0 + _total_vertices,
                    face.v1 + _total_vertices,
                    face.v2 + _total_vertices
                );

                _index_face_to_material_id_map.emplace_back(face.material_index);
            }

            _total_vertices += mesh.get_vertices().size();
            _total_triangles += mesh.get_faces().size();
        }
    }

    template <typename VectorType>
    void RenderLayer<VectorType>::transform_vertices()
    {
        if (_vertex_out_buffer.size() < _total_vertices)
        {
            _vertex_out_buffer.resize(_total_vertices);
        }

        const int num_threads { _thread_pool->get_num_threads() };
        const int num_chunks { num_threads * VERTEX_CHUNKING_FACTOR };

        const size_t chunk_size { _total_vertices / num_chunks };
        const size_t remainder { _total_vertices % num_chunks };

        std::vector<size_t> chunk_starts(num_chunks + 1);
        chunk_starts[0] = 0;
        for (int c = 0; c < num_chunks; ++c)
        {
            chunk_starts[c + 1] = chunk_starts[c] + chunk_size + (static_cast<size_t>(c) < remainder ? 1 : 0);
        }

        _thread_pool->run(
            num_chunks,
            [&](const int chunk_index) {
                process_vertex_chunk(chunk_starts[chunk_index], chunk_starts[chunk_index + 1]);
            }
        );
    }

    template <typename VectorType>
    void RenderLayer<VectorType>::process_vertex_chunk(const size_t chunk_start, const size_t chunk_end)
    {
        size_t mesh_index {
            static_cast<size_t>(
                std::ranges::upper_bound(_mesh_vertex_chunk_offsets, chunk_start)
                - _mesh_vertex_chunk_offsets.begin()) - 1
        };

        size_t i { chunk_start };
        while (i < chunk_end)
        {
            const MeshRange& mesh_range = _mesh_ranges[mesh_index];
            const size_t range_end = std::min(chunk_end, mesh_range.vertex_offset + mesh_range.vertex_count);

            for (; i < range_end; ++i)
            {
                const size_t local_index = i - mesh_range.vertex_offset;
                _vertex_out_buffer[i] = HomogenousCoordinate<VectorType>(
                    mesh_range.mesh.get_vertices()[local_index],
                    1.0,
                    mesh_range.mvp_matrix
                );
            }

            ++mesh_index;
        }
    }

    template <typename VectorType>
    void RenderLayer<VectorType>::generate_screen_triangles()
    {
        const int num_threads { _thread_pool->get_num_threads() };
        const int num_chunks { num_threads * VERTEX_CHUNKING_FACTOR };

        if (static_cast<int>(_thread_states.size()) != num_chunks)
        {
            _thread_states.resize(num_chunks);
        }

        const size_t chunk_size { _total_triangles / num_chunks };
        const size_t remainder { _total_triangles % num_chunks };

        std::vector<size_t> chunk_starts(num_chunks + 1);
        chunk_starts[0] = 0;
        for (int c = 0; c < num_chunks; ++c)
        {
            chunk_starts[c + 1] = chunk_starts[c] + chunk_size + (static_cast<size_t>(c) < remainder ? 1 : 0);
        }

        _thread_pool->run(
            num_chunks,
            [&](const int chunk_index) {
                process_triangle_chunk(
                    chunk_starts[chunk_index],
                    chunk_starts[chunk_index + 1],
                    _thread_states[chunk_index]
                );
            }
        );

        std::vector<int> offsets(num_chunks + 1, 0);
        for (int i = 0; i < num_chunks; ++i)
        {
            offsets[i + 1] = offsets[i] + static_cast<int>(_thread_states[i].num_triangles);
        }

        if (_screen_triangles.size() < offsets[num_chunks])
        {
            _screen_triangles.resize(offsets[num_chunks]);
        }
        _num_screen_triangles = offsets[num_chunks];

        _thread_pool->run(
            num_chunks,
            [&](const int chunk_index) {
                std::copy(
                    _thread_states[chunk_index].local_triangles.begin(),
                    _thread_states[chunk_index].local_triangles.begin() + _thread_states[chunk_index].num_triangles,
                    _screen_triangles.begin() + offsets[chunk_index]
                );
            }
        );
    }

    template <typename VectorType>
    void RenderLayer<VectorType>::process_triangle_chunk(
        const size_t chunk_start,
        const size_t chunk_end,
        ThreadState& thread_state
    )
    {
        size_t mesh_index {
            static_cast<size_t>(
                std::ranges::upper_bound(_mesh_triangle_chunk_offsets, chunk_start)
                - _mesh_triangle_chunk_offsets.begin()) - 1
        };

        size_t& num_added = thread_state.num_triangles = 0;
        std::vector<ScreenTriangle>& screen_triangles = thread_state.local_triangles;

        size_t chunk_size { (chunk_end - chunk_start) * 2 };
        if (screen_triangles.size() < chunk_size)
        {
            screen_triangles.resize(chunk_size);
        }

        size_t i { chunk_start };
        while (i < chunk_end)
        {
            const MeshRange& mesh_range = _mesh_ranges[mesh_index];

            const TriangleMesh<VectorType>& mesh = mesh_range.mesh;
            const MatrixType& model_matrix = mesh_range.model_matrix;

            const size_t range_end { std::min(chunk_end, mesh_range.triangle_offset + mesh_range.triangle_count) };

            for (; i < range_end; ++i)
            {
                const VertexBufferIndexFace& face = _index_faces[i];

                const HomogenousCoordinate<VectorType>& v0 = _vertex_out_buffer[face.v0];
                const HomogenousCoordinate<VectorType>& v1 = _vertex_out_buffer[face.v1];
                const HomogenousCoordinate<VectorType>& v2 = _vertex_out_buffer[face.v2];

                if constexpr (std::same_as<VectorType, Vec2d>)
                {
                    screen_triangles[num_added++] = ScreenTriangle {
                        .v0 = clip_to_screen(
                            v0,
                            mesh_range.depth,
                            model_matrix,
                            mesh,
                            face.v0 - mesh_range.vertex_offset
                        ),
                        .v1 = clip_to_screen(
                            v1,
                            mesh_range.depth,
                            model_matrix,
                            mesh,
                            face.v1 - mesh_range.vertex_offset
                        ),
                        .v2 = clip_to_screen(
                            v2,
                            mesh_range.depth,
                            model_matrix,
                            mesh,
                            face.v2 - mesh_range.vertex_offset
                        ),
                        .material_id = mesh_range.materials[_index_face_to_material_id_map[i]]->get_id()
                    };
                }
                else
                {
                    const int num_inside { num_inside_near_plane(v0, v1, v2) };

                    if (num_inside <= 0)
                    {
                        continue;
                    }

                    if (num_inside >= 3 && !is_backface(v0, v1, v2))
                    {
                        screen_triangles[num_added++] = ScreenTriangle {
                            .v0 = clip_to_screen(v0, v0.pos.z, model_matrix, mesh, face.v0 - mesh_range.vertex_offset),
                            .v1 = clip_to_screen(v1, v1.pos.z, model_matrix, mesh, face.v1 - mesh_range.vertex_offset),
                            .v2 = clip_to_screen(v2, v2.pos.z, model_matrix, mesh, face.v2 - mesh_range.vertex_offset),
                            .material_id = mesh_range.materials[_index_face_to_material_id_map[i]]->get_id()
                        };
                        continue;
                    }

                    std::array<ClipTriangle, 2> new_triangles;
                    const int num_returned {
                        clip_against_near_plane(
                            ClipTriangle {
                                .v0 = create_clip_vertex(v0, mesh, face.v0 - mesh_range.vertex_offset),
                                .v1 = create_clip_vertex(v1, mesh, face.v1 - mesh_range.vertex_offset),
                                .v2 = create_clip_vertex(v2, mesh, face.v2 - mesh_range.vertex_offset)
                            },
                            new_triangles
                        )
                    };

                    for (int j = 0; j < num_returned; ++j)
                    {
                        if (is_backface(
                            new_triangles[j].v0.coordinate,
                            new_triangles[j].v1.coordinate,
                            new_triangles[j].v2.coordinate
                        ))
                        {
                            continue;
                        }
                        screen_triangles[num_added++] = ScreenTriangle {
                            .v0          = clip_to_screen(new_triangles[j].v0, new_triangles[j].v0.coordinate.pos.z),
                            .v1          = clip_to_screen(new_triangles[j].v1, new_triangles[j].v1.coordinate.pos.z),
                            .v2          = clip_to_screen(new_triangles[j].v2, new_triangles[j].v2.coordinate.pos.z),
                            .material_id = mesh_range.materials[_index_face_to_material_id_map[i]]->get_id()
                        };
                    }
                }
            }

            ++mesh_index;
        }
    }

    template <typename VectorType>
    RenderLayer<VectorType>::ClipVertex RenderLayer<VectorType>::create_clip_vertex(
        const HomogenousCoordinate<VectorType>& coordinate,
        const TriangleMesh<VectorType>& mesh,
        const size_t attribute_index
    ) const
    {
        return ClipVertex {
            .coordinate = coordinate,
            .normal = mesh.get_normals().size() > attribute_index ? mesh.get_normals()[attribute_index] : Vec3d::zero(),
            .uv = mesh.get_uvs().size() > attribute_index ? mesh.get_uvs()[attribute_index] : Vec2d::zero(),
            .color = mesh.get_colors().size() > attribute_index ? mesh.get_colors()[attribute_index] : Color4::white(),

        };
    }

    template <typename VectorType>
    RenderLayer<VectorType>::ScreenVertex RenderLayer<VectorType>::clip_to_screen(
        const HomogenousCoordinate<VectorType>& coordinate,
        const double z,
        const MatrixType& normal_transform,
        const TriangleMesh<VectorType>& mesh,
        const size_t attribute_index
    ) const
    {
        const double inv_w { 1.0 / coordinate.w };

        if constexpr (std::same_as<VectorType, Vec3d>)
        {
            return ScreenVertex {
                .pos = {
                    (coordinate.pos.x * inv_w * 0.5 + 0.5) * _viewport.size.x,
                    (coordinate.pos.y * inv_w * 0.5 + 0.5) * _viewport.size.y
                },
                .normal = mesh.get_normals().size() > attribute_index ?
                          Transform<Vec3d>::transform_vector(mesh.get_normals()[attribute_index], normal_transform) :
                          Vec3d::zero(),
                .uv    = mesh.get_uvs().size() > attribute_index ? mesh.get_uvs()[attribute_index] : Vec2d::zero(),
                .color = mesh.get_colors().size() > attribute_index ?
                         mesh.get_colors()[attribute_index] :
                         Color4::white(),
                .inv_w    = inv_w,
                .z_over_w = z * inv_w
            };
        }
        else
        {
            return ScreenVertex {
                .pos = {
                    (coordinate.pos.x * inv_w * 0.5 + 0.5) * _viewport.size.x,
                    (coordinate.pos.y * inv_w * 0.5 + 0.5) * _viewport.size.y
                },
                .normal = Vec3d::zero(),
                .uv     = mesh.get_uvs().size() > attribute_index ? mesh.get_uvs()[attribute_index] : Vec2d::zero(),
                .color  = mesh.get_colors().size() > attribute_index ?
                         mesh.get_colors()[attribute_index] :
                         Color4::white(),
                .inv_w    = inv_w,
                .z_over_w = z * inv_w
            };
        }
    }

    template <typename VectorType>
    RenderLayer<VectorType>::ScreenVertex RenderLayer<VectorType>::clip_to_screen(
        const ClipVertex& vertex,
        const double z
    ) const
    {
        const double inv_w { 1.0 / vertex.coordinate.w };

        return ScreenVertex {
            .pos = {
                (vertex.coordinate.pos.x * inv_w * 0.5 + 0.5) * _viewport.size.x,
                (vertex.coordinate.pos.y * inv_w * 0.5 + 0.5) * _viewport.size.y
            },
            .normal   = vertex.normal,
            .uv       = vertex.uv,
            .color    = vertex.color,
            .inv_w    = inv_w,
            .z_over_w = z * inv_w
        };
    }

    template <typename VectorType>
    void RenderLayer<VectorType>::generate_tiles()
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
    void RenderLayer<VectorType>::bin_triangles()
    {
        const Vec2i resolution { _viewport.size };

        for (size_t i = 0; i < _num_screen_triangles; ++i)
        {
            const auto& triangle = _screen_triangles[i];

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
        const double inv_area { 1.0 / area };

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
    void RenderLayer<VectorType>::draw_wireframes(RenderSurface& render_surface) const
    {
        for (auto& tri : _screen_triangles)
        {
            draw_line(tri.v0.pos, tri.v1.pos, render_surface, settings.wireframe_color);
            draw_line(tri.v1.pos, tri.v2.pos, render_surface, settings.wireframe_color);
            draw_line(tri.v2.pos, tri.v0.pos, render_surface, settings.wireframe_color);
        }
    }

    template <typename VectorType>
    int RenderLayer<VectorType>::num_inside_near_plane(
        const HomogenousCoordinate<VectorType>& v0,
        const HomogenousCoordinate<VectorType>& v1,
        const HomogenousCoordinate<VectorType>& v2
    ) const
    {
        constexpr double NEAR_PLANE = 0.0;

        const std::array inside {
            v0.pos.z >= NEAR_PLANE,
            v1.pos.z >= NEAR_PLANE,
            v2.pos.z >= NEAR_PLANE
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
        const ClipTriangle& clip_triangle,
        std::array<ClipTriangle, 2>& new_triangles
    ) const requires std::same_as<VectorType, Vec3d>
    {
        constexpr double NEAR_PLANE = 0.0;

        const ClipVertex& v0 = clip_triangle.v0;
        const ClipVertex& v1 = clip_triangle.v1;
        const ClipVertex& v2 = clip_triangle.v2;

        const std::array inside {
            v0.coordinate.pos.z >= NEAR_PLANE,
            v1.coordinate.pos.z >= NEAR_PLANE,
            v2.coordinate.pos.z >= NEAR_PLANE
        };

        const int num_inside {
            (inside[0] ? 1 : 0) +
            (inside[1] ? 1 : 0) +
            (inside[2] ? 1 : 0)
        };

        const auto intersect {
            [&](const ClipVertex& vert_in, const ClipVertex& vert_out) -> ClipVertex {
                const double t = (NEAR_PLANE - vert_in.coordinate.pos.z) /
                                 (vert_out.coordinate.pos.z - vert_in.coordinate.pos.z);

                const Vec3d intersect_pos {
                    vert_in.coordinate.pos.x + (vert_out.coordinate.pos.x - vert_in.coordinate.pos.x) * t,
                    vert_in.coordinate.pos.y + (vert_out.coordinate.pos.y - vert_in.coordinate.pos.y) * t,
                    NEAR_PLANE
                };
                const Vec3d intersect_normal { Vec3d::lerp(vert_in.normal, vert_out.normal, t) };
                const Vec2d intersect_uv { Vec2d::lerp(vert_in.uv, vert_out.uv, t) };
                const Color4 intersect_color { Color4::lerp(vert_in.color, vert_out.color, t) };

                return ClipVertex {
                    .coordinate = HomogenousCoordinate<VectorType>(
                        intersect_pos,
                        vert_in.coordinate.w + (vert_out.coordinate.w - vert_in.coordinate.w) * t
                    ),
                    .normal = intersect_normal,
                    .uv     = intersect_uv,
                    .color  = intersect_color,
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
    bool RenderLayer<VectorType>::is_backface(
        const HomogenousCoordinate<VectorType>& v0,
        const HomogenousCoordinate<VectorType>& v1,
        const HomogenousCoordinate<VectorType>& v2
    )
    {
        const double area = v0.pos.x * (v1.pos.y * v2.w - v2.pos.y * v1.w) -
                            v0.pos.y * (v1.pos.x * v2.w - v2.pos.x * v1.w) +
                            v0.w * (v1.pos.x * v2.pos.y - v2.pos.x * v1.pos.y);
        return area <= 0.0;
    }
}
