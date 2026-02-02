//---------------------------------------------------------------------------//
// Copyright (c) 2026 Elena Tatuzova <elena@allocinit.xyz>
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

#define BOOST_TEST_MODULE plonk_constraint_test

#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/property_tree/ptree.hpp>

#include <nil/crypto3/test_tools/extended_log_fixture.hpp>

#include <nil/crypto3/algebra/curves/alt_bn128.hpp>
#include <nil/crypto3/algebra/fields/arithmetic_params/alt_bn128.hpp>

#include <nil/crypto3/algebra/random_element.hpp>
#include <nil/crypto3/random/algebraic_engine.hpp>

#include <nil/crypto3/zk/snark/arithmetization/r2cs/r2cs.hpp>


BOOST_GLOBAL_FIXTURE(ExtendedLogFixture);
BOOST_AUTO_TEST_SUITE(r2cs_test_suite)
    using FieldType = typename nil::crypto3::algebra::curves::alt_bn128_254::scalar_field_type;
    using value = typename FieldType::value_type;
    using constraints_container = typename nil::crypto3::zk::r2cs::r2cs_constraint_system<FieldType>::constraints_container_type;
    using constraint_type = typename nil::crypto3::zk::r2cs::r2cs_constraint_system<FieldType>::r2cs_constraint;
    using constraint_system_type = typename nil::crypto3::zk::r2cs::r2cs_constraint_system<FieldType>;

BOOST_AUTO_TEST_CASE(basic_test) {
    // (1 + x1) = x2
    constraint_type constraint;
    constraint.A[0] = 1;
    constraint.A[1] = 1;
    constraint.B[0] = 1;
    constraint.C[2] = 1;
    constraint.D[0] = 1;

    // (1 + x1) * (1 + x2) = x3
    constraint_type constraint1;
    constraint1.A[0] = 1;
    constraint1.A[1] = 1;
    constraint1.B[0] = 1;
    constraint1.B[2] = 1;
    constraint1.C[3] = 1;
    constraint1.D[0] = 1;

    // 2 * 1 = 1
    constraint_type constraint2;
    constraint2.A[0] = 2;
    constraint2.B[0] = value(1) / value(2);
    constraint2.C[0] = 1;
    constraint2.D[0] = 1;

    constraint_system_type r2cs_system({constraint, constraint1, constraint2}, 3);
    BOOST_CHECK(r2cs_system.satisfiability_check({0, 1, 2}));
    BOOST_CHECK(r2cs_system.satisfiability_check({1, 2, 6}));
    BOOST_CHECK(!r2cs_system.satisfiability_check({1, 2, 5}));
}

BOOST_AUTO_TEST_CASE(num_variables_test){
    constraint_type c0, c1;
    c0.A[0] = 1;
    c0.A[1] = 1;
    c0.B[0] = 1;
    c0.B[2] = 2;
    c0.C[2] = 1;
    c0.C[4] = 10;

    c1.A[0] = 1;
    c1.A[2] = 1;
    c1.B[0] = 1;
    c1.B[2] = 1;
    c1.C[3] = 1;
    constraint_system_type r2cs_system({c0, c1}, 3);
    BOOST_CHECK(r2cs_system.num_variables() == 4);

    constraint_type c2;
    c2.A[0] = 1;
    c2.A[5] = 1;
    c2.B[0] = 1;
    c2.B[2] = 1;
    c2.C[3] = 1;
    c2.D[6] = 1;
    constraint_system_type r2cs_system2({c0, c1, c2}, 3);
    BOOST_CHECK(r2cs_system2.num_variables() == 6);
}

BOOST_AUTO_TEST_CASE(quadratic_symmetry_test) {
    // symmetric constraint: (3 +x1) * (2 + x1) = x_1
    constraint_type symmetric_constraint;
    symmetric_constraint.A[0] = 3;
    symmetric_constraint.A[1] = 1;
    symmetric_constraint.B[0] = 2;
    symmetric_constraint.B[1] = 1;
    symmetric_constraint.C[2] = 1;
    symmetric_constraint.D[0] = 1;
    BOOST_CHECK(symmetric_constraint.is_r1_quadratically_symmetric());

    // non-symmetric constraint: (1 + 3 * x1) * (1 + 2 * x1) = x3
    constraint_type non_symmetric_constraint;
    non_symmetric_constraint.A[0] = 1;
    non_symmetric_constraint.A[1] = 3;
    non_symmetric_constraint.B[0] = 1;
    non_symmetric_constraint.B[1] = 2;
    non_symmetric_constraint.C[3] = 1;
    non_symmetric_constraint.D[0] = 1;
    BOOST_CHECK(!non_symmetric_constraint.is_r1_quadratically_symmetric());

    constraint_type c;
    c.A[0] = 1;
    c.A[1] = 2;
    c.B[0] = 3;
    c.B[2] = 2;
    c.D[0] = 1;
    BOOST_CHECK(!c.is_r1_quadratically_symmetric());

    constraint_type rc;
    rc.A[0] = 1;
    rc.B[1] = 2;
    rc.B[2] = 3;
    rc.B[3] = 4;
    rc.C[0] = 5;
    rc.C[3] = 9;
    rc.D[0] = 8;
    rc.D[3] = 9;
    BOOST_CHECK(rc.is_r1_quadratically_symmetric());

}

