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
#include <bit>
#include <compare>
#include <cstdint>
#include <deque>
#include <forward_list>
#include <initializer_list>
#include <iostream>
#include <functional>
#include <limits>
#include <list>
#include <mdspan>
#include <numeric>
#include <type_traits>

#include "Expression_Template.h"
#include <ostream>

namespace Oliver {

    /********************************************************************************************/
    //
    //                                    'SeqVector' class
    //          
    //          The SeqVector, is an expression templated wrapper class intended wrapping
    //          a std::vector.  Another specialized class is available for the std::array.
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

    template<typename VALUE = intmax_t>
    class SeqVector : public std::ranges::view_interface<SeqVector<VALUE>> {

        // TODO get rid of std::size_t.

    public:
        using impl_type              = std::vector<VALUE>;
        using value_type             = impl_type::value_type;
        using size_type              = impl_type::size_type;
        using iterator               = impl_type::iterator;
        using const_iterator         = impl_type::const_iterator;
        using reverse_iterator       = impl_type::reverse_iterator;
        using const_reverse_iterator = impl_type::const_reverse_iterator;

        constexpr SeqVector() noexcept;
        constexpr SeqVector(value_type value);
        constexpr SeqVector(const std::valarray<value_type>& val);
        constexpr SeqVector(std::initializer_list<value_type> list);

        template <typename LE, typename Op, typename RE>
        constexpr SeqVector(ExprTemplate<LE, Op, RE>&& expr);

        constexpr SeqVector(SeqVector&& arr)                  noexcept = default;
        constexpr SeqVector(const SeqVector& arr)             noexcept = default;
        constexpr SeqVector& operator =(SeqVector&& arr)      noexcept = default;
        constexpr SeqVector& operator =(const SeqVector& arr) noexcept = default;

        constexpr void swap(SeqVector& first, SeqVector& second);

        operator bool() const;

        constexpr bool                  operator ==(const SeqVector& b) const;
        constexpr std::partial_ordering operator<=>(const SeqVector& b) const;

        constexpr const value_type& operator [](std::size_t index) const;
        constexpr       value_type& operator [](std::size_t index);

        auto  begin()       noexcept;
        auto  begin() const noexcept;
        auto cbegin() const noexcept;

        auto  end()       noexcept;
        auto  end() const noexcept;
        auto cend() const noexcept;

        auto  rbegin()       noexcept;
        auto  rbegin() const noexcept;
        auto crbegin() const noexcept;

        auto  rend()       noexcept;
        auto  rend() const noexcept;
        auto crend() const noexcept;

#if __cpp_lib_mdspan
        template<std::size_t... args>
            //typename    LayoutPolicy = std::layout_right,
            //typename    AccessorPolicy = std::default_accessor<T>>
        constexpr auto exts() noexcept;
        template<std::size_t... args>
            //typename    LayoutPolicy = std::layout_right,
            //typename    AccessorPolicy = std::default_accessor<T>>
        constexpr auto span() noexcept;
#endif
        constexpr auto view() noexcept;

        constexpr std::size_t     size() const;
        constexpr std::size_t max_size() const;
        constexpr std::size_t capacity() const;

        constexpr SeqVector&  resize(std::size_t size);
        constexpr SeqVector&  resize(std::size_t size, value_type value);
        constexpr SeqVector& reserve(std::size_t size);

        constexpr SeqVector&  pop_back();
        constexpr SeqVector& push_back(value_type value);

        constexpr SeqVector& insert(std::size_t at, const auto& range);
        constexpr SeqVector& insert(std::size_t at, std::ranges::sized_range auto&& range);

        constexpr SeqVector&  shift(int index);
        constexpr SeqVector& cshift(int index);

        constexpr SeqVector& apply(value_type func(value_type));
        constexpr SeqVector& apply(value_type func(const value_type&));

        constexpr SeqVector& apply(const SeqVector& b, value_type func(value_type, value_type));
        constexpr SeqVector& apply(const SeqVector& b, value_type func(const value_type&, value_type));
        constexpr SeqVector& apply(const SeqVector& b, value_type func(value_type, const value_type&));
        constexpr SeqVector& apply(const SeqVector& b, value_type func(const value_type&, const value_type&));

        constexpr SeqVector operator +();
        constexpr SeqVector operator -();
        constexpr SeqVector operator ~();

        constexpr SeqVector& operator  +=(const SeqVector& b);
        constexpr SeqVector& operator  -=(const SeqVector& b);
        constexpr SeqVector& operator  *=(const SeqVector& b);
        constexpr SeqVector& operator  /=(const SeqVector& b);
        constexpr SeqVector& operator  %=(const SeqVector& b);
        constexpr SeqVector& operator  &=(const SeqVector& b);
        constexpr SeqVector& operator  |=(const SeqVector& b);
        constexpr SeqVector& operator  ^=(const SeqVector& b);
        constexpr SeqVector& operator <<=(const SeqVector& b);
        constexpr SeqVector& operator >>=(const SeqVector& b);

        template <typename RightExpr> SeqVector& operator   =(RightExpr&& re);
        template <typename RightExpr> SeqVector& operator  +=(RightExpr&& re);
        template <typename RightExpr> SeqVector& operator  -=(RightExpr&& re);
        template <typename RightExpr> SeqVector& operator  *=(RightExpr&& re);
        template <typename RightExpr> SeqVector& operator  /=(RightExpr&& re);
        template <typename RightExpr> SeqVector& operator  %=(RightExpr&& re);
        template <typename RightExpr> SeqVector& operator  &=(RightExpr&& re);
        template <typename RightExpr> SeqVector& operator  |=(RightExpr&& re);
        template <typename RightExpr> SeqVector& operator  ^=(RightExpr&& re);
        template <typename RightExpr> SeqVector& operator <<=(RightExpr&& re);
        template <typename RightExpr> SeqVector& operator >>=(RightExpr&& re);

