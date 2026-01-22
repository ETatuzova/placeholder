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

#include <nil/crypto3/zk/snark/arithmetization/r1cs/r1cs.hpp>


BOOST_GLOBAL_FIXTURE(ExtendedLogFixture);
BOOST_AUTO_TEST_SUITE(r1cs_test_suite)
    using FieldType = typename nil::crypto3::algebra::curves::alt_bn128_254::scalar_field_type;
    using value = typename FieldType::value_type;
    using constraints_container = typename nil::crypto3::zk::r1cs::r1cs_constraint_system<FieldType>::constraints_container_type;
    using constraint_type = typename nil::crypto3::zk::r1cs::r1cs_constraint_system<FieldType>::r1cs_constraint;
    using constraint_system_type = typename nil::crypto3::zk::r1cs::r1cs_constraint_system<FieldType>;

BOOST_AUTO_TEST_CASE(basic_test) {
    // (1 + x1) = x2
    constraint_type constraint;
    constraint.A[0] = 1;
    constraint.A[1] = 1;
    constraint.B[0] = 1;
    constraint.C[2] = 1;

    // (1 + x1) * (1 + x2) = x3
    constraint_type constraint1;
    constraint1.A[0] = 1;
    constraint1.A[1] = 1;
    constraint1.B[0] = 1;
    constraint1.B[2] = 1;
    constraint1.C[3] = 1;

    // 2 * 1 = 1
    constraint_type constraint2;
    constraint2.A[0] = 2;
    constraint2.B[0] = value(1) / value(2);
    constraint2.C[0] = 1;

    constraint_system_type r1cs_system({constraint, constraint1, constraint2}, 3);
    BOOST_CHECK(r1cs_system.satisfiability_check({0, 1, 2}));
    BOOST_CHECK(r1cs_system.satisfiability_check({1, 2, 6}));
    BOOST_CHECK(!r1cs_system.satisfiability_check({1, 2, 5}));
}

BOOST_AUTO_TEST_CASE(quadratic_symmetry_test) {
    // symmetric constraint: (3 +x1) * (2 + x1) = x_1
    constraint_type symmetric_constraint;
    symmetric_constraint.A[0] = 3;
    symmetric_constraint.A[1] = 1;
    symmetric_constraint.B[0] = 2;
    symmetric_constraint.B[1] = 1;
    symmetric_constraint.C[2] = 1;
    BOOST_CHECK(symmetric_constraint.is_quadratically_symmetric());

    // non-symmetric constraint: (1 + 3 * x1) * (1 + 2 * x1) = x3
    constraint_type non_symmetric_constraint;
    non_symmetric_constraint.A[0] = 1;
    non_symmetric_constraint.A[1] = 3;
    non_symmetric_constraint.B[0] = 1;
    non_symmetric_constraint.B[1] = 2;
    non_symmetric_constraint.C[3] = 1;
    BOOST_CHECK(!non_symmetric_constraint.is_quadratically_symmetric());
}
BOOST_AUTO_TEST_SUITE_END()