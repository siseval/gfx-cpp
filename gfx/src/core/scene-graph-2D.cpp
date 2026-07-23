#include "gfx/core/scene-graph-2D.h"

#include <algorithm>
#include <ranges>
#include <stack>
#include <utility>
#include <vector>

#include "gfx/math/matrix.h"

namespace gfx
{
    SceneGraph2D::SceneGraph2D()
        : _root(std::make_shared<SceneNode2D>(nullptr))
        , _nodes(std::unordered_map<UUID, std::shared_ptr<SceneNode2D>>()) {}

    std::shared_ptr<SceneNode2D> SceneGraph2D::get_root() const
    {
        return _root;
    }

    void SceneGraph2D::set_root_transform(const Matrix3x3d& transform) const
    {
        _root->global_transform = transform;
    }

    bool SceneGraph2D::transforms_dirty() const
    {
        for (const auto& node : _nodes | std::views::values)
        {
            if (node->primitive == nullptr)
            {
                continue;
            }
            const int64_t current_version { node->primitive->get_transform_version() };
            if (current_version != node->cached_transform_version)
            {
                return true;
            }
        }
        return false;
    }

    Matrix3x3d SceneGraph2D::get_global_transform(const std::shared_ptr<Primitive2D> primitive) const
    {
        const auto node { _nodes.contains(primitive->get_id()) ? _nodes.at(primitive->get_id()) : nullptr };
        if (node == nullptr)
        {
            return Matrix3x3d::identity();
        }
        if (node->primitive == nullptr)
        {
            return node->global_transform;
        }

        return node->global_transform;
    }

    void SceneGraph2D::update_global_transforms() const
    {
        std::stack<std::pair<std::shared_ptr<SceneNode2D>, Matrix3x3d>> stack;

        stack.push({ get_root(), get_root()->global_transform });

        while (!stack.empty())
        {
            auto [node, parent_transform] { stack.top() };
            stack.pop();

            if (node->primitive)
            {
                node->global_transform = parent_transform * node->primitive->get_transform();
            }
            else
            {
                node->global_transform = parent_transform;
            }

            node->cached_transform_version = node->primitive ? node->primitive->get_transform_version() : 0;

            for (const auto& child : node->children)
            {
                stack.push({ child, node->global_transform });
            }
        }
    }

    std::vector<std::pair<std::shared_ptr<Primitive2D>, Matrix3x3d>> SceneGraph2D::get_global_transforms()
    {
        if (transforms_dirty())
        {
            update_global_transforms();
        }

        std::vector<std::pair<std::shared_ptr<Primitive2D>, Matrix3x3d>> transforms;
        for (const auto& node : _nodes | std::views::values)
        {
            if (node->primitive != nullptr)
            {
                transforms.push_back({ node->primitive, get_global_transform(node->primitive) });
            }
        }
        return transforms;
    }

    void SceneGraph2D::add_item(const std::shared_ptr<Primitive2D> item, const std::shared_ptr<Primitive2D> parent)
    {
        const auto new_node { std::make_shared<SceneNode2D>(item) };
        if (_nodes.contains(new_node->get_id()))
        {
            return;
        }
        _nodes[new_node->get_id()] = new_node;

        if (parent != nullptr && _nodes.contains(parent->get_id()))
        {
            const auto parent_node { _nodes[parent->get_id()] };
            new_node->parent = parent_node;
            parent_node->children.push_back(new_node);
            return;
        }
        new_node->parent = _root;
        _root->children.push_back(new_node);
    }

    void SceneGraph2D::add_item(const std::shared_ptr<Primitive2D> item)
    {
        add_item(item, nullptr);
    }

    void SceneGraph2D::remove_item(const std::shared_ptr<Primitive2D> item)
    {
        if (!_nodes.contains(item->get_id()))
        {
            return;
        }

        std::erase_if(
            _nodes[item->get_id()]->parent->children,
            [item](const std::shared_ptr<SceneNode2D>& node) {
                return node->get_id() == item->get_id();
            }
        );

        std::stack<std::shared_ptr<SceneNode2D>> stack;
        stack.push(_nodes[item->get_id()]);
        while (!stack.empty())
        {
            const auto node { stack.top() };
            stack.pop();

            for (const auto& child : node->children)
            {
                stack.push(child);
            }

            _nodes.erase(node->get_id());
        }
    }

    void SceneGraph2D::clear()
    {
        _root->children.clear();
        _nodes.clear();
    }

    std::vector<std::pair<std::shared_ptr<Primitive2D>, Matrix3x3d>> SceneGraph2D::get_draw_queue()
    {
        if (transforms_dirty())
        {
            update_global_transforms();
        }

        std::vector<std::pair<std::shared_ptr<Primitive2D>, Matrix3x3d>> draw_queue;
        for (const auto& node : _nodes | std::views::values)
        {
            if (node->primitive != nullptr)
            {
                draw_queue.emplace_back(node->primitive, get_global_transform(node->primitive));
            }
        }
        std::ranges::sort(
            draw_queue,
            [](const auto& a, const auto& b) {
                return a.first->get_depth() > b.first->get_depth();
            }
        );
        return draw_queue;
    }

    int SceneGraph2D::num_items() const
    {
        return _nodes.size();
    }

    bool SceneGraph2D::contains_item(const std::shared_ptr<Primitive2D> item) const
    {
        return _nodes.contains(item->get_id());
    }
}
