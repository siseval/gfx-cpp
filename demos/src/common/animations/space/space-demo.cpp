#include <algorithm>

#include "common/animations/space/space-demo.h"
#include "common/animations/space/simulate.h"
#include "common/animations/space/units.h"
#include "common/core/demo-utils.h"

namespace demos
{
    using namespace gfx;

    void SpaceDemo::init()
    {
        time_scale = 0.01;
        clear_bodies();
        hovered_body = nullptr;

        const std::vector<Vec2d> arrow_points { { -20, -20 }, { 30, 0 }, { -20, 20 }, { 0, 0 } };
        spawn_velocity_indicator_head = render2D->create_polyline(
            { 0, 0 },
            arrow_points,
            Color4 { 255, 255, 255 },
            2.0
        );
        spawn_velocity_indicator_head->set_close(true);
        spawn_velocity_indicator_head->set_rounded_corners(true);

        spawn_velocity_indicator_line = render2D->
            create_polyline({ 0, 0 }, { { 0, 0 }, { 10, 0 } }, Color4 { 255, 255, 255 }, 2.0);
        spawn_velocity_indicator_line->set_rounded_corners(true);

        spawn_radius_indicator = render2D->create_ellipse({ 0, 0 }, Vec2d(10.0), Color4 { 255, 255, 255 }, 2.0);
        spawn_radius_indicator->set_anchor({ 0.5, 0.5 });
        spawn_radius_indicator_inner = render2D->create_ellipse({ 0, 0 }, Vec2d(10.0), Color4 { 200, 200, 200 }, 2.0);
        spawn_radius_indicator_inner->set_filled(true);

        render2D->add_item(spawn_velocity_indicator_head);
        render2D->add_item(spawn_velocity_indicator_line);
        render2D->add_item(spawn_radius_indicator);
        render2D->add_item(spawn_radius_indicator_inner, spawn_radius_indicator);

        const std::vector<Vec2d> bracket_points { { 20, 0 }, { 0, 20 }, { 0, 90, }, { 20, 110 } };
        left_bracket = render2D->create_polyline(
            { 0, 0 },
            bracket_points,
            Color4 { 255, 255, 255 },
            min_bracket_thickness
        );
        right_bracket = render2D->create_polyline(
            { 0, 0 },
            bracket_points,
            Color4 { 255, 255, 255 },
            min_bracket_thickness
        );
        right_bracket->set_rotation(std::numbers::pi);
        left_bracket->set_anchor({ 0.5, 0.5 });
        right_bracket->set_anchor({ 0.5, 0.5 });
        left_bracket->set_rounded_corners(true);
        right_bracket->set_rounded_corners(true);
        render2D->add_item(left_bracket);
        render2D->add_item(right_bracket);

        Simulate::solar_system(*this);
    }

    void SpaceDemo::render_frame(const double dt)
    {
        const double t0 { time_us() };

        t_sec += dt;

        physics_process(dt * time_scale);

        const double time_lerp { std::min(time_accumulator / PHYSICS_TIME_STEP, 1.0) };
        handle_camera(dt, time_lerp);
        update_render_items(time_lerp);
        update_trails(time_lerp);
        handle_hovered_body(dt, time_lerp);
        update_hovered_brackets(time_lerp);

        render_spawn_workflow();

        render2D->clear_frame();
        render2D->draw_frame();
        render2D->present_frame();

        last_frame_us = time_us() - t0;
    }

    void SpaceDemo::end()
    {
        clear_bodies();
    }

    void SpaceDemo::handle_char(const int input)
    {
        camera.handle_input(input);

        switch (input)
        {
        case 27:
            cancel_spawn_workflow();
            break;
        case 'r':
            if (hovered_body)
            {
                remove_body(hovered_body);
                hovered_body = nullptr;
            }
            break;
        case 't':
            trails_visible = !trails_visible;
            break;

        case ' ':
            scroll_zoom_in = !scroll_zoom_in;
            break;

        case 'K':
            time_scale *= 1.5;
            break;

        case 'J':
            time_scale /= 1.5;
            break;

        case 'n':
            cycle_tracked_body();
            break;

        case 'p':
            cycle_tracked_body(-1);
            break;

        case 'u':
            tracked_body_index = -1;
            break;

        default:
            break;
        }
    }

