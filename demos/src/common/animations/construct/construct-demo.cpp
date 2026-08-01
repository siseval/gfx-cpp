#include "common/animations/construct/construct-demo.h"

#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "common/core/demo-utils.h"
#include "gfx/shaders/diffuse-fragment-shader.h"

using namespace gfx;

namespace demos
{
    struct IndexKey
    {
        int v;
        int vt;
        int vn;

        bool operator==(const IndexKey& other) const
        {
            return v == other.v && vt == other.vt && vn == other.vn;
        }
    };

    struct IndexKeyHash
    {
        size_t operator()(const IndexKey& k) const
        {
            return (std::hash<int>()(k.v) ^
                    std::hash<int>()(k.vt) << 1) >> 1 ^
                std::hash<int>()(k.vn) << 1;
        }
    };

    int resolve_index(const int idx, const int size)
    {
        if (idx > 0)
            return idx - 1;
        if (idx < 0)
            return size + idx;
        throw std::runtime_error("OBJ index 0 is invalid");
    }

    IndexKey parse_vertex_token(const std::string& token)
    {
        IndexKey key { -1, -1, -1 };

        std::stringstream ss(token);
        std::string part;

        std::getline(ss, part, '/');
        key.v = std::stoi(part);

        if (std::getline(ss, part, '/'))
        {
            if (!part.empty())
                key.vt = std::stoi(part);

            if (std::getline(ss, part, '/'))
            {
                if (!part.empty())
                    key.vn = std::stoi(part);
            }
        }

        return key;
    }

    TriangleMesh load_obj(const std::string& path, std::unordered_map<std::string, size_t>& material_map)
    {
        std::ifstream file(path);
        if (!file)
            throw std::runtime_error("Failed to open OBJ file");

        std::vector<Vec3d> temp_v;
        std::vector<Vec3d> temp_n;
        std::vector<Vec2d> temp_uv;

        std::vector<Vec3d> vertices;
        std::vector<Vec3d> normals;
        std::vector<Vec2d> uvs;
        std::vector<TriangleMesh::Face> faces;

        size_t current_material = 0;
        size_t next_material_id = 0;

        std::unordered_map<IndexKey, size_t, IndexKeyHash> vertex_map;

        std::string line;

        while (std::getline(file, line))
        {
            if (line.empty() || line[0] == '#')
                continue;

            std::istringstream iss(line);
            std::string type;
            iss >> type;

            if (type == "v")
            {
                Vec3d v;
                iss >> v.x >> v.y >> v.z;
                temp_v.push_back(v);
                temp_v.back().z = -temp_v.back().z;
            }
            else if (type == "vn")
            {
                Vec3d n;
                iss >> n.x >> n.y >> n.z;
                temp_n.push_back(n);
                temp_n.back().z = -temp_n.back().z;
            }
            else if (type == "vt")
            {
                Vec2d uv;
                iss >> uv.x >> uv.y;
                temp_uv.push_back(uv);
                temp_uv.back().y = 1.0 - temp_uv.back().y;
            }
            else if (type == "f")
            {
                std::vector<IndexKey> face_keys;
                std::string token;

                while (iss >> token)
                    face_keys.push_back(parse_vertex_token(token));

                if (face_keys.size() < 3)
                    continue;

                std::vector<size_t> indices;

                for (auto& key : face_keys)
                {
                    key.v = resolve_index(key.v, temp_v.size());
                    if (key.vt != -1)
                        key.vt = resolve_index(key.vt, temp_uv.size());
                    if (key.vn != -1)
                        key.vn = resolve_index(key.vn, temp_n.size());

                    auto it = vertex_map.find(key);
                    if (it != vertex_map.end())
                    {
                        indices.push_back(it->second);
                    }
                    else
                    {
                        size_t new_index = vertices.size();

                        Vec3d pos = temp_v[key.v];
                        Vec3d norm = key.vn != -1 ? temp_n[key.vn] : Vec3d { 0.0, 0.0, 0.0 };
                        Vec2d uv = key.vt != -1 ? temp_uv[key.vt] : Vec2d { 0.0, 0.0 };

                        vertices.push_back(pos);
                        normals.push_back(norm);
                        uvs.push_back(uv);

                        vertex_map[key] = new_index;
                        indices.push_back(new_index);
                    }
                }

                for (size_t i = 1; i + 1 < indices.size(); ++i)
                {
                    faces.push_back({
                        indices[0],
                        indices[i + 1],
                        indices[i],
                        current_material
                    });
                }
            }
            else if (type == "usemtl")
            {
                std::string name;
                iss >> name;

                auto it = material_map.find(name);
                if (it != material_map.end())
                {
                    current_material = it->second;
                }
                else
                {
                    current_material = next_material_id;
                    material_map[name] = next_material_id++;
                }
            }
        }

        TriangleMesh mesh;
        mesh.set_vertices(vertices);
        if (!normals.empty())
            mesh.set_normals(normals);
        if (!uvs.empty())
            mesh.set_uvs(uvs);
        mesh.set_faces(faces);

        return mesh;
    }

