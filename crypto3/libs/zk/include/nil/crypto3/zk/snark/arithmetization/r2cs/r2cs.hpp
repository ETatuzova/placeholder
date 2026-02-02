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
#include <set>

#include <nil/crypto3/algebra/matrix/dmatrix.hpp>

/** Rank 2 constraint system classes. <A,x>*<B,x> = <C,x>*<D,x> */

namespace nil::crypto3::zk::r2cs {

    template<typename FieldType>
    struct r2cs_constraint_system {
        using field_type = FieldType;
        using value_type = typename FieldType::value_type;
        using compact_vector_type = std::map<std::size_t, value_type>;

        // 0-th item in A,B,C corresponds to the constant term
        // Standard A,B,C
        // Practical constraints are sparse, so we use map to store them
        struct r2cs_constraint {
            compact_vector_type A;  // Quadratic part variables coefficients
            compact_vector_type B;  // Quadratic part variables coefficients
            compact_vector_type C;  // Linear part variables coefficients
            compact_vector_type D;  // Linear part variables coefficients

            bool is_A_constant() const {
                return (A.size() == 0 ) || (A.size() == 1 && (A.find(0) != A.end()));
            }
            bool is_B_constant() const {
                return (B.size() == 0 ) || (B.size() == 1 && (B.find(0) != B.end()));
            }
            bool is_C_constant() const {
                return (C.size() == 0 ) || (C.size() == 1 && (C.find(0) != C.end()));
            }
            bool is_D_constant() const {
                return (D.size() == 0 ) || (D.size() == 1 && (D.find(0) != D.end()));
            }

            void remove_zeroes(){
                for( auto it = A.cbegin(); it != A.cend(); ) {
                    if( it->second == 0 ){
                        auto to_erase = it++;
                        A.erase(to_erase);
                    } else {
                        ++it;
                    }
                }
                for( auto it = B.cbegin(); it != B.cend(); ) {
                    if( it->second == 0 ){
                        auto to_erase = it++;
                        B.erase(to_erase);
                    } else {
                        ++it;
                    }
                }
                for( auto it = C.cbegin(); it != C.cend(); ) {
                    if( it->second == 0 ){
                        auto to_erase = it++;
                        C.erase(to_erase);
                    } else {
                        ++it;
                    }
                }
                for( auto it = D.cbegin(); it != D.cend(); ) {
                    if( it->second == 0 ){
                        auto to_erase = it++;
                        D.erase(to_erase);
                    } else {
                        ++it;
                    }
                }
            }

            // One part is linear, another is symmetricaly quadratic
            bool is_r1_quadratically_symmetric(std::set<std::size_t> zerified_vars = {}) const {
                // Case 1. A or B are constant.
                if( is_C_constant() || is_D_constant() ){
                    std::set<std::size_t> A_key_set;
                    std::set<std::size_t> B_key_set;

                    std::transform(A.begin(), A.end(),
                    std::inserter(A_key_set, A_key_set.begin()),[](const auto& pair){ return pair.first; }); // Lambda function to get the key

                    std::transform(B.begin(), B.end(),
                    std::inserter(B_key_set, B_key_set.begin()),[](const auto& pair){ return pair.first; }); // Lambda function to get the key

                    A_key_set.erase(0); // Remove constant term
                    B_key_set.erase(0); // Remove constant term
                    for( auto v: zerified_vars ){
                        A_key_set.erase(v); // Remove zerofied vars
                        B_key_set.erase(v); //
                    }

                    if( A_key_set != B_key_set ){
                        return false;
                    }

                    for(auto key: A_key_set){
                        if( A.at(key) != B.at(key) ){
                            return false;
                        }
                    }
                    return true;
                }

                if( is_A_constant() || is_B_constant() ){
                    std::set<std::size_t> C_key_set;
                    std::set<std::size_t> D_key_set;

                    std::transform(C.begin(), C.end(),
                    std::inserter(C_key_set, C_key_set.begin()),[](const auto& pair){ return pair.first; }); // Lambda function to get the key

                    std::transform(D.begin(), D.end(),
                    std::inserter(D_key_set, D_key_set.begin()),[](const auto& pair){ return pair.first; }); // Lambda function to get the key

                    C_key_set.erase(0); // Remove constant term
                    D_key_set.erase(0); // Remove constant term

                    if( C_key_set != D_key_set ){
                        return false;
                    }

                    for(auto key: C_key_set){
                        if( C.at(key) != D.at(key) ){
                            return false;
                        }
                    }
                    return true;
                }

                return false;
            }
        };

        using constraints_container_type = std::vector<r2cs_constraint>;

        r2cs_constraint_system ():
            _num_variables(0),
            _num_constraints(0) {
        }

        std::size_t num_variables() const {
            return _num_variables;
        }