    void SpaceDemo::report_mouse(const MouseEvent event)
    {
        mouse_pos = event.position * render2D->get_resolution();
        hover_mouse = true;
        switch (event.type)
        {
        case MouseEventType::LEFT_DOWN:
            {
                if (hovered_body)
                {
                    track_body(hovered_body);
                    cancel_spawn_workflow();
                }
                else
                {
                    progress_spawn_workflow(mouse_pos);
                }
                break;
            }

        case MouseEventType::SCROLL_UP:
            camera.smooth_zoom(0.9);
            break;

        case MouseEventType::SCROLL_DOWN:
            camera.smooth_zoom(1.1);
            break;

        default:
            break;
        }
    }

    std::shared_ptr<Body> SpaceDemo::spawn_body(
        const Vec2d position,
        const Vec2d velocity,
        const double radius,
        const double mass,
        const Color4 color
    )
    {
        return spawn_body("", position, velocity, radius, mass, color);
    }

    std::shared_ptr<Body> SpaceDemo::spawn_body(
        const std::string name,
        const Vec2d position,
        const Vec2d velocity,
        const double radius,
        const double mass,
        const Color4 color
    )
    {
        return spawn_body(name, position, velocity, radius, mass, false, color);
    }

    std::shared_ptr<Body> SpaceDemo::spawn_body(
        const std::string name,
        const Vec2d position,
        const Vec2d velocity,
        const double radius,
        const double mass,
        const bool locked,
        const Color4 color
    )
    {
        const auto shape = std::make_shared<Ellipse2D>();
        shape->set_filled(true);
        shape->set_anchor({ 0.5, 0.5 });
        render2D->add_item(shape);

        const auto trail = render2D->create_polyline(
            { 0, 0 },
            std::vector<Vec2d>(trail_length, { 0, 0 }),
            Color4(1.0, 1.0, 1.0),
            1.0
        );
        render2D->add_item(trail);

        RenderBody render_body { shape, trail };

        auto body = std::make_shared<Body>(name, position, velocity, radius, mass, locked, color);

        body_trails[body] = std::vector(trail_length, position);

        body_items.emplace(std::make_pair(body, render_body));
        body_list.push_back(body);

        return body_items.begin()->first;
    }

    void SpaceDemo::remove_body(const std::shared_ptr<Body> body)
    {
        if (!body_items.contains(body))
        {
            return;
        }
        render2D->remove_item(body_items.at(body).ellipse);
        render2D->remove_item(body_items.at(body).trail);
        body_items.erase(body);
        std::erase(body_list, body);
    }

    void SpaceDemo::clear_bodies()
    {
        std::vector<std::shared_ptr<Body>> to_remove;
        for (const auto& [body, render] : body_items)
        {
            to_remove.push_back(body);
        }
        for (const auto& body : to_remove)
        {
            remove_body(body);
        }
    }

    void SpaceDemo::physics_process(const double dt)
    {
        time_accumulator += dt;
        int steps = 0;

        while (time_accumulator >= PHYSICS_TIME_STEP && steps < MAX_STEPS_PER_FRAME)
        {
            for (auto& [body, render] : body_items)
            {
                render.previous_pos = body->get_position();
            }
            process_bodies(PHYSICS_TIME_STEP);
            time_accumulator -= PHYSICS_TIME_STEP;
            steps++;
        }
    }

    void SpaceDemo::process_bodies(const double dt)
    {
        for (auto& [body, item] : body_items)
        {
            for (const auto& [body2, item2] : body_items)
            {
                if (body == body2)
                {
                    continue;
                }
                if (body->is_locked())
                {
                    continue;
                }
                body->apply_gravity(body2, dt);
            }
            body->update_position(dt);
        }
    }

