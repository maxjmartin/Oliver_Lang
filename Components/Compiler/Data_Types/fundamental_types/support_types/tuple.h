#ifndef TUPLE_H
#define TUPLE_H

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
    //                               'tuple' Class Definition
    //
    //          The tuple class is implimented using Lisp inspired data tuples.  It
    //          is used to define the data sets as in Lisp.  
    //
    /********************************************************************************************/

    class tuple {

        let _data;
        let _next;

    public:

        tuple();
        tuple(let obj);
        virtual ~tuple();

        tuple(tuple&& n)                  = default;
        tuple(const tuple& n)             = default;
        tuple& operator= (tuple&& n)      = default;
        tuple& operator= (const tuple& n) = default;

        friend str_type           _type_(const tuple& self);
        friend bool_type            _is_(const tuple& self);
        friend real_type          _comp_(const tuple& self, const let& other);

        friend void                _str_(stream_type& out, const tuple& self);
        friend void               _repr_(stream_type& out, const tuple& self);

        friend size_type          _size_(const tuple& self);
        friend let                _lead_(const tuple& self);
        friend let          _join_(const tuple& self, const let& other);
        friend let           _drop_(const tuple& self);
        friend let             _reverse_(const tuple& self);

        friend let                 _add_(const tuple& self, const let& other);
    };

    /********************************************************************************************/
    //
    //                                 'tuple' Class Implimentation
    //
    /********************************************************************************************/

    tuple::tuple() : _data(), _next() {
    }

    tuple::tuple(let object) : _data(object), _next() {
    }

    tuple::~tuple() {
    }

    std::string _type_(const tuple& self) {
        return "tuple";
    }

    bool_type _is_(const tuple& self) {
        return self._data.is_something();
    }

    real_type _comp_(const tuple& self, const let& other) {

        const tuple* ptr = other.cast<tuple>();

        if (ptr) {

            let a = self;
            let b = *ptr;

            while (a.is() && b.is()) {

                if (a.lead() != b.lead()) {
                    return NOT_A_NUMBER;
                }

                a = a.drop();
                b = b.drop();
            }

            if (!a.is() && !b.is()) {
                return 0.0;
            }
        }

        return NOT_A_NUMBER;
    }

    void _str_(stream_type& out, const tuple& self) {

        if (!_is_(self)) {
            return;
        }

        let e(self);

        while (e.is()) {
            e.lead().str(out);

            e = e.drop();

            out << " ";
        }
    }

    void _repr_(stream_type& out, const tuple& self) {

        if (!_is_(self)) {
            return;
        }

        let e(self);

        while (e.is()) {
            e.lead().repr(out);

            e = e.drop();

            out << " ";
        }
    }

    size_type _size_(const tuple& self) {

        if (!_is_(self)) {
            return 0;
        }

        int_type size = 1;

        let next = self._next;

        while (next.is()) {

            size += 1;

            next = next.drop();
        }

        return size;
    }

    let _lead_(const tuple& self) {
        return self._data;
    }

    let _join_(const tuple& self, const let& other) {

        if (other.is_nothing()) {
            return self;
        }

        tuple a(other);

        if (_is_(self)) {

            a._next = self;
        }

        return a;
    }

    let _drop_(const tuple& self) {

        if (self._next.is_nothing()) {
            return tuple();
        }

        return self._next;
    }

    let _reverse_(const tuple& self) {

        if (self._next.is_nothing()) {
            return self;
        }

        let a = tuple();

        let next = self;

        while (next.is()) {

            a = a.join(next.lead());

            next = next.drop();
        }

        return a;
    }

    let _add_(const tuple& self, const let& other) {

        const tuple* ptr = other.cast<tuple>();

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
}
#endif // TUPLE_H