        template <typename RightExpr> auto operator  +(RightExpr&& re) const->ExprTemplate<const SeqVector&, Add_Op<value_type>,        decltype(std::forward<RightExpr>(re))>;
        template <typename RightExpr> auto operator  -(RightExpr&& re) const->ExprTemplate<const SeqVector&, Sub_Op<value_type>,        decltype(std::forward<RightExpr>(re))>;
        template <typename RightExpr> auto operator  *(RightExpr&& re) const->ExprTemplate<const SeqVector&, Mul_Op<value_type>,        decltype(std::forward<RightExpr>(re))>;
        template <typename RightExpr> auto operator  /(RightExpr&& re) const->ExprTemplate<const SeqVector&, Div_Op<value_type>,        decltype(std::forward<RightExpr>(re))>;
        template <typename RightExpr> auto operator  %(RightExpr&& re) const->ExprTemplate<const SeqVector&, Mod_Op<value_type>,        decltype(std::forward<RightExpr>(re))>;
        template <typename RightExpr> auto operator  &(RightExpr&& re) const->ExprTemplate<const SeqVector&, And_Op<value_type>,        decltype(std::forward<RightExpr>(re))>;
        template <typename RightExpr> auto operator  |(RightExpr&& re) const->ExprTemplate<const SeqVector&, Or_Op<value_type>,         decltype(std::forward<RightExpr>(re))>;
        template <typename RightExpr> auto operator  ^(RightExpr&& re) const->ExprTemplate<const SeqVector&, Xor_Op<value_type>,        decltype(std::forward<RightExpr>(re))>;
        template <typename RightExpr> auto operator <<(RightExpr&& re) const->ExprTemplate<const SeqVector&, LeftShift_Op<value_type>,  decltype(std::forward<RightExpr>(re))>;
        template <typename RightExpr> auto operator >>(RightExpr&& re) const->ExprTemplate<const SeqVector&, RightShift_Op<value_type>, decltype(std::forward<RightExpr>(re))>;

        constexpr SeqVector& abs();
        constexpr value_type sum() const;
        constexpr value_type max() const;
        constexpr value_type min() const;

        constexpr SeqVector&   exp();
        constexpr SeqVector&   log();
        constexpr SeqVector& log10();
        constexpr SeqVector&   pow();
        constexpr SeqVector&  sqrt();

        constexpr SeqVector&   sin();
        constexpr SeqVector&   cos();
        constexpr SeqVector&   tan();
        constexpr SeqVector&  asin();
        constexpr SeqVector&  acos();
        constexpr SeqVector&  atan();
        constexpr SeqVector& atan2();

        constexpr SeqVector& sinh();
        constexpr SeqVector& cosh();
        constexpr SeqVector& tanh();

    protected:
        constexpr ~SeqVector() noexcept = default;

    private:
        static const value_type def_value;
        impl_type _sequence = {0};  // set default values here

        constexpr SeqVector& rotate_left          (std::size_t shift);
        constexpr SeqVector& rotate_left_and_drop (std::size_t shift);
        constexpr SeqVector& rotate_right         (std::size_t shift);
        constexpr SeqVector& rotate_right_and_drop(std::size_t shift);

        template<class T>
        class View : public std::ranges::view_interface<View<T>> {
        public:
            View() = default;
            View(const auto& seq) : v_begin(seq.cbegin()), v_end(seq.cend()) {}

            auto begin() const { return v_begin; }
            auto   end() const { return v_end;   }

        private:
            typename impl_type::const_iterator v_begin{}, v_end{};
        };
    };
    
    SeqVector   abs(SeqVector a);
    value_type sum(const SeqVector& a);
    value_type max(const SeqVector& a);
    value_type min(const SeqVector& a);

    SeqVector   exp(SeqVector a);
    SeqVector   log(SeqVector a);
    SeqVector log10(SeqVector a);
    SeqVector   pow(SeqVector a);
    SeqVector  sqrt(SeqVector a);

    SeqVector   sin(SeqVector a);
    SeqVector   cos(SeqVector a);
    SeqVector   tan(SeqVector a);
    SeqVector  asin(SeqVector a);
    SeqVector  acos(SeqVector a);
    SeqVector  atan(SeqVector a);
    SeqVector atan2(SeqVector a);

    SeqVector sinh(SeqVector a);
    SeqVector cosh(SeqVector a);
    SeqVector tanh(SeqVector a);  // Can all be moved outside the class.


    /*****************************************************************************************/
    //
    //                         Default Initialization & TypeDefines
    //
    /*****************************************************************************************/

    template<typename VALUE>
    const SeqVector<VALUE>::impl_type::value_type SeqVector<VALUE>::def_value = VALUE{};

    /*****************************************************************************************/
    //
    //                                     IO Stream Overload
    //
    /*****************************************************************************************/

    template <typename VALUE, typename impl_type>
    std::ostream& operator <<(std::ostream& os, SeqVector<VALUE> const& a) {
        if (a.size() > 0) {
            os << "(";
            for (std::size_t i = 0; i < a.size(); ++i) {
                os << a[i] << ((i + VALUE{ 1 } != a.size()) ? ',' : ')');
            }
        }
        return os;
    }

    /*****************************************************************************************/
    //
    //                                    Constructors & Swap
    //
    /*****************************************************************************************/

    template<typename VALUE>
    inline constexpr SeqVector<VALUE>::SeqVector() : _sequence() {
    }

    template<typename VALUE>
    inline constexpr SeqVector<VALUE>::SeqVector(value_type value) : _sequence(1, value) {
    }

    template<typename VALUE>
    inline constexpr SeqVector<VALUE>::SeqVector(const std::valarray<value_type>& val) : _sequence(val.size(), value_type{ }) {
        const auto limit = val.size();
        for (std::size_t i = 0; i < limit; ++i) {
            _sequence[i] = val[i];
        }
    }

    template<typename VALUE>
    inline constexpr SeqVector<VALUE>::SeqVector(std::initializer_list<value_type> list) : _sequence(list) {
        std::cout << "value = " << std::is_same<decltype(&impl_type::size), void>::value << std::endl;
    }

    template<typename VALUE>
    template<typename LE, typename Op, typename RE>
    inline constexpr SeqVector<VALUE>::SeqVector(ExprTemplate<LE, Op, RE>&& expr) : _sequence(expr.size(), value_type{}) {
        const auto limit = _sequence.size();
        for (std::size_t i = 0; i < limit; ++i) {
            _sequence[i] = expr[i];
        }
    }

