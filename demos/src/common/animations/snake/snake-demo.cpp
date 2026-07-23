#include "common/animations/snake/snake-demo.h"
#include "common/core/demo-utils.h"

namespace demos
{
    using namespace gfx;

    void SnakeDemo::init()
    {
        const Vec2d resolution { get_resolution() };
        const Vec2d center { resolution / 2 };

        speed = resolution.x * 0.3;
        target_speed = speed * 1.1;
        explode_speed = speed * 3.0;

        dead = true;

        segment_length = resolution.x * 0.02;
        segment_width = segment_length * 0.5;
        segment_spacing = segment_length * 0.5;
        head_dimensions = { segment_length * 2.5, segment_width * 2.5 };
        target_bounds_margin = resolution * 0.1;

        render2D->clear_items();
        segments.clear();
        segment_palette.clear();

        for (int i = 0; i < num_colors; ++i)
        {
            const double progress { static_cast<double>(i) / num_colors };

            Color4 color { Color4::lerp({ 0.0, 1.0, 0.0 }, { 0.0, 1.0, 1.0 }, progress) };
            segment_palette.push_back(color);
        }

        const std::vector<Vec2d> head_points {
            { 0, 0 },
            { head_dimensions.x / 2, head_dimensions.y / 2 },
            { head_dimensions.x, 0 },
            { head_dimensions.x / 2, -head_dimensions.y / 2 }
        };

        head = render2D->create_polyline(center, head_points, Color4(0.0, 1.0, 0.0, 1.0), 2.0);
        head->set_close(true);
        head->set_fill(true);
        head->set_rounded_corners(true);
        head->set_scale(scale);
        head->set_depth(0);
        render2D->add_item(head);

        const std::vector<Vec2d> tongue_points {
            { 0, 0 },
            { head_dimensions.x * 0.2, head_dimensions.y * 0.05 },
            { head_dimensions.x * 0.4, head_dimensions.y * 0.2 },
            { head_dimensions.x * 0.3, 0 },
            { head_dimensions.x * 0.4, -head_dimensions.y * 0.2 },
            { head_dimensions.x * 0.2, -head_dimensions.y * 0.05 },
        };
        tongue = render2D->create_polyline({ head_dimensions.x, 0 }, tongue_points, Color4(1.0, 0.0, 0.0, 1.0), 0.5);
        tongue->set_close(true);
        tongue->set_fill(true);
        tongue->set_rounded_corners(true);
        tongue->set_depth(1);
        render2D->add_item(tongue, head);

        for (int i = 0; i < num_segments; ++i)
        {
            add_segment();
        }

        target_marker = render2D->create_circle(head_target, 1.0, Color4(1.0, 0.0, 0.0, 1.0), 1.0);
        target_marker->set_filled(true);
        render2D->add_item(target_marker);

        spawn();
    }

    void SnakeDemo::render_frame(const double dt)
    {
        const double t0 { time_us() };
        const double time_ms { t0 / 1000.0 };

        tongue->set_scale(std::sin(std::sin(time_ms / 1000) * 8));
        if (dead)
        {
            do_dead(dt);
        }
        else
        {
            move_target(dt);
            move_head(dt);
            move_segments();
        }

        render2D->clear_frame();
        render2D->draw_frame();
        render2D->present_frame();
    }

    void SnakeDemo::end()
    {
        render2D->clear_items();
        dead = true;
    }

    void SnakeDemo::handle_char(const int input)
    {
        switch (input)
        {
        case 'k':
            add_segment();
            num_segments += 1;
            break;

        case 'j':
            if (num_segments > 1)
            {
                remove_segment();
                num_segments -= 1;
            }
            break;
        case 'l':
            update_scale(scale + 0.1);
            break;
        case 'h':
            if (scale > 0.2)
            {
                update_scale(scale - 0.1);
            }
            break;

        case 'w':
            target_direction = { 0, -1 };
            break;
        case 's':
            target_direction = { 0, 1 };
            break;
        case 'a':
            target_direction = { -1, 0 };
            break;
        case 'd':
            target_direction = { 1, 0 };
            break;

        case 'm':
            if (dead)
            {
                spawn();
                break;
            }
            die();
            break;

        case 'c':
            control = !control;
            break;

        case 'f':
            add_food();
            break;
        case 'r':
            remove_food();
            break;

        case 'v':
            target_visible = !target_visible;
            break;

        default:
            break;
        }
    }

    void SnakeDemo::move_target(const double dt)
    {
        head_target += target_direction * target_speed * dt;

        if (!control)
        {
            target_direction += Vec2d {
                random_double(-target_turn_speed, target_turn_speed),
                random_double(-target_turn_speed, target_turn_speed)
            } * dt;
        }
        target_direction = target_direction.normalize();

        if (head_target.x < target_bounds_margin.x ||
            head_target.x > get_resolution().x - target_bounds_margin.x)
        {
            target_direction.x = -target_direction.x;
        }

        if (head_target.y < target_bounds_margin.y ||
            head_target.y > get_resolution().y - target_bounds_margin.y)
        {
            target_direction.y = -target_direction.y;
        }

        target_marker->set_position(head_target);
        target_marker->set_visible(target_visible);
    }

