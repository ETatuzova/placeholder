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
// @file Declaration of interfaces for:
// - a variable (i.e., x_i),
// - a linear term (i.e., a_i * x_i), and
// - a linear combination (i.e., sum_i a_i * x_i).
//---------------------------------------------------------------------------//

#ifndef CRYPTO3_ZK_PLONK_VARIABLE_HPP
#define CRYPTO3_ZK_PLONK_VARIABLE_HPP

#include <ostream>
#include <vector>
#include <functional>
#include <map>
#include <boost/functional/hash.hpp>

namespace nil::crypto3::zk::snark {
    /**
     * Forward declaration.
     */
    template<typename VariableType>
    class term;

    /**
     * Forward declaration.
     */
    template<typename VariableType>
    class expression;

    enum plonk_column_type : std::uint8_t {
        witness,
        public_input,
        constant,
        selector,
        uninitialized
    };

    /********************************* Variable **********************************/

    /**
     * A variable represents a formal expression of the form "w^{index}_{rotation}" and type type.
     */
    template<typename AssignmentType>
    class plonk_variable {

    public:
        using assignment_type = AssignmentType;

        using column_type = plonk_column_type;

        /**
         * Mnemonic typedefs.
         */
        std::size_t index;
        std::int32_t rotation;
        bool relative;
        column_type type;

        constexpr plonk_variable() : index(0), rotation(0), relative(false), type(column_type::uninitialized) {}

        constexpr plonk_variable(const std::size_t index,
                                 std::int32_t rotation,
                                 bool relative = true,
                                 column_type type = column_type::witness) :
                index(index),
                rotation(rotation), relative(relative), type(type) {}

        // Allow conversions from any other variable type.
        template<typename AnotherAssignmentType>
        constexpr plonk_variable(const plonk_variable<AnotherAssignmentType>& other)
                : index(other.index)
                , rotation(other.rotation)
                , relative(other.relative)
                , type(static_cast<column_type>(other.type)) {}

        expression<plonk_variable<AssignmentType>> pow(const std::size_t power) const {
            return term<plonk_variable<AssignmentType>>(*this).pow(power);
        }

        term<plonk_variable<AssignmentType>>
        operator*(const assignment_type &field_coeff) const {
            return term<plonk_variable<AssignmentType>>(*this) * field_coeff;
        }

        term<plonk_variable<AssignmentType>> operator*(const plonk_variable &other) const {
            return term<plonk_variable<AssignmentType>>(*this) * other;
        }

        expression<plonk_variable<AssignmentType>>
        operator+(const expression<plonk_variable<AssignmentType>> &other) const {
            expression<plonk_variable<AssignmentType>> result(*this);
            result += other;
            return result;
        }

        expression<plonk_variable<AssignmentType>>
        operator-(const expression<plonk_variable<AssignmentType>> &other) const {
            expression<plonk_variable<AssignmentType>> result(*this);
            result -= other;
            return result;
        }

        term<plonk_variable<AssignmentType>> operator-() const {
            return -term<plonk_variable<AssignmentType>>(*this);
        }

        bool operator==(const plonk_variable &other) const {
            return ((this->index == other.index) && (this->rotation == other.rotation) &&
                    this->type == other.type && this->relative == other.relative);
        }

        bool operator!=(const plonk_variable &other) const {
            return !(*this == other);
        }

        bool operator<(const plonk_variable &other) const {
            if (this->index != other.index)
                return this->index < other.index;
            if (this->rotation != other.rotation)
                return this->rotation < other.rotation;
            if (this->type != other.type)
                return this->type < other.type;
            return this->relative < other.relative;
        }

        bool operator>(const plonk_variable &other) const {
            return *this != other && !(*this < other);
        }
    };

    template<typename AssignmentType, typename LeftType,
            typename = std::enable_if_t<
                    std::is_same<LeftType, AssignmentType>::value || std::is_integral<LeftType>::value>>
    term<plonk_variable<AssignmentType>>
    operator*(const LeftType &field_coeff, const plonk_variable<AssignmentType> &var) {
        return var * field_coeff;
    }

    template<typename AssignmentType, typename LeftType,
            typename = std::enable_if_t<
                    std::is_same<LeftType, AssignmentType>::value || std::is_integral<LeftType>::value>>
    expression<plonk_variable<AssignmentType>>
    operator+(const LeftType &field_val, const plonk_variable<AssignmentType> &var) {
        return var + field_val;
    }

