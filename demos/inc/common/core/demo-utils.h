#pragma once

#include <chrono>
#include <random>

namespace demos
{
    inline double ease_in_out_cubic(const double t)
    {
        return t < 0.5 ? 4.0 * t * t * t : 1.0 - pow(-2.0 * t + 2.0, 3) / 2.0;
    }

    inline double smoothstep(const double d)
    {
        return d * d * (3.0 - 2.0 * d);
    }

    inline double smoothstep(const double edge0, const double edge1, const double x)
    {
        double t = (x - edge0) / (edge1 - edge0);
        t = t < 0 ? 0 : t > 1 ? 1 : t;
        return t * t * (3.0 - 2.0 * t);
    }

    inline double exponential_step(const double t, const double k = 5.0)
    {
        return (1.0 - std::exp(-k * t)) / (1.0 - std::exp(-k));
    }

    inline double exponential_interp_3(
        const double a,
        const double b,
        const double c,
        const double t,
        const double k = 5.0
    )
    {
        const double exp_t = (1.0 - std::exp(-k * t)) / (1.0 - std::exp(-k));

        const double ab = a + (b - a) * exp_t;
        const double bc = b + (c - b) * exp_t;

        return ab + (bc - ab) * exp_t;
    }

    inline double move_towards(const double current, const double target, const double max_delta)
    {
        if (std::abs(target - current) <= max_delta)
        {
            return target;
        }
        return current + std::copysign(max_delta, target - current);
    }

    inline double random_double(const double min, const double max)
    {
        // static std::mt19937 rng(static_cast<unsigned int>(std::chrono::steady_clock::now().time_since_epoch().count()));
        static std::mt19937 rng(0);
        std::uniform_real_distribution dist(min, max);
        return dist(rng);
    }

    inline int random_int(const int min, const int max)
    {
        static std::mt19937 rng(static_cast<unsigned int>(std::chrono::steady_clock::now().time_since_epoch().count()));
        std::uniform_int_distribution dist(min, max);
        return dist(rng);
    }

    inline double time_sec()
    {
        return static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch()
        ).count()) / 1000.0;
    }

    inline double time_ms()
    {
        return static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch()
        ).count());
    }

    inline double time_us()
    {
        return static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::steady_clock::now().time_since_epoch()
        ).count());
    }

    inline double lerp(const double a, const double b, const double t)
    {
        return a + (b - a) * (t < 0 ? 0 : t > 1 ? 1 : t);
    }

    inline double inv_lerp(const double a, const double b, const double v)
    {
        return (v - a) / (b - a);
    }
}
