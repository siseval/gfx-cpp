#pragma once

#include "gfx/core/primitive-3D.h"

namespace gfx
{
    class Cone3D final : public Primitive3D
    {
    public:

        Cone3D() = default;

        const TriangleMesh& get_mesh() const override;

        void set_radius(double new_radius);
        void set_height(double new_height);
        void set_segments(int new_segments);

        double get_radius() const;
        double get_height() const;
        int get_segments() const;

    private:

        double _radius;
        double _height;
        int _segments;
    };
}
