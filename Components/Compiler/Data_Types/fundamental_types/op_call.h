#ifndef OP_CALL_H
#define OP_CALL_H

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
    //                                'op_call' Class Definition
    //
    //        The op_call class encapsulates system calls to functions within the
    //        interpreter.  
    //
    /********************************************************************************************/


    class op_call {

        OP_CODE _value;

    public:

        op_call();
        op_call(OP_CODE val);
        op_call(str_type str);
        virtual ~op_call();

        op_call(op_call&& n)                  = default;
        op_call(const op_call& n)             = default;
        op_call& operator= (op_call&& n)      = default;
        op_call& operator= (const op_call& n) = default;

        friend bool           _is_(const op_call& self);
        friend str_type        _type_(const op_call& self);
        friend real_type       _comp_(const op_call& self, const let& other);
        friend void          _str_(stream_type& out, const op_call& self);
        friend void         _repr_(stream_type& out, const op_call& self);
        friend OP_CODE   _op_code_(const op_call& self);
    };


    op_call::op_call() : _value() {
    }

    op_call::op_call(OP_CODE val) : _value(val) {
    }

    op_call::op_call(str_type str) : _value() {

        auto it = OPERATORS.find(str);

        if (it != OPERATORS.end()) {

            _value = it->second;
        }
    }

    op_call::~op_call() {
    }

    bool _is_(const op_call& self) {
        return self._value != OP_CODE::nothing_op;
    }

    str_type _type_(const op_call& self) {
        return "op_call";
    }

    real_type _comp_(const op_call& self, const let& other) {

        const op_call* s = other.cast<op_call>();

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

    void _str_(stream_type& out, const op_call& self) {

        for (auto it = OPERATORS.cbegin(); it != OPERATORS.cend(); ++it) {

            if (it->second == self._value) {

                out << it->first;
                return;
            }
        }

        out << "unknown_operator";
    }

    void _repr_(stream_type& out, const op_call& self) {
        _str_(out, self);
    }

    OP_CODE _op_code_(const op_call& self) {

        return self._value;
    }

}
#endif // OP_CALL_H