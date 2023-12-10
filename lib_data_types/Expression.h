#pragma once

/*****************************************************************************************/
//
//                           Copyright(C) 2023 Max J Martin
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
//        along with this program.If not, see < https://www.gnu.org/licenses/>.
//    
//        The author can be reached at: maxjmartin@gmail.com
//
/*****************************************************************************************/

#include <tuple>

#include "Var.h"
#include "Number.h"

namespace Oliver {

    /********************************************************************************************/
    //
    //                               'expression' Class Definition
    //
    //          The expression class manages evaluatable term(s).  It is simply a
    //          wrapper around a std::vector<var>.  The key diffrence is that the
    //          order of sequence is reversed from the order of a vector.
    //
    /********************************************************************************************/

    class expression {

        std::vector<var> _expr;

    public:

        expression();
        expression(var x);

        friend std::string          _type_(const expression& self);
        friend std::size_t     _size_type_(const expression& self);
        friend bool                   _is_(const expression& self);
        friend auto                 _comp_(const expression& self, const var& other);

        friend std::string           _str_(const expression& self, const Format_Args& fmt);

        friend var                  _lead_(expression& self);
        friend var                  _push_(expression& self, var& other);
        friend var                  _next_(expression& self);
        friend var               _reverse_(expression& self);

        friend var                   _add_(expression& self, var& other);
    };

    var make_pair(var a, var b);
    var unwrap_expresion(var exp);

    /********************************************************************************************/
    //
    //                                 'expression' Class Implementation
    //
    /********************************************************************************************/

    expression::expression() : _expr() {
    }

    expression::expression(var x) : _expr() {
        _expr.push_back(x);
    }

    std::string _type_(const expression& self) {
        return "expression"s;
    }

    std::size_t _size_type_(const expression& self) {
        return self._expr.size();
    }

    bool _is_(const expression& self) {
        return self._expr.size();
    }

    auto _comp_(const expression& self, const var& other) {

        const expression* ptr = other.cast<expression>();

        if (ptr) {

            if (self._expr == ptr->_expr) {
                return order::equivalent;
            }
        }

        return order::unordered;
    }

    std::string _str_(const expression& self, const Format_Args& fmt) {

        if (!_is_(self)) {
            return "()"s;
        }

        std::string result = fmt::format("{}",self._expr.back().str(fmt));

        for (auto i = self._expr.crbegin() + 1; i != self._expr.crend(); ++i) {
            result = fmt::format("{}, {}", result, i->str(fmt));
        }

        return fmt::format("({})", result);
    }

    var _lead_(expression& self) {

        if (!_is_(self)) {
            return var();
        }

        var a(std::move(self._expr.back()));
        self._expr.pop_back();

        return a;
    }

    var _push_(expression& self, var& other) {

        if (other.is_nothing()) {
            return value_of(self);
        }

        self._expr.push_back(std::move(other));
        other = var();

        return value_of(self);
    }

    var _next_(expression& self) {

        if (!self._expr.empty()) {
            self._expr.pop_back();
        }

        return value_of(self);
    }

    var _reverse_(expression& self) {

        if (self._expr.empty()) {
            return value_of(self);
        }

        std::reverse(self._expr.begin(), self._expr.end());

        return value_of(self);
    }

    var _add_(expression& self, var& other) {

        if (other.type() == "expression") {
            auto ptr = other.move<expression>();

            ptr->_expr.insert(
                ptr->_expr.end(),
                std::make_move_iterator(self._expr.begin()),
                std::make_move_iterator(self._expr.end())
            );

            self._expr = std::move(ptr->_expr);

            return value_of(self);
        }

        return var();
    }

    inline var make_pair(var a, var b) {
        b = expression(b);
        b = b.push(a);
        return b;
    }

    inline var unwrap_expresion(var exp) {

        while (exp && exp.size_type() == 1ull) {

            var a = exp.lead();

            if (a.type() != "expression") {
                return expression(a);
            }
            exp = std::move(a);
        }

        return exp;
    }
}