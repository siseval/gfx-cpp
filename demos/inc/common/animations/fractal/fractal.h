#pragma once

#include <complex>
#include <gfx/math/vec2.h>

namespace demos
{
    class Fractal
    {
    public:

        virtual int compute(std::complex<double> c) = 0;

        void set_max_iterations(const int it)
        {
            max_iterations = it;
        }

        int get_max_iterations() const
        {
            return max_iterations;
        }

    protected:

        int max_iterations { 50 };
    };
}
