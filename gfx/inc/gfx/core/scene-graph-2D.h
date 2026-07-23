#pragma once

#include "gfx/core/primitive-2D.h"
#include "gfx/core/types/uuid.h"

namespace gfx
{
    struct SceneNode2D
    {
        explicit SceneNode2D(std::shared_ptr<Primitive2D> item)
            : primitive(item) {}

        UUID get_id() const
        {
            if (primitive)
            {
                return primitive->get_id();
            }
            return UUID(0, 0);
        }

        std::shared_ptr<Primitive2D> primitive;
        Matrix3x3d global_transform = Matrix3x3d::identity();
        uint64_t cached_transform_version = -1;
        std::shared_ptr<SceneNode2D> parent = nullptr;
        std::vector<std::shared_ptr<SceneNode2D>> children;
    };

    class SceneGraph2D
    {
    public:

        SceneGraph2D();

        std::shared_ptr<SceneNode2D> get_root() const;
        void set_root_transform(const Matrix3x3d& transform) const;

        bool transforms_dirty() const;
        Matrix3x3d get_global_transform(std::shared_ptr<Primitive2D> primitive) const;
        void update_global_transforms() const;

        std::vector<std::pair<std::shared_ptr<Primitive2D>, Matrix3x3d>> get_global_transforms();

        void add_item(std::shared_ptr<Primitive2D> item, std::shared_ptr<Primitive2D> parent);
        void add_item(std::shared_ptr<Primitive2D> item);

        void remove_item(std::shared_ptr<Primitive2D> item);

        void clear();

        std::vector<std::pair<std::shared_ptr<Primitive2D>, Matrix3x3d>> get_draw_queue();

        int num_items() const;
        bool contains_item(std::shared_ptr<Primitive2D> item) const;

    private:

        std::shared_ptr<SceneNode2D> _root;
        std::unordered_map<UUID, std::shared_ptr<SceneNode2D>> _nodes;
    };
}
