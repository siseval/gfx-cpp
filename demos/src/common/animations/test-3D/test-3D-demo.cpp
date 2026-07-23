#include "common/animations/test-3D/test-3D-demo.h"

#include "common/core/demo-utils.h"
#include "gfx/shaders/default-fragment-shader.h"
#include "gfx/shaders/diffuse-fragment-shader.h"

using namespace gfx;

namespace demos
{
    class FogShader : public FragmentShader
    {
        Color4 frag(const Input& input, const Uniforms& uniforms) const override
        {
            const double near { uniforms.near_plane };
            const double far { uniforms.far_plane };

            const double depth { std::clamp(input.depth, 0.0, 1.0) };
            const double depth_linear = near * far / (far - depth * (far - near));

            const double dist { std::clamp(depth_linear / far * 16.0, 0.0, 1.0) };

            const Color4 color {
                dist,
                dist,
                dist,
                0.5
            };

            return color;
        }
    };

    class DepthShader : public FragmentShader
    {
        Color4 frag(const Input& input, const Uniforms& uniforms) const override
        {
            const double near { uniforms.near_plane };
            const double far { uniforms.far_plane };

            const double depth { std::clamp(input.depth, 0.0, 1.0) };
            const double depth_linear = near * far / (far - depth * (far - near));

            const double dist { std::clamp(depth_linear / far * 16.0, 0.0, 1.0) };

            const Color4 color {
                dist,
                dist,
                dist,
                1.0
            };

            return color;
        }
    };


    class RedShader : public FragmentShader
    {
        Color4 frag(const Input& input, const Uniforms& uniforms) const override
        {
            return Color4(
                std::clamp(input.color.r_double() * 1.5, 0.0, 1.0),
                input.color.g_double(),
                input.color.b_double(),
                1.0
            );
        }
    };

    class BandingShader : public FragmentShader
    {
        Color4 frag(const Input& input, const Uniforms& uniforms) const override
        {
            const double step { 0.1 };
            return Color4(
                std::floor(input.color.r_double() / step) * step,
                std::floor(input.color.g_double() / step) * step,
                std::floor(input.color.b_double() / step) * step,
                1.0
            );
        }
    };

    class DiagonalShader : public FragmentShader
    {
        Color4 frag(const Input& input, const Uniforms& uniforms) const override
        {
            const double t { uniforms.t };
            const Vec2d uv { input.uv };

            const double intensity { (input.color.r_double() + input.color.g_double() + input.color.b_double()) / 3 };

            double diagonal { uv.x + uv.y + t };
            diagonal = std::fmod(diagonal, std::numbers::pi);

            const double r { std::sin(diagonal) };
            const double g { 1 - std::sin(diagonal) };
            const double b = 0.5;

            return Color4(
                std::clamp(r, 0.0, 1.0),
                std::clamp(g, 0.0, 1.0),
                std::clamp(b, 0.0, 1.0),
                intensity
            );
        }
    };

