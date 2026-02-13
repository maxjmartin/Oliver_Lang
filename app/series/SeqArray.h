#pragma once

/*****************************************************************************************/
//
//                           Copyright(C) 2024 Max J Martin
//
//                            This file is part of Oliver.
//                      Oliver is program language interpreter. 
//    
//        This program is free software : you can redistribute it and /or modify
//        it under the terms of the GNU Affero General Public License as published by
//        the Free Software Foundation, either version 3 of the License, or
//        (at your option) any later version.
//    
//        This program is distributed in the hope that it will be useful,
//        but WITHOUT ANY WARRANTY; without even the implied warranty of
//        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
//        GNU Affero General Public License for more details.
//    
//        You should have received a copy of the GNU Affero General Public License
//        along with this program.If not, see <https://www.gnu.org/licenses/>.
//    
//        The author can be reached at: maxjmartin@gmail.com
//
/*****************************************************************************************/

#include <algorithm>
#include <array>
#include <compare>
#include <initializer_list>
#include <limits>
#include <numeric>
#include <ranges>
#include <stdexcept>
#include <type_traits>

#include "Expression_Template.h"
#include <ostream>

namespace Oliver {

    /********************************************************************************************/
    //
    //                                    'SeqArray' class
    //          
    //          The SeqArray, is an expression templated wrapper class intended wrapping
    //          a std::araay.  Another specialized class is available for the std::array.
    //          the two classes are meant to be able to interface together seamlessly.  
    // 
    //          The original inspiration for the class was an from the book "C++ Templates:
    //          The Complete Guide" by David Vandevoorde, Nicolai M. and Douglas Gregor.
    //
    //          The material was supplemented by the Stack Overflow link: 
    //          https://stackoverflow.com/questions/11809052/expression-templates-and-c11.
    //          which heavily influenced the design of the class due to the readability
    //          of the solutions defined within the post.  
    //
    /********************************************************************************************/

    template<std::size_t SIZE, typename VALUE>
    class SeqArray : public std::ranges::view_interface<SeqArray<SIZE, VALUE>> {

    public:
        using impl_type              = std::array<VALUE, (SIZE > 0 ? SIZE : 1)>;
        using value_type             = typename impl_type::value_type;
        using size_type              = typename impl_type::size_type;
        using iterator               = typename impl_type::iterator;
        using const_iterator         = typename impl_type::const_iterator;
        using reverse_iterator       = typename impl_type::reverse_iterator;
        using const_reverse_iterator = typename impl_type::const_reverse_iterator;

        constexpr SeqArray() noexcept : _sequence() {
        }

        constexpr SeqArray(value_type value) : _sequence() {
            _sequence[0] = value;
        }

        constexpr SeqArray(std::initializer_list<value_type> list) : _sequence() {
            if (list.size() == _sequence.size()) {
                std::copy(list.begin(), list.end(), _sequence.begin());
            }
            else if (list.size() < _sequence.size()) {
                std::copy(list.begin(), list.end(), _sequence.begin());
                std::fill(_sequence.begin() + list.size(), _sequence.end(), value_type{ 0 });
            }
            else {
                std::copy(list.begin(), list.begin() + _sequence.size(), _sequence.begin());
            }
        }

        template<typename LE, typename Op, typename RE>
        constexpr SeqArray(ExprTemplate<LE, Op, RE>&& expr) : _sequence{} {
            const auto limit = _sequence.size();
            for (std::size_t i = 0; i < limit; ++i) {
                _sequence[i] = expr[i];
            }
        }

                 ~SeqArray()                                noexcept = default;
        constexpr SeqArray(SeqArray&& arr)                  noexcept = default;
        constexpr SeqArray(const SeqArray& arr)             noexcept = default;
        constexpr SeqArray& operator =(SeqArray&& arr)      noexcept = default;
        constexpr SeqArray& operator =(const SeqArray& arr) noexcept = default;

        static constexpr void swap(SeqArray<SIZE, VALUE>& first, SeqArray<SIZE, VALUE>& second) {
            std::swap(first._sequence, second._sequence);
        }

