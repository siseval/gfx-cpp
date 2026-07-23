#include "gfx/geometry/rasterize.h"

#include "gfx/geometry/types/triangle.h"


namespace gfx
{
    void Rasterize::rasterize_filled_triangle(
        const Triangle<double>& triangle,
        std::vector<Vec2i>& pixels,
        const Vec2d clip_bounds
    )
    {
        const Box2d bounds {
            static_cast<Vec2d>(Vec2d {
                std::min({ triangle.v0.x, triangle.v1.x, triangle.v2.x }),
                std::min({ triangle.v0.y, triangle.v1.y, triangle.v2.y })
            }.round_to_int()),
            static_cast<Vec2d>(Vec2d {
                std::max({ triangle.v0.x, triangle.v1.x, triangle.v2.x }),
                std::max({ triangle.v0.y, triangle.v1.y, triangle.v2.y })
            }.round_to_int()),
        };

        auto is_top_left = [](const Vec2d& a, const Vec2d& b) {
            return a.y < b.y || (a.y == b.y && a.x > b.x);
        };

        const double a { triangle.v1.y - triangle.v2.y };
        const double b { triangle.v2.x - triangle.v1.x };
        const double c { triangle.v1.x * triangle.v2.y - triangle.v1.y * triangle.v2.x };

        const double d { triangle.v2.y - triangle.v0.y };
        const double e { triangle.v0.x - triangle.v2.x };
        const double f { triangle.v2.x * triangle.v0.y - triangle.v2.y * triangle.v0.x };

        const double g { triangle.v0.y - triangle.v1.y };
        const double h { triangle.v1.x - triangle.v0.x };
        const double i { triangle.v0.x * triangle.v1.y - triangle.v0.y * triangle.v1.x };

        const bool tl0 = is_top_left(triangle.v1, triangle.v2);
        const bool tl1 = is_top_left(triangle.v2, triangle.v0);
        const bool tl2 = is_top_left(triangle.v0, triangle.v1);

        const double area { a * triangle.v0.x + b * triangle.v0.y + c };
        const double inv_area { 1.0f / area };

        if (area == 0.0)
        {
            return;
        }

        Vec3d w_row {
            a * (bounds.min.x + 0.5) + b * (bounds.min.y + 0.5) + c,
            d * (bounds.min.x + 0.5) + e * (bounds.min.y + 0.5) + f,
            g * (bounds.min.x + 0.5) + h * (bounds.min.y + 0.5) + i
        };

        for (int y = std::floor(bounds.min.y); y < std::ceil(bounds.max.y); ++y)
        {
            Vec3d w { w_row };

            for (int x = std::floor(bounds.min.x); x < std::ceil(bounds.max.x); ++x)
            {
                if ((w.x > 0 || (w.x == 0 && tl0)) && (w.y > 0 || (w.y == 0 && tl1)) && (w.z > 0 || (w.z == 0 && tl2)))
                {
                    pixels.push_back(Vec2i { x, y });
                }

                w.x += a;
                w.y += d;
                w.z += g;
            }

            w_row.x += b;
            w_row.y += e;
            w_row.z += h;
        }
    }

