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

#include <type_traits>

#include "Operator_Templates.h"

namespace Oliver {

    /********************************************************************************************/
    //
    //                                'ExprTemplate' class
    //
    //        The ExprTemplate class implements the expression templates used
    //        by the WordVector class. 
    //
    //        Due to the brevity of the code, it has been inlined within the struct
    //        definitions.  With the length of the expression template return types
    //        being as long as they are, arrow return formatting is used to try and
    //        increase the readability of the code.  
    //
    /********************************************************************************************/

    template <typename LeftExpr, typename BinaryOp, typename RightExpr>
    class ExprTemplate {

    public:
        typedef typename std::remove_reference<LeftExpr>::type::value_type value_type;

        ExprTemplate(LeftExpr l, RightExpr r) : _left_expr(std::forward<LeftExpr>(l)), _right_expr(std::forward<RightExpr>(r)) {
        }

        ExprTemplate()                                = delete;
        ExprTemplate(ExprTemplate const&)             = delete;
        ExprTemplate& operator =(ExprTemplate const&) = delete;

        ExprTemplate(ExprTemplate&&)                  = default;
        ExprTemplate& operator =(ExprTemplate&&)      = default;

        template <typename RE>
        constexpr auto operator +(RE&& re) const -> ExprTemplate<
                                            ExprTemplate<
                                                LeftExpr,
                                                BinaryOp,
                                                RightExpr
                                            > const&,
                                            Add_Op<value_type>,
                                            decltype(std::forward<RE>(re))
                                        > {
            return ExprTemplate<ExprTemplate<LeftExpr, BinaryOp, RightExpr> const&, Add_Op<value_type>, decltype(std::forward<RE>(re))>(*this, std::forward<RE>(re));
        }

        template <typename RE>
        constexpr auto operator -(RE&& re) const -> ExprTemplate<ExprTemplate<LeftExpr, BinaryOp, RightExpr> const&, Sub_Op<value_type>, decltype(std::forward<RE>(re)) > {
            return ExprTemplate<ExprTemplate<LeftExpr, BinaryOp, RightExpr> const&, Sub_Op<value_type>, decltype(std::forward<RE>(re))>(*this, std::forward<RE>(re));
        }

        template <typename RE>
        constexpr auto operator *(RE&& re) const -> ExprTemplate<ExprTemplate<LeftExpr, BinaryOp, RightExpr> const&, Mul_Op<value_type>, decltype(std::forward<RE>(re))> {
            return ExprTemplate<ExprTemplate<LeftExpr, BinaryOp, RightExpr> const&, Mul_Op<value_type>, decltype(std::forward<RE>(re))>(*this, std::forward<RE>(re));
        }

        template <typename RE>
        constexpr auto operator /(RE&& re) const -> ExprTemplate<ExprTemplate<LeftExpr, BinaryOp, RightExpr> const&, Div_Op<value_type>, decltype(std::forward<RE>(re))> {
            return ExprTemplate<ExprTemplate<LeftExpr, BinaryOp, RightExpr> const&, Div_Op<value_type>, decltype(std::forward<RE>(re))>(*this, std::forward<RE>(re));
        }

        template <typename RE>
        constexpr auto operator %(RE&& re) const -> ExprTemplate<ExprTemplate<LeftExpr, BinaryOp, RightExpr> const&, Mod_Op<value_type>, decltype(std::forward<RE>(re))> {
            return ExprTemplate<ExprTemplate<LeftExpr, BinaryOp, RightExpr> const&, Mod_Op<value_type>, decltype(std::forward<RE>(re))>(*this, std::forward<RE>(re));
        }

        template <typename RE>
        constexpr auto operator &(RE&& re) const -> ExprTemplate<ExprTemplate<LeftExpr, BinaryOp, RightExpr> const&, And_Op<value_type>, decltype(std::forward<RE>(re))> {
            return ExprTemplate<ExprTemplate<LeftExpr, BinaryOp, RightExpr> const&, And_Op<value_type>, decltype(std::forward<RE>(re))>(*this, std::forward<RE>(re));
        }