        r2cs_constraint_system (
            const constraints_container_type &constraints,
            std::size_t num_variables
        ):
            _constraints(constraints),
            _num_variables(num_variables),
            _num_constraints(constraints.size())
        {
            for( auto &constraint : _constraints ){
                constraint.remove_zeroes();
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
                for( const auto &item : constraint.D ){
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

                value_type d = constraint.D.contains(0) ? constraint.D.at(0) : 0;
                for( const auto &item : constraint.D ){
                    if( item.first == 0 ) continue;
                    d += assignment[item.first-1] * item.second;
                }

                if( a * b != c * d ){
                    BOOST_LOG_TRIVIAL(debug) << "r2cs constraint " << i << " not satisfied: "
                        << a << " * " << b << " != " << c << " * " << d;
                    return false;
                }
                i++;
            }
            return true;
        }

        bool infinity_solution_check(const std::vector<value_type> &assignment) const {
            assert( assignment.size() >= _num_variables );
            std::size_t i = 0;

            // Variables numeration starts from 1, but in vector it starts from 0
            for( const auto &constraint : _constraints ){
                value_type a = 0;
                for( const auto &item : constraint.A ){
                    if( item.first == 0 ) continue;
                    a += assignment[item.first-1] * item.second;
                }

                value_type b = 0;
                for( const auto &item : constraint.B ){
                    if( item.first == 0 ) continue;
                    b += assignment[item.first-1] * item.second;
                }

                value_type c = 0;
                for( const auto &item : constraint.C ){
                    if( item.first == 0 ) continue;
                    c += assignment[item.first-1] * item.second;
                }

                value_type d = 0;
                for( const auto &item : constraint.D ){
                    if( item.first == 0 ) continue;
                    d += assignment[item.first-1] * item.second;
                }

                if( a*b != c*d ) {
                    BOOST_LOG_TRIVIAL(debug) << "r2cs constraint " << i << " non zero on infinity: "
                        << a << " * " << b << " != 0";
                    return false;
                }
                i++;
            }
            return true;
        }

        // bool projective_safety_symmetric_check() const {
        //     return true;
        // }

        std::set<std::size_t> non_quadratic_variables_list() const {
            std::set<std::size_t> result;
            for( std::size_t i = 1; i < _num_variables + 1; i++ ){
                result.insert(i);
            }
            for( const auto constraint: _constraints ){
                // If A or B has constant terms only, then skip
                if( constraint.is_A_constant() || constraint.is_B_constant() ) continue;
                for( const auto &item_a : constraint.A ){
                    if( item_a.first == 0 ) continue; // Skip constant term
                    if( item_a.second == 0 ) continue;
                    result.erase(item_a.first);
                }
                for( const auto &item_b : constraint.B ){
                    if( item_b.first == 0 ) continue; // Skip constant term
                    if( item_b.second == 0 ) continue;
                    result.erase(item_b.first);
                }
            }
            for( const auto constraint: _constraints ){
                // If C or D has constant terms only, then skip
                if( constraint.is_C_constant() || constraint.is_D_constant() ) continue;
                for( const auto &item_c : constraint.C ){
                    if( item_c.first == 0 ) continue; // Skip constant term
                    if( item_c.second == 0 ) continue;
                    result.erase(item_c.first);
                }
                for( const auto &item_d : constraint.D ){
                    if( item_d.first == 0 ) continue; // Skip constant term
                    if( item_d.second == 0 ) continue;
                    result.erase(item_d.first);
                }
            }
            return result;
        }

        std::set<std::size_t> blessed_variables_list() const {
            std::set<std::size_t> result;
        }

        std::pair<std::vector<std::size_t>, nil::crypto3::algebra::dmatrix<value_type>> get_symmetric_part_matrix(std::set<std::size_t> zerofied_vars = {}) const {
            std::vector<std::size_t> indices;
            std::vector<compact_vector_type> symmetric_constraints;
            for( const auto &constraint: _constraints ){
                compact_vector_type relevant_part = {};
                if(!constraint.is_r1_quadratically_symmetric(zerofied_vars)) continue;
                if( constraint.is_A_constant() || constraint.is_B_constant() ){
                    relevant_part = constraint.C;
                }
                if( constraint.is_C_constant() || constraint.is_D_constant() ){
                    relevant_part = constraint.A;
                }

                // Clean zerofied constraints. May significantly reduce the matrix size
                bool is_empty = true;
                for( const auto &item : relevant_part ){
                    if( item.first == 0 ) continue; // Skip constant term
                    if( zerofied_vars.contains(item.first)) continue;
                    if( item.second != 0 ) {
                        is_empty = false;
                        break;
                    }
                }
                if( is_empty ) continue;
                symmetric_constraints.push_back(relevant_part);

                // Update variables indices
                for( const auto &item : relevant_part ){
                    if( item.first == 0 ) continue; // Skip constant term
                    if(zerofied_vars.contains(item.first)) continue;
                    if( std::find(indices.begin(), indices.end(), item.first) == indices.end() ){
                        if( item.second != 0 ) indices.push_back(item.first);
                    }
                }
            }

            std::size_t N = symmetric_constraints.size();
            std::size_t M = indices.size();
            nil::crypto3::algebra::dmatrix<value_type> A(N, M);
            for( std::size_t i = 0; i < N; i++ ){
                for( const auto [k,v]: symmetric_constraints[i] ){
                    if( k == 0 ) continue;
                    if( v == 0 ) continue;
                    if( zerofied_vars.contains(k) ) continue;
                    A[i][std::distance(indices.begin(), std::find(indices.begin(), indices.end(), k))] = v;
                }
            }

            return std::make_pair(indices, A);
        }

    protected:
        constraints_container_type _constraints;
        std::size_t _num_variables;
        std::size_t _num_constraints;
    };
}