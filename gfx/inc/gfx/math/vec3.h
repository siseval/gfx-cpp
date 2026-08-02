#pragma once

#include <cmath>
#include <functional>
#include <numbers>

namespace gfx
{
    template <typename T>
    struct Vec3
    {
        T x;
        T y;
        T z;

        Vec3()
            : x(0)
            , y(0)
            , z(0) {}

        Vec3(T x, T y, T z)
            : x(x)
            , y(y)
            , z(z) {}

        Vec3(const Vec3& other)
            : x(other.x)
            , y(other.y)
            , z(other.z) {}

        explicit Vec3(T v)
            : x(v)
            , y(v)
            , z(v) {}

        static constexpr Vec3 zero()
        {
            return { 0, 0, 0 };
        }
        
        static constexpr Vec3 one()
        {
            return { 1, 1, 1 };
        }
        
        static constexpr Vec3 min()
        {
            return Vec3 {
                std::numeric_limits<T>::lowest(),
                std::numeric_limits<T>::lowest(),
                std::numeric_limits<T>::lowest()
            };
        }
        
        static constexpr Vec3 max()
        {
            return Vec3 {
                std::numeric_limits<T>::max(), 
                std::numeric_limits<T>::max(), 
                std::numeric_limits<T>::max()
            };
        }

        static T dot(Vec3 a, Vec3 b)
        {
            return static_cast<T>(a.x * b.x + a.y * b.y + a.z * b.z);
        }

        static Vec3 cross(Vec3 a, Vec3 b)
        {
            return {
                static_cast<T>(a.y * b.z - a.z * b.y),
                static_cast<T>(a.z * b.x - a.x * b.z),
                static_cast<T>(a.x * b.y - a.y * b.x)
            };
        }

        static double distance(Vec3 a, Vec3 b)
        {
            return (b - a).length();
        }

        static Vec3 lerp(const Vec3& a, const Vec3& b, double t)
        {
            t = t < 0 ? 0 : t > 1 ? 1 : t;
            return {
                static_cast<T>(a.x + (b.x - a.x) * t),
                static_cast<T>(a.y + (b.y - a.y) * t),
                static_cast<T>(a.z + (b.z - a.z) * t)
            };
        }

        static Vec3 from_angles(const double theta, const double phi, const double length = 1.0)
        {
            const double sin_phi = std::sin(phi);
            return {
                static_cast<T>(length * sin_phi * std::cos(theta)),
                static_cast<T>(length * sin_phi * std::sin(theta)),
                static_cast<T>(length * std::cos(phi))
            };
        }

        static Vec3 from_angles_degrees(const double theta_deg, const double phi_deg, const double length = 1.0)
        {
            return from_angles(theta_deg * std::numbers::pi / 180.0, phi_deg * std::numbers::pi / 180.0, length);
        }

        Vec3 limit(const double max_magnitude)
        {
            const double magnitude = length();
            if (magnitude > max_magnitude)
            {
                return normalize() * max_magnitude;
            }
            return { x, y, z };
        }

        static Vec3 clamp(const Vec3& v, const Vec3& min, const Vec3& max)
        {
            return {
                static_cast<T>(v.x < min.x ? min.x : v.x > max.x ? max.x : v.x),
                static_cast<T>(v.y < min.y ? min.y : v.y > max.y ? max.y : v.y),
                static_cast<T>(v.z < min.z ? min.z : v.z > max.z ? max.z : v.z)
            };
        }

        static Vec3 clamp(const Vec3& v, const double min, const double max)
        {
            return clamp(v, Vec3(min), Vec3(max));
        }

        Vec3<double> normal() const
        {
            if (std::fabs(x) > 1e-6 || std::fabs(y) > 1e-6)
            {
                return Vec3<double> { -y, x, 0 }.normalize();
            }
            return Vec3<double> { 0, -z, y }.normalize();
        }
        
        T min_element() const
        {
            return std::min(x, y, z);
        }
        
        T max_element() const
        {
            return std::max(x, y, z);
        }

        double length() const
        {
            return std::sqrt(x * x + y * y + z * z);
        }

        Vec3<double> normalize() const
        {
            return { static_cast<T>(x / length()), static_cast<T>(y / length()), static_cast<T>(z / length()) };
        }

        Vec3 operator-() const
        {
            return { static_cast<T>(-x), static_cast<T>(-y), static_cast<T>(-z) };
        }

        template <typename U>
        Vec3 operator+(const Vec3<U>& other) const
        {
            return { static_cast<T>(x + other.x), static_cast<T>(y + other.y), static_cast<T>(z + other.z) };
        }

        template <typename U>
        Vec3 operator-(const Vec3<U>& other) const
        {
            return { static_cast<T>(x - other.x), static_cast<T>(y - other.y), static_cast<T>(z - other.z) };
        }

        template <typename U>
        Vec3 operator*(const Vec3<U>& other) const
        {
            return { static_cast<T>(x * other.x), static_cast<T>(y * other.y), static_cast<T>(z * other.z) };
        }

        template <typename U>
        Vec3 operator/(const Vec3<U>& other) const
        {
            return { static_cast<T>(x / other.x), static_cast<T>(y / other.y), static_cast<T>(z / other.z) };
        }

        Vec3 operator*(const double& factor) const
        {
            return { static_cast<T>(x * factor), static_cast<T>(y * factor), static_cast<T>(z * factor) };
        }

        Vec3 operator/(const double& factor) const
        {
            return { static_cast<T>(x / factor), static_cast<T>(y / factor), static_cast<T>(z / factor) };
        }

        template <typename U>
        Vec3& operator+=(const Vec3<U>& other)
        {
            x += other.x;
            y += other.y;
            z += other.z;
            return *this;
        }

        template <typename U>
        Vec3& operator-=(const Vec3<U>& other)
        {
            x -= other.x;
            y -= other.y;
            z -= other.z;
            return *this;
        }

        template <typename U>
        Vec3& operator*=(const Vec3<U>& other)
        {
            x *= other.x;
            y *= other.y;
            z *= other.z;
            return *this;
        }

        template <typename U>
        Vec3& operator/=(const Vec3<U>& other)
        {
            x /= other.x;
            y /= other.y;
            z /= other.z;
            return *this;
        }

        Vec3& operator*=(const double& factor)
        {
            x *= factor;
            y *= factor;
            z *= factor;
            return *this;
        }

        Vec3& operator/=(const double& factor)
        {
            x /= factor;
            y /= factor;
            z /= factor;
            return *this;
        }

        bool operator==(const Vec3& other) const
        {
            return x == other.x && y == other.y && z == other.z;
        }

        bool operator!=(const Vec3& other) const
        {
            return x != other.x || y != other.y || z != other.z;
        }

        Vec3& operator=(const Vec3& other)
        {
            x = other.x;
            y = other.y;
            z = other.z;

            return *this;
        }

        template <typename U>
        explicit operator Vec3<U>() const
        {
            return { static_cast<U>(x), static_cast<U>(y), static_cast<U>(z) };
        }
    };

    using Vec3i = Vec3<int>;
    using Vec3l = Vec3<long>;
    using Vec3d = Vec3<double>;
    using Vec3f = Vec3<float>;
}

template <typename T>
struct std::hash<gfx::Vec3<T>>
{
    size_t operator()(const gfx::Vec3<T>& vec) const
    {
        return std::hash<T>()(vec.x) ^ std::hash<T>()(vec.y) << 1 ^ std::hash<T>()(vec.z) << 2;
    }
};
