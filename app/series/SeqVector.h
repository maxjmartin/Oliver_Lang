#pragma once

/*****************************************************************************************/
//
//                           Copyright(C) 2025 Max J Martin
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

#include <algorithm>           // std::max_element, std::min_element, std::accumulate
#include <compare>             // operator<=> (three-way comparison)
#include <cmath>               // std::abs, std::sqrt
#include <initializer_list>    // std::initializer_list
#include <mdspan>              // std::mdspan, std::extents
#include <numeric>             // std::accumulate
#include <ranges>              // std::ranges::view_interface, std::ranges::rotate
#include <stdexcept>           // std::domain_error
#include <type_traits>         // std::forward
#include <valarray>            // std::valarray
#include <vector>              // std::vector
#include <concepts>            // std::integral, std::floating_point
#include <iterator>            // std::input_iterator
#include <span>                // std::span
#include <cassert>             // assert

#include "Expression_Template.h"

namespace Oliver {

    /********************************************************************************************/
    //
    //                                    'SeqVector' class
    //          
    //          The SeqVector, is an expression templated wrapper class intended wrapping
    //          a std::vector.   
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

    template<typename VALUE>
    class SeqVector : public std::ranges::view_interface<SeqVector<VALUE>> {

    public:
        using impl_type              = std::vector<VALUE>;
        using value_type             = typename std::vector<VALUE>::value_type;
        using allocator_type         = typename std::vector<VALUE>::allocator_type;
        using size_type              = typename std::vector<VALUE>::size_type;
        using difference_type        = typename std::vector<VALUE>::difference_type;
        using reference              = typename std::vector<VALUE>::reference;
        using const_reference        = typename std::vector<VALUE>::const_reference;
        using pointer                = typename std::vector<VALUE>::pointer;
        using const_pointer          = typename std::vector<VALUE>::const_pointer;
        using iterator               = typename std::vector<VALUE>::iterator;
        using const_iterator         = typename std::vector<VALUE>::const_iterator;
        using reverse_iterator       = typename std::vector<VALUE>::reverse_iterator;
        using const_reverse_iterator = typename std::vector<VALUE>::const_reverse_iterator;

        /*****************************************************************************************/
        //
        //                         Constructors matching std::vector
        //
        /*****************************************************************************************/

        constexpr SeqVector() noexcept(noexcept(allocator_type())) : _sequence() {
        }

        constexpr explicit SeqVector(const allocator_type& alloc) noexcept : _sequence(alloc) {
        }

        constexpr SeqVector(size_type count, const value_type& value, const allocator_type& alloc = allocator_type()) 
            : _sequence(count, value, alloc) {
        }

        constexpr explicit SeqVector(size_type count, const allocator_type& alloc = allocator_type()) 
            : _sequence(count, alloc) {
        }

        template<class InputIt>
        requires std::input_iterator<InputIt>
        constexpr SeqVector(InputIt first, InputIt last, const allocator_type& alloc = allocator_type())
             : _sequence(first, last, alloc) {
        }

        constexpr SeqVector(const SeqVector& other) : _sequence(other._sequence) {
        }

        constexpr SeqVector(const SeqVector& other, const allocator_type& alloc) 
            : _sequence(other._sequence, alloc) {
        }

        constexpr SeqVector(SeqVector&& other) noexcept : _sequence(std::move(other._sequence)) {
        }

        constexpr SeqVector(SeqVector&& other, const allocator_type& alloc) 
            : _sequence(std::move(other._sequence), alloc) {
        }

        constexpr SeqVector(std::initializer_list<value_type> init, const allocator_type& alloc = allocator_type()) 
            : _sequence(init, alloc) {
        }

        /*****************************************************************************************/
        //
        //                         Additional constructors for compatibility
        //
        /*****************************************************************************************/

        constexpr SeqVector(const std::valarray<value_type>& val) : _sequence(val.size(), value_type{}) {
            const auto limit = val.size();
            for (std::size_t i = 0; i < limit; ++i) {
                _sequence[i] = val[i];
            }
        }

        constexpr SeqVector(std::vector<VALUE>&& vec) : _sequence(std::move(vec)) {
        }

        constexpr SeqVector(const std::vector<VALUE>& vec) : _sequence(vec) {
        }

        template<typename LE, typename Op, typename RE>
        constexpr SeqVector(ExprTemplate<LE, Op, RE>&& expr) : _sequence(expr.size(), value_type{}) {
            const auto limit = _sequence.size();
            for (std::size_t i = 0; i < limit; ++i) {
                _sequence[i] = expr[i];
            }
        }