        constexpr operator bool() const {
            const auto limit = _sequence.size();
            for (std::size_t i = 0; i < limit; ++i) {
                if (static_cast<bool>(operator[](i))) {
                    return true;
                }
            }
            return false;
        }

        constexpr bool operator==(const SeqArray& b) const {
            return operator<=>(b) == std::partial_ordering::equivalent;
        }

        constexpr std::partial_ordering operator<=>(const SeqArray& b) const {
            if (size() > b.size()) {
                return std::partial_ordering::greater;
            }
            if (size() < b.size()) {
                return std::partial_ordering::less;
            }
            return std::partial_ordering::equivalent;
        }

        constexpr const value_type operator [](std::size_t index) const {
            if (SIZE) {
                return index < SIZE ? _sequence[index] : VALUE{};
            }
            return _sequence[0];
        }

        constexpr value_type& operator[](std::size_t index) {
            if (SIZE) {
                if (index >= SIZE) {
                    throw std::out_of_range("Index out of range");
                }
                return _sequence[index];
            }
            return _sequence[0];
        }

        auto begin() noexcept {
            return _sequence.begin();
        }

        auto begin() const noexcept {
            return _sequence.begin();
        }

        auto cbegin() const noexcept {
            return _sequence.cbegin();
        }

        auto end() noexcept {
            return _sequence.end();
        }

        auto end() const noexcept {
            return _sequence.end();
        }

        auto cend() const noexcept {
            return _sequence.cend();
        }

        auto rbegin() noexcept {
            return _sequence.rbegin();
        }

        auto rbegin() const noexcept {
            return _sequence.rbegin();
        }

        auto crbegin() const noexcept {
            return _sequence.crbegin();
        }

        auto rend() noexcept {
            return _sequence.rend();
        }

        auto rend() const noexcept {
            return _sequence.rend();
        }

        auto crend() const noexcept {
            return _sequence.crend();
        }

        constexpr auto view() noexcept {
            return View<impl_type>(_sequence);
        }

        constexpr bool constant() const {
            return SIZE == 0;
        }

        constexpr bool sequence() const {
            return SIZE > 0;
        }

        constexpr std::size_t size() const {
            return _sequence.size();
        }

        constexpr SeqArray<SIZE, VALUE>& shift(int index) {
            if (index > 0) {
                rotate_left_and_drop(static_cast<std::size_t>(index));
            }
            else {
                rotate_right_and_drop(static_cast<std::size_t>(std::abs(index)));
            }
            return *this;
        }

        constexpr SeqArray<SIZE, VALUE>& cshift(int index) {
            if (index > 0) {
                rotate_left(static_cast<std::size_t>(index));
            }
            else {
                rotate_right(static_cast<std::size_t>(std::abs(index)));
            }
            return *this;
        }


        constexpr SeqArray<SIZE, VALUE>& apply(value_type func(value_type)) {
            const auto limit = _sequence.size(); 
            for (std::size_t i = 0; i < limit; ++i) {
                _sequence[i] = func(_sequence[i]);
            }
            return *this;
        }

        constexpr SeqArray<SIZE, VALUE>& apply(value_type func(const value_type&)) {
            const auto limit = _sequence.size();
            for (std::size_t i = 0; i < limit; ++i) {
                _sequence[i] = func(_sequence[i]);
            }
            return *this;
        }

        constexpr SeqArray<SIZE, VALUE>& apply(const SeqArray<SIZE, VALUE>& b, value_type func(value_type, value_type)) {
            const auto limit = _sequence.size();
            for (std::size_t i = 0; i < limit; ++i) {
                _sequence[i] = func(_sequence[i], b[i]);
            }
            return *this;
        }

        constexpr SeqArray<SIZE, VALUE>& apply(const SeqArray<SIZE, VALUE>& b, value_type func(const value_type&, value_type)) {
            const auto limit = _sequence.size();
            for (std::size_t i = 0; i < limit; ++i) {
                _sequence[i] = func(_sequence[i], b[i]);
            }
            return *this;
        }

        constexpr SeqArray<SIZE, VALUE>& apply(const SeqArray<SIZE, VALUE>& b, value_type func(value_type, const value_type&)) {
            const auto limit = _sequence.size();
            for (std::size_t i = 0; i < limit; ++i) {
                _sequence[i] = func(_sequence[i], b[i]);
            }
            return *this;
        }

