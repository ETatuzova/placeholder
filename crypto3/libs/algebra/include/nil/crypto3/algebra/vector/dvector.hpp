
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

#include <nil/crypto3/algebra/vector/utility.hpp>
#include <nil/crypto3/algebra/vector/vector.hpp>

/** Dynamic size vector class */

namespace nil {
    namespace crypto3 {
        namespace algebra {

            /** @brief A container representing a matrix
             *    @tparam T scalar type to contain
             *
             *    `matrix` is a container representing a matrix.
             *    It is an aggregate type containing a single member array of type
             *    `T[N][M]` which can be initialized with aggregate initialization.
             */
            template<typename T>
            class dvector :public std::vector<T> {
            public:
                using value_type = T;
                using size_type = std::size_t;

                // Constructor
                dvector() : std::vector<T>() {}
                dvector(size_type N) : std::vector<T>(N) {}
                dvector(size_type N, const T &value) : std::vector<T>(N, value) {}
                dvector(std::initializer_list<T> init) : std::vector<T>(init) {}

                // Addition operator
                dvector<T> operator+(const dvector<T> &other) const {
                    assert (this->size() == other.size());
                    dvector<T> result(this->size());
                    std::transform(
                        this->begin(), this->end(),
                        other.begin(),
                        result.begin(),
                        [](const T &a, const T &b) { return a + b; }
                    );
                    return result;
                }

                // Subtraction operator
                dvector<T> operator-(const dvector<T> &other) const {
                    assert (this->size() == other.size());
                    dvector<T> result(this->size());
                    std::transform(
                        this->begin(), this->end(),
                        other.begin(),
                        result.begin(),
                        [](const T &a, const T &b) { return a - b; }
                    );
                    return result;
                }

                dvector<T> operator*(const T &scalar) const {
                    dvector<T> result(this->size());
                    std::transform(
                        this->begin(), this->end(),
                        result.begin(),
                        [scalar](const T &a) { return a * scalar; }
                    );
                    return result;
                }
            };
        }    // namespace algebra
    }        // namespace crypto3
}    // namespace nil