    PolygonMesh create_two_material_mesh()
    {
        PolygonMesh mesh;

        std::vector<Vec3d> vertices;
        std::vector<Vec3d> normals;
        std::vector<Vec2d> uvs;
        std::vector<PolygonMesh::Face> faces;

        const int resolution = 16;

        auto emit = [&](const Vec3d& a,
                        const Vec3d& b,
                        const Vec3d& c,
                        const Vec2d& uva,
                        const Vec2d& uvb,
                        const Vec2d& uvc,
                        const Vec3d& normal,
                        const size_t material_id)
        {
            const size_t base = vertices.size();

            vertices.push_back(a);
            vertices.push_back(b);
            vertices.push_back(c);

            normals.push_back(normal);
            normals.push_back(normal);
            normals.push_back(normal);

            uvs.push_back(uva);
            uvs.push_back(uvb);
            uvs.push_back(uvc);

            faces.push_back(PolygonMesh::Face { base + 0, base + 1, base + 2, material_id });
        };

        for (int x = 0; x < resolution; ++x)
        {
            for (int z = 0; z < resolution; ++z)
            {
                double x0 = (double)x / resolution - 0.5;
                double x1 = (double)(x + 1) / resolution - 0.5;
                double z0 = (double)z / resolution - 0.5;
                double z1 = (double)(z + 1) / resolution - 0.5;

                Vec3d v0 { x0, 0.5, z0 };
                Vec3d v1 { x1, 0.5, z0 };
                Vec3d v2 { x1, 0.5, z1 };
                Vec3d v3 { x0, 0.5, z1 };

                Vec2d uv0 { (double)x / resolution, (double)z / resolution };
                Vec2d uv1 { (double)(x + 1) / resolution, (double)z / resolution };
                Vec2d uv2 { (double)(x + 1) / resolution, (double)(z + 1) / resolution };
                Vec2d uv3 { (double)x / resolution, (double)(z + 1) / resolution };

                Vec3d n { 0, 1, 0 };

                emit(v0, v1, v2, uv0, uv1, uv2, n, 0);
                emit(v0, v2, v3, uv0, uv2, uv3, n, 0);
            }
        }

        for (int x = 0; x < resolution; ++x)
        {
            for (int z = 0; z < resolution; ++z)
            {
                double x0 = (double)x / resolution - 0.5;
                double x1 = (double)(x + 1) / resolution - 0.5;
                double z0 = (double)z / resolution - 0.5;
                double z1 = (double)(z + 1) / resolution - 0.5;

                Vec3d v0 { x0, -0.5, z0 };
                Vec3d v1 { x1, -0.5, z0 };
                Vec3d v2 { x1, -0.5, z1 };
                Vec3d v3 { x0, -0.5, z1 };

                Vec2d uv0 { (double)x / resolution, (double)z / resolution };
                Vec2d uv1 { (double)(x + 1) / resolution, (double)z / resolution };
                Vec2d uv2 { (double)(x + 1) / resolution, (double)(z + 1) / resolution };
                Vec2d uv3 { (double)x / resolution, (double)(z + 1) / resolution };

                Vec3d n { 0, -1, 0 };

                emit(v0, v2, v1, uv0, uv2, uv1, n, 1);
                emit(v0, v3, v2, uv0, uv3, uv2, n, 1);
            }
        }

        mesh.set_vertices(std::move(vertices));
        mesh.set_normals(std::move(normals));
        mesh.set_uvs(std::move(uvs));
        mesh.set_faces(std::move(faces));

        mesh.set_colors(std::vector(mesh.num_vertices(), Color4::white()));

        return mesh;
    }

    Texture create_checkered_texture()
    {
        const Vec2i resolution { 64, 64 };
        const Vec2i square_size { resolution.x / 8, resolution.y / 8 };
        Texture texture(resolution);

        for (int y = 0; y < resolution.y; ++y)
        {
            for (int x = 0; x < resolution.x; ++x)
            {
                const int square_x = x / square_size.x;
                const int square_y = y / square_size.y;

                const bool is_white = (square_x + square_y) % 2 == 0;
                const Color4 color = is_white ? Color4::white() : Color4(0.5, 0.5, 0.5);

                texture.set_pixel(x, y, color);
            }
        }

        return texture;
    }

    Texture create_gradient_texture()
    {
        const Vec2i resolution { 64, 64 };
        Texture texture(resolution);

        for (int y = 0; y < resolution.y; ++y)
        {
            for (int x = 0; x < resolution.x; ++x)
            {
                const double t = static_cast<double>(x) / (resolution.x - 1);
                const Color4 color(t, t, t, 1.0);
                texture.set_pixel(x, y, color);
            }
        }

        return texture;
    }

    Texture create_grass_texture()
    {
        const Vec2i resolution { 64, 64 };
        Texture texture(resolution);

        for (int y = 0; y < resolution.y; ++y)
        {
            for (int x = 0; x < resolution.x; ++x)
            {
                const double t = random_double(0.5, 1.0);
                const Color4 color(0.0, t, 0.0, 1.0);
                texture.set_pixel(x, y, color);
            }
        }

        return texture;
    }