    Texture decode_bmp(const std::string& filename)
    {
        std::ifstream file { filename, std::ios::binary };
        if (!file)
        {
            throw std::runtime_error { "Failed to open BMP file: " + filename };
        }

        uint16_t file_type {};
        file.read(reinterpret_cast<char*>(&file_type), 2);
        if (file_type != 0x4D42)
        {
            throw std::runtime_error { "Not a valid BMP file" };
        }

        file.ignore(8);

        uint32_t data_offset {};
        file.read(reinterpret_cast<char*>(&data_offset), 4);

        uint32_t dib_header_size {};
        file.read(reinterpret_cast<char*>(&dib_header_size), 4);

        if (dib_header_size < 40)
        {
            throw std::runtime_error { "Unsupported BMP format: DIB header too small" };
        }

        int32_t width {}, height {};
        file.read(reinterpret_cast<char*>(&width), 4);
        file.read(reinterpret_cast<char*>(&height), 4);

        uint16_t planes {}, bit_count {};
        file.read(reinterpret_cast<char*>(&planes), 2);
        file.read(reinterpret_cast<char*>(&bit_count), 2);

        uint32_t compression {};
        file.read(reinterpret_cast<char*>(&compression), 4);

        file.ignore(16);

        const uint32_t extra_header_bytes = dib_header_size > 40 ? dib_header_size - 40 : 0;
        if (extra_header_bytes > 0)
        {
            file.ignore(extra_header_bytes);
        }

        if (compression == 3)
        {
            const uint32_t mask_count = dib_header_size >= 56 ? 4 : 3;
            file.ignore(mask_count * 4);
        }

        if (planes != 1 || (bit_count != 24 && bit_count != 32))
        {
            throw std::runtime_error { "Unsupported BMP: Only 24-bit or 32-bit images supported" };
        }

        if (compression != 0 && compression != 3)
        {
            throw std::runtime_error { "Unsupported BMP: Compressed BMPs not supported" };
        }

        file.seekg(data_offset, std::ios::beg);

        const int row_padded = (bit_count * width + 31) / 32 * 4;
        const int bytes_per_pixel = bit_count / 8;

        Texture bmp { Vec2i { width, std::abs(height) } };
        const bool flip_vertically { height > 0 };

        std::vector<uint8_t> row_data(row_padded);

        for (int y = 0; y < std::abs(height); ++y)
        {
            file.read(reinterpret_cast<char*>(row_data.data()), row_padded);
            const int bmp_y = flip_vertically ? std::abs(height) - 1 - y : y;

            for (int x = 0; x < width; ++x)
            {
                const int idx = x * bytes_per_pixel;
                const uint8_t b = row_data[idx];
                const uint8_t g = row_data[idx + 1];
                const uint8_t r = row_data[idx + 2];
                const uint8_t a = bit_count == 32 ? row_data[idx + 3] : 255;

                const Color4 color { r, g, b, a };
                bmp.set_pixel(Vec2i { x, bmp_y }, color);
            }
        }

        return bmp;
    }

    std::string trim(const std::string& str)
    {
        const std::string whitespace = " \t\n\r\f\v";
        const auto first = str.find_first_not_of(whitespace);
        if (first == std::string::npos)
            return "";
        const auto last = str.find_last_not_of(whitespace);
        return str.substr(first, last - first + 1);
    }

