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

namespace Oliver {

    /********************************************************************************************/
    //
    //                                'op_call' Class Definition
    //
    //        The op_call class encapsulates system calls to functions within the
    //        interpreter.  
    //
    /********************************************************************************************/


    class op_call {

        op_code _value;

    public:

        op_call(op_code val);
        op_call(std::string str);

        friend bool                  _is_(const op_call& self);
        friend std::string         _type_(const op_call& self);
        friend std::size_t    _size_type_(const op_call& self);
        friend order               _comp_(const op_call& self, const var& other);
        friend std::string          _str_(const op_call& self, const Format_Args& fmt);
        friend op_code          _op_call_(const op_call& self);
    };

    op_call::op_call(op_code val) : _value(val) {
    }

    op_call::op_call(std::string str) : _value(op_code::nothing_op) {

        auto it = operators.find(str);

        if (it != operators.end()) {

            _value = it->second;
        }
    }

    bool _is_(const op_call& self) {
        return self._value != op_code::nothing_op;
    }

    std::string _type_(const op_call& self) {
        return "op_call"s;
    }

    std::size_t _size_type_(const op_call& self){
        return static_cast<std::size_t>(self._value);
    }

    order _comp_(const op_call& self, const var& other) {

        const op_call* s = other.cast<op_call>();

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

    std::string _str_(const op_call& self, const Format_Args& fmt) {

        for (auto it = operators.cbegin(); it != operators.cend(); ++it) {

            if (it->second == self._value) {
                return it->first;
            }
        }
        return "unknown_operator"s;
    }

    op_code _op_call_(const op_call& self) {

        return self._value;
    }

}