        constexpr SeqArray<SIZE, VALUE>& apply(const SeqArray<SIZE, VALUE>& b, value_type func(const value_type& , const value_type&)) {
            const auto limit = _sequence.size();
            for (std::size_t i = 0; i < limit; ++i) {
                _sequence[i] = func(_sequence[i], b._sequence[i]);
            }
            return *this;
        }

        constexpr SeqArray<SIZE, VALUE>& operator++() {
            for (std::size_t i = 0; i < _sequence.size(); ++i) {
                ++_sequence[i];
            }
            return *this;
        }

        constexpr SeqArray<SIZE, VALUE>  operator++(int) {
            SeqArray<SIZE, VALUE> a(*this);
            ++a;
            return a;
        }

        constexpr SeqArray<SIZE, VALUE>& operator--() {
            for (std::size_t i = 0; i < _sequence.size(); ++i) {
                --_sequence[i];
            }
            return *this;
        }

        constexpr SeqArray<SIZE, VALUE> operator--(int) {
            SeqArray<SIZE, VALUE> a(*this);
            --(*this);
            return a;
        }

        constexpr SeqArray<SIZE, VALUE> operator+() const {
            SeqArray<SIZE, VALUE> a = *this;
            const auto limit = a._sequence.size(); 
            for (std::size_t i = 0; i < limit; ++i) {
                a._sequence[i] = +a._sequence[i];
            }
            return a;
        }

        constexpr SeqArray<SIZE, VALUE> operator-() const {
            SeqArray<SIZE, VALUE> a = *this;
            const auto limit = a._sequence.size(); 
            for (std::size_t i = 0; i < limit; ++i) {
                a._sequence[i] = -a._sequence[i];
            }
            return a;
        }

        constexpr SeqArray<SIZE, VALUE> operator~() {
            const auto limit = _sequence.size(); 
            for (std::size_t i = 0; i < limit; ++i) {
                _sequence[i] = ~_sequence[i];
            }
            return *this;
        }

        constexpr SeqArray<SIZE, VALUE>& operator+=(const SeqArray& b) {
            const auto limit = _sequence.size();
            for (std::size_t i = 0; i < limit; ++i) {
                _sequence[i] += b[i];
            }
            return *this;
        }

        constexpr SeqArray<SIZE, VALUE>& operator-=(const SeqArray& b) {
            const auto limit = _sequence.size();
            for (std::size_t i = 0; i < limit; ++i) {
                _sequence[i] -= b[i];
            }
            return *this;
        }

        constexpr SeqArray<SIZE, VALUE>& operator*=(const SeqArray& b) {
            const auto limit = _sequence.size();
            for (std::size_t i = 0; i < limit; ++i) {
                _sequence[i] *= b[i];
            }
            return *this;
        }

        constexpr SeqArray<SIZE, VALUE>& operator/=(const SeqArray& b) {
            const auto limit = _sequence.size();
            for (std::size_t i = 0; i < limit; ++i) {
                if (b[i] == value_type{}) {
                    throw std::domain_error("Division by zero");
                }   
                _sequence[i] /= b[i];
            }
            return *this;
        }

        constexpr SeqArray<SIZE, VALUE>& operator%=(const SeqArray& b) {
            const auto limit = _sequence.size();
            for (std::size_t i = 0; i < limit; ++i) {
                if (b[i] == 0) {
                    throw std::domain_error("Division by zero");
                }   
                _sequence[i] %= b[i];
            }
            return *this;
        }

        constexpr SeqArray<SIZE, VALUE>& operator&=(const SeqArray& b) {
            const auto limit = _sequence.size();
            for (std::size_t i = 0; i < limit; ++i) {
                _sequence[i] &= b[i];
            }
            return *this;
        }

        constexpr SeqArray<SIZE, VALUE>& operator|=(const SeqArray& b) {
            const auto limit = _sequence.size();
            for (std::size_t i = 0; i < limit; ++i) {
                _sequence[i] |= b[i];
            }
            return *this;
        }