        /*****************************************************************************************/
        //
        //                         Destructor and Assignment Operators
        //
        /*****************************************************************************************/

        ~SeqVector() = default;

        constexpr SeqVector& operator=(const SeqVector& other) {
            if (this != &other) {
                _sequence = other._sequence; // direct assign, avoids extra alloc compared to copy-and-swap
            }
            return *this;
        }

        constexpr SeqVector& operator=(SeqVector&& other) noexcept(
            std::allocator_traits<allocator_type>::propagate_on_container_move_assignment::value ||
            std::allocator_traits<allocator_type>::is_always_equal::value) {
            if (this != &other) {
                _sequence = std::move(other._sequence);
            }
            return *this;
        }

        constexpr SeqVector& operator=(std::initializer_list<value_type> ilist) {
            _sequence = ilist; 
            return *this;
        }

        constexpr SeqVector& operator=(std::vector<VALUE>&& vec) {
            _sequence = std::move(vec);
            return *this;
        }

        constexpr SeqVector& operator=(const std::vector<VALUE>& vec) {
            _sequence = vec;
            return *this;
        }

        /*****************************************************************************************/
        //
        //                                Conversion Operators
        //
        /*****************************************************************************************/

        [[nodiscard]] constexpr std::span<value_type> as_span() noexcept {
            return std::span<value_type>(_sequence.data(), _sequence.size());
        }

        [[nodiscard]] constexpr std::span<const value_type> as_span() const noexcept {
            return std::span<const value_type>(_sequence.data(), _sequence.size());
        }

        [[nodiscard]] constexpr operator std::vector<VALUE>&() {
            return _sequence;
        }

        [[nodiscard]] constexpr operator const std::vector<VALUE>&() const {
            return _sequence;
        }

        [[nodiscard]] constexpr std::vector<VALUE>& get_vector() {
            return _sequence;
        }

        [[nodiscard]] constexpr const std::vector<VALUE>& get_vector() const {
            return _sequence;
        }

        /*****************************************************************************************/
        //
        //                               Element and Allocator Access
        //
        /*****************************************************************************************/

        constexpr allocator_type get_allocator() const noexcept {
            return _sequence.get_allocator();
        }

        [[nodiscard]] constexpr reference at(size_type pos) {
            return _sequence.at(pos);
        }

        [[nodiscard]] constexpr const_reference at(size_type pos) const {
            return _sequence.at(pos);
        }

        [[nodiscard]] constexpr reference operator[](size_type pos) noexcept {
            return _sequence[pos];
        }

        [[nodiscard]] constexpr const_reference operator[](size_type pos) const noexcept {
            return _sequence[pos];
        }

        [[nodiscard]] constexpr reference front() {
            return _sequence.front();
        }

        [[nodiscard]] constexpr const_reference front() const {
            return _sequence.front();
        }

        [[nodiscard]] constexpr reference back() {
            return _sequence.back();
        }

        [[nodiscard]] constexpr const_reference back() const {
            return _sequence.back();
        }

        [[nodiscard]] constexpr value_type* data() noexcept {
            return _sequence.data();
        }

        [[nodiscard]] constexpr const value_type* data() const noexcept {
            return _sequence.data();
        }

        /*****************************************************************************************/
        //
        //                                         Iterators
        //
        /*****************************************************************************************/

        constexpr iterator begin() noexcept {
            return _sequence.begin();
        }

        constexpr const_iterator begin() const noexcept {
            return _sequence.begin();
        }

        constexpr const_iterator cbegin() const noexcept {
            return _sequence.cbegin();
        }

        constexpr iterator end() noexcept {
            return _sequence.end();
        }

        constexpr const_iterator end() const noexcept {
            return _sequence.end();
        }

        constexpr const_iterator cend() const noexcept {
            return _sequence.cend();
        }

        constexpr reverse_iterator rbegin() noexcept {
            return _sequence.rbegin();
        }

        constexpr const_reverse_iterator rbegin() const noexcept {
            return _sequence.rbegin();
        }

        constexpr const_reverse_iterator crbegin() const noexcept {
            return _sequence.crbegin();
        }

        constexpr reverse_iterator rend() noexcept {
            return _sequence.rend();
        }

        constexpr const_reverse_iterator rend() const noexcept {
            return _sequence.rend();
        }

        constexpr const_reverse_iterator crend() const noexcept {
            return _sequence.crend();
        }

        /*****************************************************************************************/
        //
        //                                         Capacity
        //
        /*****************************************************************************************/