        template <typename RE>
        constexpr auto operator |(RE&& re) const -> ExprTemplate<ExprTemplate<LeftExpr, BinaryOp, RightExpr> const&, Or_Op<value_type>, decltype(std::forward<RE>(re))> {
            return ExprTemplate<ExprTemplate<LeftExpr, BinaryOp, RightExpr> const&, Or_Op<value_type>, decltype(std::forward<RE>(re))>(*this, std::forward<RE>(re));
        }

        template <typename RE>
        constexpr auto operator ^(RE&& re) const -> ExprTemplate<ExprTemplate<LeftExpr, BinaryOp, RightExpr> const&, Xor_Op<value_type>, decltype(std::forward<RE>(re))> {
            return ExprTemplate<ExprTemplate<LeftExpr, BinaryOp, RightExpr> const&, Xor_Op<value_type>, decltype(std::forward<RE>(re))>(*this, std::forward<RE>(re));
        }

        template <typename RE>
        constexpr auto operator <<(RE&& re) const -> ExprTemplate<ExprTemplate<LeftExpr, BinaryOp, RightExpr> const&, LeftShift_Op<value_type>, decltype(std::forward<RE>(re))> {
            return ExprTemplate<ExprTemplate<LeftExpr, BinaryOp, RightExpr> const&, LeftShift_Op<value_type>, decltype(std::forward<RE>(re))>(*this, std::forward<RE>(re));
        }

        template <typename RE>
        constexpr auto operator >>(RE&& re) const -> ExprTemplate<ExprTemplate<LeftExpr, BinaryOp, RightExpr> const&, RightShift_Op<value_type>, decltype(std::forward<RE>(re))> {
            return ExprTemplate<ExprTemplate<LeftExpr, BinaryOp, RightExpr> const&, RightShift_Op<value_type>, decltype(std::forward<RE>(re))>(*this, std::forward<RE>(re));
        }

        template <typename RE>
        constexpr auto apply(RE&& re) const -> ExprTemplate<ExprTemplate<LeftExpr, BinaryOp, RightExpr> const&, Apply_Op<value_type>, decltype(std::forward<RE>(re))> {
            return (*this)(re);
        }

        constexpr auto left_expr() -> typename std::add_lvalue_reference<LeftExpr>::type {
            return _left_expr;
        }

        constexpr auto left_expr() const -> typename std::add_lvalue_reference<typename std::add_const<LeftExpr>::type>::type {
            return _left_expr;
        }

        constexpr auto right_expr() -> typename std::add_lvalue_reference<RightExpr>::type {
            return _right_expr;
        }

        constexpr auto right_expr() const -> typename std::add_lvalue_reference<typename std::add_const<RightExpr>::type >::type {
            return _right_expr;
        }

        constexpr auto operator [](std::size_t index) const -> value_type {
            if constexpr (requires { left_expr()[index]; } && requires { right_expr()[index]; }) {
                // Both are subscriptable
                return BinaryOp::apply(left_expr()[index], right_expr()[index]);
            } else if constexpr (requires { left_expr()[index]; }) {
                // Only left is subscriptable
                return BinaryOp::apply(left_expr()[index], right_expr());
            } else if constexpr (requires { right_expr()[index]; }) {
                // Only right is subscriptable
                return BinaryOp::apply(left_expr(), right_expr()[index]);
            } else {
                // Neither is subscriptable, just apply directly
                return BinaryOp::apply(left_expr(), right_expr());
            }
        }

        constexpr auto size() const -> std::size_t {
            return left_expr().size() != 0 ? left_expr().size() : right_expr().size();
        }

        constexpr auto operator()(value_type x) const -> value_type {
            (void)x;
            return BinaryOp::apply(left_expr(), right_expr());
        }

    private:
        LeftExpr  _left_expr;
        RightExpr _right_expr;
    };
}
