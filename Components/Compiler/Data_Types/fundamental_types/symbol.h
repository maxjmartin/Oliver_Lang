#ifndef SYMBOL_H
#define SYMBOL_H

/*********************************************************************/
//
//			Copyright 2022 Max J. Martin
//
//			This file is part of Oliver.
// 
//			Oliver is free software : you can redistribute it
//			and /or modify it under the terms of the GNU General
//			Public License as published by the Free Software
//			Foundation, either version 3 of the License, or
//			(at your option) any later version.
//			
//			Oliver is distributed in the hope that it will be
//			useful, but WITHOUT ANY WARRANTY; without even the
//			implied warranty of MERCHANTABILITY or FITNESS FOR
//			A PARTICULAR PURPOSE.See the GNU General Public
//			
//			You should have received a copy of the GNU General
//			Public License along with Oliver.If not, see
//			< https://www.gnu.org/licenses/>.
//			
/*********************************************************************/

#include "../let.h"

namespace Olly {

    /********************************************************************************************/
    //
    //                                'symbol' Class Definition
    //
    //        The symbol class defines a letiable instance which exists within
    //        letiable environment of the program.  
    //
    /********************************************************************************************/


    class symbol {

        str_type _value;

    public:

        symbol();
        symbol(str_type str);
        virtual ~symbol();

        symbol(symbol&& n)                  = default;
        symbol(const symbol& n)             = default;
        symbol& operator= (symbol&& n)      = default;
        symbol& operator= (const symbol& n) = default;

        friend bool           _is_(const symbol& self);
        friend str_type     _type_(const symbol& self);
        friend real_type    _comp_(const symbol& self, const let& other);
        friend void          _str_(stream_type& out, const symbol& self);
        friend void         _repr_(stream_type& out, const symbol& self);

        friend str_type     _help_(const symbol& self);
    };


    symbol::symbol() : _value("") {
    }

    symbol::symbol(str_type str) : _value(str) {
    }

    symbol::~symbol() {
    }

    bool _is_(const symbol& self) {
        return !self._value.empty();
    }

    str_type _type_(const symbol& self) {
        return "symbol";
    }

    real_type _comp_(const symbol& self, const let& other) {

        const symbol* s = other.cast<symbol>();

        if (s) {
            if (self._value > s->_value) {
                return 1.0;
            }
            if (self._value < s->_value) {
                return -1.0;
            }
            return 0.0;
        }

        return NOT_A_NUMBER;
    }

    void _str_(stream_type& out, const symbol& self) {
        out << self._value;
    }

    void _repr_(stream_type& out, const symbol& self) {
        _str_(out, self);
    }

    str_type _help_(const symbol& self) {
        return "symbol";
    }

}
#endif // SYMBOL_H