        [[nodiscard]] constexpr bool empty() const noexcept {
            return _sequence.empty();
        }

        [[nodiscard]] constexpr size_type size() const noexcept {
            return _sequence.size();
        }

        [[nodiscard]] constexpr size_type max_size() const noexcept {
            return _sequence.max_size();
        }

        constexpr void reserve(size_type new_cap) {
            _sequence.reserve(new_cap);
        }

        [[nodiscard]] constexpr size_type capacity() const noexcept {
            return _sequence.capacity();
        }

        constexpr void shrink_to_fit() {
            _sequence.shrink_to_fit();
        }

        /*****************************************************************************************/
        //
        //                                         Modifiers
        //
        /*****************************************************************************************/

        constexpr void clear() noexcept {
            _sequence.clear();
        }

        constexpr iterator insert(const_iterator pos, const value_type& value) {
            return _sequence.insert(pos, value);
        }

        constexpr iterator insert(const_iterator pos, value_type&& value) {
            return _sequence.insert(pos, std::move(value));
        }

        constexpr iterator insert(const_iterator pos, size_type count, const value_type& value) {
            return _sequence.insert(pos, count, value);
        }

        template<class InputIt>
        constexpr iterator insert(const_iterator pos, InputIt first, InputIt last) {
            return _sequence.insert(pos, first, last);
        }

        constexpr iterator insert(const_iterator pos, std::initializer_list<value_type> ilist) {
            return _sequence.insert(pos, ilist);
        }

        constexpr SeqVector<VALUE>& insert(std::size_t at, const value_type& value) {
            return insert(at, SeqVector(std::initializer_list<value_type>{value}));
        }

        constexpr SeqVector<VALUE>& insert(std::size_t at, SeqVector<VALUE>&& range) {
            const auto cur = _sequence.size(); // reduce repeated size() calls
                if (at > cur) {
                _sequence.resize(at);
            }
            _sequence.insert(std::next(_sequence.begin(), static_cast<difference_type>(at)),
                            std::make_move_iterator(range._sequence.begin()), 
                            std::make_move_iterator(range._sequence.end()));
            return *this;
        }

        template<class... Args>
        constexpr iterator emplace(const_iterator pos, Args&&... args) {
            return _sequence.emplace(pos, std::forward<Args>(args)...);
        }

        constexpr iterator erase(const_iterator pos) {
            return _sequence.erase(pos);
        }

        constexpr iterator erase(const_iterator first, const_iterator last) {
            return _sequence.erase(first, last);
        }

        constexpr void push_back(const value_type& value) {
            _sequence.push_back(value);
        }

        constexpr void push_back(value_type&& value) {
            _sequence.push_back(std::move(value));
        }

        template<class... Args>
        constexpr reference emplace_back(Args&&... args) {
            return _sequence.emplace_back(std::forward<Args>(args)...);
        }

        constexpr void pop_back() {
            _sequence.pop_back();
        }

        constexpr void resize(size_type count) {
            _sequence.resize(count);
        }

        constexpr void resize(size_type count, const value_type& value) {
            _sequence.resize(count, value);
        }

        constexpr void swap(SeqVector& other) noexcept (
            std::allocator_traits<allocator_type>::propagate_on_container_swap::value ||
            std::allocator_traits<allocator_type>::is_always_equal::value) {
            _sequence.swap(other._sequence);
        }

        friend constexpr void swap(SeqVector& a, SeqVector& b) noexcept (
            std::allocator_traits<allocator_type>::propagate_on_container_swap::value ||
            std::allocator_traits<allocator_type>::is_always_equal::value) {
            a._sequence.swap(b._sequence);
        }

        /*****************************************************************************************/
        //
        //                               Assignment Operators
        //
        /*****************************************************************************************/

        constexpr void assign(size_type count, const value_type& value) {
            _sequence.assign(count, value);
        }

        template<class InputIt>
        constexpr void assign(InputIt first, InputIt last) {
            _sequence.assign(first, last);
        }

        constexpr void assign(std::initializer_list<value_type> ilist) {
            _sequence.assign(ilist);
        }

        /*****************************************************************************************/
        //
        //                       Comparison Operators and Conversion to Bool
        //
        /*****************************************************************************************/

        constexpr bool operator==(const SeqVector& other) const {
            return _sequence == other._sequence;
        }

        constexpr auto operator<=>(const SeqVector& other) const {
            return _sequence <=> other._sequence;
        }

        constexpr explicit operator bool() const {
            return !empty();
        }

