#ifndef TRIANGULATE_H
#define TRIANGULATE_H

#include <vector>
#include <gfx/math/vec2.h>
#include <gfx/geometry/triangle.h>
#include <gfx/geometry/types/polygon.h>

namespace gfx::geometry
{

std::vector<Triangle> triangulate_polygon(const gfx::geometry::types::Component &component);

}

#endif // TRIANGULATE_H
