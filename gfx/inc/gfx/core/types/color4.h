#pragma once

#include <cstdint>
#include <functional>

namespace gfx
{
    struct Color4
    {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;

        Color4(std::uint8_t r, uint8_t g, uint8_t b, uint8_t a);
        Color4(int r, int g, int b, int a);

        Color4(uint8_t r, uint8_t g, uint8_t b);
        Color4(int r, int g, int b);

        Color4(double r, double g, double b, double a);
        Color4(double r, double g, double b);

        explicit Color4(double x);

        Color4();

        static Color4 from_i32(int32_t color);

        static double distance(Color4 a, Color4 b);

        Color4 set_alpha(uint8_t alpha) const;
        Color4 set_alpha(double alpha) const;

        Color4 operator+(const Color4& other) const;
        Color4 operator-(const Color4& other) const;
        Color4 operator*(double scalar) const;
        Color4 operator*(const Color4& other) const;
        Color4 operator/(double scalar) const;

        bool operator==(const Color4& other) const;
        void operator=(const Color4& other);

        float r_double() const;
        float g_double() const;
        float b_double() const;
        float a_double() const;

        int32_t to_i32() const;

        static Color4 lerp(const Color4& a, const Color4& b, double t);
        static Color4 bilinear_interp(
            const Color4& c00,
            const Color4& c10,
            const Color4& c01,
            const Color4& c11,
            double sx,
            double sy
        );
        static Color4 trilinear_interp(
            const Color4& c1,
            const Color4& c2,
            const Color4& c3,
            double w1,
            double w2,
            double w3
        );

        static Color4 black();
        static Color4 white();
        static Color4 red();
        static Color4 green();
        static Color4 blue();
        static Color4 transparent();
    };
}

template <>
struct std::hash<gfx::Color4>
{
    size_t operator()(const gfx::Color4& color) const noexcept
    {
        return std::hash<uint8_t>()(color.r ^ std::hash<uint8_t>()(color.g) << 1) ^ std::hash<uint8_t>()(color.b) << 2;
    }
};