        /*****************************************************************************************/
        //
        //                       Additional Methods (SeqVector-specific)
        //
        /*****************************************************************************************/

        constexpr auto view() noexcept {
            return View<impl_type>(_sequence);
        }

        constexpr SeqVector<VALUE>& shift(int index) {
            if (index > 0) {
                rotate_left_and_drop(static_cast<std::size_t>(index));
            }
            else {
                rotate_right_and_drop(static_cast<std::size_t>(std::abs(index)));
            }
            return *this;
        }

        constexpr SeqVector<VALUE>& cshift(int index) {
            if (index > 0) {
                rotate_left(static_cast<std::size_t>(index));
            }
            else {
                rotate_right(static_cast<std::size_t>(std::abs(index)));
            }
            return *this;
        }

        template <class F>
        constexpr SeqVector<VALUE>& apply(F&& f) {
            const auto n = _sequence.size();
            if constexpr (requires(size_type i, value_type& v) { f(i, v); }) {
                for (size_type i = 0; i < n; ++i) {
                    f(i, _sequence[i]);
                }
            } else {
                for (size_type i = 0; i < n; ++i) {
                    _sequence[i] = f(_sequence[i]);
                }
            }
            return *this;
        }

        template <class F>
        constexpr SeqVector<VALUE>& apply(const SeqVector<VALUE>& rhs, F&& f) {
            const auto n = std::min(_sequence.size(), rhs._sequence.size());
            for (size_type i = 0; i < n; ++i) {
                _sequence[i] = std::invoke(f, _sequence[i], rhs._sequence[i]);
            }
            return *this;
        }

        /*****************************************************************************************/
        //
        //                                  Unary Operators
        //
        /*****************************************************************************************/

        constexpr SeqVector<VALUE>& operator++() {
            for (auto& elem : _sequence) {
                ++elem;
            }
            return *this;
        }

        constexpr SeqVector<VALUE> operator++(int) {
            SeqVector<VALUE> temp(*this);
            ++(*this);
            return temp;
        }

        constexpr SeqVector<VALUE>& operator--() {
            for (auto& elem : _sequence) {
                --elem;
            }
            return *this;
        }

        constexpr SeqVector<VALUE> operator--(int) {
            SeqVector<VALUE> temp(*this);
            --(*this);
            return temp;
        }

        constexpr SeqVector<VALUE> operator+() const {
            SeqVector<VALUE> result = *this;
            for (auto& elem : result._sequence) {
                elem = +elem;
            }
            return result;
        }

        constexpr SeqVector<VALUE> operator-() const {
            SeqVector<VALUE> result = *this;
            for (auto& elem : result._sequence) {
                elem = -elem;
            }
            return result;
        }

        constexpr SeqVector<VALUE> operator~() const {
            SeqVector<VALUE> result = *this;
            for (auto& elem : result._sequence) {
                elem = ~elem;
            }
            return result;
        }

        /*****************************************************************************************/
        //
        //                           Compound Assignment Operators
        //
        /*****************************************************************************************/

        constexpr SeqVector<VALUE>& operator&=(const SeqVector& b) {
            const auto limit = std::max(_sequence.size(), b.size());
            if (_sequence.size() < limit) {
                resize(limit + 1);
            }
            for (std::size_t i = 0; i < limit; ++i) {
                _sequence[i] &= b[i];
            }
            return *this;
        }

        constexpr SeqVector<VALUE>& operator|=(const SeqVector& b) {
            const auto limit = std::max(_sequence.size(), b.size());
            if (_sequence.size() < limit) {
                resize(limit + 1);
            }
            for (std::size_t i = 0; i < limit; ++i) {
                _sequence[i] |= b[i];
            }
            return *this;
        }

        constexpr SeqVector<VALUE>& operator^=(const SeqVector& b) {
            const auto limit = std::max(_sequence.size(), b.size());
            if (_sequence.size() < limit) {
                resize(limit + 1);
            }
            for (std::size_t i = 0; i < limit; ++i) {
                _sequence[i] ^= b[i];
            }
            return *this;
        }

        constexpr SeqVector<VALUE>& operator<<=(const SeqVector& b) {
            const auto limit = std::max(_sequence.size(), b.size());
            if (_sequence.size() < limit) {
                resize(limit + 1);
            }
            for (std::size_t i = 0; i < limit; ++i) {
                if (b[i] < 0 || b[i] >= std::numeric_limits<value_type>::digits) {
                    throw std::domain_error("Invalid input for left shift.");
                }
                _sequence[i] <<= b[i];
            }
            return *this;
        }

