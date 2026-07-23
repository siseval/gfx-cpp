#include "gfx/core/render-engine.h"

namespace gfx
{
    RenderEngine::RenderEngine(std::shared_ptr<RenderSurface> surface)
        : _surface(surface)
    {
        _render2D = std::make_shared<Render2D>(surface);
        _render3D = std::make_shared<Render3D>(surface);
    }

    void RenderEngine::render_frame() const
    {
        _render3D->draw_frame();
        _render2D->draw_frame();
    }

    void RenderEngine::present_frame() const
    {
        _surface->present();
    }

    void RenderEngine::clear_frame() const
    {
        _surface->clear_frame_buffer();
    }

    void RenderEngine::add_primitive(const std::shared_ptr<Primitive2D> primitive) const
    {
        _render2D->add_item(primitive);
    }

    void RenderEngine::add_primitive(const std::shared_ptr<Primitive3D> primitive) const
    {
        _render3D->add_item(primitive);
    }

    void RenderEngine::remove_primitive(const std::shared_ptr<Primitive2D> primitive) const
    {
        _render2D->remove_item(primitive);
    }

    void RenderEngine::remove_primitive(const std::shared_ptr<Primitive3D> primitive) const
    {
        _render3D->remove_item(primitive);
    }

    bool RenderEngine::contains_primitive(const std::shared_ptr<Primitive2D> primitive) const
    {
        return _render2D->contains_item(primitive);
    }

    bool RenderEngine::contains_primitive(const std::shared_ptr<Primitive3D> primitive) const
    {
        return _render3D->contains_item(primitive);
    }

    void RenderEngine::clear_scene() const
    {
        _render2D->clear_items();
        _render3D->clear_items();
    }

    void RenderEngine::clear_3D_scene() const
    {
        _render3D->clear_items();
    }

    void RenderEngine::clear_2D_scene() const
    {
        _render2D->clear_items();
    }

    int RenderEngine::num_primitives() const
    {
        return num_2D_primitives() + num_3D_primitives();
    }

    int RenderEngine::num_2D_primitives() const
    {
        return _render2D->num_items();
    }

    int RenderEngine::num_3D_primitives() const
    {
        return _render3D->num_items();
    }

    void RenderEngine::set_resolution(const Vec2i new_resolution) const
    {
        _surface->resize(new_resolution);
    }

    void RenderEngine::set_resolution(const int width, const int height) const
    {
        _surface->resize(Vec2i { width, height });
    }

    Vec2i RenderEngine::get_resolution() const
    {
        return _surface->get_resolution();
    }

    void RenderEngine::set_camera(const Camera& cam) const
    {
        _render3D->set_camera(cam);
    }

    void RenderEngine::set_light_direction(const Vec3d direction) const
    {
        _render3D->set_light_direction(direction);
    }

    void RenderEngine::set_light_direction(const double x, const double y, const double z) const
    {
        _render3D->set_light_direction(Vec3d { x, y, z });
    }

    void RenderEngine::set_ambient_light(const double intensity) const
    {
        _render3D->set_ambient_light(intensity);
    }

    const Camera& RenderEngine::get_camera() const
    {
        return _render3D->get_camera();
    }

    Camera& RenderEngine::get_camera()
    {
        return _render3D->get_camera();
    }

    Vec3d RenderEngine::get_light_direction() const
    {
        return _render3D->get_light_direction();
    }

    double RenderEngine::get_ambient_light() const
    {
        return _render3D->get_ambient_light();
    }

    void RenderEngine::set_font_directory(const std::filesystem::path& path) const
    {
        _render2D->set_font_directory(path);
    }

    void RenderEngine::load_font_directory(const std::filesystem::path& path) const
    {
        _render2D->load_font_directory(path);
    }

    std::filesystem::path RenderEngine::get_font_directory() const
    {
        return _render2D->get_font_directory();
    }

    std::shared_ptr<FontTTF> RenderEngine::get_font(const std::string& name) const
    {
        return _render2D->get_font(name);
    }

    bool RenderEngine::is_font_loaded(const std::string& name) const
    {
        return _render2D->is_font_loaded(name);
    }

    void RenderEngine::set_render_surface(const std::shared_ptr<RenderSurface> new_surface)
    {
        _surface = new_surface;
        _render2D->set_render_surface(new_surface);
        _render3D->set_render_surface(new_surface);
    }