    void SpaceDemo::handle_camera(const double dt, const double time_lerp)
    {
        camera.end_pos = camera.cur_pos;
        if (camera.state == Camera::State::Free)
        {
            untrack_body();
        }

        if (get_tracked_body())
        {
            RenderBody render_body { body_items.at(get_tracked_body()) };
            camera.end_pos = body_interpolated_pos(get_tracked_body(), time_lerp);
        }

        if (camera.state == Camera::State::Transitioning && previous_tracked_body)
        {
            camera.start_pos = body_interpolated_pos(previous_tracked_body, time_lerp);
        }
        camera.process(dt);
        set_view_pos(camera.cur_pos);
        set_view_size(camera.size_cur.x);
    }

    void SpaceDemo::update_trails(const double time_lerp)
    {
        if (!trails_visible)
        {
            for (const auto& [body, render] : body_items)
            {
                body_trails[body] = std::vector(trail_length, body->get_position());
                render.trail->set_visible(false);
            }
            return;
        }
        for (const auto& [body, render] : body_items)
        {
            render.trail->set_visible(true);
        }
        for (const auto& [body, render] : body_items)
        {
            const auto trail { render.trail };
            std::vector<Vec2d>& trail_points_world { body_trails[body] };

            Vec2d trail_pos_world { Vec2d::lerp(render.previous_pos, body->get_position(), time_lerp) };

            const double trail_point_spacing { view_bounds.size().length() * 0.01 };
            const double dist_to_last_point { Vec2d::distance(trail_points_world.back(), trail_pos_world) };
            if (dist_to_last_point > trail_point_spacing)
            {
                trail_points_world.erase(trail_points_world.begin());
                trail_points_world.push_back(trail_pos_world);
            }

            trail->clear_points();

            trail->add_point(
                units::metres_to_pixels(
                    Vec2d::lerp(
                        trail_points_world[0] - view_bounds.min,
                        trail_points_world[1] - view_bounds.min,
                        dist_to_last_point / trail_point_spacing
                    ),
                    view_bounds.size(),
                    static_cast<Vec2d>(get_resolution())
                )
            );

            for (int i = 1; i < trail_points_world.size(); ++i)
            {
                const Vec2d trail_point_screen {
                    units::metres_to_pixels(
                        trail_points_world[i] - view_bounds.min,
                        view_bounds.size(),
                        static_cast<Vec2d>(get_resolution())
                    )
                };
                const double margin { render2D->get_resolution().length() * 1.0 };
                if (trail_point_screen.x < -margin || trail_point_screen.x > get_resolution().x + margin ||
                    trail_point_screen.y < -margin || trail_point_screen.y > get_resolution().y + margin)
                {
                    continue;
                }
                trail->add_point(trail_point_screen);
                trail->set_segment_visible(i, true);
            }
            for (int i = 0; i < trail->get_num_points(); ++i)
            {
                const Vec2d point { trail->get_point(i) };
                const double margin { render2D->get_resolution().length() * 0.1 };
                if (point.x < -margin || point.x > get_resolution().x + margin ||
                    point.y < -margin || point.y > get_resolution().y + margin)
                {
                    trail->set_segment_visible(i, false);
                    if (i > 0)
                    {
                        trail->set_segment_visible(i - 1, false);
                    }
                }
            }
            trail->add_point(
                units::metres_to_pixels(
                    Vec2d::lerp(render.previous_pos, body->get_position(), time_lerp) - view_bounds.min,
                    view_bounds.size(),
                    static_cast<Vec2d>(get_resolution())
                )
            );
        }
    }

