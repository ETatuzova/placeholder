
//---------------------------------------------------------------------------//
// Copyright (c) 2025 Elena Tatuzova <elena@allocinit.xyz>
//
// MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//---------------------------------------------------------------------------//

#pragma once

#include <array>
#include <vector>
#include <tuple>
#include <execution>
#include <set>

#include <nil/crypto3/algebra/vector/utility.hpp>
#include <nil/crypto3/algebra/vector/dvector.hpp>

/** Dynamic size matrix class */

namespace nil::crypto3::algebra {
    /** @brief A container representing a matrix
        *    @tparam T scalar type to contain
        *
        *    `dmatrix` is a container representing a matrix.
        */
    template<typename T>
    class dmatrix: public dvector<dvector<T>> {
    public:
        using value_type = T;
        using size_type = std::size_t;
        using row_type = dvector<T>;
        using data_type = dvector<dvector<T>>;

        size_type column_size;    ///< Number of rows
        size_type row_size;       ///< Number of columns

        // Constructor
        dmatrix(size_type N, size_type M) : column_size(N), row_size(M), dvector<dvector<T>>(N, dvector<T>(M)) {}

        // Constructor with initialization
        dmatrix(std::size_t N, std::size_t M, const data_type &init_data)
            : column_size(N), row_size(M), dvector<dvector<T>>(init_data) {}

        std::size_t rows_amount() const {
            return column_size;
        }

        std::size_t columns_amount() const {
            return row_size;
        }


        dvector<T> row(std::size_t i) const {
            dvector<T> result = (*this)[i];
            return result;
        }

        dvector<T> column(std::size_t j) const{
            dvector<T> result(column_size);
            for (std::size_t i = 0; i < column_size; ++i) {
                result[i] = (*this)[i][j];
            }
            return result;
        }

        dmatrix<T> operator+(const dmatrix<T> &other) const {
            assert (column_size == other.column_size && row_size == other.row_size);
            dmatrix<T> result(column_size, row_size, dvector<dvector<T>>::operator+(other));
            return result;
        }

        dmatrix<T> operator-(const dmatrix<T> &other) const {
            assert (column_size == other.column_size && row_size == other.row_size);
            dmatrix<T> result(column_size, row_size, dvector<dvector<T>>::operator-(other));
            return result;
        }

        dmatrix<T> operator*(const dmatrix<T> &other) const {
            assert (row_size == other.column_size);
            dmatrix<T> result(column_size, other.row_size);
            for (std::size_t i = 0; i < column_size; ++i) {
                for (std::size_t j = 0; j < other.row_size; ++j) {
                    result[i][j] = 0;
                    for (std::size_t k = 0; k < row_size; ++k) {
                        result[i][j] += (*this)[i][k] * other[k][j];
                    }
                }
            }
            return result;
        }

        dvector<T> operator*(const dvector<T> &vec) const {
            assert (row_size == vec.size());
            dvector<T> result(column_size);
            for (std::size_t i = 0; i < column_size; ++i) {
                result[i] = 0;
                for (std::size_t j = 0; j < row_size; ++j) {
                    result[i] += (*this)[i][j] * vec[j];
                }
            }
            return result;
        }

        dmatrix<T> operator*(const T &scalar) const {
            dmatrix<T> result(column_size, row_size);
            for (std::size_t i = 0; i < column_size; ++i) {
                for (std::size_t j = 0; j < row_size; ++j) {
                    result[i][j] = (*this)[i][j] * scalar;
                }
            }
            return result;
        }

        T determinant() const {
            assert (column_size == row_size);
            T det = 1;
            if (column_size == 0) return 1;
            T sign = 1; // Check!

            dvector<dvector<T>> tmp = *this; // Make a copy to perform row operations
            for( std::size_t i = 0; i < column_size; i++ ){
                // Find pivot
                std::size_t pivot = i;
                while( pivot < column_size && tmp[pivot][i] == 0 ) pivot++;
                if( pivot == column_size ) return 0; // Singular matrix

                if( pivot != i ){
                    std::swap( tmp[i], tmp[pivot] );
                    sign = -sign;
                }

                det *= tmp[i][i];
                // Eliminate below
                for( std::size_t j = i + 1; j < column_size; j++ ){
                    T factor = tmp[j][i] / tmp[i][i];
                    for( std::size_t k = i; k < row_size; k++ ){
                        tmp[j][k] -= factor * tmp[i][k];
                    }
                }

                if (sign != 1) det = -det;
            }
            return det;
        }

