#ifndef GFX_PRIMITIVE_STORE_2D_H
#define GFX_PRIMITIVE_STORE_2D_H

#include <utility>
#include <gfx/core/primitive-2D.h>
#include <gfx/utils/uuid.h>

namespace gfx::core
{


struct SceneNode2D
{
    SceneNode2D(std::shared_ptr<Primitive2D> item) : primitive(item) {}

    utils::UUID get_id() const
    {
        if (primitive)
        {
            return primitive->get_id();
        }
        return utils::UUID(0, 0);
    }
    std::shared_ptr<Primitive2D> primitive;
    gfx::math::Matrix3x3d global_transform = gfx::math::Matrix3x3d::identity();
    uint64_t cached_transform_version = 0;
    std::shared_ptr<SceneNode2D> parent = nullptr;
    std::vector<std::shared_ptr<SceneNode2D>> children;
};

class SceneGraph2D
{

public:

    SceneGraph2D() : 
        root(std::make_shared<SceneNode2D>(nullptr)),
        nodes(std::unordered_map<gfx::utils::UUID, std::shared_ptr<SceneNode2D>>()) {}

    inline std::shared_ptr<SceneNode2D> get_root() const { return root; }
    inline void set_root_transform(const gfx::math::Matrix3x3d& transform) { root->global_transform = transform; }

    bool transforms_dirty() const;
    gfx::math::Matrix3x3d get_global_transform(const std::shared_ptr<Primitive2D> primitive);
    void update_global_transforms();

    std::vector<std::pair<std::shared_ptr<Primitive2D>, gfx::math::Matrix3x3d>> get_global_transforms();

    void add_item(const std::shared_ptr<Primitive2D> item, const std::shared_ptr<Primitive2D> parent);

    inline void add_item(const std::shared_ptr<Primitive2D> item)
    {
        add_item(item, nullptr);
    }

    void remove_item(const std::shared_ptr<Primitive2D> item);

    inline void clear()
    {
        root->children.clear();
        nodes.clear();
    }

    std::vector<std::pair<std::shared_ptr<Primitive2D>, gfx::math::Matrix3x3d>> get_draw_queue();

    inline int num_items() const { return nodes.size(); }
    inline bool contains_item(const std::shared_ptr<Primitive2D> item) const { return nodes.contains(item->get_id()); }

    inline int get_transform_recalculation_count() const { return transform_recalculation_count; }

    double longest_recalc_time = 0;
    double previous_recalc_time = 0;

private:
    std::shared_ptr<SceneNode2D> root;
    std::unordered_map<gfx::utils::UUID, std::shared_ptr<SceneNode2D>> nodes;

    int transform_recalculation_count = 0;

};

}

#endif // GFX_PRIMITIVE_STORE_2D_H
