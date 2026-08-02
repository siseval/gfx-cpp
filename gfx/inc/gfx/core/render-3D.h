#pragma once

#include <map>
#include <memory>

#include "gfx/core/camera.h"
#include "gfx/core/primitive.h"
#include "gfx/core/render-surface.h"
#include "gfx/core/scene-graph.h"
#include "gfx/core/thread-pool.h"

namespace gfx
{
    class Render3D
    {
    public:

        explicit Render3D(const std::shared_ptr<RenderSurface>& surface);

        void draw_frame() const;
        void clear_frame() const;
        void present_frame() const;

        void add_item(const std::shared_ptr<Primitive>& item) const;
        void remove_item(const std::shared_ptr<Primitive>& item) const;
        bool contains_item(const std::shared_ptr<Primitive>& item) const;
        void clear_items() const;

        std::shared_ptr<SceneGraph> get_scene_graph() const;
        std::shared_ptr<RenderSurface> get_render_surface() const;

        void set_camera(const Camera& cam);

        void set_light_direction(Vec3d dir);
        void set_ambient_light(double intensity);

        void set_render_surface(std::shared_ptr<RenderSurface> new_surface);

        void set_texture_filtering_mode(Texture::FilteringMode mode);
        Texture::FilteringMode get_texture_filtering_mode() const;

        int num_items() const;
        Vec2i get_resolution() const;

        int get_num_triangles() const;

        const Camera& get_camera() const;
        Camera& get_camera();

        Vec3d get_light_direction() const;
        double get_ambient_light() const;

        void add_fullscreen_shader(std::shared_ptr<FragmentShader> shader);
        void remove_fullscreen_shader(std::shared_ptr<FragmentShader> shader);
        void clear_fullscreen_shaders();

        void set_multicore_vertex_transformation(bool enable);
        void set_multicore_rasterization(bool enable);

    private:

        static constexpr int TILE_SIZE = 32;

        struct ClipVertex
        {
            Vec3d xyz;
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

        static void rasterize_triangle_in_tile(const ScreenTriangle& triangle, int tri_index, Tile& tile);

        void generate_screen_triangles(std::unordered_map<int, std::shared_ptr<Material>>& material_map) const;

        void generate_screen_triangles_multithreaded(
            std::unordered_map<int, std::shared_ptr<Material>>& material_map
        ) const;

        void generate_tiles() const;
        void bin_triangles(const std::vector<ScreenTriangle>& triangles, std::vector<Tile>& tiles) const;

        void render_tile(
            Tile& tile,
            const std::unordered_map<int, std::shared_ptr<Material>>& material_map,
            double t
        ) const;

        static int clip_against_near_plane(std::array<ClipTriangle, 2>& clip_triangles);
       
        static VertexShader::Output default_vertex_shader(
            const VertexShader::Input& input,
            const VertexShader::Uniforms& uniforms
        );

        mutable std::vector<ScreenTriangle> _screen_triangles;

        Camera _camera;
        Vec3d _light_dir;
        double _ambient_light;

        std::shared_ptr<SceneGraph> _scene_graph;
        std::shared_ptr<RenderSurface> _surface;

        std::vector<std::shared_ptr<FragmentShader>> _fullscreen_shaders;

        bool _multicore_vertex_transformation { true };
        bool _multicore_rasterization { true };

        Texture::FilteringMode _texture_filtering_mode { Texture::FilteringMode::NEAREST };

        std::shared_ptr<ThreadPool> _thread_pool;

        mutable Vec2i _last_resolution { 0, 0 };
        mutable std::vector<Tile> _tiles;
    };
}