    template<typename VALUE>
    constexpr void swap(SeqVector<VALUE>& first, SeqVector<VALUE>& second) {
        std::swap(first, second);
    }

    /*****************************************************************************************/
    //
    //                       Functional Alias, Methods, & Copy Definitions
    //
    /*****************************************************************************************/

    template<typename VALUE>
    inline constexpr SeqVector<VALUE>& SeqVector<VALUE>::abs() {
        for (std::size_t i = 0, limit = size(); i < limit; ++i) {
            _sequence[i] = std::abs(_sequence[i]);
        }
        return *this;
    }

    template<typename VALUE>
    SeqVector<VALUE> abs(SeqVector<VALUE> a) {
        return a.abs();
    }

    template<typename VALUE>
    inline constexpr SeqVector<VALUE>::value_type SeqVector<VALUE>::sum() const {
        return std::accumulate(cbegin(), cend());
    }

    template<typename VALUE>
    SeqVector<VALUE>::value_type sum(const SeqVector<VALUE>& a) {
        return std::accumulate(a.cbegin(), a.cend());
    }

    template<typename VALUE>
    inline constexpr SeqVector<VALUE>::value_type SeqVector<VALUE>::max() const {
        return std::ranges::max(cbegin(), cend());
    }

    template<typename VALUE>
    SeqVector<VALUE>::value_type max(const SeqVector<VALUE>& a) {
        return std::ranges::max(a.cbegin(), a.cend());
    }

    template<typename VALUE>
    inline constexpr SeqVector<VALUE>::value_type SeqVector<VALUE>::min() const {
        return std::ranges::min(cbegin(), cend());
    }

    template<typename VALUE>
    SeqVector<VALUE>::value_type min(const SeqVector<VALUE>& a) {
        return std::ranges::min(a.cbegin(), a.cend());
    }

    template<typename VALUE>
    inline constexpr SeqVector<VALUE>& SeqVector<VALUE>::exp() {
        for (std::size_t i = 0, limit = size(); i < limit; ++i) {
            _sequence[i] = std::exp(_sequence[i]);
        }
        return *this;
    }

    template<typename VALUE>
    SeqVector<VALUE> exp(SeqVector<VALUE> a) {
        return a.exp();
    }

    template<typename VALUE>
    inline constexpr SeqVector<VALUE>& SeqVector<VALUE>::log() {
        for (std::size_t i = 0, limit = size(); i < limit; ++i) {
            _sequence[i] = std::log(_sequence[i]);
        }
        return *this;
    }

    template<typename VALUE>
    SeqVector<VALUE> log(SeqVector<VALUE> a) {
        return a.log();
    }

    template<typename VALUE>
    inline constexpr SeqVector<VALUE>& SeqVector<VALUE>::log10() {
        for (std::size_t i = 0, limit = size(); i < limit; ++i) {
            _sequence[i] = std::log10(_sequence[i]);
        }
        return *this;
    }

    template<typename VALUE>
    SeqVector<VALUE> log10(SeqVector<VALUE> a) {
        return a.log10();
    }

    template<typename VALUE>
    inline constexpr SeqVector<VALUE>& SeqVector<VALUE>::pow() {
        for (std::size_t i = 0, limit = size(); i < limit; ++i) {
            _sequence[i] = std::pow(_sequence[i]);
        }
        return *this;
    }

    template<typename VALUE>
    SeqVector<VALUE> pow(SeqVector<VALUE> a) {
        return a.pow();
    }

    template<typename VALUE>
    inline constexpr SeqVector<VALUE>& SeqVector<VALUE>::sqrt() {
        for (std::size_t i = 0, limit = size(); i < limit; ++i) {
            _sequence[i] = std::sqrt(_sequence[i]);
        }
        return *this;
    }

    template<typename VALUE>
    SeqVector<VALUE> sqrt(SeqVector<VALUE> a) {
        return a.sqrt();
    }

    template<typename VALUE>
    inline constexpr SeqVector<VALUE>& SeqVector<VALUE>::sin() {
        for (std::size_t i = 0, limit = size(); i < limit; ++i) {
            _sequence[i] = std::sin(_sequence[i]);
        }
        return *this;
    }

    template<typename VALUE>
    SeqVector<VALUE> sin(SeqVector<VALUE> a) {
        return a.sin();
    }

    template<typename VALUE>
    inline constexpr SeqVector<VALUE>& SeqVector<VALUE>::cos() {
        for (std::size_t i = 0, limit = size(); i < limit; ++i) {
            _sequence[i] = std::cos(_sequence[i]);
        }
        return *this;
    }

    template<typename VALUE>
    SeqVector<VALUE> cos(SeqVector<VALUE> a) {
        return a.cos();
    }

    template<typename VALUE>
    inline constexpr SeqVector<VALUE>& SeqVector<VALUE>::tan() {
        for (std::size_t i = 0, limit = size(); i < limit; ++i) {
            _sequence[i] = std::tan(_sequence[i]);
        }
        return *this;
    }

    template<typename VALUE>
    SeqVector<VALUE> tan(SeqVector<VALUE> a) {
        return a.tan();
    }

    template<typename VALUE>
    inline constexpr SeqVector<VALUE>& SeqVector<VALUE>::asin() {
        for (std::size_t i = 0, limit = size(); i < limit; ++i) {
            _sequence[i] = std::asin(_sequence[i]);
        }
        return *this;
    }

    template<typename VALUE>
    SeqVector<VALUE> asin(SeqVector<VALUE> a) {
        return a.sin();
    }

    template<typename VALUE>
    inline constexpr SeqVector<VALUE>& SeqVector<VALUE>::acos() {
        for (std::size_t i = 0, limit = size(); i < limit; ++i) {
            _sequence[i] = std::acos(_sequence[i]);
        }
        return *this;
    }

    template<typename VALUE>
    SeqVector<VALUE> acos(SeqVector<VALUE> a) {
        return a.acos();
    }

    template<typename VALUE>
    inline constexpr SeqVector<VALUE>& SeqVector<VALUE>::atan() {
        for (std::size_t i = 0, limit = size(); i < limit; ++i) {
            _sequence[i] = std::atan(_sequence[i]);
        }
        return *this;
    }

    template<typename VALUE>
    SeqVector<VALUE> atan(SeqVector<VALUE> a) {
        return a.atan();
    }

