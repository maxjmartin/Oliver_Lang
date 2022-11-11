#ifndef ERROR_H
#define ERROR_H

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

#include "expression.h"

namespace Olly {

    /********************************************************************************************/
    //
    //                               'error' Class Definition
    //
    //        The error class provides a wrapper around an error message to pass at
    //        runtime.  
    //
    /********************************************************************************************/


    class error {

        str_type _value;

    public:

        error();
        error(str_type val);
        virtual ~error();

        error(error&& n)                  = default;
        error(const error& n)             = default;
        error& operator= (error&& n)      = default;
        error& operator= (const error& n) = default;

        friend str_type    _type_(const error& self);
        friend bool          _is_(const error& self);
        friend real_type   _comp_(const error& self, const let& other);
        friend void         _str_(stream_type& out, const error& self);
        friend void        _repr_(stream_type& out, const error& self);
    };


    error::error() : _value("") {
    }

    error::error(str_type val) : _value(val) {
    }

    error::~error() {
    }

    str_type _type_(const error& self) {
        return "ERROR";
    }

    bool _is_(const error& self) {
        return !self._value.empty();
    }

    real_type _comp_(const error& self, const let& other) {

        const error* s = other.cast<error>();

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

    void _str_(stream_type& out, const error& self) {
        out << self._value;
    }

    void _repr_(stream_type& out, const error& self) {
        _str_(out, self);
        out << "ERROR";
    }

}
#endif // ERROR_H