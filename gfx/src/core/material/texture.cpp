#include "gfx/core/material/texture.h"

namespace gfx
{
    Texture::Texture(const Vec2i resolution)
        : _resolution(resolution)
        , _data(resolution.x * resolution.y, Color4(0, 0, 0, 0)) {}

    Texture::Texture(const Vec2i resolution, const std::vector<Color4>& data)
        : _resolution(resolution)
        , _data(data) {}

    void Texture::resize(const Vec2i new_resolution)
    {
        _resolution = new_resolution;
        _data.resize(_resolution.x * _resolution.y, Color4(0, 0, 0, 0));
    }

    Vec2i Texture::get_resolution() const
    {
        return _resolution;
    }

    void Texture::set_pixel(const Vec2i pos, const Color4& color)
    {
        if (pos.x < 0 || pos.x >= _resolution.x || pos.y < 0 || pos.y >= _resolution.y)
        {
            return;
        }

        _data[pos.y * _resolution.x + pos.x] = color;
    }

    void Texture::set_pixel(const int x, const int y, const Color4& color)
    {
        set_pixel(Vec2i { x, y }, color);
    }

    Color4 Texture::get_pixel(const Vec2i pos) const
    {
        if (pos.x < 0 || pos.x >= _resolution.x || pos.y < 0 || pos.y >= _resolution.y)
        {
            return Color4(0, 0, 0, 255);
        }
        return _data[pos.y * _resolution.x + pos.x];
    }

    Color4 Texture::sample(const Vec2d uv, const FilteringMode mode) const
    {
        switch (mode)
        {
        case FilteringMode::NEAREST:
            return sample_nearest(uv);
        case FilteringMode::BILINEAR:
            return sample_bilinear(uv);
        default:
            return sample_nearest(uv);
        }
    }

    Color4 Texture::sample_nearest(const Vec2d uv) const
    {
        const Vec2d wrapped_uv { uv.x - std::floor(uv.x), uv.y - std::floor(uv.y) };

        const int x = static_cast<int>(wrapped_uv.x * _resolution.x) % _resolution.x;
        const int y = static_cast<int>(wrapped_uv.y * _resolution.y) % _resolution.y;

        return _data[y * _resolution.x + x];
    }

    Color4 Texture::sample_bilinear(const Vec2d uv) const
    {
        const Vec2d wrapped_uv { uv.x - std::floor(uv.x), uv.y - std::floor(uv.y) };

        const double x = wrapped_uv.x * _resolution.x - 0.5;
        const double y = wrapped_uv.y * _resolution.y - 0.5;

        const int ix0 = static_cast<int>(std::floor(x));
        const int iy0 = static_cast<int>(std::floor(y));

        auto wrap = [](const int v, const int max) {
            return (v % max + max) % max;
        };

        const int x0 = wrap(ix0, _resolution.x);
        const int y0 = wrap(iy0, _resolution.y);
        const int x1 = wrap(ix0 + 1, _resolution.x);
        const int y1 = wrap(iy0 + 1, _resolution.y);

        const double sx = x - std::floor(x);
        const double sy = y - std::floor(y);

        const Color4& c00 = _data[y0 * _resolution.x + x0];
        const Color4& c10 = _data[y0 * _resolution.x + x1];
        const Color4& c01 = _data[y1 * _resolution.x + x0];
        const Color4& c11 = _data[y1 * _resolution.x + x1];

        return Color4::bilinear_interp(c00, c10, c01, c11, sx, sy);
    }
}