        constexpr SeqArray<SIZE, VALUE>& operator^=(const SeqArray& b) {
            const auto limit = _sequence.size();
            for (std::size_t i = 0; i < limit; ++i) {
                _sequence[i] ^= b[i];
            }
            return *this;
        }

        constexpr SeqArray<SIZE, VALUE>& operator<<=(const SeqArray& b) {
            const auto limit = _sequence.size();
            for (std::size_t i = 0; i < limit; ++i) {
                if (b[i] < 0 || b[i] >= std::numeric_limits<value_type>::digits) {
                    throw std::domain_error("Invalid input for left shift.");
                }
                _sequence[i] <<= b[i];
            }
            return *this;
        }

        constexpr SeqArray<SIZE, VALUE>& operator>>=(const SeqArray& b) {
            const auto limit = _sequence.size();
            for (std::size_t i = 0; i < limit; ++i) {
                if (b[i] < 0 || b[i] >= std::numeric_limits<value_type>::digits) {
                    throw std::domain_error("Invalid input for right shift.");
                }
                _sequence[i] >>= b[i];
            }
            return *this;
        }

        template<typename RightExpr>
        constexpr SeqArray<SIZE, VALUE>& operator=(RightExpr&& re) {
            const auto limit = _sequence.size();
            if constexpr (requires { re[0]; }) {
                for (std::size_t i = 0; i < limit; ++i) {
                    _sequence[i] = re[i];
                }
            } else {
                for (std::size_t i = 0; i < limit; ++i) {
                    _sequence[i] = re;
                }
            }
            return *this;
        }

        template<typename RightExpr>
        constexpr SeqArray<SIZE, VALUE>& operator+=(RightExpr&& re) {
            const auto limit = _sequence.size();
            if constexpr (requires { re[0]; }) {
                for (std::size_t i = 0; i < limit; ++i) {
                    _sequence[i] += re[i];
                }
            } else {
                for (std::size_t i = 0; i < limit; ++i) {
                    _sequence[i] += re;
                }
            }
            return *this;
        }

        template<typename RightExpr>
        constexpr SeqArray<SIZE, VALUE>& operator-=(RightExpr&& re) {
            const auto limit = _sequence.size();
            if constexpr (requires { re[0]; }) {
                for (std::size_t i = 0; i < limit; ++i) {
                    _sequence[i] -= re[i];
                }
            } else {
                for (std::size_t i = 0; i < limit; ++i) {
                    _sequence[i] -= re;
                }
            }
            return *this;
        }

        template<typename RightExpr>
        constexpr SeqArray<SIZE, VALUE>& operator*=(RightExpr&& re) {
            const auto limit = _sequence.size();
            if constexpr (requires { re[0]; }) {
                for (std::size_t i = 0; i < limit; ++i) {
                    _sequence[i] *= re[i];
                }
            } else {
                for (std::size_t i = 0; i < limit; ++i) {
                    _sequence[i] *= re;
                }
            }
            return *this;
        }
        template<typename RightExpr>
        constexpr SeqArray<SIZE, VALUE>& operator/=(RightExpr&& re) {
            const auto limit = _sequence.size();
            if constexpr (requires { re[0]; }) {
            for (std::size_t i = 0; i < limit; ++i) {
                if (re[i] == 0) {
                    throw std::domain_error("Division by zero");
                }
                _sequence[i] /= re[i];
            }
            } else {
                if (re == value_type(static_cast<int64_t>(0))) {
                    throw std::domain_error("Division by zero");
                }
                for (std::size_t i = 0; i < limit; ++i) {
                    _sequence[i] /= re;
                }
            }
            return *this;
        }

        template<typename RightExpr>
        constexpr SeqArray<SIZE, VALUE>& operator%=(RightExpr&& re) {
            const auto limit = _sequence.size();
            if constexpr (requires { re[0]; }) {
            for (std::size_t i = 0; i < limit; ++i) {
                if (re[i] == 0) {
                    throw std::domain_error("Division by zero");
                }
                _sequence[i] %= re[i];
            }
            } else {
                if (re == 0) {
                    throw std::domain_error("Division by zero");
                }
                for (std::size_t i = 0; i < limit; ++i) {
                    _sequence[i] %= re;
                }
            }
            return *this;
        }