        constexpr SeqVector<VALUE>& operator>>=(const SeqVector& b) {
            const auto limit = std::max(_sequence.size(), b.size());
            if (_sequence.size() < limit) {
                resize(limit + 1);
            }
            for (std::size_t i = 0; i < limit; ++i) {
                if (b[i] < 0 || b[i] >= std::numeric_limits<value_type>::digits) {
                    throw std::domain_error("Invalid input for right shift.");
                }
                _sequence[i] >>= b[i];
            }
            return *this;
        }

        constexpr SeqVector<VALUE>& operator<<=(std::size_t b) {
            const auto n = _sequence.size();
            for (std::size_t i = 0; i < n; ++i) {
                _sequence[i] <<= b;
            }
            return *this;
        }

        constexpr SeqVector<VALUE>& operator>>=(std::size_t b) {
            const auto n = _sequence.size();
            for (std::size_t i = 0; i < n; ++i) {
                _sequence[i] >>= b;
            }
            return *this;
        }

        constexpr SeqVector<VALUE>& operator+=(const SeqVector& b) {
            const auto size_a = _sequence.size();
            const auto size_b = b._sequence.size();
            const auto n  = (size_a >= size_b) ? size_a : size_b;
            if (size_a < n) _sequence.resize(n, value_type{});
            for (size_type i = 0; i < n; ++i) {
                const value_type value_b = (i < size_b) ? b._sequence[i] : value_type{};
                _sequence[i] += value_b;
            }
            return *this;
        }
        
        constexpr SeqVector<VALUE>& operator+=(const value_type& s) {
            const auto n = _sequence.size();
            for (size_type i = 0; i < n; ++i) {
                _sequence[i] += s;
            }
            return *this;
        }

        constexpr SeqVector<VALUE>& operator-=(const SeqVector& b) {
            const auto size_a = _sequence.size();
            const auto size_b = b._sequence.size();
            const auto n  = (size_a >= size_b) ? size_a : size_b;
            if (size_a < n) _sequence.resize(n, value_type{});
            for (size_type i = 0; i < n; ++i) {
                const value_type value_b = (i < size_b) ? b._sequence[i] : value_type{};
                _sequence[i] -= value_b;
            }
            return *this;
        }

        constexpr SeqVector<VALUE>& operator-=(const value_type& s) {
            const auto n = _sequence.size();
            for (size_type i = 0; i < n; ++i) {
                _sequence[i] -= s;
            }
            return *this;
        }

        constexpr SeqVector<VALUE>& operator*=(const SeqVector& b) {
            const auto size_a = _sequence.size();
            const auto size_b = b._sequence.size();
            const auto n  = (size_a >= size_b) ? size_a : size_b;
            if (size_a < n) _sequence.resize(n, value_type{});
            for (size_type i = 0; i < n; ++i) {
                const value_type value_b = (i < size_b) ? b._sequence[i] : value_type{1}; // neutral element 1
                _sequence[i] *= value_b;
            }
            return *this;
        }

        constexpr SeqVector<VALUE>& operator*=(const value_type& s) {
            const auto n = _sequence.size();
            for (size_type i = 0; i < n; ++i) {
                _sequence[i] *= s;
            }
            return *this;
        }

        constexpr SeqVector<VALUE>& operator/=(const SeqVector& b) {
            const auto size_a = _sequence.size();
            const auto size_b = b._sequence.size();
            // Do NOT resize here; dividing tail by implicit 1 leaves values unchanged
            for (size_type i = 0; i < size_a; ++i) {
                if (i < size_b) {
                    if (b._sequence[i] == value_type{}) {
                        throw std::domain_error("Division by zero");
                    }
                    _sequence[i] /= b._sequence[i];
                } // else: divide by implicit 1 (no change)
            }
            return *this;
        }

        constexpr SeqVector<VALUE>& operator/=(const value_type& s) {
            const auto n = _sequence.size();
            for (size_type i = 0; i < n; ++i) {
                _sequence[i] /= s;
            }
            return *this;
        }

        /*****************************************************************************************/
        //
        //                           Expression Template Operators
        //
        /*****************************************************************************************/

        template<typename RightExpr>
        constexpr auto operator+(RightExpr&& re) const -> ExprTemplate<const SeqVector&, Add_Op<value_type>, decltype(std::forward<RightExpr>(re))> {
            return ExprTemplate<const SeqVector&, Add_Op<value_type>, decltype(std::forward<RightExpr>(re))>(*this, std::forward<RightExpr>(re));
        }

