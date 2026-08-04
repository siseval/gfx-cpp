// #pragma once
//
// #include "gfx/core/render-engine.h"
// #include "gfx/core/types/color4.h"
// #include "gfx/core/types/obb-2D.h"
// #include "gfx/math/box2.h"
// #include "gfx/primitives/circle-2D.h"
// #include "gfx/primitives/polyline-2D.h"
// #include "gfx/primitives/text-2D.h"
//
// #include <deque>
//
//
// namespace gfx
// {
//     class DebugViewer
//     {
//         struct RendererInfo
//         {
//             double fps;
//             int num_items;
//         };
//
//         struct ItemInfo2D
//         {
//             UUID id;
//             Box2d aabb;
//             OBB2D obb;
//             Vec2d anchor_point;
//         };
//
//         struct DebugRender2D
//         {
//             std::shared_ptr<Polyline2D> aabb;
//             std::shared_ptr<Polyline2D> obb;
//             std::shared_ptr<Circle2D> anchor;
//         };
//
//     public:
//
//         void update(std::shared_ptr<RenderEngine> render_engine);
//
//         void set_enabled(bool enable);
//         void set_font(std::shared_ptr<FontTTF> fnt);
//         void set_font_size(double size);
//         void set_text_color(Color4 color);
//         void set_show_fps(bool show);
//         void set_show_num_items(bool show);
//         void set_text_position(Vec2d position);
//
//         void add_debug_line(const std::string& line, int line_number = -1);
//
//         bool get_enabled() const;
//         std::shared_ptr<FontTTF> get_font() const;
//         double get_font_size() const;
//         Color4 get_text_color() const;
//         bool get_show_fps() const;
//         bool get_show_num_items() const;
//         Vec2d get_text_position() const;
//
//     private:
//
//         void populate(std::shared_ptr<RenderEngine> render_engine);
//         void clear(std::shared_ptr<RenderEngine> render_engine) const;
//
//         RendererInfo gather_renderer_info(std::shared_ptr<RenderEngine> render_engine);
//         void update_text(std::shared_ptr<RenderEngine> render_engine, const RendererInfo& info);
//         int num_debug_items() const;
//
//         bool _enabled { false };
//
//         bool _show_fps { true };
//         bool _show_num_items { true };
//
//         std::shared_ptr<FontTTF> _font;
//         double _font_size { 8.0 };
//         Color4 _text_color { Color4::white() };
//         Vec2d _text_position { 4.0, 4.0 };
//
//         std::shared_ptr<Text2D> _text;
//         std::vector<std::string> _debug_lines;
//
//         std::deque<double> _fps_history;
//         int _fps_history_max_size { 20 };
//         double _last_frame_timestamp_ms { 0.0 };
//     };
// }