    // void Rasterize::rasterize_filled_triangle(
    //     const BarycentricTriangle& triangle,
    //     std::vector<Vec2i>& pixels,
    //     const Vec2d clip_bounds
    // )
    // {
    //     Box2d bounds {
    //         static_cast<Vec2d>(Vec2d {
    //             std::min({ triangle._v0.x, triangle._v1.x, triangle._v2.x }),
    //             std::min({ triangle._v0.y, triangle._v1.y, triangle._v2.y })
    //         }.round_to_int()),
    //         static_cast<Vec2d>(Vec2d {
    //             std::max({ triangle._v0.x, triangle._v1.x, triangle._v2.x }),
    //             std::max({ triangle._v0.y, triangle._v1.y, triangle._v2.y })
    //         }.round_to_int())
    //     };
    //
    //     if (clip_bounds.x > 0.0 && clip_bounds.y > 0.0)
    //     {
    //         bounds.min.x = std::max(0.0, bounds.min.x);
    //         bounds.min.y = std::max(0.0, bounds.min.y);
    //         bounds.max.x = std::min(clip_bounds.x - 1, bounds.max.x);
    //         bounds.max.y = std::min(clip_bounds.y - 1, bounds.max.y);
    //     }
    //
    //     const int corners_inside = triangle.corners_inside(bounds);
    //     if (corners_inside == 4)
    //     {
    //         for (int y = bounds.min.y; y <= bounds.max.y; ++y)
    //         {
    //             for (int x = bounds.min.x; x <= bounds.max.x; ++x)
    //             {
    //                 pixels.push_back(Vec2i { x, y });
    //             }
    //         }
    //         return;
    //     }
    //
    //     const double a = triangle.get_a();
    //     const double b = triangle.get_b();
    //     const double c = triangle.get_c();
    //
    //     const double d = triangle.get_d();
    //     const double e = triangle.get_e();
    //     const double f = triangle.get_f();
    //
    //     const double g = triangle.get_g();
    //     const double h = triangle.get_h();
    //     const double i = triangle.get_i();
    //
    //     const double area = triangle.get_area();
    //
    //     if (area == 0.0)
    //     {
    //         return;
    //     }
    //
    //     double w0_row = a * (bounds.min.x + 0.5) + b * (bounds.min.y + 0.5) + c;
    //     double w1_row = d * (bounds.min.x + 0.5) + e * (bounds.min.y + 0.5) + f;
    //     double w2_row = g * (bounds.min.x + 0.5) + h * (bounds.min.y + 0.5) + i;
    //
    //     for (int y = bounds.min.y; y < bounds.max.y; ++y)
    //     {
    //         double w0 = w0_row;
    //         double w1 = w1_row;
    //         double w2 = w2_row;
    //
    //         for (int x = bounds.min.x; x < bounds.max.x; ++x)
    //         {
    //             if (w0 * w1 >= 0 && w1 * w2 >= 0)
    //             {
    //                 pixels.push_back(Vec2i { x, y });
    //             }
    //
    //             w0 += a;
    //             w1 += d;
    //             w2 += g;
    //         }
    //
    //         w0_row += b;
    //         w1_row += e;
    //         w2_row += h;
    //     }
    // }