BOOST_AUTO_TEST_CASE(infinity_solution_test){
    constraint_type c0, c1;
    c0.A[0] = 1;
    c0.A[1] = 1;
    c0.B[0] = 1;
    c0.B[2] = 2;
    c0.C[2] = 1;
    c0.D[0] = 1;

    c1.A[0] = 1;
    c1.A[2] = 1;
    c1.B[0] = 1;
    c1.B[2] = 1;
    c1.C[3] = 1;
    c1.D[0] = 1;
    constraint_system_type r2cs_system({c0, c1}, 3);
    BOOST_CHECK(r2cs_system.infinity_solution_check({0, 0, 0}));
    BOOST_CHECK(r2cs_system.infinity_solution_check({1, 0, 0}));
    BOOST_CHECK(!r2cs_system.infinity_solution_check({0, 1, 0}));
}

BOOST_AUTO_TEST_CASE(non_quadratic_variables_list_test){
    constraint_type c0, c1, c2;
    c0.A[0] = 1;
    c0.A[1] = 1;
    c0.B[0] = 1;
    c0.B[2] = 2;
    c0.C[2] = 1;
    c0.C[4] = 10;
    c0.D[1] = 1;

    c1.A[0] = 1;
    c1.A[2] = 1;
    c1.B[0] = 1;
    c1.B[2] = 1;
    c1.C[3] = 1;
    c1.D[0] = 1;

    c2.A[0] = 1;
    c2.B[5] = -1;
    c2.B[4] = 1;
    c2.C[1] = 1;
    c2.D[0] = 1;

    constraint_system_type r2cs_system({c0, c1}, 5);
    std::set<std::size_t> non_quadratic_vars = r2cs_system.non_quadratic_variables_list();
    for(std::size_t v: non_quadratic_vars){
        BOOST_LOG_TRIVIAL(debug) << "Non quadratic variable: " << v;
    }
    BOOST_CHECK(non_quadratic_vars.size() == 2);
    BOOST_CHECK(non_quadratic_vars.find(1) == non_quadratic_vars.end());
    BOOST_CHECK(non_quadratic_vars.find(2) == non_quadratic_vars.end());
    BOOST_CHECK(non_quadratic_vars.find(3) != non_quadratic_vars.end());
    BOOST_CHECK(non_quadratic_vars.find(4) == non_quadratic_vars.end());
    BOOST_CHECK(non_quadratic_vars.find(5) != non_quadratic_vars.end());

    // Infinity solution is vector with arbitrary values on non-quadratic variables and 0 on quadratic ones
    std::vector<value> inifinity_solution(5);
    for(std::size_t v: non_quadratic_vars){
      inifinity_solution[v - 1] = value::one();
    }
    BOOST_CHECK(r2cs_system.infinity_solution_check(inifinity_solution));
}

BOOST_AUTO_TEST_CASE(symmetric_part_matrix_test){
    constraint_type c0, c1, c2, c3;
    // symmetric
    c0.A[0] = 1;
    c0.A[1] = 1;
    c0.B[0] = 1;
    c0.B[1] = 1;
    c0.C[2] = 1;
    c0.D[0] = 1;

    // non-symmetric
    c1.A[0] = 1;
    c1.A[2] = 1;
    c1.B[0] = 1;
    c1.B[2] = 2;
    c1.C[3] = 1;
    c1.D[0] = 1;

    // symmetric
    c2.A[0] = 2;
    c2.A[3] = 1;
    c2.B[0] = 3;
    c2.B[3] = 1;
    c2.C[4] = 1;
    c2.D[0] = 1;

    c3.A[0] = 1;
    c3.B[1] = 2;
    c3.B[3] = 3;
    c3.C[0] = 5;
    c3.C[4] = 9;
    c3.D[0] = 8;
    c3.D[4] = 9;

    constraint_system_type r2cs_system({c0, c1, c2, c3}, 4);
    auto [indices, A] = r2cs_system.get_symmetric_part_matrix();
    BOOST_CHECK(indices.size() == 3);
    BOOST_CHECK(indices == std::vector<std::size_t>({1, 3, 4}));

    nil::crypto3::algebra::dmatrix<value> expected_A(3, 3, { {1, 0,0}, {0, 1, 0}, {0, 0, 9} });
    BOOST_CHECK(A == expected_A);
}
BOOST_AUTO_TEST_SUITE_END()