    template<typename VALUE>
    inline constexpr SeqVector<VALUE>& SeqVector<VALUE>::atan2() {
        for (std::size_t i = 0, limit = size(); i < limit; ++i) {
            _sequence[i] = std::atan2(_sequence[i]);
        }
        return *this;
    }

    template<typename VALUE>
    SeqVector<VALUE> atan2(SeqVector<VALUE> a) {
        return a.atan2();
    }

    template<typename VALUE>
    inline constexpr SeqVector<VALUE>& SeqVector<VALUE>::sinh() {
        for (std::size_t i = 0, limit = size(); i < limit; ++i) {
            _sequence[i] = std::sinh(_sequence[i]);
        }
        return *this;
    }

    template<typename VALUE>
    SeqVector<VALUE> sinh(SeqVector<VALUE> a) {
        for (std::size_t i = 0, limit = a.size(); i < limit; ++i) {
            a[i] = std::sinh(a[i]);
        }
        return a;
    }

    template<typename VALUE>
    inline constexpr SeqVector<VALUE>& SeqVector<VALUE>::cosh() {
        for (std::size_t i = 0, limit = size(); i < limit; ++i) {
            _sequence[i] = std::cosh(_sequence[i]);
        }
        return *this;
    }

    template<typename VALUE>
    SeqVector<VALUE> cosh(SeqVector<VALUE> a) {
        return a.cosh();
    }

    template<typename VALUE>
    inline constexpr SeqVector<VALUE>& SeqVector<VALUE>::tanh() {
        for (std::size_t i = 0, limit = size(); i < limit; ++i) {
            _sequence[i] = std::tanh(_sequence[i]);
        }
        return *this;
    }

    template<typename VALUE>
    SeqVector<VALUE> tanh(SeqVector<VALUE> a) {
        return a.tanh();
    }

    //template<typename VALUE>
    //SeqVector<VALUE> X(SeqVector<VALUE> a) {
    //    for (std::size_t i = 0, limit = a.size(); i < limit; ++i) {
    //        a[i] = std::X(a[i]);
    //    }
    //    return a;
    //}

    /*****************************************************************************************/
    //
    //                                      Boolean Operation
    //
    /*****************************************************************************************/

    template<typename VALUE>
    inline SeqVector<VALUE>::operator bool() const {
        const auto limit = _sequence.size();
        for (std::size_t i = 0; i < limit; ++i) {
            if (static_cast<bool>(operator[](i))) {
                return true;
            }
        }
        return false;
    }

    template<typename VALUE>
    inline constexpr bool SeqVector<VALUE>::operator==(const SeqVector& b) const {
        return operator<=>(b) == std::partial_ordering::equivalent;
    }

    template<typename VALUE>
    inline constexpr std::partial_ordering SeqVector<VALUE>::operator<=>(const SeqVector& b) const {
        if (size() > b.size()) {
            return std::partial_ordering::greater;
        }
        if (size() < b.size()) {
            return std::partial_ordering::less;
        }
        return std::partial_ordering::equivalent;
    }

    /*****************************************************************************************/
    //
    //                                     Subscript Methods
    //
    /*****************************************************************************************/

    template<typename VALUE>
    inline constexpr const SeqVector<VALUE>::value_type& SeqVector<VALUE>::operator[](std::size_t index) const {
        if (index < _sequence.size()) {
            return _sequence[index];
        }
        return def_value;
    }    

    template<typename VALUE>
    inline constexpr SeqVector<VALUE>::value_type& SeqVector<VALUE>::operator[](std::size_t index) {
        if (index >= _sequence.size()) {
            resize(index + 1);
        }
        return _sequence[index];
    }

    /*****************************************************************************************/
    //
    //                                      Iterators
    //
    /*****************************************************************************************/

    template<typename VALUE>
    inline auto SeqVector<VALUE>::begin() noexcept {
        return _sequence.begin();
    }

    template<typename VALUE>
    inline auto SeqVector<VALUE>::begin() const noexcept {
        return _sequence.begin();
    }

    template<typename VALUE>
    inline auto SeqVector<VALUE>::cbegin() const noexcept {
        return _sequence.cbegin();
    }

    template<typename VALUE>
    inline auto SeqVector<VALUE>::end() noexcept {
        return _sequence.end();
    }

    template<typename VALUE>
    inline auto SeqVector<VALUE>::end() const noexcept {
        return _sequence.end();
    }

    template<typename VALUE>
    inline auto SeqVector<VALUE>::cend() const noexcept {
        return _sequence.cend();
    }

    template<typename VALUE>
    inline auto SeqVector<VALUE>::rbegin() noexcept {
        return _sequence.rbegin();
    }

    template<typename VALUE>
    inline auto SeqVector<VALUE>::rbegin() const noexcept {
        return _sequence.rbegin();
    }

    template<typename VALUE>
    inline auto SeqVector<VALUE>::crbegin() const noexcept {
        return _sequence.crbegin();
    }

    template<typename VALUE>
    inline auto SeqVector<VALUE>::rend() noexcept {
        return _sequence.rend();
    }

    template<typename VALUE>
    inline auto SeqVector<VALUE>::rend() const noexcept {
        return _sequence.rend();
    }

    template<typename VALUE>
    inline auto SeqVector<VALUE>::crend() const noexcept {
        return _sequence.crend();
    }    

    /*****************************************************************************************/
    //
    //                                View Adapters & Methods
    //
    /*****************************************************************************************/

    template<class T, class A>
    class VectorView : public std::ranges::view_interface<VectorView<T, A>> {
    public:
        VectorView() = default;

        VectorView(const std::vector<T, A>& vec) : m_begin(vec.cbegin()), m_end(vec.cend()) {}

        auto begin() const { return m_begin; }
        auto   end() const { return m_end;   }

    private:
        typename std::vector<T, A>::const_iterator m_begin{}, m_end{};
    };

    //template<typename VALUE>
    //template<SeqVector<VALUE>::value_type, std::extents<std::size_t>...> //, class LayoutPolicy, class AccessorPolicy>
    //inline constexpr auto SeqVector<VALUE>::span(std::extents... args) noexcept {
    //    //return std::mdspan<LayoutPolicy, AccessorPolicy>(_sequence.data(), &args...);
    //    return std::mdspan(_sequence.data(), &args...);
    //}
#if __cpp_lib_mdspan
    template<typename VALUE>
    template<std::size_t... args>
    inline constexpr auto SeqVector<VALUE>::exts() noexcept {
        return std::extents(args...);
    }

