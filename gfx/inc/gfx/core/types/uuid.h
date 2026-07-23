#pragma once

#include <functional>

namespace gfx
{
    struct UUID
    {
        uint64_t part1;
        uint64_t part2;

        UUID();
        UUID(uint64_t p1, uint64_t p2);

        bool operator==(const UUID& other) const;
        bool operator!=(const UUID& other) const;

        static UUID generate();
    };
}

template <>
struct std::hash<gfx::UUID>
{
    size_t operator()(const gfx::UUID& uuid) const noexcept
    {
        return std::hash<uint64_t>()(uuid.part1) ^ std::hash<uint64_t>()(uuid.part2) << 1;
    }
};