    template<typename AssignmentType, typename LeftType,
            typename = std::enable_if_t<
                    std::is_same<LeftType, AssignmentType>::value || std::is_integral<LeftType>::value>>
    expression<plonk_variable<AssignmentType>>
    operator-(const LeftType &field_val, const plonk_variable<AssignmentType> &var) {
        return -(var - field_val);
    }

    // Used in the unit test, so we can use BOOST_CHECK_EQUALS, and see
    // the values of terms, when the check fails.
    template<typename AssignmentType>
    std::ostream &operator<<(std::ostream &os, const plonk_variable<AssignmentType> &var) {
        std::map<typename plonk_variable<AssignmentType>::column_type, std::string> type_map = {
                {plonk_variable<AssignmentType>::column_type::witness,      "w"},
                {plonk_variable<AssignmentType>::column_type::public_input, "pub"},
                {plonk_variable<AssignmentType>::column_type::constant,     "c"},
                {plonk_variable<AssignmentType>::column_type::selector,     "sel"},
                {plonk_variable<AssignmentType>::column_type::uninitialized,"NaN"}
        };
        os << type_map[var.type] << "_" << var.index;
        if (!var.relative) {
            os << "_abs";
        }
        if (var.rotation != 0) {
            os << "_rot(" << var.rotation << ")";
        }
        return os;
    }

    /**
     * A non-relative variable with no rotation.
     */
    template<typename AssignmentType>
    class plonk_variable_without_rotation {

    public:
        using assignment_type = AssignmentType;

        using column_type = plonk_column_type;

        std::size_t index;
        column_type type;

        constexpr plonk_variable_without_rotation()
            : index(0)
            , type(column_type::uninitialized) {}

        constexpr plonk_variable_without_rotation(std::size_t index, column_type type)
            : index(index)
            , type(type) {}

        plonk_variable_without_rotation(const plonk_variable_without_rotation&) = default;
        plonk_variable_without_rotation(const plonk_variable<AssignmentType>& var)
            : index(var.index)
            , type(static_cast<column_type>(var.type)) {}

        // Allow conversions from any other variable type.
        template<typename AnotherAssignmentType>
        constexpr plonk_variable_without_rotation(
            const plonk_variable_without_rotation<AnotherAssignmentType>& other)
                : index(other.index)
                , type(other.type) {}

        auto operator<=>(plonk_variable_without_rotation const &) const = default;
    };

    // Used in the unit test, so we can use BOOST_CHECK_EQUALS, and see
    // the values of terms, when the check fails.
    template<typename AssignmentType>
    std::ostream &operator<<(std::ostream &os, const plonk_variable_without_rotation<AssignmentType> &var) {
        std::map<typename plonk_variable_without_rotation<AssignmentType>::column_type, std::string> type_map = {
                {plonk_variable_without_rotation<AssignmentType>::column_type::witness,      "w"},
                {plonk_variable_without_rotation<AssignmentType>::column_type::public_input, "pub"},
                {plonk_variable_without_rotation<AssignmentType>::column_type::constant,     "c"},
                {plonk_variable_without_rotation<AssignmentType>::column_type::selector,     "sel"},
                {plonk_variable_without_rotation<AssignmentType>::column_type::uninitialized,"NaN"}
        };
        os << type_map[var.type] << "_" << var.index;
        return os;
    }
} // namespace nil::crypto3::zk::snark

template<typename AssignmentType>
struct std::hash<nil::crypto3::zk::snark::plonk_variable<AssignmentType>> {
    std::size_t operator()(const nil::crypto3::zk::snark::plonk_variable<AssignmentType> &var) const {
        std::size_t result = std::hash<std::int32_t>()(var.rotation);
        boost::hash_combine(result, std::hash<std::int8_t>()(var.type));
        boost::hash_combine(result, std::hash<std::size_t>()(var.index));
        boost::hash_combine(result, std::hash<bool>()(var.relative));
        return result;
    }
};

template<typename AssignmentType>
struct std::hash<nil::crypto3::zk::snark::plonk_variable_without_rotation<AssignmentType>> {
    std::size_t operator()(const nil::crypto3::zk::snark::plonk_variable_without_rotation<AssignmentType> &var) const {
        std::size_t result = std::hash<std::int32_t>()(var.index);
        boost::hash_combine(result, std::hash<std::int8_t>()(var.type));
        return result;
    }
};

#endif    // CRYPTO3_ZK_PLONK_VARIABLE_HPP