    void SpaceDemo::update_render_items(const double time_lerp)
    {
        for (const auto& [body, render] : body_items)
        {
            const auto ellipse { render.ellipse };

            Vec2d pos_lerp { Vec2d::lerp(render.previous_pos, body->get_position(), time_lerp) };
            const Vec2d pos {
                units::metres_to_pixels(pos_lerp - view_bounds.min, view_bounds.size(), static_cast<Vec2d>(get_resolution()))
            };
            Vec2d radius { units::metres_to_pixels(Vec2d(body->get_radius()), view_bounds.size(), static_cast<Vec2d>(get_resolution())) };

            if (pos.x + radius.x < 0 || pos.x - radius.x > get_resolution().round_to_int().x ||
                pos.y + radius.y < 0 || pos.y - radius.y > get_resolution().round_to_int().y)
            {
                ellipse->set_visible(false);
                continue;
            }

            ellipse->set_rotation(ellipse->get_rotation() + 0.005);
            ellipse->set_visible(true);
            ellipse->set_position(pos);
            ellipse->set_radius({ radius.x, radius.y * 0.985 });
            ellipse->set_color(body->get_color());
        }
    }

    void SpaceDemo::zoom(const double factor)
    {
        const Vec2d anchor_pos { view_bounds.min + view_bounds.size() * view_anchor };
        view_bounds.min = anchor_pos - (anchor_pos - view_bounds.min) * factor;
        view_bounds.max = anchor_pos + (view_bounds.max - anchor_pos) * factor;
    }

    Vec2d SpaceDemo::get_screen_pos(const Vec2d world_pos) const
    {
        return units::metres_to_pixels(world_pos - view_bounds.min, view_bounds.size(), static_cast<Vec2d>(get_resolution()));
    }

    Vec2d SpaceDemo::get_world_pos(const Vec2d screen_pos) const
    {
        const Vec2d resolution { get_resolution() };
        return view_bounds.min + Vec2d {
            screen_pos.x / resolution.x * view_bounds.size().x,
            screen_pos.y / resolution.y * view_bounds.size().y
        };
    }

    std::shared_ptr<Body> SpaceDemo::get_closest_body(const Vec2d position, const double max_distance) const
    {
        std::shared_ptr<Body> closest_body { nullptr };
        double closest_distance { std::numeric_limits<double>::max() };

        for (const auto body : body_list)
        {
            const double distance = Vec2d::distance(position, body->get_position());
            if (distance < closest_distance)
            {
                closest_distance = distance;
                closest_body = body;
            }
        }
        return max_distance == 0 || closest_distance <= max_distance ? closest_body : nullptr;
    }

    std::shared_ptr<Body> SpaceDemo::get_closest_body_screenspace(
        const Vec2d screen_pos,
        const double max_distance,
        const double euqality_threshold,
        const double time_lerp
    ) const
    {
        std::shared_ptr<Body> closest_body { nullptr };
        double closest_distance { std::numeric_limits<double>::max() };

        for (const auto body : body_list)
        {
            const Vec2d pos { get_screen_pos(body_interpolated_pos(body, time_lerp)) };
            const double distance {
                Vec2d::distance(screen_pos, pos) -
                units::metres_to_pixels(Vec2d(body->get_radius()), view_bounds.size(), static_cast<Vec2d>(get_resolution())).x
            };
            if (distance < closest_distance)
            {
                const bool equal { std::abs(distance - closest_distance) < euqality_threshold };
                if (equal && body->get_radius() < closest_body->get_radius())
                {
                    continue;
                }
                closest_distance = distance;
                closest_body = body;
            }
        }
        return max_distance == 0 || closest_distance <= max_distance ? closest_body : nullptr;
    }

    std::shared_ptr<Body> SpaceDemo::get_tracked_body()
    {
        if (tracked_body_index < 0 || tracked_body_index >= body_list.size())
        {
            return nullptr;
        }
        return body_list[tracked_body_index];
    }

    void SpaceDemo::cycle_tracked_body(const int direction)
    {
        const auto body { body_list[((tracked_body_index + direction) % body_list.size())] };
        track_body(body);
    }