    std::unordered_map<std::string, std::string> load_mtl_texture_map(const std::string& path)
    {
        std::ifstream file(path);
        if (!file)
            throw std::runtime_error("Failed to open MTL file");

        std::unordered_map<std::string, std::string> material_to_texture;

        std::string line;
        std::string current_material;

        while (std::getline(file, line))
        {
            if (line.empty() || line[0] == '#')
                continue;

            std::istringstream iss(line);
            std::string type;
            iss >> type;

            if (type == "newmtl")
            {
                iss >> current_material;
            }
            else if (type == "map_Kd")
            {
                // Everything after map_Kd is the texture path (can include spaces)
                std::string texture;
                std::getline(iss, texture);

                // trim leading spaces
                size_t start = texture.find_first_not_of(" \t");
                if (start != std::string::npos)
                {
                    texture = texture.substr(start);
                }

                if (!current_material.empty() && !texture.empty())
                {
                    material_to_texture[current_material] = trim(texture);
                }
            }
        }

        return material_to_texture;
    }

    Texture downsample(const Texture& source, const Vec2i target_res)
    {
        Texture target(target_res);

        const Vec2i source_res = source.get_resolution();
        if (target_res.x <= 0 || target_res.y <= 0)
            return target;

        for (int y = 0; y < target_res.y; ++y)
        {
            for (int x = 0; x < target_res.x; ++x)
            {
                const int x_start = x * source_res.x / target_res.x;
                const int y_start = y * source_res.y / target_res.y;
                int x_end = (x + 1) * source_res.x / target_res.x;
                int y_end = (y + 1) * source_res.y / target_res.y;

                x_end = std::max(x_end, x_start + 1);
                y_end = std::max(y_end, y_start + 1);

                x_end = std::min(x_end, source_res.x);
                y_end = std::min(y_end, source_res.y);

                uint64_t r = 0, g = 0, b = 0, a = 0;
                uint64_t count = 0;

                for (int sy = y_start; sy < y_end; ++sy)
                {
                    for (int sx = x_start; sx < x_end; ++sx)
                    {
                        const Color4 c = source.get_pixel({ sx, sy });
                        r += c.r;
                        g += c.g;
                        b += c.b;
                        a += c.a;
                        count++;
                    }
                }

                if (count > 0)
                {
                    target.set_pixel({ x, y }, Color4 {
                                         static_cast<uint8_t>(r / count),
                                         static_cast<uint8_t>(g / count),
                                         static_cast<uint8_t>(b / count),
                                         static_cast<uint8_t>(a / count)
                                     });
                }
            }
        }
        return target;
    }


    void replaceAll(std::string& str, const std::string& from, const std::string& to)
    {
        size_t start_pos = 0;
        while ((start_pos = str.find(from, start_pos)) != std::string::npos)
        {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length();
        }
    }

    class WaterVertexShader : public VertexShader
    {
        Output vert(const Input& input, const Uniforms& uniforms) const override
        {
            const Matrix4x1d pos_h {
                { input.pos.x },
                { input.pos.y },
                { input.pos.z },
                { 1.0 }
            };

            const Matrix4x1d normal_h {
                { input.normal.x },
                { input.normal.y },
                { input.normal.z },
                { 0.0 }
            };

            const Matrix4x1d normal_clip = uniforms.model_matrix * normal_h;
            const Matrix4x1d pos_clip = uniforms.mvp_matrix * pos_h + normal_clip * std::sin(uniforms.t * 2.0 + input.pos.x * 0.5 + input.pos.z * 0.5) * 0.1;

            return Output {
                .xyz    = { pos_clip(0, 0), pos_clip(1, 0), pos_clip(2, 0) },
                .w      = pos_clip(3, 0),
                .normal = { normal_clip(0, 0), normal_clip(1, 0), normal_clip(2, 0) }
            };
        }
    };

