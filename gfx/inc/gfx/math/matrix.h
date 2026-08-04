#pragma once

#include <functional>
#include <stdexcept>

namespace gfx
{
    template <typename T, int ROWS, int COLS>
    class Matrix
    {
    public:

        Matrix() {}

        Matrix(std::initializer_list<std::initializer_list<T>> array)
        {
            int r = 0;
            for (const auto& row : array)
            {
                int c = 0;
                for (const auto& val : row)
                {
                    _data[r][c] = val;
                    ++c;
                }
                ++r;
            }
        }

        static constexpr Matrix identity()
        {
            Matrix result;
            for (int r = 0; r < ROWS; ++r)
            {
                for (int c = 0; c < COLS; ++c)
                {
                    result(r, c) = r == c ? T(1) : T(0);
                }
            }
            return result;
        }

        static constexpr Matrix zero()
        {
            Matrix result;
            for (int r = 0; r < ROWS; ++r)
            {
                for (int c = 0; c < COLS; ++c)
                {
                    result(r, c) = T(0);
                }
            }
            return result;
        }

        T& operator()(int r, int c)
        {
            return _data[r][c];
        }

        const T& operator()(int r, int c) const
        {
            return _data[r][c];
        }

        Matrix operator+(const Matrix& other) const
        {
            Matrix result;
            for (int r = 0; r < ROWS; ++r)
            {
                for (int c = 0; c < COLS; ++c)
                {
                    result(r, c) = _data[r][c] + other(r, c);
                }
            }
            return result;
        }

        Matrix operator-(const Matrix& other) const
        {
            Matrix result;
            for (int r = 0; r < ROWS; ++r)
            {
                for (int c = 0; c < COLS; ++c)
                {
                    result(r, c) = _data[r][c] - other(r, c);
                }
            }
            return result;
        }

        Matrix operator*(T scalar) const
        {
            Matrix result;
            for (int r = 0; r < ROWS; ++r)
            {
                for (int c = 0; c < COLS; ++c)
                {
                    result(r, c) = _data[r][c] * scalar;
                }
            }
            return result;
        }

        Matrix operator/(T scalar) const
        {
            Matrix result;
            for (int r = 0; r < ROWS; ++r)
            {
                for (int c = 0; c < COLS; ++c)
                {
                    result(r, c) = _data[r][c] / scalar;
                }
            }
            return result;
        }
        
       Matrix inverse() const
        {
            static_assert(ROWS == COLS, "matrix not invertible");

            Matrix result { identity() };
            Matrix temp { *this };

            for (int i = 0; i < ROWS; ++i)
            {
                int pivot_row { i };
                T max_val { std::abs(temp(i, i)) };
                
                for (int r = i + 1; r < ROWS; ++r)
                {
                    if (std::abs(temp(r, i)) > max_val)
                    {
                        max_val = std::abs(temp(r, i));
                        pivot_row = r;
                    }
                }

                if constexpr (std::is_floating_point_v<T>)
                {
                    if (std::abs(temp(pivot_row, i)) < T(1e-6))
                    {
                        throw std::runtime_error("matrix not invertible");
                    }
                }
                else 
                {
                    if (temp(pivot_row, i) == T(0))
                    {
                        throw std::runtime_error("matrix not invertible");
                    }
                }

                if (pivot_row != i)
                {
                    for (int c = 0; c < COLS; ++c)
                    {
                        std::swap(temp(i, c), temp(pivot_row, c));
                        std::swap(result(i, c), result(pivot_row, c));
                    }
                }

                T pivot_val = temp(i, i);
                for (int c = 0; c < COLS; ++c)
                {
                    temp(i, c) /= pivot_val;
                    result(i, c) /= pivot_val;
                }

                for (int r = 0; r < ROWS; ++r)
                {
                    if (r != i)
                    {
                        T factor = temp(r, i);
                        for (int c = 0; c < COLS; ++c)
                        {
                            temp(r, c) -= factor * temp(i, c);
                            result(r, c) -= factor * result(i, c);
                        }
                    }
                }
            }

            return result;
        } 

        Matrix<T, COLS, ROWS> transpose() const
        {
            Matrix<T, COLS, ROWS> result;
            for (int r = 0; r < ROWS; ++r)
            {
                for (int c = 0; c < COLS; ++c)
                {
                    result(c, r) = _data[r][c];
                }
            }
            return result;
        }

        Matrix apply(std::function<T(T)> func) const
        {
            Matrix result;
            for (int r = 0; r < ROWS; ++r)
            {
                for (int c = 0; c < COLS; ++c)
                {
                    result(r, c) = func(_data[r][c]);
                }
            }
            return result;
        }

        template <int cols2>
        Matrix<T, ROWS, cols2> operator*(const Matrix<T, COLS, cols2>& other) const
        {
            Matrix<T, ROWS, cols2> result;
            for (int r = 0; r < ROWS; ++r)
            {
                for (int c = 0; c < cols2; ++c)
                {
                    result(r, c) = T();
                    for (int k = 0; k < COLS; ++k)
                    {
                        result(r, c) += _data[r][k] * other(k, c);
                    }
                }
            }
            return result;
        }

        template <typename U>
        explicit operator Matrix<U, ROWS, COLS>() const
        {
            Matrix<U, ROWS, COLS> result;
            for (int r = 0; r < ROWS; ++r)
            {
                for (int c = 0; c < COLS; ++c)
                {
                    result(r, c) = static_cast<U>(_data[r][c]);
                }
            }
            return result;
        }

    private:

        T _data[ROWS][COLS];
    };

    using Matrix2x2d = Matrix<double, 3, 3>;
    using Matrix2x1d = Matrix<double, 3, 3>;
    
    using Matrix3x3d = Matrix<double, 3, 3>;
    using Matrix3x1d = Matrix<double, 3, 1>;
    using Matrix4x4d = Matrix<double, 4, 4>;
    using Matrix4x1d = Matrix<double, 4, 1>;

    using Matrix3x3f = Matrix<float, 3, 3>;
    using Matrix3x1f = Matrix<float, 3, 1>;
    using Matrix4x4f = Matrix<float, 4, 4>;
    using Matrix4x1f = Matrix<float, 4, 1>;

    using Matrix3x3i = Matrix<int, 3, 3>;
    using Matrix3x1i = Matrix<int, 3, 1>;
    using Matrix4x4i = Matrix<int, 4, 4>;
    using Matrix4x1i = Matrix<int, 4, 1>;
}