    void SpaceDemo::track_body(const std::shared_ptr<Body> body)
    {
        camera.size0 = view_bounds.size();
        camera.start_pos = get_anchor_world_pos();
        camera.size_cur = view_bounds.size();
        camera.cur_pos = get_anchor_world_pos();

        previous_tracked_body = camera.state == Camera::State::Tracking ? get_tracked_body() : nullptr;

        for (int i = 0; i < body_list.size(); ++i)
        {
            if (body_list[i] == body)
            {
                tracked_body_index = i;
            }
        }

        camera.zoom_out_size = Vec2d(
            std::max(
                get_tracked_body()->get_radius() * tracked_body_zoom_level,
                Vec2d::distance(camera.start_pos, get_tracked_body()->get_position()) * 4.0
            )
        );

        camera.size1 = Vec2d(get_tracked_body()->get_radius() * tracked_body_zoom_level);
        camera.end_pos = get_tracked_body()->get_position();
        camera.state = Camera::State::Transitioning;
        camera.velocity = { 0.0, 0.0 };
        camera.zoom_velocity = 0.0;
        camera.track_time = 0.0;
    }

    void SpaceDemo::untrack_body()
    {
        tracked_body_index = -1;
    }

    void SpaceDemo::progress_spawn_workflow(const Vec2d pos)
    {
        if (spawn_workflow_state == SpawnWorkflowState::Inactive)
        {
            spawn_workflow_state = SpawnWorkflowState::RadiusSelection;
            spawn_screen_pos = pos;
        }
        else if (spawn_workflow_state == SpawnWorkflowState::RadiusSelection)
        {
            spawn_workflow_state = SpawnWorkflowState::VelocitySelection;
            spawn_radius = units::pixels_to_metres(pos - spawn_screen_pos, view_bounds.size(), static_cast<Vec2d>(get_resolution()));
        }
        else if (spawn_workflow_state == SpawnWorkflowState::VelocitySelection)
        {
            spawn_workflow_state = SpawnWorkflowState::Inactive;
            spawn_velocity = units::pixels_to_metres(pos - spawn_screen_pos, view_bounds.size(), static_cast<Vec2d>(get_resolution()));
            if (get_tracked_body())
            {
                spawn_velocity += get_tracked_body()->get_velocity();
            }
            spawn_body(
                get_world_pos(spawn_screen_pos),
                spawn_velocity * spawn_velocity_multiplier,
                spawn_radius.length(),
                spawn_radius.length() * spawn_mass_multiplier,
                Color4 { 200, 200, 255 }
            );
        }
    }

    void SpaceDemo::cancel_spawn_workflow()
    {
        spawn_workflow_state = SpawnWorkflowState::Inactive;
    }

    void SpaceDemo::render_spawn_workflow() const
    {
        if (spawn_workflow_state == SpawnWorkflowState::Inactive)
        {
            spawn_velocity_indicator_head->set_visible(false);
            spawn_velocity_indicator_line->set_visible(false);
            spawn_radius_indicator->set_visible(false);
            spawn_radius_indicator_inner->set_visible(false);
        }
        else if (spawn_workflow_state == SpawnWorkflowState::RadiusSelection)
        {
            spawn_velocity_indicator_head->set_visible(false);
            spawn_velocity_indicator_line->set_visible(false);
            spawn_radius_indicator->set_visible(true);
            spawn_radius_indicator_inner->set_visible(false);

            spawn_radius_indicator->set_position(spawn_screen_pos);

            const double radius { Vec2d::distance(spawn_screen_pos, mouse_pos) };
            spawn_radius_indicator->set_radius(Vec2d(radius));
            spawn_radius_indicator_inner->set_radius(Vec2d(radius));
        }
        else if (spawn_workflow_state == SpawnWorkflowState::VelocitySelection)
        {
            spawn_velocity_indicator_head->set_visible(true);
            spawn_velocity_indicator_line->set_visible(true);
            spawn_radius_indicator->set_visible(true);
            spawn_radius_indicator_inner->set_visible(true);

            const Vec2d mouse_screen_pos { mouse_pos };

            const Vec2d dir { (mouse_screen_pos - spawn_screen_pos).normalize() };
            const Vec2d line_origin { spawn_screen_pos + dir * spawn_radius_indicator->get_radius().x };
            const double line_length { Vec2d::distance(mouse_screen_pos, line_origin) };

            const double scale_t {
                std::min(line_length / spawn_radius_indicator->get_line_thickness(), 1.0)
            };
            spawn_velocity_indicator_line->set_line_thickness(
                std::lerp(min_bracket_thickness, spawn_velocity_indicator_max_thickness, scale_t)
            );

            spawn_velocity_indicator_line->set_position(line_origin);
            spawn_velocity_indicator_line->set_point(1, mouse_screen_pos - line_origin);

            const double angle { dir.angle() };
            spawn_velocity_indicator_head->set_rotation(angle);
            spawn_velocity_indicator_head->set_position(mouse_screen_pos);
            spawn_velocity_indicator_head->set_scale(
                std::lerp(0.01, spawn_velocity_indicator_max_scale, scale_t)
            );
        }
    }