    std::shared_ptr<RenderSurface> RenderEngine::get_render_surface() const
    {
        return _surface;
    }

    std::shared_ptr<Render2D> RenderEngine::get_render_2D() const
    {
        return _render2D;
    }

    std::shared_ptr<Render3D> RenderEngine::get_render_3D() const
    {
        return _render3D;
    }

    void RenderEngine::set_clear_color(const Color4 color) const
    {
        _surface->set_clear_color(color);
    }

    Color4 RenderEngine::get_clear_color() const
    {
        return _surface->get_clear_color();
    }

    std::shared_ptr<Circle2D> RenderEngine::create_circle(
        const Vec2d position,
        const double radius,
        const Color4 color,
        const double line_thickness
    ) const
    {
        return _render2D->create_circle(position, radius, color, line_thickness);
    }

    std::shared_ptr<Circle2D> RenderEngine::create_circle(
        const double x,
        const double y,
        const double radius,
        const Color4 color,
        const double line_thickness
    ) const
    {
        return create_circle(Vec2d { x, y }, radius, color, line_thickness);
    }

    std::shared_ptr<Ellipse2D> RenderEngine::create_ellipse(
        const Vec2d position,
        const Vec2d radius,
        const Color4 color,
        const double line_thickness
    ) const
    {
        return _render2D->create_ellipse(position, radius, color, line_thickness);
    }

    std::shared_ptr<Ellipse2D> RenderEngine::create_ellipse(
        const double x,
        const double y,
        const double radius_x,
        const double radius_y,
        const Color4 color,
        const double line_thickness
    ) const
    {
        return create_ellipse(Vec2d { x, y }, Vec2d { radius_x, radius_y }, color, line_thickness);
    }

    std::shared_ptr<Polyline2D> RenderEngine::create_polyline(
        const Vec2d position,
        const std::vector<Vec2d>& points,
        const Color4 color,
        const double line_thickness
    ) const
    {
        return _render2D->create_polyline(position, points, color, line_thickness);
    }

    std::shared_ptr<Polyline2D> RenderEngine::create_polyline(
        const double x,
        const double y,
        const std::vector<Vec2d>& points,
        const Color4 color,
        const double line_thickness
    ) const
    {
        return create_polyline(Vec2d { x, y }, points, color, line_thickness);
    }

    std::shared_ptr<Polygon2D> RenderEngine::create_polygon(
        const Vec2d position,
        const std::vector<Vec2d>& points,
        const Color4 color
    ) const
    {
        return _render2D->create_polygon(position, points, color);
    }

    std::shared_ptr<Polygon2D> RenderEngine::create_polygon(
        const double x,
        const double y,
        const std::vector<Vec2d>& points,
        const Color4 color
    ) const
    {
        return create_polygon(Vec2d { x, y }, points, color);
    }

    // std::shared_ptr<Bitmap2D> RenderEngine::create_bitmap(const Vec2d position, const Bitmap &bm) const
    // {
    //     return render2D->create_bitmap(position, bm);
    // }
    //
    // std::shared_ptr<Bitmap2D> RenderEngine::create_bitmap(const double x, const double y, const Bitmap &bm) const
    // {
    //     return create_bitmap(Vec2d { x, y }, bm);
    // }
    //
    // std::shared_ptr<Bitmap2D> RenderEngine::create_bitmap(const Vec2d position, const Vec2i resolution) const
    // {
    //     return render2D->create_bitmap(position, resolution);
    // }
    //
    // std::shared_ptr<Bitmap2D> RenderEngine::create_bitmap(const double x, const double y, const Vec2i resolution) const
    // {
    //     return create_bitmap(Vec2d { x, y }, resolution);
    // }

    std::shared_ptr<Text2D> RenderEngine::create_text(
        const Vec2d position,
        const std::string& text,
        const std::shared_ptr<FontTTF> font,
        const double font_size,
        const Color4 color
    ) const
    {
        return get_render_2D()->create_text(position, text, font, font_size, color);
    }

    std::shared_ptr<Text2D> RenderEngine::create_text(
        const double x,
        const double y,
        const std::string& text,
        const std::shared_ptr<FontTTF> font,
        const double font_size,
        const Color4 color
    ) const
    {
        return create_text(Vec2d { x, y }, text, font, font_size, color);
    }
}
