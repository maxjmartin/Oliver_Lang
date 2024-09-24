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

#include "Var.h"
#include "Number.h"

namespace Oliver {


    /********************************************************************************************/
    //
    //                               'text' Class Definition
    //
    //        The text class provides a wrapper around normal C++ texts.  Why
    //        re-invent the wheel?
    //
    /********************************************************************************************/


    class text {

        std::string _value;

    public:

        text();
        text(std::string_view str);

        friend std::string _type_(const text& self);
        friend bool          _is_(const text& self);
        friend order       _comp_(const text& self, const var& other);
        friend std::string  _str_(const text& self, const Format_Args& fmt);

        friend var          _abs_(text& self);
        friend var         _lead_(text& self);
        friend var         _push_(text& self, var& other);
        friend var      _reverse_(text& self);

    private:
        text(char c);
    };


    text::text() : _value("") {
    }

    text::text(std::string_view str) : _value(str) {
    }

    text::text(char c) : _value("") {
        _value = c;
    }

    std::string _type_(const text& self) {
        return "text"s;
    }

    bool _is_(const text& self) {
        return !self._value.empty();
    }

    order _comp_(const text& self, const var& other) {

        const text* s = other.cast<text>();

        if (s) {

            if (self._value > s->_value) {
                return order::greater;
            }

            if (self._value < s->_value) {
                return order::less;
            }

            return order::equivalent;
        }
        return order::unordered;
    }

    std::string _str_(const text& self, const Format_Args& fmt) {
        // fmt::println("\n{}\n", fmt.print());
        return self._value;
    }

    var _abs_(text& self) {
        return number(self._value.size());
    }

    var _lead_(text& self) {
        return self._value.empty() ? var() : text(self._value.front());
    }

    var _push_(text& self, var& other) {

        const text* s = other.cast<text>();

        if (s) {

            self._value = s->_value + self._value;

            return std::move(self);
        }

        return nothing();
    }

    var _reverse_(text& self) {

        std::reverse(self._value.begin(), self._value.end());

        return std::move(self);
    }
}