#include "gfx/core/types/color4.h"

#include <algorithm>
#include <cmath>

namespace gfx
{
    Color4::Color4(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a)
        : r(r)
        , g(g)
        , b(b)
        , a(a) {}

    Color4::Color4(const int r, const int g, const int b, const int a)
        : r(r)
        , g(g)
        , b(b)
        , a(a) {}

    Color4::Color4(const uint8_t r, const uint8_t g, const uint8_t b)
        : r(r)
        , g(g)
        , b(b)
        , a(255) {}

    Color4::Color4(const int r, const int g, const int b)
        : r(r)
        , g(g)
        , b(b)
        , a(255) {}

    Color4::Color4(const double r, const double g, const double b, const double a)
        : r(static_cast<uint8_t>(r * 255.0))
        , g(static_cast<uint8_t>(g * 255.0))
        , b(static_cast<uint8_t>(b * 255.0))
        , a(static_cast<uint8_t>(a * 255.0)) {}

    Color4::Color4(const double r, const double g, const double b)
        : r(static_cast<uint8_t>(r * 255.0))
        , g(static_cast<uint8_t>(g * 255.0))
        , b(static_cast<uint8_t>(b * 255.0))
        , a(255) {}

    Color4::Color4(const double x)
        : r(static_cast<uint8_t>(x * 255.0))
        , g(static_cast<uint8_t>(x * 255.0))
        , b(static_cast<uint8_t>(x * 255.0))
        , a(255) {}

    Color4::Color4()
        : Color4(0, 0, 0, 0) {}

    Color4 Color4::from_i32(const int32_t color)
    {
        return Color4(
            static_cast<uint8_t>(color >> 24 & 0xFF),
            static_cast<uint8_t>(color >> 16 & 0xFF),
            static_cast<uint8_t>(color >> 8 & 0xFF),
            static_cast<uint8_t>(color & 0xFF)
        );
    }

    double Color4::distance(const Color4 a, const Color4 b)
    {
        return std::sqrt(
            (a.r - b.r) * (a.r - b.r) + (a.g - b.g) * (a.g - b.g) + (a.b - b.b) * (a.b - b.b) + (a.a - b.a) * (a.a - b.
                a)
        );
    }

    Color4 Color4::set_alpha(const uint8_t alpha) const
    {
        return Color4(r, g, b, alpha);
    }

    Color4 Color4::set_alpha(const double alpha) const
    {
        return Color4(r, g, b, static_cast<uint8_t>(std::clamp(alpha * 255.0, 0.0, 255.0)));
    }

    Color4 Color4::operator+(const Color4& other) const
    {
        return Color4(
            static_cast<uint8_t>(std::min(255, r + other.r)),
            static_cast<uint8_t>(std::min(255, g + other.g)),
            static_cast<uint8_t>(std::min(255, b + other.b)),
            static_cast<uint8_t>(std::min(255, a + other.a))
        );
    }

    Color4 Color4::operator-(const Color4& other) const
    {
        return Color4(
            static_cast<uint8_t>(std::max(0, r - other.r)),
            static_cast<uint8_t>(std::max(0, g - other.g)),
            static_cast<uint8_t>(std::max(0, b - other.b)),
            static_cast<uint8_t>(std::max(0, a - other.a))
        );
    }

    Color4 Color4::operator*(const double scalar) const
    {
        return Color4(
            static_cast<uint8_t>(std::clamp(static_cast<int>(r * scalar), 0, 255)),
            static_cast<uint8_t>(std::clamp(static_cast<int>(g * scalar), 0, 255)),
            static_cast<uint8_t>(std::clamp(static_cast<int>(b * scalar), 0, 255)),
            static_cast<uint8_t>(std::clamp(static_cast<int>(a * scalar), 0, 255))
        );
    }

    Color4 Color4::operator*(const Color4& other) const
    {
        return Color4(
            static_cast<uint8_t>(r * other.r / 255),
            static_cast<uint8_t>(g * other.g / 255),
            static_cast<uint8_t>(b * other.b / 255),
            static_cast<uint8_t>(a * other.a / 255)
        );
    }

    Color4 Color4::operator/(const double scalar) const
    {
        if (scalar == 0)
        {
            return Color4(255, 255, 255, 255);
        }

        return Color4(
            static_cast<uint8_t>(std::clamp(static_cast<int>(r / scalar), 0, 255)),
            static_cast<uint8_t>(std::clamp(static_cast<int>(g / scalar), 0, 255)),
            static_cast<uint8_t>(std::clamp(static_cast<int>(b / scalar), 0, 255)),
            static_cast<uint8_t>(std::clamp(static_cast<int>(a / scalar), 0, 255))
        );
    }

    bool Color4::operator==(const Color4& other) const
    {
        return r == other.r && g == other.g && b == other.b && a == other.a;
    }

    void Color4::operator=(const Color4& other)
    {
        r = other.r;
        g = other.g;
        b = other.b;
        a = other.a;
    }

    float Color4::r_double() const
    {
        return r / 255.0f;
    }

    float Color4::g_double() const
    {
        return g / 255.0f;
    }

    float Color4::b_double() const
    {
        return b / 255.0f;
    }

    float Color4::a_double() const
    {
        return a / 255.0f;
    }

    int32_t Color4::to_i32() const
    {
        return r << 24 | g << 16 | b << 8 | a;
    }

    Color4 Color4::lerp(const Color4& a, const Color4& b, double t)
    {
        t = t < 0 ? 0 : t > 1 ? 1 : t;
        return Color4(
            static_cast<uint8_t>(a.r + (b.r - a.r) * t),
            static_cast<uint8_t>(a.g + (b.g - a.g) * t),
            static_cast<uint8_t>(a.b + (b.b - a.b) * t),
            static_cast<uint8_t>(a.a + (b.a - a.a) * t)
        );
    }

    Color4 Color4::bilinear_interp(
        const Color4& c00,
        const Color4& c10,
        const Color4& c01,
        const Color4& c11,
        const double sx,
        const double sy
    )
    {
        const Color4 c0 = lerp(c00, c10, sx);
        const Color4 c1 = lerp(c01, c11, sx);
        return lerp(c0, c1, sy);
    }

    Color4 Color4::trilinear_interp(
        const Color4& c1,
        const Color4& c2,
        const Color4& c3,
        const double w1,
        const double w2,
        const double w3
    )
    {
        return Color4(
            static_cast<uint8_t>(c1.r * w1 + c2.r * w2 + c3.r * w3),
            static_cast<uint8_t>(c1.g * w1 + c2.g * w2 + c3.g * w3),
            static_cast<uint8_t>(c1.b * w1 + c2.b * w2 + c3.b * w3),
            static_cast<uint8_t>(c1.a * w1 + c2.a * w2 + c3.a * w3)
        );
    }

    Color4 Color4::black()
    {
        return Color4(0, 0, 0, 255);
    }

    Color4 Color4::white()
    {
        return Color4(255, 255, 255, 255);
    }

    Color4 Color4::red()
    {
        return Color4(255, 0, 0, 255);
    }

    Color4 Color4::green()
    {
        return Color4(0, 255, 0, 255);
    }

    Color4 Color4::blue()
    {
        return Color4(0, 0, 255, 255);
    }

    Color4 Color4::transparent()
    {
        return Color4(0, 0, 0, 0);
    }
}
