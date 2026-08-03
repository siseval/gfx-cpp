#pragma once

#include "gfx/core/view/projection.h"
#include "gfx/math/matrix.h"

namespace gfx
{
    class PerspectiveProjection : Projection<Vec3d>
    {
    public:

        PerspectiveProjection() = default;
        PerspectiveProjection(double z_near, double z_far, double fov);

        Matrix4x4d get_matrix(double aspect_ratio) const override;

        void set_fov(double fov);
        void set_fov_degrees(double fov);

        void set_z_near(double z_near);
        void set_z_far(double z_far);

        double get_fov() const;
        double get_z_near() const;
        double get_z_far() const;

        ViewBounds<Vec3d> get_view_bounds(const View<Vec3d>& view, const Viewport& viewport) const override;

    private:

        double _z_near;
        double _z_far;
        double _fov;
    };
}
