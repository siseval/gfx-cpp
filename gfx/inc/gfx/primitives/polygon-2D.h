#pragma once

#include "gfx/core/primitive-2D.h"
#include "../geometry/types/polygon.h"
#include "gfx/math/box2.h"
#include "gfx/math/matrix.h"
#include "gfx/math/vec2.h"

namespace gfx
{
    class Polygon2D final : public Primitive2D
    {
    public:

        RasterizeOutput rasterize(const Matrix3x3d& transform) const override;

        Box2d get_geometry_size() const override;

        void add_vertex(Vec2d vertex, int component = 0);
        void add_vertices(const std::vector<Vec2d>& new_vertices, int component = 0);

        void set_vertex(size_t index, Vec2d vertex, int component = 0);
        void set_vertices(const std::vector<Vec2d>& new_vertices, int component = 0);

        void clear_vertices(int component = 0);

        std::vector<Vec2d> get_vertices(int component = 0) const;

        void add_hole_vertex(Vec2d vertex, int component = 0, int hole = 0);
        void add_hole_vertices(const std::vector<Vec2d>& new_vertices, int component = 0, int hole = 0);

        void set_hole_vertex(size_t index, Vec2d vertex, int component = 0, int hole = 0);
        void set_hole_vertices(const std::vector<Vec2d>& new_vertices, int component = 0, int hole = 0);

        void clear_hole_vertices(int component = 0, int hole = 0);

        std::vector<Vec2d> get_hole_vertices(int component = 0, int hole = 0) const;

    private:

        bool cache_clockwise(int component);
        bool cache_clockwise_hole(int component, int hole);

        static void rasterize_component(
            const Polygon<double>& component,
            const Matrix3x3d& transform,
            std::vector<Vec2i>& pixels
        );

        std::vector<Polygon<double>> _components;
        static constexpr int CORNER_SEGMENTS { 8 };
    };
}