    template<typename VALUE>
    template<std::size_t... args>
    inline constexpr auto SeqVector<VALUE>::span() noexcept {
        //std::vector<std::size_t> ext_args{};
        //auto values = { args... };
        //for (auto i = values.begin(); i != values.end(); ++i) {
        //    ext_args.push_back(*i);
        //}
        //fmt::println("args = {}", ext_args);

        // return View<impl_type>(_sequence);
        using dim_ext = std::dextents<size_t, size_of_arg_pack(args...)>;
        return std::mdspan<value_type, dim_ext>(_sequence.data(), std::extents(args...));
    }
#endif

    template<typename VALUE>
    inline constexpr auto SeqVector<VALUE>::view() noexcept {
        return View<impl_type>(_sequence);
    }

    /*****************************************************************************************/
    //
    //                                 Size & Capacity Methods
    //
    /*****************************************************************************************/

    template<typename VALUE>
    inline constexpr std::size_t SeqVector<VALUE>::size() const {
        return _sequence.size();
    }

    template<typename VALUE>
    inline constexpr std::size_t SeqVector<VALUE>::max_size() const {
        return _sequence.max_size();
    }

    template<typename VALUE>
    inline constexpr std::size_t SeqVector<VALUE>::capacity() const {
        return _sequence.capacity();
    }

    template<typename VALUE>
    inline constexpr SeqVector<VALUE>& SeqVector<VALUE>::resize(std::size_t size) {
        resize(size, value_type{});
        return *this;
    }

    template<typename VALUE>
    inline constexpr SeqVector<VALUE>& SeqVector<VALUE>::resize(std::size_t size, value_type value) {
        if (size >= _sequence.size()) {
            _sequence.reserve(size);
            _sequence.resize(size, value);
        }
        else if (size > 0) {
            _sequence.resize(size);
        }
        else {
            _sequence = impl_type{};
        }
        return *this;
    }

    template<typename VALUE>
    inline constexpr SeqVector<VALUE>& SeqVector<VALUE>::reserve(std::size_t size) {
        _sequence.reserve(size);
        return *this;
    }

    template<typename VALUE>
    inline constexpr SeqVector<VALUE>& SeqVector<VALUE>::pop_back() {
        if (_sequence.size() > 0) {
            _sequence.pop_back();
        }
        return *this;
    }

    template<typename VALUE>
    inline constexpr SeqVector<VALUE>& SeqVector<VALUE>::push_back(value_type value) {
        _sequence.push_back(value);
        return *this;
    }

    template<typename VALUE>
    inline constexpr SeqVector<VALUE>& SeqVector<VALUE>::insert(std::size_t at, const auto& range) {
        return insert(at, SeqVector( std::initializer_list<value_type>{range} ));
    }

    template<typename VALUE>
    inline constexpr SeqVector<VALUE>& SeqVector<VALUE>::insert(std::size_t at, std::ranges::sized_range auto&& range) {
        // TODO add insert at beginning, ending, and in the middle algorithms.
        if (at > _sequence.size()) {
            resize(at);
        }
        _sequence.insert(std::next(_sequence.begin(), at), range._sequence.begin(), range._sequence.end());
        return *this;
    }

    template<typename VALUE>
    inline constexpr SeqVector<VALUE>& SeqVector<VALUE>::shift(int index) {
        if (index > 0) {
            rotate_left_and_drop(index);
        }
        else {
            rotate_right_and_drop(std::abs(index));
        }
        return *this;
    }

    template<typename VALUE>
    inline constexpr SeqVector<VALUE>& SeqVector<VALUE>::cshift(int index) {
        if (index > 0) {
            rotate_left(index);
        }
        else {
            rotate_right(std::abs(index));
        }
        return *this;
    }

    /*****************************************************************************************/
    //
    //                                      Apply Methods
    //
    /*****************************************************************************************/

    template<typename VALUE>
    inline constexpr SeqVector<VALUE>& SeqVector<VALUE>::apply(SeqVector<VALUE>::value_type func(SeqVector<VALUE>::value_type)) {
        const auto limit = _sequence.size(); 
        for (std::size_t i = 0; i < limit; ++i) {
            _sequence[i] = func(_sequence[i]);
        }
        return *this;
    }

    template<typename VALUE>
    inline constexpr SeqVector<VALUE>& SeqVector<VALUE>::apply(SeqVector<VALUE>::value_type func(const SeqVector<VALUE>::value_type&)) {
        const auto limit = _sequence.size();
        for (std::size_t i = 0; i < limit; ++i) {
            _sequence[i] = func(_sequence[i]);
        }
        return *this;
    }

    template<typename VALUE>
    inline constexpr SeqVector<VALUE>& SeqVector<VALUE>::apply(const SeqVector<VALUE>& b, SeqVector<VALUE>::value_type func(SeqVector<VALUE>::value_type, SeqVector<VALUE>::value_type)) {
        const auto limit = max_val(_sequence.size(), b._sequence.size());
        if (_sequence.size() < limit) {
            resize(limit + 1);
        }
        for (std::size_t i = 0; i < limit; ++i) {
            _sequence[i] = func(_sequence[i], b[i]);
        }
        return *this;
    }

    template<typename VALUE>
    inline constexpr SeqVector<VALUE>& SeqVector<VALUE>::apply(const SeqVector<VALUE>& b, SeqVector<VALUE>::value_type func(const SeqVector<VALUE>::value_type&, SeqVector<VALUE>::value_type)) {
        const auto limit = max_val(_sequence.size(), b._sequence.size());
        if (_sequence.size() < limit) {
            resize(limit + 1);
        }
        for (std::size_t i = 0; i < limit; ++i) {
            _sequence[i] = func(_sequence[i], b[i]);
            std::cout << "i = " << i << std::endl;
        }
        return *this;
    }

