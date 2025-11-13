#include <gfx/debug/debug-viewer.h>
#include <gfx/utils/transform.h>

namespace gfx::debug
{

using namespace gfx::math;
using namespace gfx::core::types;

void DebugViewer::clear()
{
    aabb_items.clear();
    obb_items.clear();
    anchor_items.clear();
}

std::vector<std::shared_ptr<gfx::core::Primitive2D>> DebugViewer::get_debug_items() const
{
    std::vector<std::shared_ptr<gfx::core::Primitive2D>> items;

    for (const auto& [id, item] : aabb_items)
    {
        items.push_back(item);
    }
    
    for (const auto& [id, item] : obb_items)
    {
        items.push_back(item);
    }

    for (const auto& [id, item] : anchor_items)
    {
        items.push_back(item);
    }

    if (fps_text_item)
    {
        items.push_back(fps_text_item);
    }

    if (num_items_text_item)
    {
        items.push_back(num_items_text_item);
    }

    return items; 
}

void DebugViewer::populate(const DebugInfo &info)
{
    clear(); 

    for (const auto& [primitive, transform] : info.items)
    {
        if (!primitive->is_visible())
        {
            continue;
        }

        if (show_aabb)
        {
            Box2d aabb { primitive->get_axis_aligned_bounding_box(transform) };
            if (aabb_items.find(primitive->get_id()) == aabb_items.end())
            {
                auto box_primitive { std::make_shared<gfx::primitives::Polyline2D>() };
                box_primitive->set_color(bounds_color);
                box_primitive->set_line_thickness(1.0);
                box_primitive->set_close(true);
                box_primitive->set_points(aabb.get_corners());
                aabb_items[primitive->get_id()] = box_primitive;
            }
            else
            {
                auto box_primitive = aabb_items[primitive->get_id()];
                box_primitive->set_points(aabb.get_corners());
            }
        }

        if (show_obb)
        {
            OBB2D obb { primitive->get_oriented_bounding_box(transform) };
            if (obb_items.find(primitive->get_id()) == obb_items.end())
            {
                auto box_primitive { std::make_shared<gfx::primitives::Polyline2D>() };
                box_primitive->set_color(bounds_color);
                box_primitive->set_line_thickness(1.0);
                box_primitive->set_close(true);
                box_primitive->set_points(obb.get_corners());
                obb_items[primitive->get_id()] = box_primitive;
            }
            else
            {
                auto box_primitive = obb_items[primitive->get_id()];
                box_primitive->set_points(obb.get_corners());
            }
        }

        if (show_anchor)
        {
            Vec2d anchor_world = primitive->get_position();
            if (anchor_items.find(primitive->get_id()) == anchor_items.end())
            {
                auto anchor_primitive { std::make_shared<gfx::primitives::Circle2D>() };
                anchor_primitive->set_color(anchor_color);
                anchor_primitive->set_radius(1.0);
                anchor_primitive->set_filled(true);
                anchor_primitive->set_anchor(0.5, 0.5);
                anchor_primitive->set_position(anchor_world);
                anchor_items[primitive->get_id()] = anchor_primitive;
            }
            else
            {
                auto anchor_primitive = anchor_items[primitive->get_id()];
                anchor_primitive->set_position(anchor_world);
            }
        }

        if (!font)
        {
            return;
        }

        double text_scale = (info.resolution.y / font_size) * 0.08;

        if (show_fps)
        {
            if (!fps_text_item)
            {
                fps_text_item = std::make_shared<gfx::primitives::Text2D>();
                fps_text_item->set_font(font);
                fps_text_item->set_font_size(font_size);
                fps_text_item->set_color(text_color);
                fps_text_item->set_scale(text_scale);
                fps_text_item->set_position(5.0, 5.0);
            }
            fps_text_item->set_text("FPS: " + std::to_string(static_cast<int>(info.fps)));
        }
        if (show_num_items)
        {
            if (!num_items_text_item)
            {
                num_items_text_item = std::make_shared<gfx::primitives::Text2D>();
                num_items_text_item->set_font(font);
                num_items_text_item->set_font_size(font_size);
                num_items_text_item->set_color(text_color);
                num_items_text_item->set_scale(text_scale);
                num_items_text_item->set_position(5.0, font_size * text_scale + 5.0);
            }
            num_items_text_item->set_text("Items: " + std::to_string(info.num_items));
        }
    }
}


}
