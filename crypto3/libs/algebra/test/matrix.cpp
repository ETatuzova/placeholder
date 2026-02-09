//---------------------------------------------------------------------------//
// Copyright (c) 2020-2021 Mikhail Komarov <nemo@nil.foundation>
// Copyright (c) 2020-2021 Nikita Kaskov <nbering@nil.foundation>
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

#define BOOST_TEST_MODULE matrix_test

#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>

#include <nil/crypto3/algebra/matrix/matrix.hpp>
#include <nil/crypto3/algebra/matrix/dmatrix.hpp>
#include <nil/crypto3/algebra/matrix/math.hpp>
#include <nil/crypto3/algebra/matrix/operators.hpp>
#include <nil/crypto3/algebra/matrix/utility.hpp>
#include <nil/crypto3/algebra/vector/vector.hpp>
#include <nil/crypto3/algebra/vector/operators.hpp>

#include <nil/crypto3/algebra/fields/goldilocks.hpp>

#include <nil/crypto3/test_tools/extended_log_fixture.hpp>

using namespace nil::crypto3::algebra;

using field = fields::goldilocks;
using value = field::value_type;

// Uniform initialization
constexpr matrix<value, 3, 3> m1 = {1, 2, 3, 4, 5, 6, 7, 8, 9};

// Type deduction
constexpr matrix m2 = {{{value(1), value(2)}}};

constexpr matrix m22 = {{{value(1), value(3)}, {value(2), value(7)}}};

static_assert(m1[0][2] == 3, "matrix[]");

static_assert(m1.row(2) == vector {value(7), value(8), value(9)}, "matrix row");

static_assert(m1.column(2) == vector {value(3), value(6), value(9)}, "matrix column");

static_assert(fill<2, 2>(value(3)) == matrix {{{value(3), value(3)}, {value(3), value(3)}}}, "matrix fill");

static_assert(matmul(m1, m1) == matrix<value, 3, 3> {
    {
        { 30,  36,  42},
        { 66,  81,  96},
        {102, 126, 150}
    }
}, "real matrix multiply");

