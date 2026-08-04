// #include <thread>
// #include <complex>
//
// #include "common/animations/fractal/fractal-demo.h"
// #include "common/animations/fractal/julia.h"
// #include "common/core/demo-utils.h"
//
// namespace demos
// {
//
// using namespace gfx;
//
// void FractalDemo::init()
// {
//     render2D->clear_items();
//
//     cursor_pos = render2D->center();
//     view = { { -2.0, -2.0 }, { 2.0, 2.0 } };
//     view.min *= render2D->get_aspect_ratio();
//     view.max *= render2D->get_aspect_ratio();
//     zoom_velocity = 0.0;
//     pan_velocity = { 0.0, 0.0 };
//
//     fractal = Julia();
//     Vec2i resolution { render2D->get_resolution() };
//     bitmap = render2D->create_bitmap({ 0, 0 }, resolution * render2D->get_viewport_scaling());
//     bitmap->set_scale(Vec2d { 1.0, 1.0 } / render2D->get_viewport_scaling());
//
//     for (int i = 0; i < num_colors; ++i)
//     {
//         double t = static_cast<double>(i) / static_cast<double>(num_colors);
//         colors.emplace_back(Color4::lerp(color0, color1, exponential_step(t, 30.0)));
//     }
//
//     render2D->add_item(bitmap);
// }
//
// void FractalDemo::update_view(const double dt)
// {
//     if (zoom_velocity != 0.0)
//     {
//         Vec2d cursor_world = screen_to_world(cursor_pos);
//
//         double zoom_scale = 1.0 - zoom_velocity * dt;
//
//         Vec2d new_min = cursor_world + (view.min - cursor_world) * zoom_scale;
//         Vec2d new_max = cursor_world + (view.max - cursor_world) * zoom_scale;
//
//         view.min = new_min;
//         view.max = new_max;
//
//         zoom_velocity *= std::exp(-5 * dt);
//         if (std::abs(zoom_velocity) < 0.01)
//         {
//             zoom_velocity = 0.0;
//         }
//     }
//
//     if (pan_velocity.x != 0.0 || pan_velocity.y != 0.0)
//     {
//         Vec2d shift { pan_velocity * dt };
//         view.min += shift;
//         view.max += shift;
//
//         pan_velocity *= std::exp(-5 * dt);
//         if (pan_velocity.length() < 0.01 * view.size().x)
//         {
//             pan_velocity = { 0.0, 0.0 };
//         }
//     }
// }
//
// void FractalDemo::handle_mouse_pan(const double dt)
// {
//     Vec2d resolution { render2D->get_resolution() };
//     Vec2d center { resolution / 2.0 };
//     if (Vec2d::distance(cursor_pos, center) > resolution.y * mouse_pan_threshold)
//     {
//         Vec2d dir { (cursor_pos - center) };
//         pan(dir * mouse_pan_sensitivity);
//     }
// }
//
// void FractalDemo::render_frame(const double dt)
// {
//     double t0 { time_us() };
//     double time_ms { t0 / 1000.0 };
//
//     update_view(dt);
//     handle_mouse_pan(dt);
//
//     // fractal.set_max_iterations(std::max(100 * std::log2(4.0 / (view.size().x)), 100.0));
//     fractal.set_max_iterations(300);
//
//     Vec2i resolution { bitmap->get_resolution() };
//     double aspect_ratio { static_cast<double>(resolution.x) / static_cast<double>(resolution.y) };
//
//     unsigned int num_threads = { std::thread::hardware_concurrency() };
//     int rows_per_thread { static_cast<int>(resolution.y / num_threads) };
//
//     if (!paused)
//     {
//         double t { time_sec() };
//         a += a_frequency * dt;
//         if (a > 2 * std::numbers::pi)
//         {
//             a -= 2 * std::numbers::pi;
//         }
//         fractal.constant = 0.7885 * std::exp(std::complex<double>(0, a));
//     }
//
//     auto worker = [&](int start_row, int end_row)
//         {
//             for (int y = start_row; y < end_row; ++y)
//             {
//                 for (int x = 0; x < resolution.x; ++x)
//                 {
//                     double re = view.min.x + (static_cast<double>(x) / static_cast<double>(resolution.x)) * (view.max.x - view.min.x);
//                     double im = view.min.y + (static_cast<double>(y) / static_cast<double>(resolution.y)) * (view.max.y - view.min.y);
//                     // if (is_inside_cardioid_or_bulb(re, im))
//                     // {
//                     //     bitmap->set_pixel({ x, y }, { 0, 0, 0, 0 });
//                     //     continue;
//                     // }
//                     std::complex<double> c { re, im };
//
//                     int iterations = fractal.compute(c);
//
//                     int smooth_iter;
//                     if (iterations < fractal.get_max_iterations()) 
//                     {
//                         double log_zn = log(std::norm(c)) / 2.0;
//                         double nu = log(log_zn / log(2)) / log(2);
//                         smooth_iter = iterations + 1 - nu;
//                     } 
//                     else 
//                     {
//                         smooth_iter = iterations;
//                     }
//
//                     int color_value = static_cast<int>(255.0 * smooth_iter / static_cast<double>(fractal.get_max_iterations()));
//                     if (iterations >= fractal.get_max_iterations())
//                     {
//                         bitmap->set_pixel({ x, y }, { 0, 0, 0, 0 });
//                         continue;
//                     }
//                     double t { static_cast<double>(iterations) / fractal.get_max_iterations() };
//                     int color_index { static_cast<int>(t * num_colors) };
//
//                     bitmap->set_pixel({ x, y }, colors[color_index % colors.size()]);
//                 }
//             }
//         };
//
//     std::vector<std::thread> threads;
//     for (unsigned int i = 0; i < num_threads; ++i)
//     {
//         int start_row = i * rows_per_thread;
//         int end_row = (i == num_threads - 1) ? resolution.y : start_row + rows_per_thread;
//         threads.emplace_back(worker, start_row, end_row);
//     }
//
//     for (auto& thread : threads)
//     {
//         thread.join();
//     }
//
//     render2D->clear_frame();
//     render2D->draw_frame();
//     render2D->present_frame();
// }
//
// bool FractalDemo::is_inside_cardioid_or_bulb(double re, double im)
// {
//     double re_plus1 = re + 1.0;
//     if (re_plus1 * re_plus1 + im * im < 0.0625)
//     {
//         return true;
//     }
//
//     double re_minus_quarter = re - 0.25;
//     double q = re_minus_quarter * re_minus_quarter + im * im;
//
//     return q * (q + re_minus_quarter) < 0.25 * im * im;
// }
//
// void FractalDemo::zoom(const double factor)
// {
//     zoom_velocity += (factor - 1.0) * zoom_factor;
//     zoom_velocity = std::clamp(zoom_velocity, -max_zoom_velocity, max_zoom_velocity);
// }
//
// void FractalDemo::pan(const Vec2d acceleration)
// {
//     pan_velocity += acceleration * pan_factor * view.size().x;
//     pan_velocity.x = std::clamp(pan_velocity.x, -max_pan_velocity, max_pan_velocity);
//     pan_velocity.y = std::clamp(pan_velocity.y, -max_pan_velocity, max_pan_velocity);
// }
//
// void FractalDemo::set_a_frequency(const double freq)
// {
//     a_frequency = freq;
// }
//
// void FractalDemo::handle_char(const int input)
// {
//     switch (input)
//     {
//         case ' ':
//             paused = !paused;
//             break;
//
//         case 'h':
//             set_a_frequency(a_frequency * 0.9);
//             break;
//         case 'l':
//             set_a_frequency(a_frequency * 1.1);
//             break;
//
//         case 'k':
//             zoom(1.25);
//             break;
//         case 'j':
//             zoom(0.75);
//             break;
//
//         case 'a':
//             pan({ -1, 0 });
//             break;
//         case 'd':
//             pan({ 1, 0 });
//             break;
//         case 'w':
//             pan({ 0, -1 });
//             break;
//         case 's':
//             pan({ 0, 1 });
//             break;
//
//         default:
//             break;
//     }
//     fflush(stdin);
// }
//
// void FractalDemo::report_mouse(const MouseEvent event)
// {
//     if (event.type == MouseEventType::SCROLL_UP)
//     {
//         zoom(1.1);
//     }
//     else if (event.type == MouseEventType::SCROLL_DOWN)
//     {
//         zoom(0.9);
//     }
//     else if (event.type == MouseEventType::MOVE)
//     {
//         cursor_pos = event.position * render2D->get_resolution();
//     }
//     else if (event.type == MouseEventType::LEFT_DOWN)
//     {
//         zoom(2.0);
//     }
// }
//
// void FractalDemo::end()
// {
//     render2D->clear_items();
// }
//
// }
