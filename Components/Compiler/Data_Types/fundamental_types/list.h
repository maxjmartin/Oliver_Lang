#ifndef LIST_H
#define LIST_H

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
    //                               'list' Class Definition
    //
    //          The list class is implimented using Lisp inspired data nodes.  It
    //          is used to define the data sets as in Lisp.  
    //
    /********************************************************************************************/

    class list {

        let      _list;
        int_type _size;

    public:

        list();
        list(let x);
        virtual ~list();

        list(list&& n) = default;
        list(const list& n) = default;
        list& operator= (list&& n) = default;
        list& operator= (const list& n) = default;

        friend str_type           _type_(const list& self);
        friend bool_type            _is_(const list& self);
        friend real_type          _comp_(const list& self, const let& other);

        friend void                _str_(stream_type& out, const list& self);
        friend void               _repr_(stream_type& out, const list& self);

        friend size_type          _size_(const list& self);
        friend let                _lead_(const list& self);
        friend let          _join_(const list& self, const let& other);
        friend let           _drop_(const list& self);
        friend let             _reverse_(const list& self);

        friend let                 _add_(const list& self, const let& other);
    };

    /********************************************************************************************/
    //
    //                                 'list' Class Implimentation
    //
    /********************************************************************************************/

    list::list() : _list(tuple()), _size(0) {
    }

    list::list(let x) : _list(tuple(x)), _size(static_cast<int_type>(_list.size())) {
    }

    list::~list() {
    }

    std::string _type_(const list& self) {
        return "list";
    }

    bool_type _is_(const list& self) {
        return self._size;
    }

    real_type _comp_(const list& self, const let& other) {

        const list* ptr = other.cast<list>();

        if (ptr) {

            if (self._size == ptr->_size) {

                return self._list.comp(ptr->_list);
            }
        }

        return NOT_A_NUMBER;
    }

    void _str_(stream_type& out, const list& self) {

        out << "[";

        if (self._list.is()) {

            self._list.str(out);
            out.seekp(-1, out.cur);
        }

        out << "]";
    }

    void _repr_(stream_type& out, const list& self) {
        
        out << "[";

        if (self._list.is()) {

            self._list.repr(out);
            out.seekp(-1, out.cur);
        }

        out << "]";
    }

    size_type _size_(const list& self) {
        return self._size;
    }

    let _lead_(const list& self) {
        return self._list.lead();
    }

    let _join_(const list& self, const let& other) {

        if (other.is_nothing()) {
            return self;
        }

        list a(self);

        a._list = a._list.join(other);
        a._size += 1;

        return a;
    }

    let _drop_(const list& self) {

        if (!_is_(self)) {
            return list();
        }

        list a(self);

        a._list = a._list.drop();
        a._size -= 1;

        return a;
    }

    let _reverse_(const list& self) {

        if (self._list.is_nothing()) {
            return self;
        }

        list a;

        a._list = self._list.reverse();
        a._size = self._size;

        return a;
    }

    let _add_(const list& self, const let& other) {

        const list* ptr = other.cast<list>();

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
#endif // LIST_H