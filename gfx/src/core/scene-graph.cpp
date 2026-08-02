#include <stack>
#include <utility>

#include "gfx/core/scene-graph.h"

#include <ranges>

#include "gfx/math/matrix.h"

namespace gfx
{
    SceneGraph::SceneGraph3D()
        : _root(std::make_shared<SceneNode>(nullptr))
        , _nodes(std::unordered_map<UUID, std::shared_ptr<SceneNode>>()) {}

    std::shared_ptr<SceneNode> SceneGraph::get_root() const
    {
        return _root;
    }

    void SceneGraph::set_root_transform(const Matrix4x4d& transform) const
    {
        _root->global_transform = transform;
    }

    bool SceneGraph::transforms_dirty() const
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

    Matrix4x4d SceneGraph::get_global_transform(const std::shared_ptr<Primitive> primitive) const
    {
        const auto node { _nodes.contains(primitive->get_id()) ? _nodes.at(primitive->get_id()) : nullptr };
        if (node == nullptr)
        {
            return Matrix4x4d::identity();
        }
        if (node->primitive == nullptr)
        {
            return node->global_transform;
        }

        return node->global_transform;
    }

    void SceneGraph::update_global_transforms() const
    {
        std::stack<std::pair<std::shared_ptr<SceneNode>, Matrix4x4d>> stack;

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

    std::vector<std::pair<std::shared_ptr<Primitive>, Matrix4x4d>> SceneGraph::get_global_transforms()
    {
        if (transforms_dirty())
        {
            update_global_transforms();
        }

        std::vector<std::pair<std::shared_ptr<Primitive>, Matrix4x4d>> transforms;
        for (const auto& node : _nodes | std::views::values)
        {
            if (node->primitive != nullptr)
            {
                transforms.push_back({ node->primitive, get_global_transform(node->primitive) });
            }
        }

        return transforms;
    }

    void SceneGraph::add_item(const std::shared_ptr<Primitive> item, const std::shared_ptr<Primitive> parent)
    {
        const auto new_node { std::make_shared<SceneNode>(item) };
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

    void SceneGraph::add_item(const std::shared_ptr<Primitive> item)
    {
        add_item(item, nullptr);
    }

    void SceneGraph::remove_item(const std::shared_ptr<Primitive> item)
    {
        if (!_nodes.contains(item->get_id()))
        {
            return;
        }

        std::erase_if(
            _nodes[item->get_id()]->parent->children,
            [item](const std::shared_ptr<SceneNode> node) {
                return node->get_id() == item->get_id();
            }
        );

        std::stack<std::shared_ptr<SceneNode>> stack;
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

    void SceneGraph::clear()
    {
        _root->children.clear();
        _nodes.clear();
    }

    std::vector<std::pair<std::shared_ptr<Primitive>, Matrix4x4d>>& SceneGraph::get_draw_queue(
        const Frustum& frustum
    ) const
    {
        _draw_queue.clear();

        if (transforms_dirty())
        {
            update_global_transforms();
        }

        for (const auto& node : _nodes | std::views::values)
        {
            if (node->primitive != nullptr)
            {
                BoundingBall transformed_sphere {
                    node->primitive->get_bounding_sphere().transformed(
                        node->primitive->get_position(),
                        node->primitive->get_scale()
                    )
                };

                if (!sphere_in_frustum(transformed_sphere, frustum))
                {
                    continue;
                }

                _draw_queue.push_back({ node->primitive, get_global_transform(node->primitive) });
            }
        }

        return _draw_queue;
    }

    int SceneGraph::num_items() const
    {
        return _nodes.size();
    }

    bool SceneGraph::contains_item(const std::shared_ptr<Primitive> item) const
    {
        return _nodes.contains(item->get_id());
    }

    bool SceneGraph::sphere_in_frustum(const BoundingBall& sphere, const Frustum& frustum)
    {
        return frustum.sphere_in_frustum(sphere.center, sphere.radius);
    }
}