        template<typename RightExpr>
        constexpr SeqArray<SIZE, VALUE>& operator&=(RightExpr&& re) {
            const auto limit = _sequence.size();
            if constexpr (requires { re[0]; }) {
                for (std::size_t i = 0; i < limit; ++i) {
                    _sequence[i] &= re[i];
                }
            } else {
                for (std::size_t i = 0; i < limit; ++i) {
                    _sequence[i] &= re;
                }
            }
            return *this;
        }

        template<typename RightExpr>
        constexpr SeqArray<SIZE, VALUE>& operator|=(RightExpr&& re) {
            const auto limit = _sequence.size();
            if constexpr (requires { re[0]; }) {
                for (std::size_t i = 0; i < limit; ++i) {
                    _sequence[i] |= re[i];
                }
            } else {
                for (std::size_t i = 0; i < limit; ++i) {
                    _sequence[i] |= re;
                }
            }
            return *this;
        }

        template<typename RightExpr>
        constexpr SeqArray<SIZE, VALUE>& operator^=(RightExpr&& re) {
            const auto limit = _sequence.size();
            if constexpr (requires { re[0]; }) {
                for (std::size_t i = 0; i < limit; ++i) {
                    _sequence[i] ^= re[i];
                }
            } else {
                for (std::size_t i = 0; i < limit; ++i) {
                    _sequence[i] ^= re;
                }
            }
            return *this;
        }

        template<typename RightExpr>
        constexpr SeqArray<SIZE, VALUE>& operator<<=(RightExpr&& re) {
            const auto limit = _sequence.size();
            if constexpr (requires { re[0]; }) {
                for (std::size_t i = 0; i < limit; ++i) {
                    if (re[i] < 0 || re[i] >= std::numeric_limits<value_type>::digits) {
                        throw std::domain_error("Invalid input for left shift.");
                    }
                    _sequence[i] <<= re[i];
                }
            } else {
                if (re < 0 || re >= std::numeric_limits<value_type>::digits) {
                    throw std::domain_error("Invalid input for left shift.");
                }
                for (std::size_t i = 0; i < limit; ++i) {
                    _sequence[i] <<= re;
                }
            }
            return *this;
        }

        template<typename RightExpr>
        constexpr SeqArray<SIZE, VALUE>& operator>>=(RightExpr&& re) {
            const auto limit = _sequence.size();
            if constexpr (requires { re[0]; }) {
                for (std::size_t i = 0; i < limit; ++i) {
                    if (re[i] < 0 || re[i] >= std::numeric_limits<value_type>::digits) {
                        throw std::domain_error("Invalid input for right shift.");
                    }
                    _sequence[i] >>= re[i];
                }
            } else {
                if (re < 0 || re >= std::numeric_limits<value_type>::digits) {
                    throw std::domain_error("Invalid input for right shift.");
                }
                for (std::size_t i = 0; i < limit; ++i) {
                    _sequence[i] >>= re;
                }
            }
            return *this;
        }

        template<typename RightExpr>
        constexpr auto operator+(RightExpr&& re) const -> ExprTemplate<const SeqArray&, Add_Op<value_type>, decltype(std::forward<RightExpr>(re))> {
            return ExprTemplate<const SeqArray&, Add_Op<value_type>, decltype(std::forward<RightExpr>(re))>(*this, std::forward<RightExpr>(re));
        }

        template<typename RightExpr>
        constexpr auto operator-(RightExpr&& re) const -> ExprTemplate<const SeqArray&, Sub_Op<value_type>, decltype(std::forward<RightExpr>(re))> {
            return ExprTemplate<const SeqArray&, Sub_Op<value_type>, decltype(std::forward<RightExpr>(re))>(*this, std::forward<RightExpr>(re));
        }

        template<typename RightExpr>
        constexpr auto operator*(RightExpr&& re) const -> ExprTemplate<const SeqArray&, Mul_Op<value_type>, decltype(std::forward<RightExpr>(re))> {
            return ExprTemplate<const SeqArray&, Mul_Op<value_type>, decltype(std::forward<RightExpr>(re))>(*this, std::forward<RightExpr>(re));
        }