        template<typename RightExpr>
        constexpr auto operator-(RightExpr&& re) const -> ExprTemplate<const SeqVector&, Sub_Op<value_type>, decltype(std::forward<RightExpr>(re))> {
            return ExprTemplate<const SeqVector&, Sub_Op<value_type>, decltype(std::forward<RightExpr>(re))>(*this, std::forward<RightExpr>(re));
        }

        template<typename RightExpr>
        constexpr auto operator*(RightExpr&& re) const -> ExprTemplate<const SeqVector&, Mul_Op<value_type>, decltype(std::forward<RightExpr>(re))> {
            return ExprTemplate<const SeqVector&, Mul_Op<value_type>, decltype(std::forward<RightExpr>(re))>(*this, std::forward<RightExpr>(re));
        }

        template<typename RightExpr>
        constexpr auto operator/(RightExpr&& re) const -> ExprTemplate<const SeqVector&, Div_Op<value_type>, decltype(std::forward<RightExpr>(re))> {
            return ExprTemplate<const SeqVector&, Div_Op<value_type>, decltype(std::forward<RightExpr>(re))>(*this, std::forward<RightExpr>(re));
        }


        /*****************************************************************************************/
        //
        //                                  Mathematical Functions
        //
        /*****************************************************************************************/

        constexpr SeqVector<VALUE>& abs() {
            for (std::size_t i = 0, limit = size(); i < limit; ++i) {
                _sequence[i] = std::abs(_sequence[i]);
            }
            return *this;
        }

        constexpr SeqVector<VALUE> abs(SeqVector<VALUE> a) {
            return a.abs();
        }

        constexpr value_type sum() const {
            if (_sequence.empty()) {
                return value_type{};
            }
            return std::accumulate(cbegin(), cend(), value_type{});
        }

        constexpr value_type sum(const SeqVector<VALUE>& a) {
            if (a._sequence.empty()) {
                return value_type{};
            }
            return std::accumulate(a.cbegin(), a.cend(), value_type{});
        }

        constexpr value_type max_val() const {
            if (_sequence.empty()) {
                return value_type{};
            }
            value_type result{0};
            for (std::size_t i = 0, limit = _sequence.size(); i < limit; ++i) {
                if (_sequence[i] > result) {
                    result = _sequence[i];
                }
            }
            return result;
        }

        constexpr value_type max_val(const SeqVector<VALUE>& a) noexcept {
            return a.max_val();
        }

        constexpr value_type min_val() const {
            if (_sequence.empty()) {
                return value_type{};
            }
            value_type result{0};
            for (std::size_t i = 0, limit = _sequence.size(); i < limit; ++i) {
                if (_sequence[i] < result) {
                    result = _sequence[i];
                }
            }
            return result;
        }

        constexpr value_type min_val(const SeqVector<VALUE>& a) noexcept {
            return a.min_val();
        }

        constexpr SeqVector<VALUE>& exp() {
            for (std::size_t i = 0, limit = size(); i < limit; ++i) {
                _sequence[i] = std::exp(_sequence[i]);
            }
            return *this;
        }

        constexpr SeqVector<VALUE> exp(SeqVector<VALUE> a) {
            return a.exp();
        }

        constexpr SeqVector<VALUE>& log() {
            for (std::size_t i = 0, limit = size(); i < limit; ++i) {
                if (_sequence[i] <= 0) {
                    throw std::domain_error("Logarithm of non-positive value encountered.");
                }
                _sequence[i] = std::log(_sequence[i]);
            }
            return *this;
        }

        constexpr SeqVector<VALUE> log(SeqVector<VALUE> a) {
            return a.log();
        }

        constexpr SeqVector<VALUE>& log10() {
            for (std::size_t i = 0, limit = size(); i < limit; ++i) {
                if (_sequence[i] <= 0) {
                    throw std::domain_error("Logarithm of non-positive value encountered.");
                }
                _sequence[i] = std::log10(_sequence[i]);
            }
            return *this;
        }

        constexpr SeqVector<VALUE> log10(SeqVector<VALUE> a) {
            return a.log10();
        }

        constexpr SeqVector<VALUE>& pow(const value_type& exp) {
            for (std::size_t i = 0, limit = size(); i < limit; ++i) {
                _sequence[i] = std::pow(_sequence[i], exp);
            }
            return *this;
        }

        constexpr SeqVector<VALUE> pow(SeqVector<VALUE> a) {
            return a.pow();
        }

        constexpr SeqVector<VALUE>& sqrt() {
            for (std::size_t i = 0, limit = size(); i < limit; ++i) {
                if (_sequence[i] < 0) {
                    throw std::domain_error("Square root of negative value encountered.");
                }
                _sequence[i] = std::sqrt(_sequence[i]);
            }
            return *this;
        }

