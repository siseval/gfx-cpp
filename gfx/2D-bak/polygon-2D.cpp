#include "polygon-2D.h"

#include "rasterize.h"
#include "gfx/geometry/transform-2D.h"
#include "triangulate.h"


namespace gfx
{
    Primitive2D::RasterizeOutput Polygon2D::rasterize(const Matrix3x3d& transform) const
    {
        RasterizeOutput output;

        for (const auto& component : _components)
        {
            rasterize_component(component, transform, output.pixels);
        }

        return output;
    }

    Box2d Polygon2D::get_geometry_size() const
    {
        Box2d bounds { Vec2d::zero(), Vec2d::zero() };

        for (auto component : _components)
        {
            for (auto point : component.contour.vertices)
            {
                bounds.expand(point);
            }
            for (auto hole : component.holes)
            {
                for (auto point : hole.vertices)
                {
                    bounds.expand(point);
                }
            }
        }

        return bounds;
    }

    void Polygon2D::add_vertex(const Vec2d vertex, const int component)
    {
        if (component >= _components.size())
        {
            _components.resize(component + 1);
        }
        _components[component].contour.vertices.push_back(vertex);
        cache_clockwise(component);
        set_obb_dirty();
    }

    void Polygon2D::add_vertices(const std::vector<Vec2d>& new_vertices, const int component)
    {
        if (component >= _components.size())
        {
            _components.resize(component + 1);
        }
        std::vector<Vec2d>& points { _components[component].contour.vertices };
        points.insert(points.end(), new_vertices.begin(), new_vertices.end());
        cache_clockwise(component);
        set_obb_dirty();
    }

    void Polygon2D::set_vertex(const size_t index, const Vec2d vertex, const int component)
    {
        if (component >= _components.size())
        {
            _components.resize(component + 1);
        }
        std::vector<Vec2d>& points { _components[component].contour.vertices };
        if (index < points.size())
        {
            points[index] = vertex;
            cache_clockwise(component);
            set_obb_dirty();
        }
    }

    void Polygon2D::set_vertices(const std::vector<Vec2d>& new_vertices, const int component)
    {
        if (component >= _components.size())
        {
            _components.resize(component + 1);
        }
        _components[component].contour.vertices = new_vertices;
        cache_clockwise(component);
        set_obb_dirty();
    }

    void Polygon2D::clear_vertices(const int component)
    {
        if (component >= _components.size())
        {
            return;
        }
        _components[component].contour.vertices.clear();
        set_obb_dirty();
    }

    std::vector<Vec2d> Polygon2D::get_vertices(const int component) const
    {
        if (component >= _components.size())
        {
            return {};
        }
        return _components[component].contour.vertices;
    }

    void Polygon2D::add_hole_vertex(const Vec2d vertex, const int component, const int hole)
    {
        if (component >= _components.size())
        {
            _components.resize(component + 1);
        }
        _components[component].holes[hole].vertices.push_back(vertex);
        cache_clockwise_hole(component, hole);
        set_obb_dirty();
    }

    void Polygon2D::add_hole_vertices(const std::vector<Vec2d>& new_vertices, const int component, const int hole)
    {
        if (component >= _components.size())
        {
            _components.resize(component + 1);
        }
        if (hole >= _components[component].holes.size())
        {
            _components[component].holes.resize(hole + 1);
        }
        std::vector<Vec2d>& points { _components[component].holes[hole].vertices };
        points.insert(points.end(), new_vertices.begin(), new_vertices.end());
        cache_clockwise_hole(component, hole);
        set_obb_dirty();
    }

    void Polygon2D::set_hole_vertex(const size_t index, const Vec2d vertex, const int component, const int hole)
    {
        if (component >= _components.size())
        {
            _components.resize(component + 1);
        }
        if (hole >= _components[component].holes.size())
        {
            _components[component].holes.resize(hole + 1);
        }
        std::vector<Vec2d>& points { _components[component].holes[hole].vertices };
        if (component < points.size())
        {
            points[index] = vertex;
            cache_clockwise_hole(component, hole);
            set_obb_dirty();
        }
    }

    void Polygon2D::set_hole_vertices(const std::vector<Vec2d>& new_vertices, const int component, const int hole)
    {
        if (component >= _components.size())
        {
            _components.resize(component + 1);
        }
        if (hole >= _components[component].holes.size())
        {
            _components[component].holes.resize(hole + 1);
        }
        _components[component].holes[hole].vertices = new_vertices;
        cache_clockwise_hole(component, hole);
        set_obb_dirty();
    }

    void Polygon2D::clear_hole_vertices(const int component, const int hole)
    {
        if (component >= _components.size())
        {
            return;
        }
        if (hole >= _components[component].holes.size())
        {
            return;
        }
        _components[component].holes[hole].vertices.clear();
        set_obb_dirty();
    }

    std::vector<Vec2d> Polygon2D::get_hole_vertices(const int component, const int hole) const
    {
        if (component >= _components.size())
        {
            return {};
        }
        if (hole >= _components[component].holes.size())
        {
            return {};
        }
        return _components[component].holes[hole].vertices;
    }

    bool Polygon2D::cache_clockwise(const int component)
    {
        Polygon<double>::Contour& contour = _components[component].contour;
        double sum = 0.0;
        for (int i = 0; i < contour.vertices.size(); ++i)
        {
            const Vec2d p0 { contour.vertices[i] };
            const Vec2d p1 { contour.vertices[(i + 1) % contour.vertices.size()] };
            sum += (p1.x - p0.x) * (p1.y + p0.y);
        }
        contour.clockwise = sum < 0.0;
        return contour.clockwise;
    }

    bool Polygon2D::cache_clockwise_hole(const int component, const int hole)
    {
        Polygon<double>::Contour& contour = _components[component].holes[hole];
        double sum = 0.0;
        for (int i = 0; i < contour.vertices.size(); ++i)
        {
            const Vec2d p0 { contour.vertices[i] };
            const Vec2d p1 { contour.vertices[(i + 1) % contour.vertices.size()] };
            sum += (p1.x - p0.x) * (p1.y + p0.y);
        }
        contour.clockwise = sum < 0.0;
        return contour.clockwise;
    }

    void Polygon2D::rasterize_component(
        const Polygon<double>& component,
        const Matrix3x3d& transform,
        std::vector<Vec2i>& pixels
    )
    {
        std::vector<Polygon<double>::Contour> transformed_holes;
        for (const auto& hole : component.holes)
        {
            transformed_holes.push_back(
                Polygon<double>::Contour { Transform2D::transform_points(hole.vertices, transform), hole.clockwise }
            );
        }

        const Polygon<double> transformed_component {
            Transform2D::transform_points(component.contour.vertices, transform),
            component.contour.clockwise,
            transformed_holes
        };

        const std::vector triangles { Triangulate::triangulate_polygon(transformed_component) };

        for (auto triangle : triangles)
        {
            // Rasterize::rasterize_filled_triangle(triangle, pixels);
        }
    }
}
