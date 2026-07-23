#include "gfx/core/types/bitmap.h"

#include <algorithm>
#include <fstream>
#include <stdexcept>
#include <vector>


namespace gfx
{
    Bitmap::Bitmap(const Vec2i resolution)
        : _resolution(resolution)
        , _pixels(std::vector(resolution.x * resolution.y, Color4 { 0, 0, 0, 255 }))
    {
        if (resolution.x <= 0 || resolution.y <= 0)
        {
            this->_resolution = { 1, 1 };
            _pixels.resize(1, Color4 { 0, 0, 0, 255 });
        }
    }

    void Bitmap::resize(const Vec2i new_resolution)
    {
        _resolution = new_resolution;
        _pixels.resize(_resolution.x * _resolution.y, Color4 { 0, 0, 0, 255 });
    }

    void Bitmap::set_pixel(const Vec2i pos, const Color4 color)
    {
        if (pos.x < 0 || pos.y < 0 || pos.x >= _resolution.x || pos.y >= _resolution.y)
        {
            return;
        }
        _pixels[pos.x + pos.y * _resolution.x] = color;
    }

    Color4 Bitmap::get_pixel(const Vec2i pos) const
    {
        if (pos.x < 0 || pos.y < 0 || pos.x >= _resolution.x || pos.y >= _resolution.y)
        {
            return Color4 { 0, 0, 0, 255 };
        }
        return _pixels[pos.x + pos.y * _resolution.x];
    }

    void Bitmap::compress_colors(const std::vector<Color4>& palette)
    {
        for (auto& pixel : _pixels)
        {
            auto it = std::ranges::min_element(
                palette,
                [&pixel](const Color4& a, const Color4& b) {
                    return Color4::distance(a, pixel) < Color4::distance(b, pixel);
                }
            );
            if (it != palette.end())
            {
                pixel = *it;
            }
        }
    }

    Bitmap Bitmap::decode_bmp(const std::string& filename)
    {
        std::ifstream file { filename, std::ios::binary };
        if (!file)
        {
            throw std::runtime_error { "Failed to open BMP file: " + filename };
        }

        uint16_t file_type {};
        file.read(reinterpret_cast<char*>(&file_type), 2);
        if (file_type != 0x4D42)
        {
            throw std::runtime_error { "Not a valid BMP file" };
        }

        file.ignore(8);

        uint32_t data_offset {};
        file.read(reinterpret_cast<char*>(&data_offset), 4);

        uint32_t dib_header_size {};
        file.read(reinterpret_cast<char*>(&dib_header_size), 4);

        if (dib_header_size < 40)
        {
            throw std::runtime_error { "Unsupported BMP format: DIB header too small" };
        }

        int32_t width {}, height {};
        file.read(reinterpret_cast<char*>(&width), 4);
        file.read(reinterpret_cast<char*>(&height), 4);

        uint16_t planes {}, bit_count {};
        file.read(reinterpret_cast<char*>(&planes), 2);
        file.read(reinterpret_cast<char*>(&bit_count), 2);

        uint32_t compression {};
        file.read(reinterpret_cast<char*>(&compression), 4);

        file.ignore(16);

        const uint32_t extra_header_bytes = dib_header_size > 40 ? dib_header_size - 40 : 0;
        if (extra_header_bytes > 0)
        {
            file.ignore(extra_header_bytes);
        }

        if (compression == 3)
        {
            const uint32_t mask_count = dib_header_size >= 56 ? 4 : 3;
            file.ignore(mask_count * 4);
        }

        if (planes != 1 || (bit_count != 24 && bit_count != 32))
        {
            throw std::runtime_error { "Unsupported BMP: Only 24-bit or 32-bit images supported" };
        }

        if (compression != 0 && compression != 3)
        {
            throw std::runtime_error { "Unsupported BMP: Compressed BMPs not supported" };
        }

        file.seekg(data_offset, std::ios::beg);

        const int row_padded = (bit_count * width + 31) / 32 * 4;
        const int bytes_per_pixel = bit_count / 8;

        Bitmap bmp { Vec2i { width, std::abs(height) } };
        const bool flip_vertically { height > 0 };

        std::vector<uint8_t> row_data(row_padded);

        for (int y = 0; y < std::abs(height); ++y)
        {
            file.read(reinterpret_cast<char*>(row_data.data()), row_padded);
            const int bmp_y = flip_vertically ? std::abs(height) - 1 - y : y;

            for (int x = 0; x < width; ++x)
            {
                const int idx = x * bytes_per_pixel;
                const uint8_t b = row_data[idx];
                const uint8_t g = row_data[idx + 1];
                const uint8_t r = row_data[idx + 2];
                const uint8_t a = bit_count == 32 ? row_data[idx + 3] : 255;

                const Color4 color { r, g, b, a };
                bmp.set_pixel(Vec2i { x, bmp_y }, color);
            }
        }

        return bmp;
    }
}
