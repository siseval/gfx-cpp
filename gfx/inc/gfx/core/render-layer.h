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

        Settings settings;

    private:

        static constexpr int TILE_SIZE = 32;

        struct ClipVertex
        {
            VectorType pos;
            double w;
            Vec2d uv;
            Vec3d normal;
            Color4 color;
        };

        struct ClipTriangle
        {
            ClipVertex v0;
            ClipVertex v1;
            ClipVertex v2;

            ClipTriangle()
                : v0()
                , v1()
                , v2() {}

            ClipTriangle(const ClipVertex& v0, const ClipVertex& v1, const ClipVertex& v2)
                : v0(v0)
                , v1(v1)
                , v2(v2) {}
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

            ScreenTriangle(
                const ScreenVertex& v0,
                const ScreenVertex& v1,
                const ScreenVertex& v2,
                const int material_id
            )
                : v0(v0)
                , v1(v1)
                , v2(v2)
                , material_id(material_id) {}
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

        using DrawQueue = std::vector<std::pair<std::shared_ptr<Primitive<VectorType>>, Transform<VectorType>>>;

        std::unordered_map<int, std::shared_ptr<Material>> build_material_map(
            const DrawQueue& draw_queue
        ) const;

        void generate_screen_triangles(
            const View<VectorType>& view,
            const Projection<VectorType>& projection,
            const DrawQueue& draw_queue
        ) const;

        void generate_screen_triangles_multithreaded(
            const View<VectorType>& view,
            const Projection<VectorType>& projection,
            const DrawQueue& draw_queue
        ) const;

        void generate_tiles() const;
        void bin_triangles(const std::vector<ScreenTriangle>& triangles, std::vector<Tile>& tiles) const;

        void render_tile(
            Tile& tile,
            RenderSurface& render_surface,
            const std::unordered_map<int, std::shared_ptr<Material>>& material_map,
            double t
        ) const;

        static void rasterize_triangle_in_tile(const ScreenTriangle& triangle, int tri_index, Tile& tile);

        /* 3D specific culling */

        static int clip_against_near_plane(std::array<ClipTriangle, 2>& clip_triangles) requires std::same_as<
            VectorType, Vec3d>;

        static bool is_backface(const ScreenVertex& v0, const ScreenVertex& v1, const ScreenVertex& v2) requires
            std::same_as<VectorType, Vec3d>;


        VertexShader<VectorType>::Output default_vertex_shader(
            const VertexShader<VectorType>::Input& input,
            const VertexShader<VectorType>::Uniforms& uniforms
        ) const;

        Viewport _viewport;

        std::shared_ptr<SceneGraph<VectorType>> _scene_graph;
        std::shared_ptr<ThreadPool> _thread_pool;
        std::vector<std::shared_ptr<FragmentShader>> _fullscreen_shaders;

        mutable std::vector<ScreenTriangle> _screen_triangles;

        mutable Vec2i _last_resolution { 0, 0 };
        mutable std::vector<Tile> _tiles;
    };

    inline void draw_line(
        Vec2i viewport_offset,
        Vec2i a,
        Vec2i b,
        RenderSurface& surface,
        Color4 color
    )
    {
        int x0 = a.x;
        int y0 = a.y;
        int x1 = b.x;
        int y1 = b.y;

        int dx = std::abs(x1 - x0);
        int dy = std::abs(y1 - y0);

        int sx = (x0 < x1) ? 1 : -1;
        int sy = (y0 < y1) ? 1 : -1;

        int err = dx - dy;

        while (true)
        {
            surface.write_pixel(viewport_offset, Vec2i { x0, y0 }, color);

            if (x0 == x1 && y0 == y1)
            {
                break;
            }

            int e2 = 2 * err;

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
        const double t {
            std::chrono::duration<double, std::milli>(
                std::chrono::high_resolution_clock::now().time_since_epoch()
            ).count() / 1000.0
        };

        const DrawQueue& draw_queue { _scene_graph->get_draw_queue(projection.get_view_bounds(view, _viewport)) };
        const auto material_map { build_material_map(draw_queue) };

        if (settings.multicore_vertex_transformation)
        {
            generate_screen_triangles_multithreaded(view, projection, draw_queue);
        }
        else
        {
            generate_screen_triangles(view, projection, draw_queue);
        }

        generate_tiles();
        bin_triangles(_screen_triangles, _tiles);

        if (settings.multicore_rasterization)
        {
            _thread_pool->run(
                static_cast<int>(_tiles.size()),
                [&](const int tile_index) {
                    render_tile(_tiles[tile_index], render_surface, material_map, t);
                }
            );
        }
        else
        {
            for (auto& tile : _tiles)
            {
                render_tile(tile, render_surface, material_map, t);
            }
        }
    }

    template <typename VectorType>
    std::unordered_map<int, std::shared_ptr<Material>> RenderLayer<VectorType>::build_material_map(
        const DrawQueue& draw_queue
    ) const
    {
        std::unordered_map<int, std::shared_ptr<Material>> material_map;

        for (const auto& primitive : draw_queue | std::views::keys)
        {
            for (const auto& material : primitive->get_materials())
            {
                if (material)
                {
                    material_map[material->get_id()] = material;
                }
            }
        }

        return material_map;
    }

    template <typename VectorType>
    void RenderLayer<VectorType>::generate_screen_triangles(
        const View<VectorType>& view,
        const Projection<VectorType>& projection,
        const DrawQueue& draw_queue
    ) const
    {
        _screen_triangles.clear();

        const double t {
            std::chrono::duration<double, std::milli>(
                std::chrono::high_resolution_clock::now().time_since_epoch()
            ).count() / 1000.0
        };

        const Vec2i resolution { _viewport.size };
        const double aspect_ratio { _viewport.get_aspect_ratio() };

        const MatrixType& view_matrix { view.get_matrix() };
        const MatrixType& projection_matrix { projection.get_matrix(aspect_ratio) };
        const MatrixType vp_matrix { projection_matrix * view_matrix };

        const int num_primitives = static_cast<int>(draw_queue.size());

        for (int i = 0; i < num_primitives; ++i)
        {
            const auto& [primitive, transform] = draw_queue[i];

            const TriangleMesh<VectorType>& mesh { primitive->get_mesh() };
            const auto& materials = primitive->get_materials();

            const MatrixType model_matrix { transform.get_matrix() };

            const auto& vertices = mesh.get_vertices();
            const auto& normals = mesh.get_normals();
            const auto& uvs = mesh.get_uvs();
            const auto& colors = mesh.get_colors();

            const MatrixType mvp_matrix = vp_matrix * model_matrix;

            std::vector<ClipVertex> vertex_cache(vertices.size());

            const auto& vert_shader = primitive->get_vertex_shader();

            const typename VertexShader<VectorType>::Uniforms uniforms {
                .t            = t,
                .model_matrix = model_matrix,
                .mvp_matrix   = mvp_matrix
            };

            for (size_t j = 0; j < vertices.size(); ++j)
            {
                const typename VertexShader<VectorType>::Input vert_in {
                    .pos    = vertices[j],
                    .normal = normals.size() > j ? normals[j] : Vec3d::zero()
                };

                const typename VertexShader<VectorType>::Output vert_out {
                    vert_shader ? vert_shader->vert(vert_in, uniforms) : default_vertex_shader(vert_in, uniforms)
                };

                vertex_cache[j] = ClipVertex {
                    .pos    = vert_out.pos,
                    .w      = vert_out.w,
                    .uv     = uvs.size() > j ? uvs[j] : Vec2d::zero(),
                    .normal = vert_out.normal,
                    .color  = colors.size() > j ? colors[j] : primitive->get_color()

                };
            }

            for (const auto& face : mesh.get_faces())
            {
                std::array<ClipTriangle, 2> clip_triangles;
                clip_triangles[0] = { vertex_cache[face.v0], vertex_cache[face.v1], vertex_cache[face.v2] };

                int num_clipped { 1 };
                if constexpr (std::same_as<VectorType, Vec3d>)
                {
                    num_clipped = clip_against_near_plane(clip_triangles);
                }

                for (int tri_index = 0; tri_index < num_clipped; ++tri_index)
                {
                    const auto& clip_triangle = clip_triangles[tri_index];

                    const auto project {
                        [&](const ClipVertex& clip_vertex) {
                            const double inv_w = 1.0 / clip_vertex.w;

                            double z;
                            if constexpr (std::same_as<VectorType, Vec3d>)
                            {
                                z = clip_vertex.pos.z;
                            }
                            else
                            {
                                z = primitive->get_depth();
                            }

                            return ScreenVertex {
                                .pos = {
                                    (clip_vertex.pos.x * inv_w * 0.5 + 0.5) * resolution.x,
                                    (clip_vertex.pos.y * inv_w * 0.5 + 0.5) * resolution.y
                                },
                                .normal   = clip_vertex.normal,
                                .uv       = clip_vertex.uv,
                                .color    = clip_vertex.color,
                                .z_over_w = z * inv_w,
                                .inv_w    = inv_w
                            };
                        }
                    };

                    const ScreenVertex sv0 = project(clip_triangle.v0);
                    const ScreenVertex sv1 = project(clip_triangle.v1);
                    const ScreenVertex sv2 = project(clip_triangle.v2);

                    if constexpr (std::same_as<VectorType, Vec3d>)
                    {
                        if (is_backface(sv0, sv1, sv2))
                        {
                            continue;
                        }
                    }

                    _screen_triangles.emplace_back(
                        std::move(sv0),
                        std::move(sv1),
                        std::move(sv2),
                        materials[face.material_index]->get_id()
                    );
                }
            }
        }
    }

    template <typename VectorType>
    void RenderLayer<VectorType>::generate_screen_triangles_multithreaded(
        const View<VectorType>& view,
        const Projection<VectorType>& projection,
        const DrawQueue& draw_queue
    ) const
    {
        _screen_triangles.clear();

        const double t {
            std::chrono::duration<double, std::milli>(
                std::chrono::high_resolution_clock::now().time_since_epoch()
            ).count() / 1000.0
        };

        const Vec2i resolution { _viewport.size };
        const double aspect_ratio { _viewport.get_aspect_ratio() };

        const MatrixType& view_matrix { view.get_matrix() };
        const MatrixType& projection_matrix { projection.get_matrix(aspect_ratio) };
        const MatrixType vp_matrix { projection_matrix * view_matrix };

        const int num_primitives { static_cast<int>(draw_queue.size()) };

        std::mutex merge_mutex;

        _thread_pool->run(
            num_primitives,
            [&](const int i) {
                const auto& [primitive, transform] = draw_queue[i];
                const TriangleMesh<VectorType>& mesh { primitive->get_mesh() };
                const auto& materials = primitive->get_materials();

                const MatrixType model_matrix { transform.get_matrix() };

                const auto& vertices = mesh.get_vertices();
                const auto& normals = mesh.get_normals();
                const auto& uvs = mesh.get_uvs();
                const auto& colors = mesh.get_colors();

                const MatrixType mvp_matrix = vp_matrix * model_matrix;

                std::vector<ScreenTriangle> local_triangles;
                local_triangles.reserve(mesh.get_faces().size());

                std::vector<ClipVertex> vertex_cache(vertices.size());

                const auto& vert_shader = primitive->get_vertex_shader();

                const typename VertexShader<VectorType>::Uniforms uniforms {
                    .t            = t,
                    .model_matrix = model_matrix,
                    .mvp_matrix   = mvp_matrix
                };

                for (size_t j = 0; j < vertices.size(); ++j)
                {
                    const typename VertexShader<VectorType>::Input vert_in {
                        .pos    = vertices[j],
                        .normal = normals.size() > j ? normals[j] : Vec3d::zero()
                    };

                    const typename VertexShader<VectorType>::Output vert_out {
                        vert_shader ? vert_shader->vert(vert_in, uniforms) : default_vertex_shader(vert_in, uniforms)
                    };

                    vertex_cache[j] = {
                        .pos    = vert_out.pos,
                        .w      = vert_out.w,
                        .uv     = uvs.size() > j ? uvs[j] : Vec2d::zero(),
                        .normal = vert_out.normal,
                        .color  = colors.size() > j ? colors[j] : primitive->get_color()
                    };
                }

                for (const auto& face : mesh.get_faces())
                {
                    std::array<ClipTriangle, 2> clip_triangles;
                    clip_triangles[0] = { vertex_cache[face.v0], vertex_cache[face.v1], vertex_cache[face.v2] };

                    int num_clipped { 1 };
                    if constexpr (std::same_as<VectorType, Vec3d>)
                    {
                        num_clipped = clip_against_near_plane(clip_triangles);
                    }

                    for (int tri_index = 0; tri_index < num_clipped; ++tri_index)
                    {
                        const auto& clip_triangle = clip_triangles[tri_index];

                        const auto project = [&](const ClipVertex& clip_vertex) {
                            const double inv_w = 1.0 / clip_vertex.w;

                            double z;
                            if constexpr (std::same_as<VectorType, Vec3d>)
                            {
                                z = clip_vertex.pos.z;
                            }
                            else
                            {
                                z = primitive->get_depth();
                            }

                            return ScreenVertex {
                                .pos = {
                                    (clip_vertex.pos.x * inv_w * 0.5 + 0.5) * resolution.x,
                                    (clip_vertex.pos.y * inv_w * 0.5 + 0.5) * resolution.y
                                },
                                .normal   = clip_vertex.normal,
                                .uv       = clip_vertex.uv,
                                .color    = clip_vertex.color,
                                .z_over_w = z * inv_w,
                                .inv_w    = inv_w
                            };
                        };

                        const ScreenVertex sv0 = project(clip_triangle.v0);
                        const ScreenVertex sv1 = project(clip_triangle.v1);
                        const ScreenVertex sv2 = project(clip_triangle.v2);

                        if constexpr (std::same_as<VectorType, Vec3d>)
                        {
                            if (is_backface(sv0, sv1, sv2))
                            {
                                continue;
                            }
                        }

                        local_triangles.emplace_back(
                            std::move(sv0),
                            std::move(sv1),
                            std::move(sv2),
                            materials[face.material_index]->get_id()
                        );
                    }
                }

                if (!local_triangles.empty())
                {
                    std::lock_guard lock(merge_mutex);
                    _screen_triangles.append_range(local_triangles);
                }
            }
        );
    }

    // void Render3D::generate_screen_triangles(std::unordered_map<int, std::shared_ptr<Material>> &material_map) const
    // {
    //     screen_triangles.clear();
    //
    //     const double t {
    //         std::chrono::duration<double, std::milli>(
    //             std::chrono::high_resolution_clock::now().time_since_epoch()
    //         ).count() / 1000.0
    //     };
    //     const Vec2i resolution { surface->get_resolution() };
    //     const double aspect_ratio {
    //         static_cast<double>(resolution.x) /
    //         static_cast<double>(resolution.y)
    //     };
    //
    //     const Camera &view { get_camera() };
    //     const MatrixType &view_matrix { view.get_view_matrix() };
    //     const MatrixType &projection_matrix { view.get_projection_matrix(aspect_ratio) };
    //     const MatrixType &vp_matrix { projection_matrix * view_matrix };
    //
    //     const auto &draw_queue { scene_graph->get_draw_queue(view.get_frustum(aspect_ratio)) };
    //     for (const auto &[primitive, transform] : draw_queue)
    //     {
    //         for (const auto &material : primitive->get_materials())
    //         {
    //             if (material)
    //             {
    //                 material_map[material->get_id()] = material;
    //             }
    //         }
    //     }
    //
    //     for (const auto &[primitive, transform] : draw_queue)
    //     {
    //         const PolygonMesh &mesh { primitive->get_mesh() };
    //
    //         for (PolygonMesh::Face face : mesh.get_faces())
    //         {
    //             const std::shared_ptr<Material> &material { primitive->get_material(face.material_index) };
    //             const std::shared_ptr<VertexShader> &vert_shader { primitive->get_vertex_shader() };
    //
    //             const VertexShader::Uniforms uniforms {
    //                 .t = t,
    //                 .model_matrix = transform,
    //                 .view_matrix = view_matrix,
    //                 .projection_matrix = projection_matrix,
    //                 .mvp_matrix = vp_matrix * transform
    //             };
    //
    //             const auto model_to_clip {
    //                 [&](const int source_index) -> ClipVertex {
    //                     const VertexShader::Input vert_in {
    //                         .pos = mesh.get_vertices()[source_index],
    //                         .normal = mesh.get_normals()[source_index],
    //                     };
    //                     const VertexShader::Output out { 
    //                         vert_shader ?
    //                         vert_shader->vert(vert_in, uniforms) :
    //                         default_vertex_shader(vert_in, uniforms)
    //                     };
    //
    //                     return ClipVertex {
    //                         .pos = out.pos,
    //                         .w = out.w,
    //                         .uv = mesh.get_uvs()[source_index],
    //                         .normal = out.normal,
    //                         .color = mesh.get_colors()[source_index],
    //                     };
    //                 }
    //             };
    //
    //             std::array<ClipTriangle, 2> clip_triangles;
    //             clip_triangles[0] = {
    //                 model_to_clip(face.v1),
    //                 model_to_clip(face.v2),
    //                 model_to_clip(face.v3),
    //             };
    //
    //             const int num_triangles { clip_against_near_plane(clip_triangles) };
    //
    //             const auto clip_to_screen {
    //                 [&](const ClipVertex clip_vertex) {
    //                     const double inv_w { 1.0 / clip_vertex.w };
    //                     const Vec3d ndc { clip_vertex.pos * inv_w };
    //
    //                     return ScreenVertex {
    //                         .pos = {
    //                             (ndc.x * 0.5 + 0.5) * resolution.x,
    //                             (ndc.y * 0.5 + 0.5) * resolution.y
    //                         },
    //                         .normal = clip_vertex.normal,
    //                         .uv = clip_vertex.uv,
    //                         .color = clip_vertex.color,
    //                         .z_over_w = ndc.z,
    //                         .inv_w = inv_w
    //                     };
    //                 }
    //             };
    //
    //             for (int triangle_index = 0; triangle_index < num_triangles; ++triangle_index)
    //             {
    //                 std::array<ScreenVertex, 3> screen_vertices {
    //                     clip_to_screen(clip_triangles[triangle_index].v0),
    //                     clip_to_screen(clip_triangles[triangle_index].v1),
    //                     clip_to_screen(clip_triangles[triangle_index].v2)
    //                 };
    //
    //                 const auto is_backface { 
    //                     [] (const ScreenVertex& v0, const ScreenVertex& v1, const ScreenVertex& v2) -> bool {
    //                         const double area = 
    //                             (v1.pos.x - v0.pos.x) * (v2.pos.y - v0.pos.y) - 
    //                             (v1.pos.y - v0.pos.y) * (v2.pos.x - v0.pos.x);
    //                         return area <= 0.0;
    //                     }
    //                 };
    //
    //                 if (is_backface(screen_vertices[0], screen_vertices[1], screen_vertices[2]))
    //                 {
    //                     continue;
    //                 }
    //
    //                 screen_triangles.emplace_back(
    //                     std::move(screen_vertices[0]),
    //                     std::move(screen_vertices[1]),
    //                     std::move(screen_vertices[2]),
    //                     material->get_id()
    //                 );
    //
    //             }
    //         }
    //     }
    // }

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
    void RenderLayer<VectorType>::bin_triangles(
        const std::vector<ScreenTriangle>& triangles,
        std::vector<Tile>& tiles
    ) const
    {
        const Vec2i resolution { _viewport.size };

        for (const auto& triangle : triangles)
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

                    tiles[tile_index].material_batches[triangle.material_id].push_back(triangle);
                }
            }
        }
    }

    template <typename VectorType>
    void RenderLayer<VectorType>::render_tile(
        Tile& tile,
        RenderSurface& render_surface,
        const std::unordered_map<int, std::shared_ptr<Material>>& material_map,
        const double t
    ) const
    {
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
            const auto& material { material_map.at(material_id) };
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
    int RenderLayer<VectorType>::clip_against_near_plane(std::array<ClipTriangle, 2>& clip_triangles) requires
        std::same_as<VectorType, Vec3d>
    {
        constexpr double NEAR_PLANE = 0.0;
        const ClipTriangle& tri = clip_triangles[0];

        const std::array inside {
            tri.v0.pos.z >= NEAR_PLANE,
            tri.v1.pos.z >= NEAR_PLANE,
            tri.v2.pos.z >= NEAR_PLANE
        };

        const int num_inside {
            (inside[0] ? 1 : 0) +
            (inside[1] ? 1 : 0) +
            (inside[2] ? 1 : 0)
        };

        if (num_inside == 3)
        {
            return 1;
        }
        if (num_inside == 0)
        {
            return 0;
        }

        const auto intersect {
            [&](const ClipVertex& vert_in, const ClipVertex& vert_out) -> ClipVertex {
                const double t = (NEAR_PLANE - vert_in.pos.z) / (vert_out.pos.z - vert_in.pos.z);

                const Vec3d intersect_pos {
                    vert_in.pos.x + (vert_out.pos.x - vert_in.pos.x) * t,
                    vert_in.pos.y + (vert_out.pos.y - vert_in.pos.y) * t,
                    NEAR_PLANE
                };
                const Vec3d intersect_normal {
                    vert_in.normal.x + t * (vert_out.normal.x - vert_in.normal.x),
                    vert_in.normal.y + t * (vert_out.normal.y - vert_in.normal.y),
                    vert_in.normal.z + t * (vert_out.normal.z - vert_in.normal.z)
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
                    .pos    = intersect_pos,
                    .w      = vert_in.w + (vert_out.w - vert_in.w) * t,
                    .uv     = intersect_uv,
                    .normal = intersect_normal,
                    .color  = intersect_color,
                };
            }
        };

        if (num_inside == 1)
        {
            if (inside[0])
            {
                clip_triangles[0] = ClipTriangle(tri.v0, intersect(tri.v0, tri.v1), intersect(tri.v0, tri.v2));
            }
            else if (inside[1])
            {
                clip_triangles[0] = ClipTriangle(intersect(tri.v1, tri.v0), tri.v1, intersect(tri.v1, tri.v2));
            }
            else
            {
                clip_triangles[0] = ClipTriangle(intersect(tri.v2, tri.v0), intersect(tri.v2, tri.v1), tri.v2);
            }
            return 1;
        }

        if (num_inside == 2)
        {
            if (!inside[0])
            {
                const ClipVertex v10 = intersect(tri.v1, tri.v0);
                const ClipVertex v20 = intersect(tri.v2, tri.v0);
                clip_triangles[0] = ClipTriangle(v10, tri.v1, tri.v2);
                clip_triangles[1] = ClipTriangle(v10, tri.v2, v20);
            }
            else if (!inside[1])
            {
                const ClipVertex v01 = intersect(tri.v0, tri.v1);
                const ClipVertex v21 = intersect(tri.v2, tri.v1);
                clip_triangles[0] = ClipTriangle(tri.v0, v01, tri.v2);
                clip_triangles[1] = ClipTriangle(v01, v21, tri.v2);
            }
            else
            {
                const ClipVertex v02 = intersect(tri.v0, tri.v2);
                const ClipVertex v12 = intersect(tri.v1, tri.v2);
                clip_triangles[0] = ClipTriangle(tri.v0, tri.v1, v02);
                clip_triangles[1] = ClipTriangle(v02, tri.v1, v12);
            }
            return 2;
        }
        return 0;
    }

    template <typename VectorType>
    bool RenderLayer<VectorType>::is_backface(
        const ScreenVertex& v0,
        const ScreenVertex& v1,
        const ScreenVertex& v2
    ) requires
        std::same_as<VectorType, Vec3d>
    {
        const double area = (v1.pos.x - v0.pos.x) * (v2.pos.y - v0.pos.y) -
                            (v1.pos.y - v0.pos.y) * (v2.pos.x - v0.pos.x);
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
}
