#ifndef INTEGER_VALUE_H
#define INTEGER_VALUE_H

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

#include "../../let.h"

namespace Olly {

    /********************************************************************************************/
    //
    //                                 'integer_type' class
    //
    //        The integer_type class implements a mathematical integer_type using the C++ 'complex'
    //        integer_type type.   
    //
    //        TODO: re-implement the class using arbitrary arithmetic.
    //
    /********************************************************************************************/

    class integer_type {

    public:

        integer_type();
        integer_type(const int_type& value);
        virtual ~integer_type();

        integer_type(integer_type&& n)                  = default;
        integer_type(const integer_type& n)             = default;
        integer_type& operator= (integer_type&& n)      = default;
        integer_type& operator= (const integer_type& n) = default;

        friend str_type       _type_(const integer_type& self);
        friend bool_type        _is_(const integer_type& self);
        friend real_type      _comp_(const integer_type& self, const let& other);
        friend void            _str_(stream_type& out, const integer_type& self);
        friend void           _repr_(stream_type& out, const integer_type& self);

        friend let             _add_(const integer_type& self, const let& other);
        friend let             _sub_(const integer_type& self, const let& other);
        friend let             _mul_(const integer_type& self, const let& other);
        friend let             _div_(const integer_type& self, const let& other);
        friend let             _mod_(const integer_type& self, const let& other);
        friend let             _neg_(const integer_type& self);

        friend int_type _to_integer_(const integer_type& self);

    private:
        int_type _value;
    };

    integer_type::integer_type() : _value(0) {
    }

    integer_type::integer_type(const int_type& value) : _value(value) {
    }

    integer_type::~integer_type() {
    }

    str_type _type_(const integer_type& self) {
        return "integer_type";
    }

    bool_type _is_(const integer_type& self) {
        return self._value;
    }

    real_type _comp_(const integer_type& self, const let& other) {

        const integer_type* n = other.cast<integer_type>();

        if (n) {

            if (self._value > n->_value) {
                return 1.0;
            }

            if (self._value < n->_value) {
                return -1.0;
            }

            return 0.0;
        }

        return NOT_A_NUMBER;
    }

    void _str_(stream_type& out, const integer_type& self) {
        out << self._value;
    }

    void _repr_(stream_type& out, const integer_type& self) {
        out << "\'";
        _str_(out, self);
        out << "\'";
    }

    let _add_(const integer_type& self, const let& other) {

        const integer_type* n = other.cast<integer_type>();

        if (n) {
            return integer_type(self._value + n->_value);
        }
        return nothing();
    }

    let _sub_(const integer_type& self, const let& other) {

        const integer_type* n = other.cast<integer_type>();

        if (n) {
            return integer_type(self._value - n->_value);
        }
        return nothing();
    }

    let _mul_(const integer_type& self, const let& other) {

        const integer_type* n = other.cast<integer_type>();

        if (n) {
            return integer_type(self._value * n->_value);
        }
        return nothing();
    }

    let _div_(const integer_type& self, const let& other) {

        const integer_type* n = other.cast<integer_type>();

        if (n) {
            return integer_type(self._value / n->_value);
        }
        return nothing();
    }

    let _mod_(const integer_type& self, const let& other) {

        const integer_type* n = other.cast<integer_type>();

        if (n) {
            return integer_type(self._value % n->_value);
        }
        return nothing();
    }

    let _neg_(const integer_type& self) {

        return integer_type(-self._value);
    }

    int_type _to_integer_(const integer_type& self) {
        return self._value;
    }

}
#endif // INTEGER_VALUE_H