        template<typename RightExpr>
        constexpr auto operator/(RightExpr&& re) const -> ExprTemplate<const SeqArray&, Div_Op<value_type>, decltype(std::forward<RightExpr>(re))> {
            return ExprTemplate<const SeqArray&, Div_Op<value_type>, decltype(std::forward<RightExpr>(re))>(*this, std::forward<RightExpr>(re));
        }

        template<typename RightExpr>
        constexpr auto operator%(RightExpr&& re) const -> ExprTemplate<const SeqArray&, Mod_Op<value_type>, decltype(std::forward<RightExpr>(re))> {
            return ExprTemplate<const SeqArray&, Mod_Op<value_type>, decltype(std::forward<RightExpr>(re))>(*this, std::forward<RightExpr>(re));
        }

        template<typename RightExpr>
        constexpr auto operator&(RightExpr&& re) const -> ExprTemplate<const SeqArray&, And_Op<value_type>, decltype(std::forward<RightExpr>(re))> {
            return ExprTemplate<const SeqArray&, And_Op<value_type>, decltype(std::forward<RightExpr>(re))>(*this, std::forward<RightExpr>(re));
        }

        template<typename RightExpr>
        constexpr auto operator|(RightExpr&& re) const -> ExprTemplate<const SeqArray&, Or_Op<value_type>, decltype(std::forward<RightExpr>(re))> {
            return ExprTemplate<const SeqArray&, Or_Op<value_type>, decltype(std::forward<RightExpr>(re))>(*this, std::forward<RightExpr>(re));
        }

        template<typename RightExpr>
        constexpr auto operator^(RightExpr&& re) const -> ExprTemplate<const SeqArray&, Xor_Op<value_type>, decltype(std::forward<RightExpr>(re))> {
            return ExprTemplate<const SeqArray&, Xor_Op<value_type>, decltype(std::forward<RightExpr>(re))>(*this, std::forward<RightExpr>(re));
        }

        template<typename RightExpr>
        constexpr auto operator<<(RightExpr&& re) const -> ExprTemplate<const SeqArray&, LeftShift_Op<value_type>, decltype(std::forward<RightExpr>(re))> {
            return ExprTemplate<const SeqArray&, LeftShift_Op<value_type>, decltype(std::forward<RightExpr>(re))>(*this, std::forward<RightExpr>(re));
        }

        template<typename RightExpr>
        constexpr auto operator>>(RightExpr&& re) const -> ExprTemplate<const SeqArray&, RightShift_Op<value_type>, decltype(std::forward<RightExpr>(re))> {
            return ExprTemplate<const SeqArray&, RightShift_Op<value_type>, decltype(std::forward<RightExpr>(re))>(*this, std::forward<RightExpr>(re));
        }
        
        constexpr SeqArray<SIZE, VALUE>& abs() {
            for (std::size_t i = 0, limit = size(); i < limit; ++i) {
                _sequence[i] = std::abs(_sequence[i]);
            }
            return *this;
        }

        static constexpr SeqArray<SIZE, VALUE> abs(SeqArray<SIZE, VALUE> a) {
            return a.abs();
        }

        constexpr value_type sum() const {
            return std::accumulate(cbegin(), cend(), value_type{});
        }

        static constexpr value_type sum(const SeqArray<SIZE, VALUE>& a) {
            return std::accumulate(a.cbegin(), a.cend(), value_type{});
        }

        constexpr value_type max_val() const {
            value_type result = 0;
            for (std::size_t i = 0, limit = _sequence.size(); i < limit; ++i) {
                if (_sequence[i] > result) {
                    result = _sequence[i];
                }
            }
            return result;
        }

        static constexpr value_type max_val(const SeqArray<SIZE, VALUE>& a) {
            return a.max_val();
        }

        constexpr value_type min_val() const {
            value_type result = 0;
            for (std::size_t i = 0, limit = _sequence.size(); i < limit; ++i) {
                if (_sequence[i] < result) {
                    result = _sequence[i];
                }
            }
            return result;
        }

