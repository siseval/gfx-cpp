#include "gfx/core/types/uuid.h"

#include <random>

namespace gfx
{
    UUID::UUID()
        : UUID(generate()) {}

    UUID::UUID(const uint64_t p1, const uint64_t p2)
        : part1(p1)
        , part2(p2) {}

    bool UUID::operator==(const UUID& other) const
    {
        return part1 == other.part1 && part2 == other.part2;
    }

    bool UUID::operator!=(const UUID& other) const
    {
        return !(*this == other);
    }

    UUID UUID::generate()
    {
        static uint64_t counter = 0;

        static std::mt19937_64 rng(std::random_device {}());
        static std::uniform_int_distribution<uint64_t> dist;

        UUID id(0, 0);
        id.part1 = dist(rng) << 32 | dist(rng);
        id.part2 = dist(rng) << 32 | dist(rng);
        id.part2 ^= counter++;
        return id;
    }
}