    template<typename VALUE>
    inline constexpr SeqVector<VALUE>& SeqVector<VALUE>::apply(const SeqVector<VALUE>& b, SeqVector<VALUE>::value_type func(SeqVector<VALUE>::value_type, const SeqVector<VALUE>::value_type&)) {
        const auto limit = max_val(_sequence.size(), b._sequence.size());
        if (_sequence.size() < limit) {
            resize(limit + 1);
        }
        for (std::size_t i = 0; i < limit; ++i) {
            _sequence[i] = func(_sequence[i], b[i]);
        }
        return *this;
    }

    template<typename VALUE>
    inline constexpr SeqVector<VALUE>& SeqVector<VALUE>::apply(const SeqVector<VALUE>& b, SeqVector<VALUE>::value_type func(const SeqVector<VALUE>::value_type& , const SeqVector<VALUE>::value_type&)) {
        const auto limit = max_val(_sequence.size(), b._sequence.size()); 
        if (_sequence.size() < limit) {
            resize(limit + 1);
        }
        for (std::size_t i = 0; i < limit; ++i) {
            _sequence[i] = func(_sequence[i], b._sequence[i]);
        }
        return *this;
    }

    /*****************************************************************************************/
    //
    //                                  Unary Math Operations
    //
    /*****************************************************************************************/

    template<typename VALUE>
    inline constexpr SeqVector<VALUE> SeqVector<VALUE>::operator+() {
        SeqVector<VALUE> a = *this;
        const auto limit = a._sequence.size(); 
        for (std::size_t i = 0; i < limit; ++i) {
            a._sequence[i] = +a._sequence[i];
        }
        return a;
    }

    template<typename VALUE>
    inline constexpr SeqVector<VALUE> SeqVector<VALUE>::operator-() {
        SeqVector<VALUE> a = *this;
        const auto limit = a._sequence.size(); 
        for (std::size_t i = 0; i < limit; ++i) {
            a._sequence[i] = -a._sequence[i];
        }
        return a;
    }

    template<typename VALUE>
    inline constexpr SeqVector<VALUE> SeqVector<VALUE>::operator~() {
        const auto limit = _sequence.size(); 
        for (std::size_t i = 0; i < limit; ++i) {
            _sequence[i] = ~_sequence[i];
        }
        return *this;
    }

    /*****************************************************************************************/
    //
    //                             Self Assigning Math Operations
    //
    /*****************************************************************************************/

    template<typename VALUE>
    inline constexpr SeqVector<VALUE>& SeqVector<VALUE>::operator+=(const SeqVector& b) {
        const auto limit = max_val(_sequence.size(), b.size());
        if (_sequence.size() < limit) {
            resize(limit + 1);
        }
        for (std::size_t i = 0; i < limit; ++i) {
            _sequence[i] += b[i];
        }
        return *this;
    }

    template<typename VALUE>
    inline constexpr SeqVector<VALUE>& SeqVector<VALUE>::operator-=(const SeqVector& b) {
        const auto limit = max_val(_sequence.size(), b.size());
        if (_sequence.size() < limit) {
            resize(limit + 1);
        }
        for (std::size_t i = 0; i < limit; ++i) {
            _sequence[i] -= b[i];
        }
        return *this;
    }

    template<typename VALUE>
    inline constexpr SeqVector<VALUE>& SeqVector<VALUE>::operator*=(const SeqVector& b) {
        const auto limit = max_val(_sequence.size(), b.size());
        if (_sequence.size() < limit) {
            resize(limit + 1);
        }
        for (std::size_t i = 0; i < limit; ++i) {
            _sequence[i] *= b[i];
        }
        return *this;
    }

    template<typename VALUE>
    inline constexpr SeqVector<VALUE>& SeqVector<VALUE>::operator/=(const SeqVector& b) {
        const auto limit = max_val(_sequence.size(), b.size());
        if (_sequence.size() < limit) {
            resize(limit + 1);
        }
        for (std::size_t i = 0; i < limit; ++i) {
            _sequence[i] /= b[i];
        }
        return *this;
    }

    template<typename VALUE>
    inline constexpr SeqVector<VALUE>& SeqVector<VALUE>::operator%=(const SeqVector& b) {
        const auto limit = max_val(_sequence.size(), b.size());
        if (_sequence.size() < limit) {
            resize(limit + 1);
        }
        for (std::size_t i = 0; i < limit; ++i) {
            _sequence[i] %= b[i];
        }
        return *this;
    }

    template<typename VALUE>
    inline constexpr SeqVector<VALUE>& SeqVector<VALUE>::operator&=(const SeqVector& b) {
        const auto limit = max_val(_sequence.size(), b.size());
        if (_sequence.size() < limit) {
            resize(limit + 1);
        }
        for (std::size_t i = 0; i < limit; ++i) {
            _sequence[i] &= b[i];
        }
        return *this;
    }

    template<typename VALUE>
    inline constexpr SeqVector<VALUE>& SeqVector<VALUE>::operator|=(const SeqVector& b) {
        const auto limit = max_val(_sequence.size(), b.size());
        if (_sequence.size() < limit) {
            resize(limit + 1);
        }
        for (std::size_t i = 0; i < limit; ++i) {
            _sequence[i] |= b[i];
        }
        return *this;
    }

    template<typename VALUE>
    inline constexpr SeqVector<VALUE>& SeqVector<VALUE>::operator^=(const SeqVector& b) {
        const auto limit = max_val(_sequence.size(), b.size());
        if (_sequence.size() < limit) {
            resize(limit + 1);
        }
        for (std::size_t i = 0; i < limit; ++i) {
            _sequence[i] ^= b[i];
        }
        return *this;
    }

    template<typename VALUE>
    inline constexpr SeqVector<VALUE>& SeqVector<VALUE>::operator<<=(const SeqVector& b) {
        const auto limit = max_val(_sequence.size(), b.size());
        if (_sequence.size() < limit) {
            resize(limit + 1);
        }
        for (std::size_t i = 0; i < limit; ++i) {
            _sequence[i] <<= b[i];
        }
        return *this;
    }

    template<typename VALUE>
    inline constexpr SeqVector<VALUE>& SeqVector<VALUE>::operator>>=(const SeqVector& b) {
        const auto limit = max_val(_sequence.size(), b.size());
        if (_sequence.size() < limit) {
            resize(limit + 1);
        }
        for (std::size_t i = 0; i < limit; ++i) {
            _sequence[i] >>= b[i];
        }
        return *this;
    }