        static constexpr value_type min_val(const SeqArray<SIZE, VALUE>& a) noexcept {
            return a.min_val();
        }

        constexpr SeqArray<SIZE, VALUE>& exp() {
            for (std::size_t i = 0, limit = size(); i < limit; ++i) {
                _sequence[i] = std::exp(_sequence[i]);
            }
            return *this;
        }

        static constexpr SeqArray<SIZE, VALUE> exp(SeqArray<SIZE, VALUE> a) {
            return a.exp();
        }

        constexpr SeqArray<SIZE, VALUE>& log() {
            for (std::size_t i = 0, limit = size(); i < limit; ++i) {
                if (_sequence[i] <= 0) {
                    throw std::domain_error("Logarithm of non-positive value.");
                }
                _sequence[i] = std::log(_sequence[i]);
            }
            return *this;
        }

        static constexpr SeqArray<SIZE, VALUE> log(SeqArray<SIZE, VALUE> a) {
            return a.log();
        }

        constexpr SeqArray<SIZE, VALUE>& log10() {
            for (std::size_t i = 0, limit = size(); i < limit; ++i) {
                if (_sequence[i] <= 0) {
                    throw std::domain_error("Logarithm of non-positive value.");
                }
                _sequence[i] = std::log10(_sequence[i]);
            }
            return *this;
        }

        static constexpr SeqArray<SIZE, VALUE> log10(SeqArray<SIZE, VALUE> a) {
            return a.log10();
        }

        constexpr SeqArray<SIZE, VALUE>& pow() {
            for (std::size_t i = 0, limit = size(); i < limit; ++i) {
                _sequence[i] = std::pow(_sequence[i]);
            }
            return *this;
        }

        static constexpr SeqArray<SIZE, VALUE> pow(SeqArray<SIZE, VALUE> a) {
            return a.pow();
        }

        constexpr SeqArray<SIZE, VALUE>& sqrt() {
            for (std::size_t i = 0, limit = size(); i < limit; ++i) {
                if (_sequence[i] < 0) {
                    throw std::domain_error("Square root of negative value.");
                }
                _sequence[i] = std::sqrt(_sequence[i]);
            }
            return *this;
        }

        static constexpr SeqArray<SIZE, VALUE> sqrt(SeqArray<SIZE, VALUE> a) {
            return a.sqrt();
        }

        constexpr SeqArray<SIZE, VALUE>& sin() {
            for (std::size_t i = 0, limit = size(); i < limit; ++i) {
                _sequence[i] = std::sin(_sequence[i]);
            }
            return *this;
        }

        static constexpr SeqArray<SIZE, VALUE> sin(SeqArray<SIZE, VALUE> a) {
            return a.sin();
        }

        constexpr SeqArray<SIZE, VALUE>& cos() {
            for (std::size_t i = 0, limit = size(); i < limit; ++i) {
                _sequence[i] = std::cos(_sequence[i]);
            }
            return *this;
        }

        static constexpr SeqArray<SIZE, VALUE> cos(SeqArray<SIZE, VALUE> a) {
            return a.cos();
        }

        constexpr SeqArray<SIZE, VALUE>& tan() {
            for (std::size_t i = 0, limit = size(); i < limit; ++i) {
                _sequence[i] = std::tan(_sequence[i]);
            }
            return *this;
        }

        static constexpr SeqArray<SIZE, VALUE> tan(SeqArray<SIZE, VALUE> a) {
            return a.tan();
        }

        constexpr SeqArray<SIZE, VALUE>& asin() {
            for (std::size_t i = 0, limit = size(); i < limit; ++i) {
                _sequence[i] = std::asin(_sequence[i]);
            }
            return *this;
        }

        static constexpr SeqArray<SIZE, VALUE> asin(SeqArray<SIZE, VALUE> a) {
            return a.sin();
        }

        constexpr SeqArray<SIZE, VALUE>& acos() {
            for (std::size_t i = 0, limit = size(); i < limit; ++i) {
                _sequence[i] = std::acos(_sequence[i]);
            }
            return *this;
        }

        static constexpr SeqArray<SIZE, VALUE> acos(SeqArray<SIZE, VALUE> a) {
            return a.acos();
        }

