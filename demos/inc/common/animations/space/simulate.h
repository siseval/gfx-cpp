#pragma once

namespace demos
{
    class Body;
    class SpaceDemo;

    class Simulate
    {
    public:

        static void solar_system(SpaceDemo& demo);
        static void three_body_problem(SpaceDemo& demo);
        static void chaos(SpaceDemo& demo);
        static void binary_system(SpaceDemo& demo);
    };
}