    void ConstructDemo::init()
    {
        renderer->clear_2D_scene();
        renderer->clear_3D_scene();
        renderer->set_clear_color(Color4(0.7, 0.7, 0.9, 1.0));
        renderer->set_ambient_light(0.5);
        renderer->set_light_direction(-1.0, 1.0, -1.0);
        renderer->get_render_3D()->set_texture_filtering_mode(Texture::FilteringMode::BILINEAR);

        auto& camera = renderer->get_camera();
        camera.set_position(20.0, -85.0, -15.0);
        camera.set_rotation_degrees(0.0, -90.0, 0.0);
        camera.set_fov_degrees(104.0);

        const Vec2i texture_res { 128, 128 };

        const Vec2i res20 { 40, 20 };
        const Vec2i res80 { 142, 80 };
        const Vec2i res360 { 640, 360 };
        const Vec2i res480 { 854, 480 };
        const Vec2i res720 { 1280, 720 };
        const Vec2i res1080 { 1920, 1080 };
        const Vec2i res1440 { 2560, 1440 };
        const Vec2i res1964 { 3024, 1964 };
        const Vec2i res2160 { 3840, 2160 };

        renderer->set_resolution(res360);

        const std::string assets_dir { "/home/sisev/Projects/code/cpp/sigfx/assets/models/ImageToStl/" };

        const auto diffuse_shader { std::make_shared<DiffuseFragmentShader>() };

        const auto water_texture { std::make_shared<Texture>(downsample(decode_bmp(assets_dir + "water.bmp"), texture_res)) };

        const auto default_material { std::make_shared<Material>(diffuse_shader) };
        const auto water_material { std::make_shared<Material>(diffuse_shader, water_texture) };

        std::unordered_map<std::string, size_t> material_map;
        std::unordered_map<std::string, std::string> mtl_texture_map = load_mtl_texture_map(assets_dir + "gm_construct_in_flatgrass.mtl");

        const TriangleMesh map_mesh = load_obj(assets_dir + "gm_construct_in_flatgrass.obj", material_map);
        map = std::make_shared<Polygon3D>();
        map->set_mesh(map_mesh);
        for (const auto& [name, mat_id] : material_map)
        {
            auto it = mtl_texture_map.find(name);
            if (it != mtl_texture_map.end())
            {
                std::string& texture_path = it->second;
                try
                {
                    replaceAll(texture_path, ".png", ".bmp");
                    const auto mat_texture = std::make_shared<Texture>(downsample(decode_bmp(assets_dir + texture_path), texture_res));
                    map->set_material(std::make_shared<Material>(diffuse_shader, mat_texture), mat_id);
                }
                catch (const std::exception& e)
                {
                    // If texture loading fails, use default material
                    map->set_material(default_material, mat_id);
                }
            }
            else
            {
                // No texture specified for this material, use default
                map->set_material(water_material, mat_id);
            }
        }
        renderer->add_primitive(map);

        crosshair = std::make_shared<Ellipse2D>();
        crosshair->set_radius(1.0, 1.0);
        crosshair->set_color(Color4(1.0, 1.0, 1.0, 0.5));
        crosshair->set_position(static_cast<Vec2d>(renderer->get_resolution() / 2));
        crosshair->set_filled(true);
        renderer->add_primitive(crosshair);
    }

    void ConstructDemo::render_frame(const double dt)
    {
        poll_held_keys(dt);
        update_camera(dt);

        debug_viewer->add_debug_line("triangles: " + std::to_string(renderer->get_render_3D()->get_num_triangles()), 0);

        renderer->clear_frame();
        renderer->render_frame();
        renderer->present_frame();
    }

    void ConstructDemo::end()
    {
        renderer->clear_scene();
    }

    void ConstructDemo::handle_char(const int input)
    {
    }

    void ConstructDemo::report_key(const KeyEvent event)
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

    void ConstructDemo::report_mouse(const MouseEvent event)
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

    void ConstructDemo::update_camera(const double dt)
    {
        camera_velocity = camera_velocity * std::pow(0.85, dt * 60.0);
        camera_velocity = camera_velocity.limit(max_camera_speed);
        Camera& camera = renderer->get_camera();
        camera.set_position(camera.get_position() + camera_velocity * dt);
    }

    void ConstructDemo::camera_movement(const Key key, const double dt)
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
        case Key::SHIFT:
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

    void ConstructDemo::poll_held_keys(const double dt)
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
