#ifndef EXPRESSION_H
#define EXPRESSION_H

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

#include "./support_types/tuple.h"

namespace Olly {

    /********************************************************************************************/
    //
    //                               'expression' Class Definition
    //
    //          The expression class is implimented using Lisp inspired data nodes.  It
    //          is used to define the data sets as in Lisp.  
    //
    /********************************************************************************************/

    class expression {

        let      _expression;
        int_type _size;

    public:

        expression();
        expression(let x);
        virtual ~expression();

        expression(expression&& n) = default;
        expression(const expression& n) = default;
        expression& operator= (expression&& n) = default;
        expression& operator= (const expression& n) = default;

        friend str_type           _type_(const expression& self);
        friend bool_type            _is_(const expression& self);
        friend real_type          _comp_(const expression& self, const let& other);

        friend void                _str_(stream_type& out, const expression& self);
        friend void               _repr_(stream_type& out, const expression& self);

        friend size_type          _size_(const expression& self);
        friend let                _lead_(const expression& self);
        friend let          _join_(const expression& self, const let& other);
        friend let           _drop_(const expression& self);
        friend let             _reverse_(const expression& self);

        friend let                 _add_(const expression& self, const let& other);
    };

    let make_pair(let key, let val);

    /********************************************************************************************/
    //
    //                                 'expression' Class Implimentation
    //
    /********************************************************************************************/

    expression::expression() : _expression(tuple()), _size(0) {
    }

    expression::expression(let x) : _expression(tuple(x)), _size(static_cast<int_type>(_expression.size())) {
    }

    expression::~expression() {
    }

    std::string _type_(const expression& self) {
        return "expression";
    }

    bool_type _is_(const expression& self) {
        return self._size;
    }

    real_type _comp_(const expression& self, const let& other) {

        const expression* ptr = other.cast<expression>();

        if (ptr) {

            if (self._size == ptr->_size) {

                return self._expression.comp(ptr->_expression);
            }
        }

        return NOT_A_NUMBER;
    }

    void _str_(stream_type& out, const expression& self) {

        out << "(";

        if (self._expression.is()) {

            self._expression.str(out);
            out.seekp(-1, out.cur);
        }

        out << ")";
    }

    void _repr_(stream_type& out, const expression& self) {

        out << "(";

        if (self._expression.is()) {

            self._expression.repr(out);
            out.seekp(-1, out.cur);
        }

        out << ")";
    }

    size_type _size_(const expression& self) {
        return self._size;
    }

    let _lead_(const expression& self) {
        return self._expression.lead();
    }

    let _join_(const expression& self, const let& other) {

        if (other.is_nothing()) {
            return self;
        }

        expression a(self);

        a._expression = a._expression.join(other);
        a._size += 1;

        return a;
    }

    let _drop_(const expression& self) {

        if (!_is_(self)) {
            return expression();
        }

        expression a(self);

        a._expression = a._expression.drop();
        a._size -= 1;

        return a;
    }

    let _reverse_(const expression& self) {

        if (self._expression.is_nothing()) {
            return self;
        }

        expression a;

        a._expression = self._expression.reverse();
        a._size = self._size;

        return a;
    }

    let _add_(const expression& self, const let& other) {

        const expression* ptr = other.cast<expression>();

        if (ptr) {

            let a = _reverse_(self);
            let b = *ptr;

            while (a.is()) {
                b = b.join(pop_lead(a));
            }

            return b;
        }

        return nothing();
    }

    let make_pair(let key, let val) {

        let exp = expression();

        exp = exp.join(val);
        exp = exp.join(key);

        return exp;
    }
}
#endif // EXPRESSION_H