#pragma once

#include <stack>

#include "gfx/core/primitive.h"
#include "gfx/core/types/uuid.h"
#include "view/view-bounds.h"

namespace gfx
{
    template <typename VectorType>
    struct SceneNode
    {
        explicit SceneNode(std::shared_ptr<Primitive<VectorType>> item)
            : primitive(item) {}

        UUID get_id() const
        {
            if (primitive)
            {
                return primitive->get_id();
            }
            return UUID(0, 0);
        }

        std::shared_ptr<Primitive<VectorType>> primitive;
        Transform<VectorType> global_transform;

        uint64_t cached_transform_generation = -1;

        std::shared_ptr<SceneNode> parent = nullptr;
        std::vector<std::shared_ptr<SceneNode>> children;
    };


    template <typename VectorType>
    class SceneGraph
    {
    public:

        SceneGraph();

        std::shared_ptr<SceneNode<VectorType>> get_root() const;
        void set_root_transform(const Transform<VectorType>& transform) const;

        bool transforms_dirty() const;
        Transform<VectorType> get_global_transform(std::shared_ptr<Primitive<VectorType>> primitive) const;
        void update_global_transforms() const;

        std::vector<std::pair<std::shared_ptr<Primitive<VectorType>>, Transform<VectorType>>> get_global_transforms();

        void add_item(std::shared_ptr<Primitive<VectorType>> item, std::shared_ptr<Primitive<VectorType>> parent);
        void add_item(std::shared_ptr<Primitive<VectorType>> item);

        void remove_item(std::shared_ptr<Primitive<VectorType>> item);

        void clear();

        std::vector<std::pair<std::shared_ptr<Primitive<VectorType>>, Transform<VectorType>>>& get_draw_queue(
            const ViewBounds<VectorType>& view_bounds
        ) const;

        int num_items() const;
        bool contains_item(std::shared_ptr<Primitive<VectorType>> item) const;

    private:

        mutable std::vector<std::pair<std::shared_ptr<Primitive<VectorType>>, Transform<VectorType>>> _draw_queue;
        std::shared_ptr<SceneNode<VectorType>> _root;
        std::unordered_map<UUID, std::shared_ptr<SceneNode<VectorType>>> _nodes;
    };

    template <typename VectorType>
    SceneGraph<VectorType>::SceneGraph()
        : _root(std::make_shared<SceneNode<VectorType>>(nullptr))
        , _nodes(std::unordered_map<UUID, std::shared_ptr<SceneNode<VectorType>>>()) {}

    template <typename VectorType>
    std::shared_ptr<SceneNode<VectorType>> SceneGraph<VectorType>::get_root() const
    {
        return _root;
    }

    template <typename VectorType>
    void SceneGraph<VectorType>::set_root_transform(const Transform<VectorType>& transform) const
    {
        _root->global_transform = transform;
    }

    template <typename VectorType>
    bool SceneGraph<VectorType>::transforms_dirty() const
    {
        for (const auto& node : _nodes | std::views::values)
        {
            if (node->primitive == nullptr)
            {
                continue;
            }
            const int64_t current_generation { node->primitive->get_transform_generation() };
            if (current_generation != node->cached_transform_generation)
            {
                return true;
            }
        }
        return false;
    }

    template <typename VectorType>
    Transform<VectorType> SceneGraph<VectorType>::get_global_transform(
        const std::shared_ptr<Primitive<VectorType>> primitive
    ) const
    {
        const auto node { _nodes.contains(primitive->get_id()) ? _nodes.at(primitive->get_id()) : nullptr };
        if (node == nullptr)
        {
            return Transform<VectorType>();
        }
        if (node->primitive == nullptr)
        {
            return node->global_transform;
        }

        return node->global_transform;
    }

    template <typename VectorType>
    void SceneGraph<VectorType>::update_global_transforms() const
    {
        std::stack<std::pair<std::shared_ptr<SceneNode<VectorType>>, Transform<VectorType>>> stack;

        stack.push({ get_root(), get_root()->global_transform });

        while (!stack.empty())
        {
            auto [node, parent_transform] { stack.top() };
            stack.pop();

            if (node->primitive)
            {
                node->global_transform = parent_transform.combine(node->primitive->get_transform());
            }
            else
            {
                node->global_transform = parent_transform;
            }

            node->cached_transform_generation = node->primitive ? node->primitive->get_transform_generation() : 0;

            for (const auto& child : node->children)
            {
                stack.push({ child, node->global_transform });
            }
        }
    }

    template <typename VectorType>
    std::vector<std::pair<std::shared_ptr<Primitive<VectorType>>, Transform<VectorType>>> SceneGraph<
        VectorType>::get_global_transforms()
    {
        if (transforms_dirty())
        {
            update_global_transforms();
        }

        std::vector<std::pair<std::shared_ptr<Primitive<VectorType>>, Transform<VectorType>>> transforms;
        for (const auto& node : _nodes | std::views::values)
        {
            if (node->primitive != nullptr)
            {
                transforms.push_back({ node->primitive, get_global_transform(node->primitive) });
            }
        }

        return transforms;
    }

    template <typename VectorType>
    void SceneGraph<VectorType>::add_item(
        const std::shared_ptr<Primitive<VectorType>> item,
        const std::shared_ptr<Primitive<VectorType>> parent
    )
    {
        const auto new_node { std::make_shared<SceneNode<VectorType>>(item) };
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

    template <typename VectorType>
    void SceneGraph<VectorType>::add_item(const std::shared_ptr<Primitive<VectorType>> item)
    {
        add_item(item, nullptr);
    }

    template <typename VectorType>
    void SceneGraph<VectorType>::remove_item(const std::shared_ptr<Primitive<VectorType>> item)
    {
        if (!_nodes.contains(item->get_id()))
        {
            return;
        }

        std::erase_if(
            _nodes[item->get_id()]->parent->children,
            [item](const std::shared_ptr<SceneNode<VectorType>> node) {
                return node->get_id() == item->get_id();
            }
        );

        std::stack<std::shared_ptr<SceneNode<VectorType>>> stack;
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

    template <typename VectorType>
    void SceneGraph<VectorType>::clear()
    {
        _root->children.clear();
        _nodes.clear();
    }

    template <typename VectorType>
    std::vector<std::pair<std::shared_ptr<Primitive<VectorType>>, Transform<VectorType>>>& SceneGraph<
        VectorType>::get_draw_queue(
        const ViewBounds<VectorType>& view_bounds
    ) const
    {
        _draw_queue.clear();

        if (transforms_dirty())
        {
            update_global_transforms();
        }

        for (const auto& node : _nodes | std::views::values)
        {
            auto& primitive { node->primitive };
            if (primitive != nullptr)
            {
                BoundingBall<VectorType> transformed_sphere {
                    primitive->get_bounding_sphere().transformed(
                        primitive->get_position(),
                        primitive->get_scale(),
                        primitive->get_anchor() * primitive->get_aabb().size()
                    )
                };

                if (!view_bounds.ball_in_view(transformed_sphere))
                {
                    continue;
                }

                _draw_queue.push_back({ primitive, get_global_transform(primitive) });
            }
        }

        return _draw_queue;
    }

    template <typename VectorType>
    int SceneGraph<VectorType>::num_items() const
    {
        return _nodes.size();
    }

    template <typename VectorType>
    bool SceneGraph<VectorType>::contains_item(const std::shared_ptr<Primitive<VectorType>> item) const
    {
        return _nodes.contains(item->get_id());
    }
}