    /*****************************************************************************************/
    //
    //                    Expression Templated Self Assigning Math Operations
    //
    /*****************************************************************************************/

    template<typename VALUE>
    template<typename RightExpr>
    inline SeqVector<VALUE>& SeqVector<VALUE>::operator=(RightExpr&& re) {
        const auto limit = max_val(_sequence.size(), re.size());
        if (_sequence.size() < limit) {
            resize(limit + 1);
        }
        for (std::size_t i = 0; i < limit; ++i) {
            _sequence[i] = re[i];
        }
        return *this;
    }

    template<typename VALUE>
    template<typename RightExpr>
    inline SeqVector<VALUE>& SeqVector<VALUE>::operator+=(RightExpr&& re) {
        const auto limit = max_val(_sequence.size(), re.size());
        if (_sequence.size() < limit) {
            resize(limit + 1);
        }
        for (std::size_t i = 0; i < limit; ++i) {
            _sequence[i] += re[i];
        }
        return *this;
    }

    template<typename VALUE>
    template<typename RightExpr>
    inline SeqVector<VALUE>& SeqVector<VALUE>::operator-=(RightExpr&& re) {
        const auto limit = max_val(_sequence.size(), re.size());
        if (_sequence.size() < limit) {
            resize(limit + 1);
        }
        for (std::size_t i = 0; i < limit; ++i) {
            _sequence[i] -= re[i];
        }
        return *this;
    }

    template<typename VALUE>
    template<typename RightExpr>
    inline SeqVector<VALUE>& SeqVector<VALUE>::operator*=(RightExpr&& re) {
        const auto limit = max_val(_sequence.size(), re.size());
        if (_sequence.size() < limit) {
            resize(limit + 1);
        }
        for (std::size_t i = 0; i < limit; ++i) {
            _sequence[i] *= re[i];
        }
        return *this;
    }

    template<typename VALUE>
    template<typename RightExpr>
    inline SeqVector<VALUE>& SeqVector<VALUE>::operator/=(RightExpr&& re) {
        const auto limit = max_val(_sequence.size(), re.size());
        if (_sequence.size() < limit) {
            resize(limit + 1);
        }
        for (std::size_t i = 0; i < limit; ++i) {
            _sequence[i] /= re[i];
        }
        return *this;
    }

    template<typename VALUE>
    template<typename RightExpr>
    inline SeqVector<VALUE>& SeqVector<VALUE>::operator%=(RightExpr&& re) {
        const auto limit = max_val(_sequence.size(), re.size());
        if (_sequence.size() < limit) {
            resize(limit + 1);
        }
        for (std::size_t i = 0; i < limit; ++i) {
            _sequence[i] %= re[i];
        }
        return *this;
    }

    template<typename VALUE>
    template<typename RightExpr>
    inline SeqVector<VALUE>& SeqVector<VALUE>::operator&=(RightExpr&& re) {
        const auto limit = max_val(_sequence.size(), re.size());
        if (_sequence.size() < limit) {
            resize(limit + 1);
        }
        for (std::size_t i = 0; i < limit; ++i) {
            _sequence[i] &= re[i];
        }
        return *this;
    }

    template<typename VALUE>
    template<typename RightExpr>
    inline SeqVector<VALUE>& SeqVector<VALUE>::operator|=(RightExpr&& re) {
        const auto limit = max_val(_sequence.size(), re.size());
        if (_sequence.size() < limit) {
            resize(limit + 1);
        }
        for (std::size_t i = 0; i < limit; ++i) {
            _sequence[i] |= re[i];
        }
        return *this;
    }

    template<typename VALUE>
    template<typename RightExpr>
    inline SeqVector<VALUE>& SeqVector<VALUE>::operator^=(RightExpr&& re) {
        const auto limit = max_val(_sequence.size(), re.size());
        if (_sequence.size() < limit) {
            resize(limit + 1);
        }
        for (std::size_t i = 0; i < limit; ++i) {
            _sequence[i] ^= re[i];
        }
        return *this;
    }

    template<typename VALUE>
    template<typename RightExpr>
    inline SeqVector<VALUE>& SeqVector<VALUE>::operator<<=(RightExpr&& re) {
        const auto limit = max_val(_sequence.size(), re.size());
        if (_sequence.size() < limit) {
            resize(limit + 1);
        }
        for (std::size_t i = 0; i < limit; ++i) {
            _sequence[i] <<= re[i];
        }
        return *this;
    }

    template<typename VALUE>
    template<typename RightExpr>
    inline SeqVector<VALUE>& SeqVector<VALUE>::operator>>=(RightExpr&& re) {
        const auto limit = max_val(_sequence.size(), re.size());
        if (_sequence.size() < limit) {
            resize(limit + 1);
        }
        for (std::size_t i = 0; i < limit; ++i) {
            _sequence[i] >>= re[i];
        }
        return *this;
    }

    /*****************************************************************************************/
    //
    //                                 Binary Expression Templates
    //
    /*****************************************************************************************/

    template<typename VALUE>
    template<typename RightExpr>
    inline auto SeqVector<VALUE>::operator+(RightExpr&& re) const -> ExprTemplate<const SeqVector&, Add_Op<value_type>, decltype(std::forward<RightExpr>(re))> {
        return ExprTemplate<const SeqVector&, Add_Op<value_type>, decltype(std::forward<RightExpr>(re))>(*this, std::forward<RightExpr>(re));
    }

    template<typename VALUE>
    template<typename RightExpr>
    inline auto SeqVector<VALUE>::operator-(RightExpr&& re) const -> ExprTemplate<const SeqVector&, Sub_Op<value_type>, decltype(std::forward<RightExpr>(re))> {
        return ExprTemplate<const SeqVector&, Sub_Op<value_type>, decltype(std::forward<RightExpr>(re))>(*this, std::forward<RightExpr>(re));
    }

    template<typename VALUE>
    template<typename RightExpr>
    inline auto SeqVector<VALUE>::operator*(RightExpr&& re) const -> ExprTemplate<const SeqVector&, Mul_Op<value_type>, decltype(std::forward<RightExpr>(re))> {
        return ExprTemplate<const SeqVector&, Mul_Op<value_type>, decltype(std::forward<RightExpr>(re))>(*this, std::forward<RightExpr>(re));
    }

