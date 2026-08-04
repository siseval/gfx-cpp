#pragma once

#include <complex>
#include <demos/common/animations/fractal/fractal.h>

namespace demos
{
    class Julia : public Fractal
    {
    public:

        int compute(const std::complex<double> c) override
        {
            int iterations = 0;
            std::complex<double> z = c;
            while (std::abs(z) <= 2.0 && iterations < max_iterations)
            {
                z = z * z * z + constant;
                ++iterations;
            }
            return iterations;
        }

        std::complex<double> constant { -0.285, 0.01 };
    };
}
