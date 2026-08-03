#pragma once

#include "viewport.h"
#include "gfx/core/transform.h"
#include "gfx/core/view/view-bounds.h"
#include "gfx/core/view/view.h"

namespace gfx
{
    template <typename VectorType>
    class Projection
    {
    public:

        virtual ~Projection() = default;

        virtual Transform<VectorType>::MatrixType get_matrix(double aspect_ratio) const = 0;
        virtual ViewBounds<VectorType> get_view_bounds(const View<VectorType>& view, const Viewport& viewport) const = 0;
    };
}