        constexpr SeqVector<VALUE> sqrt(SeqVector<VALUE> a) {
            return a.sqrt();
        }

        constexpr SeqVector<VALUE>& sin() {
            for (std::size_t i = 0, limit = size(); i < limit; ++i) {
                _sequence[i] = std::sin(_sequence[i]);
            }
            return *this;
        }

        constexpr SeqVector<VALUE> sin(SeqVector<VALUE> a) {
            return a.sin();
        }

        constexpr SeqVector<VALUE>& cos() {
            for (std::size_t i = 0, limit = size(); i < limit; ++i) {
                _sequence[i] = std::cos(_sequence[i]);
            }
            return *this;
        }

        constexpr SeqVector<VALUE> cos(SeqVector<VALUE> a) {
            return a.cos();
        }

        constexpr SeqVector<VALUE>& tan() {
            for (std::size_t i = 0, limit = size(); i < limit; ++i) {
                _sequence[i] = std::tan(_sequence[i]);
            }
            return *this;
        }

        constexpr SeqVector<VALUE> tan(SeqVector<VALUE> a) {
            return a.tan();
        }

        constexpr SeqVector<VALUE>& asin() {
            for (std::size_t i = 0, limit = size(); i < limit; ++i) {
                if (_sequence[i] < -1 || _sequence[i] > 1) {
                    throw std::domain_error("Invalid input for asin.");
                }
                _sequence[i] = std::asin(_sequence[i]);
            }
            return *this;
        }

        constexpr SeqVector<VALUE> asin(SeqVector<VALUE> a) {
            return a.asin();
        }

        constexpr SeqVector<VALUE>& acos() {
            for (std::size_t i = 0, limit = size(); i < limit; ++i) {
                if (_sequence[i] < -1 || _sequence[i] > 1) {
                    throw std::domain_error("Invalid input for acos.");
                }
                _sequence[i] = std::acos(_sequence[i]);
            }
            return *this;
        }

        constexpr SeqVector<VALUE> acos(SeqVector<VALUE> a) {
            return a.acos();
        }

        constexpr SeqVector<VALUE>& atan() {
            for (std::size_t i = 0, limit = size(); i < limit; ++i) {
                _sequence[i] = std::atan(_sequence[i]);
            }
            return *this;
        }

        constexpr SeqVector<VALUE> atan(SeqVector<VALUE> a) {
            return a.atan();
        }

        constexpr SeqVector<VALUE>& atan2() {
            for (std::size_t i = 0, limit = size(); i < limit; ++i) {
                _sequence[i] = std::atan2(_sequence[i]);
            }
            return *this;
        }

        constexpr SeqVector<VALUE>& sinh() {
            for (std::size_t i = 0, limit = size(); i < limit; ++i) {
                _sequence[i] = std::sinh(_sequence[i]);
            }
            return *this;
        }

        constexpr SeqVector<VALUE> sinh(SeqVector<VALUE> a) {
            for (std::size_t i = 0, limit = a.size(); i < limit; ++i) {
                a[i] = std::sinh(a[i]);
            }
            return a;
        }

        constexpr SeqVector<VALUE>& cosh() {
            for (std::size_t i = 0, limit = size(); i < limit; ++i) {
                _sequence[i] = std::cosh(_sequence[i]);
            }
            return *this;
        }

        constexpr SeqVector<VALUE> cosh(SeqVector<VALUE> a) {
            return a.cosh();
        }

        constexpr SeqVector<VALUE>& tanh() {
            for (std::size_t i = 0, limit = size(); i < limit; ++i) {
                _sequence[i] = std::tanh(_sequence[i]);
            }
            return *this;
        }

        constexpr SeqVector<VALUE> tanh(SeqVector<VALUE> a) {
            return a.tanh();
        }

        constexpr SeqVector<VALUE> atan2(SeqVector<VALUE> a) {
            return a.atan2();
        }

        constexpr SeqVector<VALUE>& asinh() {
            for (std::size_t i = 0, limit = size(); i < limit; ++i) {
                _sequence[i] = std::asinh(_sequence[i]);
            }
            return *this;
        }

        constexpr SeqVector<VALUE> asinh(SeqVector<VALUE> a) {
            for (std::size_t i = 0, limit = a.size(); i < limit; ++i) {
                a[i] = std::asinh(a[i]);
            }
            return a;
        }