    Texture smooth_noise_texture(const Vec2i& resolution)
    {
        Texture texture(resolution);

        for (int y = 0; y < resolution.y; ++y)
        {
            for (int x = 0; x < resolution.x; ++x)
            {
                const double yt = static_cast<double>(y) / (resolution.y - 1);
                const double xt = static_cast<double>(x) / (resolution.x - 1);
                const double t = (std::sin(xt * std::numbers::pi * 4) + std::sin(yt * std::numbers::pi * 4)) * 0.5 + 0.5;
                const Color4 color(t, t, t, 1.0);
                texture.set_pixel(x, y, color);
            }
        }

        return texture;
    }

    Texture sphere_texture(const Vec2i& resolution)
    {
        Texture texture(resolution);

        for (int y = 0; y < resolution.y; ++y)
        {
            for (int x = 0; x < resolution.x; ++x)
            {
                const double u = static_cast<double>(x) / (resolution.x - 1);
                const double v = static_cast<double>(y) / (resolution.y - 1);

                const double theta = u * std::numbers::pi * 2;
                const double phi = v * std::numbers::pi;

                const double r = std::sin(phi) * std::cos(theta) * 0.5 + 0.5;
                const double g = std::sin(phi) * std::sin(theta) * 0.5 + 0.5;
                const double b = std::cos(phi) * 0.5 + 0.5;

                texture.set_pixel(x, y, Color4(r, g, b, 1.0));
            }
        }

        return texture;
    }


    Color4 Test3DDemo::WaterSurfaceShader::frag(const Input& input, const Uniforms& uniforms) const
    {
        Color4 out;

        constexpr double wave_freq = 20.0;
        constexpr double wave_speed = 3.0;
        constexpr double dist_falloff = 4.0;
        constexpr double amp_scale = 0.3;
        constexpr double fade_decay = 7.0;
        constexpr double base_brightness = 2.0;

        const double global_time_sec { time_ms() / 1000.0 };

        double ripple_amount = base_brightness;
        for (const auto& ripple : ripples)
        {
            const double distance = Vec2d::distance(input.uv, ripple->center);

            const double ripple_time_sec { global_time_sec - ripple->start_time / 1000.0 };
            const double t { ripple_time_sec / ripple_lifetime_sec };

            const double fade_in { smoothstep(std::clamp(t / 0.1, 0.0, 1.0)) };

            const double wave { std::sin(wave_freq * distance - wave_speed * ripple_time_sec) };
            const double falloff { std::exp(-dist_falloff * distance - fade_decay * t) };

            ripple_amount += wave * falloff * amp_scale * fade_in;
        }

        ripple_amount = inv_lerp(-0.05, 0.05, ripple_amount);
        ripple_amount = fmod(ripple_amount, 1.0);

        out = base_color + Color4(std::fmod(ripple_amount, 1.0));

        return out;
    }

    void Test3DDemo::spawn_ripple(const Vec2d position) const
    {
        const auto ripple = std::make_shared<Ripple>();
        ripple->center = position / quad->get_size();
        ripple->start_time = time_ms();
        water_shader->ripples.push_back(ripple);
    }

    Vec2d Test3DDemo::get_random_position() const
    {
        const Vec2d resolution { get_resolution() };
        const double x = random_double(0.0, quad->get_size().x);
        const double y = random_double(0.0, quad->get_size().y);
        return Vec2d { x, y };
    }

    void Test3DDemo::update_ripples(const double dt) const
    {
        std::vector<std::shared_ptr<Ripple>> to_remove;
        for (auto& ripple_center : water_shader->ripples)
        {
            if (time_ms() - ripple_center->start_time > water_shader->ripple_lifetime_sec * 1000.0)
            {
                to_remove.push_back(ripple_center);
            }
        }
        for (const auto& ripple : to_remove)
        {
            auto it = std::ranges::find(water_shader->ripples, ripple);
            if (it != water_shader->ripples.end())
            {
                water_shader->ripples.erase(it);
            }
        }
    }

