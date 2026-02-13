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
//        along with this program.If not, see <https://www.gnu.org/licenses/>.
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
        friend var                  _drop_(expression& self);
        friend var                 _shift_(expression& self);
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
        _expr.push_back(std::move(x));
    }

    std::string _type_(const expression& self) {
        (void)self;
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

        std::string result = std::format("{}",self._expr.back().str(fmt));

        for (auto i = self._expr.crbegin() + 1; i != self._expr.crend(); ++i) {
            result = std::format("{}, {}", result, i->str(fmt));
        }

        return std::format("({})", result);
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
            return std::move(self);
        }

        self._expr.push_back(std::move(other));
        other = var();

        return std::move(self);
    }

    var _drop_(expression& self) {

        if (!self._expr.empty()) {
            self._expr.pop_back();
        }

        return std::move(self);
    }

    var _shift_(expression& self) {

        var a = _lead_(self);

        a = make_pair(std::move(a), self);

        return a;
    }

    var _reverse_(expression& self) {

        if (self._expr.empty()) {
            return std::move(self);
        }

        std::reverse(self._expr.begin(), self._expr.end());

        return std::move(self);
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

            return std::move(self);
        }

        return var();
    }

    var make_pair(var a, var b) {
        b = expression(std::move(b));
        b = b.push(a);
        return b;
    }

    var unwrap_expresion(var exp) {

        while (exp && exp.size_type() == uint64_t(1)) {

            var a = exp.lead();

            if (a.type() != "expression") {
                return expression(std::move(a));
            }
            exp = std::move(a);
        }

        return exp;
    }
}