    template<typename VALUE>
    template<typename RightExpr>
    inline auto SeqVector<VALUE>::operator/(RightExpr&& re) const -> ExprTemplate<const SeqVector&, Div_Op<value_type>, decltype(std::forward<RightExpr>(re))> {
        return ExprTemplate<const SeqVector&, Div_Op<value_type>, decltype(std::forward<RightExpr>(re))>(*this, std::forward<RightExpr>(re));
    }

    template<typename VALUE>
    template<typename RightExpr>
    inline auto SeqVector<VALUE>::operator%(RightExpr&& re) const -> ExprTemplate<const SeqVector&, Mod_Op<value_type>, decltype(std::forward<RightExpr>(re))> {
        return ExprTemplate<const SeqVector&, Mod_Op<value_type>, decltype(std::forward<RightExpr>(re))>(*this, std::forward<RightExpr>(re));
    }

    template<typename VALUE>
    template<typename RightExpr>
    inline auto SeqVector<VALUE>::operator&(RightExpr&& re) const -> ExprTemplate<const SeqVector&, And_Op<value_type>, decltype(std::forward<RightExpr>(re))> {
        return ExprTemplate<const SeqVector&, And_Op<value_type>, decltype(std::forward<RightExpr>(re))>(*this, std::forward<RightExpr>(re));
    }

    template<typename VALUE>
    template<typename RightExpr>
    inline auto SeqVector<VALUE>::operator|(RightExpr&& re) const -> ExprTemplate<const SeqVector&, Or_Op<value_type>, decltype(std::forward<RightExpr>(re))> {
        return ExprTemplate<const SeqVector&, Or_Op<value_type>, decltype(std::forward<RightExpr>(re))>(*this, std::forward<RightExpr>(re));
    }

    template<typename VALUE>
    template<typename RightExpr>
    inline auto SeqVector<VALUE>::operator^(RightExpr&& re) const -> ExprTemplate<const SeqVector&, Xor_Op<value_type>, decltype(std::forward<RightExpr>(re))> {
        return ExprTemplate<const SeqVector&, Xor_Op<value_type>, decltype(std::forward<RightExpr>(re))>(*this, std::forward<RightExpr>(re));
    }

    template<typename VALUE>
    template<typename RightExpr>
    inline auto SeqVector<VALUE>::operator<<(RightExpr&& re) const -> ExprTemplate<const SeqVector&, LeftShift_Op<value_type>, decltype(std::forward<RightExpr>(re))> {
        return ExprTemplate<const SeqVector&, LeftShift_Op<value_type>, decltype(std::forward<RightExpr>(re))>(*this, std::forward<RightExpr>(re));
    }

    template<typename VALUE>
    template<typename RightExpr>
    inline auto SeqVector<VALUE>::operator>>(RightExpr&& re) const -> ExprTemplate<const SeqVector&, RightShift_Op<value_type>, decltype(std::forward<RightExpr>(re))> {
        return ExprTemplate<const SeqVector&, RightShift_Op<value_type>, decltype(std::forward<RightExpr>(re))>(*this, std::forward<RightExpr>(re));
    }

    //template<typename VALUE>
    //template<typename RightExpr>
    //inline auto SeqVector<VALUE>::apply(RightExpr&& re) const->ExprTemplate<const SeqVector&, Apply_Op<value_type>, decltype(std::forward<RightExpr>(re)) > {
    //    return ExprTemplate<const SeqVector&, Apply_Op<value_type>, decltype(std::forward<RightExpr>(re))>(*this, std::forward<RightExpr>(re));
    //}

    //template<typename VALUE>
    //template<typename RightExpr>
    //inline auto SeqVector<VALUE>::apply(RightExpr&& re) const->ExprTemplate<const SeqVector&, std::function<VALUE(VALUE, VALUE)>, decltype(std::forward<RightExpr>(re)) > {
    //    return ExprTemplate<const SeqVector&, std::function<VALUE(VALUE, VALUE)>, decltype(std::forward<RightExpr>(re))>(*this, std::forward<RightExpr>(re));
    //}

    /*****************************************************************************************/
    //
    //                                     Private Methods
    //
    /*****************************************************************************************/

    template<typename VALUE>
    constexpr SeqVector<VALUE>& SeqVector<VALUE>::rotate_left(std::size_t shift) {
        //shift = _sequence.size() - shift;
        //while (shift-- > 0) {
        //    value_type last = _sequence[0];
        //    const auto limit = _sequence.size() - 1;
        //    for (std::size_t i = 0; i < limit; ++i) {
        //        _sequence[i] = _sequence[i + 1];
        //    }
        //    _sequence[_sequence.size() - 1] = last;
        //}
        if (_sequence.size() > 0) {
            shift %= _sequence.size();
            std::ranges::rotate(_sequence, _sequence.end() - shift);
        }
        return *this;
    }

    template<typename VALUE>
    constexpr SeqVector<VALUE>& SeqVector<VALUE>::rotate_left_and_drop(std::size_t shift) {
        rotate_left(shift);
        if (_sequence.size() > 0) {
            shift %= _sequence.size();
            for (std::size_t i = 0; i < shift; ++i) {
                _sequence[i] = value_type{ 0 };
            }
        }
        return *this;
    }

    template<typename VALUE>
    constexpr SeqVector<VALUE>& SeqVector<VALUE>::rotate_right(std::size_t shift) {
        //while (shift-- > 0) {
        //    value_type last = _sequence[0];
        //    const auto limit = _sequence.size() - 1;
        //    for (std::size_t i = 0; i < limit; ++i) {
        //        _sequence[i] = _sequence[i + 1];
        //    }
        //    _sequence[_sequence.size() - 1] = last;
        //}
        if (_sequence.size() > 0) {
            shift %= _sequence.size();
            std::ranges::rotate(_sequence, _sequence.begin() + shift);
        }
        return *this;
    }

    template<typename VALUE>
    constexpr SeqVector<VALUE>& SeqVector<VALUE>::rotate_right_and_drop(std::size_t shift) {
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
}
