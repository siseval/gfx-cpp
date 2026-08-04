#pragma once

#include "gfx/core/primitive.h"
#include "gfx/math/vec3.h"

namespace gfx
{
    class Cone3D final : public Primitive<Vec3d>
    {
    public:

        Cone3D() = default;
        
        void set_radius(double new_radius);
        void set_height(double new_height);
        void set_segments(int new_segments);

        double get_radius() const;
        double get_height() const;
        int get_segments() const;

    protected:
        
        void generate_mesh() const override;
        
    private:

        double _radius;
        double _height;
        int _segments;
    };
}