    const Vec2i res80 { 142, 80 };
    const Vec2i res160 { 284, 160 };
    const Vec2i res240 { 426, 240 };
    const Vec2i res360 { 640, 360 };
    const Vec2i res480 { 854, 480 };
    const Vec2i res720 { 1280, 720 };
    const Vec2i res1080 { 1920, 1080 };
    const Vec2i res1440 { 2560, 1440 };
    const Vec2i res1964 { 3024, 1964 };
    const Vec2i res2160 { 3840, 2160 };

    void Test3DDemo::init()
    {
        renderer->clear_2D_scene();
        renderer->clear_3D_scene();
        renderer->set_resolution(res480);
        renderer->set_clear_color(Color4(0.0, 0.0, 0.0));
        renderer->get_render_3D()->set_texture_filtering_mode(Texture::FilteringMode::NEAREST);
        Camera& camera { renderer->get_camera() };

        camera.set_position(0.0, 0.0, -5.0);
        camera.set_rotation_degrees(0.0, 0.0, 0.0);
        camera.set_fov_degrees(75.0);
        camera.set_z_near(0.1);
        camera.set_z_far(1000.0);

        renderer->set_light_direction(-1.0, 1.0, -1.0);
        renderer->set_ambient_light(0.5);

        const auto diffuse_shader { std::make_shared<DiffuseFragmentShader>() };
        const auto default_shader { std::make_shared<DefaultFragmentShader>() };
        const auto red_shader { std::make_shared<RedShader>() };
        const auto fog_shader { std::make_shared<FogShader>() };
        const auto banding_shader { std::make_shared<BandingShader>() };
        const auto diagonal_shader { std::make_shared<DiagonalShader>() };

        const auto checkered_texture { std::make_shared<Texture>(create_checkered_texture()) };
        const auto grass_texture { std::make_shared<Texture>(create_grass_texture()) };
        const auto gradient_texture { std::make_shared<Texture>(create_gradient_texture()) };
        const auto noise_texture { std::make_shared<Texture>(smooth_noise_texture({ 256, 256 })) };
        const auto sph_texture { std::make_shared<Texture>(sphere_texture({ 256, 256 })) };

        const auto diffuse_material { std::make_shared<Material>(diffuse_shader) };
        const auto grass_material { std::make_shared<Material>(diffuse_shader, grass_texture) };
        const auto rainbow_checker_material { std::make_shared<Material>(diagonal_shader, checkered_texture) };
        const auto smooth_noise_material { std::make_shared<Material>(diffuse_shader, noise_texture) };
        const auto fog_material { std::make_shared<Material>(fog_shader) };
        const auto gradient_material { std::make_shared<Material>(diffuse_shader, gradient_texture) };
        const auto rainbow_material { std::make_shared<Material>(diagonal_shader) };
        const auto sphere_material { std::make_shared<Material>(diffuse_shader, sph_texture) };

        const auto default_material { diffuse_material };

        water_shader = std::make_shared<WaterSurfaceShader>();
        const auto water_material { std::make_shared<Material>(water_shader) };
        quad = std::make_shared<Plane3D>();
        quad->set_size(200.0, 100.0);
        quad->set_rotation_degrees(-90.0, 0.0, 0.0);
        quad->set_material(water_material);
        // renderer->add_primitive(quad);

        teapot = std::make_shared<Polygon3D>();
        teapot->set_mesh(create_two_material_mesh());
        teapot->set_position(0.0, 0.0, 30.0);
        teapot->set_scale(10.0, 10.0, 10.0);
        teapot->set_material(default_material, 0);
        teapot->set_material(fog_material, 1);
        // renderer->add_primitive(teapot);

        sphere = std::make_shared<Sphere3D>();
        sphere->set_radius(1.0);
        sphere->set_color(0.8, 0.4, 0.4);
        sphere->set_num_segments(16);
        sphere->set_material(rainbow_checker_material);
        renderer->add_primitive(sphere);

        plane = std::make_shared<Plane3D>();
        plane->set_position(0.0, -15.0, 0.0);
        plane->set_size(20.0, 20.0);
        plane->set_color(0.4, 0.8, 0.4);
        plane->set_material(grass_material);
        renderer->add_primitive(plane);

        cube = std::make_shared<Cuboid3D>();
        cube->set_size(2.0, 2.0, 2.0);
        cube->set_color(0.4, 0.4, 0.8);
        cube->set_material(smooth_noise_material);
        renderer->add_primitive(cube);

        crosshair = renderer->create_ellipse(
            static_cast<Vec2d>(renderer->get_resolution() / 2),
            { 1, 1 },
            Color4(1.0, 1.0, 1.0, 0.75),
            1.0
        );

        // renderer->get_render_3D()->add_fullscreen_shader(std::make_shared<DepthShader>());
        // renderer->get_render_3D()->add_fullscreen_shader(std::make_shared<DiagonalShader>());
        // renderer->get_render_3D()->add_fullscreen_shader(std::make_shared<BandingShader>());

        crosshair->set_anchor(0.5, 0.5);
        crosshair->set_filled(true);
        // renderer->add_primitive(crosshair);

        constexpr double min_range = 128.0;
        constexpr double max_range = 256.0;
        constexpr double min_radius = 1.0;
        constexpr double max_radius = 3.0;
        constexpr int num_boxes = 0;
        constexpr int num_spheres = 1000;
        constexpr int num_segments = 12;

        const auto rand_pos = [](const double min, const double max)
        {
            return Vec3d::from_angles(random_double(0.0, 2.0 * std::numbers::pi), random_double(0.0, std::numbers::pi)).normalize() *
                random_double(min, max);
        };

        for (int i = 0; i < num_boxes; i++)
        {
            auto box { std::make_shared<Cuboid3D>() };

            box->set_position(rand_pos(min_range, max_range));
            box->set_size(Vec3d {
                random_double(1.0, 3.0),
                random_double(1.0, 3.0),
                random_double(1.0, 3.0)
            });
            box->set_color(0.8, 0.8, 0.6);
            box->set_material(default_material);

            scene_items.push_back(box);
            renderer->add_primitive(box);
        }

        for (int i = 0; i < num_spheres; i++)
        {
            auto sphere { std::make_shared<Sphere3D>() };

            sphere->set_position(rand_pos(min_range, max_range));
            sphere->set_radius(random_double(min_radius, max_radius));
            sphere->set_color(Color4::white());
            sphere->set_material(random_double(0.0, 1.0) < 0.1 ? sphere_material : diffuse_material);
            sphere->set_num_segments(num_segments);

            scene_items.push_back(sphere);
            renderer->add_primitive(sphere);
        }
    }

