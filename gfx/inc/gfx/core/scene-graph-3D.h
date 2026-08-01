#pragma once

#include "gfx/core/primitive-3D.h"
#include "gfx/core/types/frustum.h"
#include "gfx/core/types/uuid.h"

namespace gfx
{
    struct SceneNode3D
    {
        explicit SceneNode3D(std::shared_ptr<Primitive3D> item)
            : primitive(item) {}

        UUID get_id() const
        {
            if (primitive)
            {
                return primitive->get_id();
            }
            return UUID(0, 0);
        }

        std::shared_ptr<Primitive3D> primitive;
        Matrix4x4d global_transform = Matrix4x4d::identity();
        uint64_t cached_transform_version = -1;
        std::shared_ptr<SceneNode3D> parent = nullptr;
        std::vector<std::shared_ptr<SceneNode3D>> children;
    };


    class SceneGraph3D
    {
    public:

        SceneGraph3D();

        std::shared_ptr<SceneNode3D> get_root() const;
        void set_root_transform(const Matrix4x4d& transform) const;

        bool transforms_dirty() const;
        Matrix4x4d get_global_transform(std::shared_ptr<Primitive3D> primitive) const;
        void update_global_transforms() const;

        std::vector<std::pair<std::shared_ptr<Primitive3D>, Matrix4x4d>> get_global_transforms();

        void add_item(std::shared_ptr<Primitive3D> item, std::shared_ptr<Primitive3D> parent);
        void add_item(std::shared_ptr<Primitive3D> item);

        void remove_item(std::shared_ptr<Primitive3D> item);

        void clear();

        std::vector<std::pair<std::shared_ptr<Primitive3D>, Matrix4x4d>>& get_draw_queue(const Frustum& frustum) const;

        int num_items() const;
        bool contains_item(std::shared_ptr<Primitive3D> item) const;

    private:

        static bool sphere_in_frustum(const BoundingBall& sphere, const Frustum& frustum);

        mutable std::vector<std::pair<std::shared_ptr<Primitive3D>, Matrix4x4d>> _draw_queue;
        std::shared_ptr<SceneNode3D> _root;
        std::unordered_map<UUID, std::shared_ptr<SceneNode3D>> _nodes;
    };
}
