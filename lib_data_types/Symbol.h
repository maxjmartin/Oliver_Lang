#pragma once

/********************************************************************************************/
//
//			Copyright 2019 Max J. Martin
//
//			This file is part of Oliver.
//			
//			Oliver is free software : you can redistribute it and / or modify
//			it under the terms of the GNU General Public License as published by
//			the Free Software Foundation, either version 3 of the License, or
//			(at your option) any later version.
//			
//			Oliver is distributed in the hope that it will be useful,
//			but WITHOUT ANY WARRANTY; without even the implied warranty of
//			MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//			GNU General Public License for more details.
//			
//			You should have received a copy of the GNU General Public License
//			along with Oliver.If not, see < https://www.gnu.org/licenses/>.
//			
/********************************************************************************************/

#include "Var.h"

namespace Oliver {

    /********************************************************************************************/
    //
    //                                'symbol' Class Definition
    //
    //        The symbol class defines a variable instance which exists within
    //        variable environment of the program.  
    //
    /********************************************************************************************/


    class symbol {

        std::string _value;

    public:

        symbol();
        symbol(std::string str);

        friend bool           _is_(const symbol& self);
        friend std::string  _type_(const symbol& self);
        friend order        _comp_(const symbol& self, const var& other);
        friend std::string   _str_(const symbol& self, const Format_Args& fmt);

        friend std::string     _help_(const symbol& self);
    };


    symbol::symbol() : _value("") {
    }

    symbol::symbol(std::string str) : _value(str) {
    }

    bool _is_(const symbol& self) {
        return !self._value.empty();
    }

    std::string _type_(const symbol& self) {
        return "symbol"s;
    }

    order _comp_(const symbol& self, const var& other) {

        const symbol* s = other.cast<symbol>();

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

    std::string _str_(const symbol& self, const Format_Args& fmt) {
        return self._value;
    }

    std::string _help_(const symbol& self) {
        return "symbol"s;
    }

}