    void Test3DDemo::render_frame(const double dt)
    {
        const double t_sec { time_sec() };

        for (const auto primitive : scene_items)
        {
            primitive->set_rotation_degrees(
                primitive->get_rotation_degrees().x + dt * 15.0,
                primitive->get_rotation_degrees().y + dt * 30.0,
                primitive->get_rotation_degrees().z + dt * 45.0
            );
        }

        random_spawn_timer += dt;
        if (random_spawn&& random_spawn_timer 
        >
        random_spawn_interval_sec
        )
        {
            spawn_ripple(get_random_position());
            random_spawn_timer = 0.0;
        }
        update_ripples(dt);
        water_shader->base_color = Color4(
            std::sin(t_sec * 0.5) * 0.1 + 0.2,
            std::sin(t_sec * 0.3 + 2.0) * 0.1 + 0.2,
            std::sin(t_sec * 0.7 + 4.0) * 0.1 + 0.2
        );


        debug_viewer->add_debug_line("triangles: " + std::to_string(renderer->get_render_3D()->get_num_triangles()), 0);

        cube->set_rotation_degrees(
            cube->get_rotation_degrees().x + dt * 30.0,
            cube->get_rotation_degrees().y + dt * 45.0,
            cube->get_rotation_degrees().z + dt * 60.0
        );

        cube->set_position(
            cube->get_position().x,
            std::sin(t_sec) * 2.0,
            cube->get_position().z
        );

        cube->set_scale(
            1.0,
            1.0,
            1.0
        );

        sphere->set_position(
            std::cos(t_sec) * 5.0,
            std::sin(t_sec) * 2.0,
            std::sin(t_sec) * 5.0
        );

        poll_held_keys(dt);
        update_camera(dt);

        renderer->clear_frame();
        renderer->render_frame();
        renderer->present_frame();
    }

