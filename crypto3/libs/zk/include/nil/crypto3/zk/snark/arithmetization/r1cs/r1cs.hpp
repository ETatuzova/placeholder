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

#include <algorithm>
#include <memory>
#include <tuple>
#include <map>

// #include <crypto3/libs/algebra/include/nil/crypto3/algebra/vector/dvector.hpp>

/** R1CS constraint system classes */

namespace nil::crypto3::zk::r1cs {

    template<typename FieldType>
    struct r1cs_constraint_system {
        using field_type = FieldType;
        using value_type = typename FieldType::value_type;
        using compact_vector_type = std::map<std::size_t, value_type>;

        // 0-th item in A,B,C corresponds to the constant term
        // Standard A,B,C
        struct r1cs_constraint {
            compact_vector_type A;  // Quadratic part variables coefficients
            compact_vector_type B;  // Quadratic part variables coefficients
            compact_vector_type C;  // Linear part variables coefficients
        };

        using constraints_container_type = std::vector<r1cs_constraint>;

        r1cs_constraint_system ():
            _num_variables(0),
            _num_constraints(0) {
        }

        r1cs_constraint_system (
            const constraints_container_type &constraints,
            std::size_t num_variables
        ):
            _constraints(constraints),
            _num_variables(num_variables),
            _num_constraints(constraints.size())
        {
            for( const auto &constraint : constraints ){
                for( const auto &item : constraint.A ){
                    if( item.first >= _num_variables ){
                        _num_variables = item.first;
                    }
                }
                for( const auto &item : constraint.B ){
                    if( item.first >= _num_variables ){
                        _num_variables = item.first;
                    }
                }
                for( const auto &item : constraint.C ){
                    if( item.first >= _num_variables ){
                        _num_variables = item.first;
                    }
                }
            }
        }

        // Variables are indexed from 1 to n, 0 is reserved for the constant term
        bool satisfiability_check(const std::vector<value_type> &assignment) const {
            assert( assignment.size() >= _num_variables );
            std::size_t i = 0;

            // Variables numeration starts from 1, but in vector it starts from 0
            for( const auto &constraint : _constraints ){
                value_type a = constraint.A.contains(0) ? constraint.A.at(0) : 0;
                for( const auto &item : constraint.A ){
                    if( item.first == 0 ) continue;
                    a += assignment[item.first-1] * item.second;
                }

                value_type b = constraint.B.contains(0) ? constraint.B.at(0) : 0;
                for( const auto &item : constraint.B ){
                    if( item.first == 0 ) continue;
                    b += assignment[item.first-1] * item.second;
                }

                value_type c = constraint.C.contains(0) ? constraint.C.at(0) : 0;
                for( const auto &item : constraint.C ){
                    if( item.first == 0 ) continue;
                    c += assignment[item.first-1] * item.second;
                }

                if( a * b != c ){
                    BOOST_LOG_TRIVIAL(debug) << "R1CS constraint " << i << " not satisfied: "
                        << a << " * " << b << " != " << c;
                    return false;
                }
                i++;
            }
            return true;
        }

        bool projective_safety_symmetric_check() const {
            return true;
        }

    protected:
        constraints_container_type _constraints;
        std::size_t _num_variables;
        std::size_t _num_constraints;
    };
}