    // void Rasterize::rasterize_filled_triangle(const BarycentricTriangle &triangle, std::vector<Vec2i> &pixels, const Vec2d clip_bounds)
    // {
    //     Box2d bounds {
    //         Vec2d {
    //             std::min({ triangle.v0.x, triangle.v1.x, triangle.v2.x }),
    //             std::min({ triangle.v0.y, triangle.v1.y, triangle.v2.y })
    //         }.round(),
    //         Vec2d {
    //             std::max({ triangle.v0.x, triangle.v1.x, triangle.v2.x }),
    //             std::max({ triangle.v0.y, triangle.v1.y, triangle.v2.y })
    //         }.round()
    //     };
    //
    //     if (clip_bounds.x > 0.0 && clip_bounds.y > 0.0)
    //     {
    //         bounds.min.x = std::max(0.0, bounds.min.x);
    //         bounds.min.y = std::max(0.0, bounds.min.y);
    //         bounds.max.x = std::min((clip_bounds.x) - 1, bounds.max.x);
    //         bounds.max.y = std::min((clip_bounds.y) - 1, bounds.max.y);
    //     }
    //
    //     int corners_inside = triangle.corners_inside(bounds);
    //     if (corners_inside == 4)
    //     {
    //         for (int y = bounds.min.y; y <= bounds.max.y; ++y)
    //         {
    //             for (int x = bounds.min.x; x <= bounds.max.x; ++x)
    //             {
    //                 pixels.push_back(Vec2i { x, y });
    //             }
    //         }
    //         return;
    //     }
    //
    //     double a = triangle.get_a();
    //     double b = triangle.get_b();
    //     double c = triangle.get_c();
    //
    //     double d = triangle.get_d();
    //     double e = triangle.get_e();
    //     double f = triangle.get_f();
    //
    //     double g = triangle.get_g();
    //     double h = triangle.get_h();
    //     double i = triangle.get_i();
    //
    //     double area = triangle.get_area();
    //
    //     if (area == 0.0)
    //     {
    //         return;
    //     }
    //
    //     constexpr int TILE_SIZE = 32;
    //
    //     int width  = bounds.max.x - bounds.min.x + 1;
    //     int height = bounds.max.y - bounds.min.y + 1;
    //     std::size_t resolution = static_cast<std::size_t>(width) * static_cast<std::size_t>(height);
    //
    //     struct Tile { Box2i box; };
    //     std::vector<Tile> tiles;
    //     for (int ty = bounds.min.y; ty <= bounds.max.y; ty += TILE_SIZE)
    //     {
    //         for (int tx = bounds.min.x; tx <= bounds.max.x; tx += TILE_SIZE)
    //         {
    //             Box2i tb;
    //             tb.min.x = tx;
    //             tb.min.y = ty;
    //             tb.max.x = std::min(tx + TILE_SIZE, static_cast<int>(bounds.max.x) + 1);
    //             tb.max.y = std::min(ty + TILE_SIZE, static_cast<int>(bounds.max.y) + 1);
    //             tiles.push_back(Tile { tb });
    //         }
    //     }
    //
    //     auto rasterize_tile = [&](const Box2i &tb) {
    //
    //         double start_x = static_cast<double>(tb.min.x) + 0.5;
    //         double start_y = static_cast<double>(tb.min.y) + 0.5;
    //
    //         double w0_row = a * (tb.min.x + 0.5) + b * (tb.min.y + 0.5) + c;
    //         double w1_row = d * (tb.min.x + 0.5) + e * (tb.min.y + 0.5) + f;
    //         double w2_row = g * (tb.min.x + 0.5) + h * (tb.min.y + 0.5) + i;
    //
    //         for (int y = tb.min.y; y < tb.max.y; ++y)
    //         {
    //             double w0 = w0_row;
    //             double w1 = w1_row;
    //             double w2 = w2_row;
    //
    //             for (int x = tb.min.x; x < tb.max.x; ++x)
    //             {
    //                 if ((w0 * w1 >= 0) && (w1 * w2 >= 0))
    //                 {
    //                     pixels.push_back(Vec2i { x, y });
    //                 }
    //
    //                 w0 += a;
    //                 w1 += d;
    //                 w2 += g;
    //             }
    //
    //             w0_row += b;
    //             w1_row += e;
    //             w2_row += h;
    //         }
    //     };
    //
    //     if (resolution > MIN_MULTITHREAD_PIXELS && tiles.size() > 1)
    //     {
    //         unsigned int hw = std::thread::hardware_concurrency();
    //         unsigned int num_threads = hw ? hw : 2;
    //         num_threads = static_cast<unsigned int>(std::min<std::size_t>(num_threads, tiles.size()));
    //
    //         std::vector<std::thread> threads;
    //         threads.reserve(num_threads);
    //
    //         for (unsigned int ti = 0; ti < num_threads; ++ti)
    //         {
    //             threads.emplace_back([ti, num_threads, &tiles, &rasterize_tile]() {
    //                 for (std::size_t idx = ti; idx < tiles.size(); idx += num_threads)
    //                 {
    //                     rasterize_tile(tiles[idx].box);
    //                 }
    //             });
    //         }
    //
    //         for (auto &th : threads)
    //         {
    //             if (th.joinable()) th.join();
    //         }
    //     }
    //     else
    //     {
    //         for (const auto &t : tiles)
    //         {
    //             rasterize_tile(t.box);
    //         }
    //     }
    // }
}