    void SnakeDemo::move_head(const double dt)
    {
        const Vec2d goal { food.empty() ? head_target : closest_food() };

        Vec2d to_target { (goal - head->get_position()).normalize() };

        head->set_rotation(Vec2d::from_angle(head->get_rotation()).rotate_towards(to_target, turn_speed * dt).angle());
        head->set_position(head->get_position() + Vec2d::from_angle(head->get_rotation(), speed * dt));

        for (int i = 0; i < food.size(); ++i)
        {
            if (Vec2d::distance(head->get_position(), food[i]->get_position()) < segment_length * 0.8)
            {
                render2D->remove_item(food[i]);
                food.erase(food.begin() + i);
                update_scale(scale + 0.02);
                add_segment();
                num_segments += 1;
            }
        }
    }

    void SnakeDemo::move_segments() const
    {
        segments[0]->set_position(head->get_position() - Vec2d::from_angle(head->get_rotation(), segment_spacing));
        segments[0]->set_rotation(head->get_rotation());
        for (int i = 1; i < num_segments; ++i)
        {
            Vec2d direction { segments[i - 1]->get_position() - segments[i]->get_position() };
            segments[i]->set_rotation(direction.angle());
            segments[i]->set_position(
                segments[i - 1]->get_position() - Vec2d::from_angle(
                    segments[i]->get_rotation(),
                    segment_spacing * scale
                )
            );
        }
    }

    void SnakeDemo::spawn()
    {
        const Vec2d resolution { get_resolution() };
        const Vec2d center { resolution / 2 };

        head_target = center;
        head->set_position(center);
        head->set_visible(true);
        for (int i = 0; i < segments.size(); ++i)
        {
            segments[i]->set_position(center);
            segments[i]->set_visible(true);
        }
        update_segments();
        update_scale(scale);

        dead = false;
    }

    void SnakeDemo::die()
    {
        dead_time = 0.0;
        head->set_rotation_degrees(std::rand() % 360);
        for (int i = 0; i < segments.size(); ++i)
        {
            segments[i]->set_rotation_degrees(std::rand() % 360);
        }
        dead = true;
    }

    void SnakeDemo::do_dead(const double dt)
    {
        head->set_position(head->get_position() + Vec2d::from_angle(head->get_rotation(), explode_speed * dt));
        head->set_scale(Vec2d::lerp(Vec2d(scale), Vec2d(0.001), dead_time * 3));
        if (head->get_scale().x <= 0.01)
        {
            head->set_visible(false);
        }
        for (int i = 0; i < segments.size(); ++i)
        {
            const auto segment { segments[i] };
            Vec2d dir { Vec2d::from_angle(segment->get_rotation()) };

            segment->set_position(segment->get_position() + dir * explode_speed * dt);
            segment->set_scale(Vec2d::lerp(Vec2d(scale), Vec2d(0.001), dead_time * 3));
            if (segment->get_scale().x <= 0.01)
            {
                segment->set_visible(false);
            }
        }
        dead_time += dt;
    }

    void SnakeDemo::remove_food()
    {
        if (food.empty())
        {
            return;
        }

        render2D->remove_item(food.back());
        food.pop_back();
    }

    void SnakeDemo::add_food()
    {
        const Vec2i pos {
            random_int(target_bounds_margin.x, get_resolution().x - target_bounds_margin.x),
            random_int(target_bounds_margin.y, get_resolution().y - target_bounds_margin.y)
        };
        food.push_back(render2D->create_circle(static_cast<Vec2d>(pos), food_radius, Color4(1.0, 0.7, 0.0, 1.0), 1.0));
        food.back()->set_filled(true);
        render2D->add_item(food.back());
    }

    Vec2d SnakeDemo::closest_food() const
    {
        double closest_dist { Vec2d::distance(head->get_position(), food[0]->get_position()) };
        Vec2d closest_pos { food[0]->get_position() };
        for (int i = 1; i < food.size(); ++i)
        {
            const double dist { Vec2d::distance(head->get_position(), food[i]->get_position()) };
            if (dist < closest_dist)
            {
                closest_dist = dist;
                closest_pos = food[i]->get_position();
            }
        }
        return closest_pos;
    }

    void SnakeDemo::update_segments()
    {
        for (int i = 0; i < segments.size(); ++i)
        {
            const double progress { static_cast<double>(i) / segments.size() };
            const int color_index { static_cast<int>(progress * (segment_palette.size() - 1)) };

            const Vec2d radius { Vec2d::lerp({ segment_length, segment_width }, { 0.1, 0.1 }, progress) };
            segments[i]->set_radius(radius);
            segments[i]->set_color(segment_palette[color_index]);
            segments[i]->set_depth(i + 1);
        }
    }

    void SnakeDemo::add_segment()
    {
        const auto segment { render2D->create_ellipse({ 0, 0 }, { 0, 0 }, Color4(0, 0, 0)) };
        segment->set_filled(true);
        segment->set_anchor({ 0.5, 0.5 });
        if (!segments.empty())
        {
            const Vec2d pos {
                dead ? static_cast<Vec2d>(render2D->get_resolution() / 2.0) : segments.back()->get_position()
            };
            segment->set_position(pos);
            segment->set_rotation(segments.back()->get_rotation());
        }
        segment->set_scale(scale);

        segments.push_back(segment);
        render2D->add_item(segment);

        update_segments();
    }

    void SnakeDemo::remove_segment()
    {
        if (segments.empty())
        {
            return;
        }

        render2D->remove_item(segments.back());
        segments.pop_back();

        update_segments();
    }

    void SnakeDemo::update_scale(const double s)
    {
        for (const auto& segment : segments)
        {
            segment->set_scale(s);
        }
        head->set_scale(s);
        scale = s;
    }
}