        std::size_t rank() const {
            dvector<dvector<T>> tmp = *this; // Make a copy to perform row operations
            std::size_t rank = 0;

            std::size_t current_row = 0;
            for (std::size_t i = 0; i < row_size; ++i) {
                // Find pivot
                std::size_t pivot = current_row;
                while (pivot < column_size && tmp[pivot][i] == 0) pivot++;
                if (pivot == column_size) continue; // No pivot in this column

                if (pivot != current_row) {
                    std::swap(tmp[current_row], tmp[pivot]);
                }

                // Normalize current row
                T pivot_value = tmp[current_row][i];
                for (std::size_t k = i; k < row_size; ++k) {
                    tmp[current_row][k] /= pivot_value;
                }

                // Eliminate below
                for (std::size_t j = current_row + 1; j < column_size; ++j) {
                    T factor = tmp[j][i] / tmp[current_row][i];
                    for (std::size_t k = i; k < row_size; ++k) {
                        tmp[j][k] -= factor * tmp[current_row][k];
                    }
                }
                current_row++;
                rank++;
            }

            return rank;
        }

        dvector<dvector<T>> right_kernel_basis(){
            dvector<dvector<T>> tmp = *this; // Make a copy to perform
            std::vector<std::size_t> basis_coords;
            std::size_t row = 0;

            for( std::size_t col = 0; col < row_size && row < column_size; ++col ){
                // Find pivot
                std::size_t pivot = row;
                while( pivot < column_size && tmp[pivot][col] == 0 ) pivot++;
                if( pivot == column_size ) continue; // No pivot in this column

                if( pivot != row ){
                    std::swap( tmp[row], tmp[pivot] );
                }

                // Normalize current
                T pivot_value = tmp[row][col];
                for( std::size_t k = col; k < row_size; ++k ){
                    tmp[row][k] /= pivot_value;
                }

                basis_coords.push_back(col);

                // Eliminate below
                for( std::size_t j = row + 1; j < column_size; ++j ){
                    T factor = tmp[j][col] / tmp[row][col];
                    for( std::size_t k = col; k < row_size; ++k ){
                        tmp[j][k] -= factor * tmp[row][k];
                    }
                }
                row++;
            }

            // Eliminate above to get row echelon form
            for( std::size_t i = row; i > 0; --i ){
                std::size_t current_row = i - 1;
                std::size_t pivot_col = basis_coords[current_row];
                for( std::size_t j = 0; j < current_row; ++j ){
                    T factor = tmp[j][pivot_col] / tmp[current_row][pivot_col];
                    for( std::size_t k = pivot_col; k < row_size; ++k ){
                        tmp[j][k] -= factor * tmp[current_row][k];
                    }
                }
            }

            dvector<dvector<T>> kernel_basis;
            for( std::size_t col = 0; col < row_size; ++col ){
                if( std::find(basis_coords.begin(), basis_coords.end(), col) != basis_coords.end() ) continue; // Skip pivot columns

                dvector<T> basis_vector(row_size);
                basis_vector[col] = 1; // Free variable

                for( std::size_t pivot_row = 0; pivot_row < basis_coords.size(); ++pivot_row ){
                    std::size_t pivot_col = basis_coords[pivot_row];
                    basis_vector[pivot_col] = -tmp[pivot_row][col];
                }
                kernel_basis.push_back(basis_vector);
            }
            return kernel_basis;
        }
    };

    template<typename T>
    dmatrix<T> identity_dmatrix(std::size_t N) {
        dmatrix<T> result(N, N);
        for (std::size_t i = 0; i < N; ++i) {
            result[i][i] = 1;
        }
        return result;
    }

    template<typename T>
    dmatrix<T> identity_dmatrix(std::size_t N, std::size_t M) {
        dmatrix<T> result(N, M);
        std::size_t min_size = std::min(N, M);
        for (std::size_t i = 0; i < min_size; ++i) {
            result[i][i] = 1;
        }
        return result;
    }

    template<typename T>
    dmatrix<T> row_dmatrix(const dvector<T> &vec) {
        dmatrix<T> result(1, vec.size());
        result[0] = vec;
        return result;
    }

    template<typename T>
    dmatrix<T> column_dmatrix(const dvector<T> &vec) {
        dmatrix<T> result(vec.size(), 1);
        for (std::size_t i = 0; i < vec.size(); ++i) {
            result[i][0] = vec[i];
        }
        return result;
    }
}  // namespace nil::crypto3::algebra