        constexpr SeqArray<SIZE, VALUE>& atan() {
            for (std::size_t i = 0, limit = size(); i < limit; ++i) {
                _sequence[i] = std::atan(_sequence[i]);
            }
            return *this;
        }

        static constexpr SeqArray<SIZE, VALUE> atan(SeqArray<SIZE, VALUE> a) {
            return a.atan();
        }

        constexpr SeqArray<SIZE, VALUE>& atan2() {
            for (std::size_t i = 0, limit = size(); i < limit; ++i) {
                _sequence[i] = std::atan2(_sequence[i]);
            }
            return *this;
        }

        static constexpr SeqArray<SIZE, VALUE> atan2(SeqArray<SIZE, VALUE> a) {
            return a.atan2();
        }

        constexpr SeqArray<SIZE, VALUE>& sinh() {
            for (std::size_t i = 0, limit = size(); i < limit; ++i) {
                _sequence[i] = std::sinh(_sequence[i]);
            }
            return *this;
        }

        static constexpr SeqArray<SIZE, VALUE> sinh(SeqArray<SIZE, VALUE> a) {
            for (std::size_t i = 0, limit = a.size(); i < limit; ++i) {
                a[i] = std::sinh(a[i]);
            }
            return a;
        }

        constexpr SeqArray<SIZE, VALUE>& cosh() {
            for (std::size_t i = 0, limit = size(); i < limit; ++i) {
                _sequence[i] = std::cosh(_sequence[i]);
            }
            return *this;
        }

        static constexpr SeqArray<SIZE, VALUE> cosh(SeqArray<SIZE, VALUE> a) {
            return a.cosh();
        }

        constexpr SeqArray<SIZE, VALUE>& tanh() {
            for (std::size_t i = 0, limit = size(); i < limit; ++i) {
                _sequence[i] = std::tanh(_sequence[i]);
            }
            return *this;
        }

        static constexpr SeqArray<SIZE, VALUE> tanh(SeqArray<SIZE, VALUE> a) {
            return a.tanh();
        }

    private:
        impl_type _sequence = {0};

        constexpr SeqArray<SIZE, VALUE>& rotate_left(std::size_t shift) {
            if (shift) {
                std::ranges::rotate(_sequence, std::next(_sequence.begin(), -static_cast<typename std::vector<VALUE>::difference_type>(shift)));
            }
            return *this;
        }

        constexpr SeqArray<SIZE, VALUE>& rotate_left_and_drop(std::size_t shift) {
            rotate_left(shift);
            if (_sequence.size() > 0) {
                shift %= _sequence.size();
                for (std::size_t i = 0; i < shift; ++i) {
                    _sequence[i] = value_type{ 0 };
                }
            }
            return *this;
        }

        constexpr SeqArray<SIZE, VALUE>& rotate_right(std::size_t shift) {
            if (_sequence.size() > 0) {
                shift %= _sequence.size();
                std::ranges::rotate(_sequence, std::next(_sequence.begin(), static_cast<typename std::vector<VALUE>::difference_type>(shift)));
            }
            return *this;
        }

        constexpr SeqArray<SIZE, VALUE>& rotate_right_and_drop(std::size_t shift) {
            rotate_right(shift);
            if (_sequence.size() > 0) {
                shift %= _sequence.size();
                const auto limit = _sequence.size() - shift;
                auto i = _sequence.size();
                while (i-- > limit) {
                    _sequence[i] = 0;
                }
            }
            return *this;
        }

        template<class T>
        class View : public std::ranges::view_interface<View<T>> {
        public:
            constexpr View() = default;
            constexpr View(const auto& seq) : v_begin(seq.cbegin()), v_end(seq.cend()) {}

            constexpr auto begin() const { return v_begin; }
            constexpr auto   end() const { return v_end;   }

        private:
            typename impl_type::const_iterator v_begin{}, v_end{};
        };
    };

    /*****************************************************************************************/
    //
    //                               Scalar Type Definition
    //
    /*****************************************************************************************/

    template <typename VALUE>
    using scalar = SeqArray<0, VALUE>;
}