        constexpr SeqVector<VALUE>& acosh() {
            for (std::size_t i = 0, limit = size(); i < limit; ++i) {
                _sequence[i] = std::acosh(_sequence[i]);
            }
            return *this;
        }

        constexpr SeqVector<VALUE> acosh(SeqVector<VALUE> a) {
            return a.acosh();
        }

        constexpr SeqVector<VALUE>& atanh() {
            for (std::size_t i = 0, limit = size(); i < limit; ++i) {
                _sequence[i] = std::atanh(_sequence[i]);
            }
            return *this;
        }

        constexpr SeqVector<VALUE> atanh(SeqVector<VALUE> a) {
            return a.atanh();
        }

    private:
        impl_type _sequence;

        constexpr SeqVector<VALUE>& rotate_left(std::size_t shift) {
            const auto n = _sequence.size();
            if (n > 0) {
                shift %= n;
                if (shift) {
                    std::ranges::rotate(_sequence, std::next(_sequence.begin(), static_cast<difference_type>(shift)));
                }
            }
            return *this;
        }

        constexpr SeqVector<VALUE>& rotate_left_and_drop(std::size_t shift) {
            const auto n = _sequence.size();
            if (n == 0) {
                return *this;
            }
            shift %= n;
            if (shift) {
                std::ranges::rotate(_sequence, std::next(_sequence.begin(), static_cast<difference_type>(shift)));
            }
            for (std::size_t i = 0; i < shift; ++i) {
                _sequence[i] = value_type{};
            }
            return *this;
        }

        constexpr SeqVector<VALUE>& rotate_right(std::size_t shift) {
            const auto n = _sequence.size();
            if (n == 0) {
                return *this;
            }
            shift %= n;
            if (shift) {
                std::ranges::rotate(_sequence, std::next(_sequence.begin(), static_cast<difference_type>(n - shift)));
            }
            return *this;
        }

        constexpr SeqVector<VALUE>& rotate_right_and_drop(std::size_t shift) {
            const auto n = _sequence.size();
            if (n == 0) {
                return *this;
            }
            shift %= n;
            if (shift) {
                std::ranges::rotate(_sequence, std::next(_sequence.begin(), static_cast<difference_type>(n - shift)));
            }
            const auto start = n - shift;
            for (std::size_t i = start; i < n; ++i) {
                _sequence[i] = value_type{};
            }
            return *this;
        }

        /*****************************************************************************************/
        //
        //                                    Standard Range and MD Spans
        //
        /*****************************************************************************************/

        template<class T, class A>
        class VectorView : public std::ranges::view_interface<VectorView<T, A>> {
        public:
            constexpr VectorView() = default;

            constexpr VectorView(const std::vector<T, A>& vec) : m_begin(vec.cbegin()), m_end(vec.cend()) {}

            constexpr auto begin() const { return m_begin; }
            constexpr auto end() const { return m_end; }

        private:
            typename std::vector<T, A>::const_iterator m_begin{}, m_end{};
        };

        template<typename Extents>
        constexpr auto span(Extents ext) noexcept {
            #ifndef NDEBUG
                size_t prod = 1;
                for (size_t r = 0; r < Extents::rank(); ++r) { 
                    prod *= static_cast<size_t>(ext.extent(r)); 
                }
                assert(prod <= _sequence.size() && "mdspan extents exceed underlying size()");
            #endif
            return std::mdspan<value_type, Extents>(_sequence.data(), ext);
        }

        template<typename Extents>
        constexpr auto span(Extents ext) const noexcept {
            #ifndef NDEBUG
                size_t prod = 1;
                for (size_t r = 0; r < Extents::rank(); ++r) { 
                    prod *= static_cast<size_t>(ext.extent(r)); 
                }
                assert(prod <= _sequence.size() && "mdspan extents exceed underlying size()");
            #endif
            return std::mdspan<const value_type, Extents>(_sequence.data(), ext);
        }

        template<std::size_t... Dims>
        constexpr auto make_extents() noexcept {
            return std::extents<std::size_t, Dims...>{};
        }

        template<std::size_t... Dims>
        constexpr auto span() noexcept {
            #ifndef NDEBUG
                constexpr size_t prod = (static_cast<size_t>(Dims) * ... * 1ull);
                static_assert(prod > 0, "static mdspan must have positive total size");
                assert(prod <= _sequence.size() && "static mdspan extents exceed underlying size()");
            #endif
            return std::mdspan<value_type, std::extents<std::size_t, Dims...>>(
                _sequence.data(), std::extents<std::size_t, Dims...>{});
        }
    };
}
