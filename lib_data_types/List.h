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

#include "Var.h"
#include "Number.h"

namespace Oliver {

    /********************************************************************************************/
    //
    //                               'list' Class Definition
    //
    //          The list class is implemented as a wrapper around a std::vector<var>.  
    //          The order of sequence for a list is reversed from that of a vector.   
    //
    /********************************************************************************************/

    class list {

        std::vector<var> _expr;

    public:

        list();
        list(var x);

        friend std::string          _type_(const list& self);
        friend std::size_t     _size_type_(const list& self);
        friend bool                   _is_(const list& self);
        friend auto                 _comp_(const list& self, const var& other);

        friend std::string           _str_(const list& self, const Format_Args& fmt);

        friend var                  _lead_(list& self);
        friend var                  _push_(list& self, var& other);
        friend var                  _next_(list& self);
        friend var               _reverse_(list& self);

        friend var                   _add_(list& self, var& other);
    };

    /********************************************************************************************/
    //
    //                                 'list' Class Implementation
    //
    /********************************************************************************************/

    list::list() : _expr() {
    }

    list::list(var x) : _expr() {
        _expr.push_back(x);
    }

    std::string _type_(const list& self) {
        return "list"s;
    }

    std::size_t _size_type_(const list& self) {
        return self._expr.size();
    }

    bool _is_(const list& self) {
        return self._expr.size();
    }

    auto _comp_(const list& self, const var& other) {

        const list* ptr = other.cast<list>();

        if (ptr) {

            if (self._expr == ptr->_expr) {
                return order::equivalent;
            }
        }

        return order::unordered;
    }

    std::string _str_(const list& self, const Format_Args& fmt) {

        if (!_is_(self)) {
            return "[]"s;
        }

        std::string result = fmt::format("{}", self._expr.back().str(fmt));

        for (auto i = self._expr.crbegin() + 1; i != self._expr.crend(); ++i) {
            result = fmt::format("{}, {}", result, i->str(fmt));
        }

        return fmt::format("[{}]", result);
    }

    var _lead_(list& self) {

        if (self._expr.empty()) {
            return var();
        }
        return self._expr.back();
    }

    var _push_(list& self, var& other) {

        if (other.is_nothing()) {
            return value_of(self);
        }

        self._expr.push_back(other);

        return value_of(self);
    }

    var _next_(list& self) {

        if (!self._expr.empty()) {
            self._expr.pop_back();
        }

        return value_of(self);
    }

    var _reverse_(list& self) {

        if (self._expr.empty()) {
            return value_of(self);
        }

        std::reverse(self._expr.begin(), self._expr.end());

        return value_of(self);
    }

    var _add_(list& self, var& other) {

        if (other.type() == "list") {
            auto ptr = other.move<list>();

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
}