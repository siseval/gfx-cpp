#pragma once

#include "gfx/core/primitive-3D.h"

namespace gfx
{
    class Sphere3D final : public Primitive3D
    {
    public:

        const TriangleMesh& get_mesh() const override;

        void set_radius(double r);
        double get_radius() const;

        void set_num_lat_segments(int segments);
        void set_num_lon_segments(int segments);
        void set_num_segments(int lat, int lon);
        void set_num_segments(int segments);

    private:

        double _radius { 0 };
        int _lat_segments { 12 };
        int _lon_segments { 12 };
    };
}