static_assert(identity<value, 3> == matrix<value,3,3> {{{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}}, "identity");

static_assert(identity<value, 3> == inverse(identity<value, 3>), "inverse-identity");

static_assert(inverse(m22) == matrix<value, 2, 2> {
    {
        {7, -3},
        {-2, 1}
    }
}, "inverse");

static_assert(matmul(inverse(m22), matrix<value, 2, 1> {{{1}, {1}}}) == matrix<value, 2, 1> {{{4}, {-1}}}, "A^-1*b = x");

static_assert(horzcat(identity<value, 2>, identity<value, 2>) ==
                  matrix<value, 2, 4> {{{1, 0, 1, 0}, {0, 1, 0, 1}}},
              "horzcat");

static_assert(submat<2, 2>(m1, 1, 1) == matrix<value, 2, 2> {{{5, 6}, {8, 9}}}, "submat");

static_assert(rref(m1) == matrix<value, 3, 3> {{{1, 0, -1}, {0, 1, 2}, {0, 0, 0}}}, "rref");

static_assert(rank(m1) == 2, "rank");


BOOST_GLOBAL_FIXTURE(ExtendedLogFixture);
BOOST_AUTO_TEST_SUITE(matrix_test)
    using matrix_2_2 = matrix<value, 2, 2>;
    using matrix_2_2 = matrix<value, 2, 2>;
BOOST_AUTO_TEST_CASE(equality){
    matrix_2_2 a = {{{1, 2}, {3, 4}}};
    matrix_2_2 b = {{{1, 2}, {3, 4}}};
    matrix_2_2 c = {{{5, 6}, {7, 8}}};
    BOOST_CHECK(a == b);
    BOOST_CHECK(a != c);
}
BOOST_AUTO_TEST_CASE(addition){

    matrix_2_2 a = {{{1, 2}, {3, 4}}};
    matrix_2_2 b = {{{5, 6}, {7, 8}}};
    matrix_2_2 c = a + b;
    matrix_2_2 result = {{{6, 8}, {10, 12}}};
    BOOST_CHECK(c == result);
    BOOST_CHECK(c == matrix_2_2({{{6, 8}, {10, 12}}}));
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(dmatrix_test)
using dynamic_matrix = dmatrix<value>;

BOOST_AUTO_TEST_CASE(construction){
    dynamic_matrix dm(3, 4);
    BOOST_CHECK(dm.column_size == 3);
    BOOST_CHECK(dm.row_size == 4);

    dynamic_matrix dm_init(2, 3, {{ {1, 2, 3}, {4, 5, 6} }});
    BOOST_CHECK(dm_init.column_size == 2);
    BOOST_CHECK(dm_init.row_size == 3);
    BOOST_CHECK(dm_init[0][0] == 1);
    BOOST_CHECK(dm_init[0][1] == 2);
    BOOST_CHECK(dm_init[0][2] == 3);
    BOOST_CHECK(dm_init[1][0] == 4);
    BOOST_CHECK(dm_init[1][1] == 5);
    BOOST_CHECK(dm_init[1][2] == 6);

    auto row = dm_init.row(1);
    BOOST_CHECK(row == dynamic_matrix::row_type({4, 5, 6}));

    auto column = dm_init.column(2);
    BOOST_CHECK(column == dynamic_matrix::row_type({3, 6}));
}

BOOST_AUTO_TEST_CASE(equality){
    dynamic_matrix a(2, 2, {{ {1, 2}, {3, 4} }});
    dynamic_matrix b(2, 2, {{ {1, 2}, {3, 4} }});
    dynamic_matrix c(2, 2, {{ {5, 6}, {7, 8} }});
    BOOST_CHECK(a == b);
    BOOST_CHECK(a != c);
}

BOOST_AUTO_TEST_CASE(addition){
    dynamic_matrix a(2, 3, {{ {1, 2, 3}, {4, 5, 6} }});
    dynamic_matrix b(2, 3, {{ {5, 6, 7}, {8, 9, 10} }});
    dynamic_matrix c = a + b;
    dynamic_matrix result(2, 3, {{ {6, 8, 10}, {12, 14, 16} }});
    BOOST_CHECK(c == result);
}

BOOST_AUTO_TEST_CASE(subtraction){
    dynamic_matrix a(3, 2, {{ {5, 6}, {7, 8}, {9, 10} }});
    dynamic_matrix b(3, 2, {{ {1, 2}, {3, 4}, {5, 6} }});
    dynamic_matrix c = a - b;
    dynamic_matrix result(3, 2, {{ {4, 4}, {4, 4}, {4, 4} }});
    BOOST_CHECK(c == result);
}

BOOST_AUTO_TEST_CASE(multiplication){
    dynamic_matrix a(2, 3, {{ {1, 2, 3}, {4, 5, 6} }});
    dynamic_matrix b(3, 4, {{ {7, 8, 9, 10}, {11, 12, 13, 14}, {15, 16, 17, 18} }});
    dynamic_matrix c = a * b;
    dynamic_matrix result(2, 4, {{ {74, 80, 86, 92}, {173, 188, 203, 218} }});
    BOOST_CHECK(c == result);

    dynamic_matrix d(3, 3, {{ {1, 2, 3}, {0, 1, 4}, {5, 6, 0} }});
    dynamic_matrix e = identity_dmatrix<value>(3);
    dynamic_matrix f = d * e;
    BOOST_CHECK(f == d);
}

BOOST_AUTO_TEST_CASE(determinant){
    dynamic_matrix a(2, 2, {{ {4, 6}, {3, 8} }});
    value det = a.determinant();
    BOOST_CHECK_EQUAL(det, 14);

    // Test determinant of 3x3 matrix
    dynamic_matrix b(3, 3, {{ {6, 1, 1}, {4, -2, 5}, {2, 8, 7} }});
    value det_b = b.determinant();
    BOOST_CHECK_EQUAL(det_b, -306);

    // Test zero determinant
    dynamic_matrix c(2, 2, {{ {1, 2}, {2, 4} }});
    value det_c = c.determinant();
    BOOST_CHECK_EQUAL(det_c, 0);

    dynamic_matrix d(1, 1, {{ {5} }});
    value det_d = d.determinant();
    BOOST_CHECK_EQUAL(det_d, 5);

    dynamic_matrix e(0, 0);
    value det_e = e.determinant();
    BOOST_CHECK_EQUAL(det_e, 1);

    dynamic_matrix id = identity_dmatrix<value>(5);
    value det_id = id.determinant();
    BOOST_CHECK_EQUAL(det_id, 1);
}

BOOST_AUTO_TEST_CASE(rank){
    dynamic_matrix a(3, 3, {{ {1, 2, 3}, {4, 5, 6}, {7, 8, 9} }});
    std::size_t rank_a = a.rank();
    BOOST_CHECK_EQUAL(rank_a, 2);

    dynamic_matrix b(2, 3, {{ {1, 2, 3}, {4, 5, 6} }});
    std::size_t rank_b = b.rank();
    BOOST_CHECK_EQUAL(rank_b, 2);

    dynamic_matrix c(3, 2, {{ {1, 2}, {2, 4}, {3, 6} }});
    std::size_t rank_c = c.rank();
    BOOST_CHECK_EQUAL(rank_c, 1);

    dynamic_matrix d(2, 2, {{ {1, 0}, {0, 1} }});
    std::size_t rank_d = d.rank();
    BOOST_CHECK_EQUAL(rank_d, 2);

    dynamic_matrix e(0, 0);
    std::size_t rank_e = e.rank();
    BOOST_CHECK_EQUAL(rank_e, 0);

    dynamic_matrix f(3, 3, {{ {0, 0, 0}, {0, 0, 0}, {0, 0, 0} }});
    std::size_t rank_f = f.rank();
    BOOST_CHECK_EQUAL(rank_f, 0);

    dynamic_matrix id = identity_dmatrix<value>(10);
    std::size_t rank_id = id.rank();
    BOOST_CHECK_EQUAL(rank_id, 10);

    dynamic_matrix full_rank(3, 5, {
        { -1, 1, 0, 0, 0 },
        { 1, 0, 0, 1, 0 },
        { 2, 0, 0, 0, 1 }
    });
    std::size_t rank_full = full_rank.rank();
    BOOST_CHECK_EQUAL(rank_full, 3);

    dynamic_matrix one_row = row_dmatrix<value>({{1, -2, 1}});
    std::size_t rank_one_row = one_row.rank();
    BOOST_CHECK_EQUAL(rank_one_row, 1);

    // TODO: Fix matrix brace initializer
    dynamic_matrix one_column = column_dmatrix<value>({{1}, {-2}, {1}});
    std::size_t rank_one_column = one_column.rank();
    BOOST_CHECK_EQUAL(rank_one_column, 1);
}

BOOST_AUTO_TEST_CASE(right_kernel_basis){
    dynamic_matrix a(2, 3, {{ {1, 2, 3}, {4, 5, 6} }});
    auto basis_a = a.right_kernel_basis();
    BOOST_CHECK(basis_a.size() == a.columns_amount() - a.rank());
    std::stringstream ss_a;
    ss_a << "Right kernel basis for matrix a: " << std::endl;
    for( auto vec: basis_a ){
        ss_a << "[ ";
        for( auto val: vec ){
            ss_a << val << " ";
        }
        ss_a << "] " << std::endl;
    }
    BOOST_LOG_TRIVIAL(trace) << ss_a.str() << std::endl;
    for( auto vec: basis_a ){
        dynamic_matrix vec_matrix(vec.size(), 1);
        for( std::size_t i = 0; i < vec.size(); ++i ){
            vec_matrix[i][0] = vec[i];
        }
        dynamic_matrix result = a * vec_matrix;
        for( std::size_t i = 0; i < result.column_size; ++i ){
            BOOST_CHECK(result[i][0] == 0);
        }
    }
    //check that the basis vectors are linearly independent
    dynamic_matrix rref_a(basis_a.size(), 3, basis_a);
    BOOST_LOG_TRIVIAL(trace) << "Basis size = " << basis_a.size() << " basis rank = " << rref_a.rank() << std::endl;
    BOOST_CHECK(rref_a.rank() == basis_a.size());

    dynamic_matrix b(2,4, {{ {1, 2, 3, 4}, {5, 6, 7, 8} }});
    auto basis_b = b.right_kernel_basis();
    BOOST_CHECK(basis_b.size() == b.columns_amount() - b.rank());
    std::stringstream ss_b;
    ss_b << "Right kernel basis for matrix b: " << std::endl;
    for( auto vec: basis_b ){
        ss_b << "[ ";
        for( auto val: vec ){
            ss_b << val << " ";
        }
        ss_b << "] " << std::endl;
    }
    BOOST_LOG_TRIVIAL(trace) << ss_b.str() << std::endl;
    dynamic_matrix rref_b(basis_b.size(), 4, basis_b);
    BOOST_CHECK(rref_b.rank() == basis_b.size());

    for( auto vec: basis_b ){
        dynamic_matrix vec_matrix(vec.size(), 1);
        for( std::size_t i = 0; i < vec.size(); ++i ){
            vec_matrix[i][0] = vec[i];
        }
        dynamic_matrix result = b * vec_matrix;
        for( std::size_t i = 0; i < result.column_size; ++i ){
            BOOST_CHECK(result[i][0] == 0);
        }
    }

    dynamic_matrix c(3,5, {{ {1, 2, 3, 4, 5}, {1, 2, 3, 4, 5}, {5, 6, 7, 8, 9} }});
    auto basis_c = c.right_kernel_basis();
    BOOST_CHECK(basis_c.size() == c.columns_amount() - c.rank());
    std::stringstream ss_c;
    ss_c << "Right kernel basis for matrix c: " << std::endl;
    for( auto vec: basis_c ){
        ss_c << "[ ";
        for( auto val: vec ){
            ss_c << val << " ";
        }
        ss_c << "] " << std::endl;
    }
    BOOST_LOG_TRIVIAL(trace) << ss_c.str() << std::endl;
    dynamic_matrix rref_c(basis_c.size(), 4, basis_c);
    BOOST_CHECK(rref_c.rank() == basis_c.size());
    for( auto vec: basis_c ){
        dynamic_matrix vec_matrix(vec.size(), 1);
        for( std::size_t i = 0; i < vec.size(); ++i ){
            vec_matrix[i][0] = vec[i];
        }
        dynamic_matrix result = c * vec_matrix;
        for( std::size_t i = 0; i < result.column_size; ++i ){
            BOOST_CHECK(result[i][0] == 0);
        }
    }

    dynamic_matrix d(3,5, {{1,1,2,3,4}, {1,1,2,3,4}, {5,5,6,7,8}});
    auto basis_d = d.right_kernel_basis();
    BOOST_CHECK(basis_d.size() == d.columns_amount() - d.rank());
    std::stringstream ss_d;
    ss_d << "Right kernel basis for matrix d: " << std::endl;
    for( auto vec: basis_d ){
        ss_d << "[ ";
        for( auto val: vec ){
            ss_d << val << " ";
        }
        ss_d << "] " << std::endl;
    }
    BOOST_LOG_TRIVIAL(trace) << ss_d.str() << std::endl;
    dynamic_matrix rref_d(basis_d.size(), d.columns_amount(), basis_d);
    BOOST_LOG_TRIVIAL(trace) << "Basis size = " << basis_d.size() << " basis rank = " << rref_d.rank() << std::endl;
    BOOST_CHECK(rref_d.rank() == basis_d.size());
    for( auto vec: basis_d ){
        dynamic_matrix vec_matrix(vec.size(), 1);
        for( std::size_t i = 0; i < vec.size(); ++i ){
            vec_matrix[i][0] = vec[i];
        }
        dynamic_matrix result = d * vec_matrix;
        for( std::size_t i = 0; i < result.column_size; ++i ){
            BOOST_CHECK(result[i][0] == 0);
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()