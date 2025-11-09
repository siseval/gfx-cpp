#ifndef DEBUG_VIEWER_H
#define DEBUG_VIEWER_H

#include <memory>
#include <unordered_map>
#include <vector>
#include <utility>
#include <gfx/math/box2.h>
#include <gfx/core/types/obb-2D.h>
#include <gfx/core/types/pixel.h>
#include <gfx/core/types/color4.h>
#include <gfx/core/primitive-2D.h>
#include <gfx/primitives/polyline-2D.h>
#include <gfx/primitives/circle-2D.h>
#include <gfx/primitives/text-2D.h>

namespace gfx::debug
{

struct DebugInfo
{
    std::vector<std::pair<std::shared_ptr<gfx::core::Primitive2D>, gfx::math::Matrix3x3d>> items;
    double fps;
    int num_items;
    gfx::math::Vec2d resolution;
};

class DebugViewer
{

public:

    void render_debug_view(DebugInfo &info);
    void populate(const DebugInfo &info);
    void clear();

    std::vector<std::shared_ptr<gfx::core::Primitive2D>> get_debug_items() const;
    inline std::unordered_map<gfx::utils::UUID, std::shared_ptr<gfx::primitives::Polyline2D>> get_aabb_items() const { return aabb_items; }
    inline std::unordered_map<gfx::utils::UUID, std::shared_ptr<gfx::primitives::Polyline2D>> get_obb_items() const { return obb_items; }
    inline std::unordered_map<gfx::utils::UUID, std::shared_ptr<gfx::primitives::Circle2D>> get_anchor_items() const { return anchor_items; }

    void set_enabled(const bool enable) { enabled = enable; }
    bool is_enabled() const { return enabled; }

    void set_font(const std::shared_ptr<gfx::text::FontTTF> fnt) { font = fnt; }

    void set_show_aabb(const bool show) { show_aabb = show; }
    void set_show_obb(const bool show) { show_obb = show; }
    void set_show_anchor(const bool show) { show_anchor = show; }
    void set_show_fps(const bool show) { show_fps = show; }
    void set_show_num_items(const bool show) { show_num_items = show; }

    bool get_show_aabb() const { return show_aabb; }
    bool get_show_obb() const { return show_obb; }
    bool get_show_anchor() const { return show_anchor; }
    bool get_show_fps() const { return show_fps; }
    bool get_show_num_items() const { return show_num_items; }


    void set_bounds_color(const gfx::core::types::Color4 &color) { bounds_color = color; }

private:

    void cleanup();

    void render_aabb(const gfx::math::Box2d &aabb);
    void render_obb(const gfx::core::types::OBB2D &obb);
    void render_anchor_point(const gfx::math::Vec2d anchor);

    bool enabled = true;

    bool show_aabb = true;
    bool show_obb = true;
    bool show_anchor = true;

    bool show_fps = true;
    bool show_num_items = true;

    std::shared_ptr<gfx::text::FontTTF> font;
    double font_size = 36.0;
    gfx::core::types::Color4 text_color { 255, 255, 255 };

    gfx::core::types::Color4 bounds_color { 255, 255, 255 };
    gfx::core::types::Color4 anchor_color { 255, 255, 0 };

    std::shared_ptr<gfx::primitives::Text2D> fps_text_item;
    std::shared_ptr<gfx::primitives::Text2D> num_items_text_item;

    std::unordered_map<gfx::utils::UUID, std::shared_ptr<gfx::primitives::Polyline2D>> aabb_items;
    std::unordered_map<gfx::utils::UUID, std::shared_ptr<gfx::primitives::Polyline2D>> obb_items;
    std::unordered_map<gfx::utils::UUID, std::shared_ptr<gfx::primitives::Circle2D>> anchor_items;

};

}

#endif // DEBUG_VIEWER_H