    void SpaceDemo::update_hovered_brackets(const double time_lerp) const
    {
        if (hovered_body == nullptr)
        {
            left_bracket->set_visible(false);
            right_bracket->set_visible(false);
            return;
        }
        left_bracket->set_visible(true);
        right_bracket->set_visible(true);

        const auto body { hovered_body };

        const double body_screen_radius {
            units::metres_to_pixels(Vec2d(body->get_radius()), view_bounds.size(), static_cast<gfx::Vec2d>(get_resolution())).x
        };

        const Vec2d pos { get_screen_pos(body_interpolated_pos(body, time_lerp)) };

        const double offset {
            std::max(body_screen_radius * 3.0, min_bracket_distance) +
            1 + std::sin(t_sec * std::numbers::pi * 2.0 * bracket_frequency) * bracket_amplitude
        };

        left_bracket->set_position(pos - Vec2d { offset, 0 });
        right_bracket->set_position(pos + Vec2d { offset, 0 });

        const double scale {
            std::lerp(
                0.0,
                std::max(body_screen_radius * 2.5, min_bracket_scale) / 100,
                hovered_time
            )
        };

        left_bracket->set_scale(scale);
        right_bracket->set_scale(scale);

        const double thickness {
            std::clamp(body_screen_radius / 3, min_bracket_thickness, max_bracket_thickness)
        };
        left_bracket->set_line_thickness(thickness / left_bracket->get_scale().x);
        right_bracket->set_line_thickness(thickness / right_bracket->get_scale().x);
    }

    void SpaceDemo::handle_hovered_body(const double dt, const double time_lerp)
    {
        const auto previous_render_body { hovered_body };
        if (hovered_time >= hovered_poll_time)
        {
            hovered_body = get_closest_body_screenspace(mouse_pos, max_hovered_distance, 2.0, time_lerp);
        }

        if (hovered_body != previous_render_body)
        {
            hovered_time = 0.0;
        }
        else if (hovered_time <= 1.0)
        {
            hovered_time += dt / bracket_grow_time;
        }
        else
        {
            hovered_time = 1.0;
        }
    }

    Vec2d SpaceDemo::body_interpolated_pos(const std::shared_ptr<Body> body, const double time_lerp) const
    {
        const auto [ellipse, trail, previous_pos] { body_items.at(body) };
        return Vec2d::lerp(
            previous_pos,
            body->get_position(),
            time_lerp
        );
    }

    void SpaceDemo::set_view_pos(const Vec2d pos)
    {
        const Vec2d size { view_bounds.size() };
        view_bounds.min = pos - size * view_anchor;
        view_bounds.max = view_bounds.min + size;
    }


    void SpaceDemo::set_view_size(const double width)
    {
        const Vec2d anchor_pos { view_bounds.min + view_bounds.size() * view_anchor };
        const Vec2d resolution { get_resolution() };
        view_bounds.min = anchor_pos - Vec2d {
            width * view_anchor.x,
            width * resolution.y / resolution.x * view_anchor.y
        };
        view_bounds.max = view_bounds.min + Vec2d { width, (width * resolution.y / resolution.x) };
    }
}