    void Test3DDemo::end()
    {
        renderer->clear_scene();
    }

    void Test3DDemo::handle_char(const int input)
    {
    }

    void Test3DDemo::report_key(const KeyEvent event)
    {
        switch (event.type)
        {
        case KeyEventType::KEY_PRESS:
            {
                key_states[event.key] = true;
                break;
            }
        case KeyEventType::KEY_RELEASE:
            {
                key_states[event.key] = false;
                break;
            }
        case KeyEventType::KEY_REPEAT:
            {
                break;
            }
        }
        if (event.type == KeyEventType::KEY_PRESS)
        {
            switch (event.key)
            {
            case Key::E:
                {
                    crosshair->set_visible(!crosshair->is_visible());
                    break;
                }
            default:
                {
                    break;
                }
            }
        }
    }

    void Test3DDemo::report_mouse(const MouseEvent event)
    {
        switch (event.type)
        {
        case MouseEventType::MOVE:
            {
                const Vec2d delta { event.position - prev_mouse_pos };
                Camera& camera = renderer->get_camera();
                camera.set_rotation(
                    {
                        camera.get_rotation().x + delta.y,
                        camera.get_rotation().y + delta.x,
                        camera.get_rotation().z
                    }
                );
                prev_mouse_pos = event.position;
                break;
            }
        case MouseEventType::LEFT_DOWN:
            {
                break;
            }
        case MouseEventType::LEFT_UP:
            {
                break;
            }
        case MouseEventType::RIGHT_DOWN:
            {
                break;
            }
        case MouseEventType::RIGHT_UP:
            {
                break;
            }
        case MouseEventType::SCROLL_UP:
            {
                break;
            }
        case MouseEventType::SCROLL_DOWN:
            {
                break;
            }
        }
    }

    void Test3DDemo::update_camera(const double dt)
    {
        camera_velocity = camera_velocity * std::pow(0.85, dt * 60.0);
        camera_velocity = camera_velocity.limit(max_camera_speed);
        Camera& camera = renderer->get_camera();
        camera.set_position(camera.get_position() + camera_velocity * dt);
    }

    void Test3DDemo::camera_movement(const Key key, const double dt)
    {
        Vec3d forward { renderer->get_camera().get_forward() };
        forward.y = 0.0;

        switch (key)
        {
        case Key::W:
            {
                camera_velocity += forward * camera_acceleration * dt;
                break;
            }
        case Key::S:
            {
                camera_velocity -= forward * camera_acceleration * dt;
                break;
            }
        case Key::A:
            {
                camera_velocity += Vec3d::cross(forward, Vec3d { 0, 1, 0 }).normalize() * camera_acceleration * dt;
                break;
            }
        case Key::D:
            {
                camera_velocity -= Vec3d::cross(forward, Vec3d { 0, 1, 0 }).normalize() * camera_acceleration * dt;
                break;
            }
        case Key::SPACE:
            {
                camera_velocity += Vec3d { 0, 1, 0 } * camera_acceleration * dt;
                break;
            }
        case Key::CTRL:
            {
                camera_velocity -= Vec3d { 0, 1, 0 } * camera_acceleration * dt;
                break;
            }
        default:
            {
                break;
            }
        }
    }

    void Test3DDemo::poll_held_keys(const double dt)
    {
        for (const auto& [key, held] : key_states)
        {
            if (held)
            {
                camera_movement(key, dt);
            }
        }
    }
}
