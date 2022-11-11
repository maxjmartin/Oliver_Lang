#ifndef LOGICAL_TERM_H
#define LOGICAL_TERM_H

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
    //                              'logical_term' Class Definition
    //
    //        The logical_term class defines a fuzzy logical_term object, defined as a weighted
    //        term.  That is the weight of the logical_term defines the threshold of certainty
    //        which the term is considered true if equal to or greater than.  Both the 
    //        term and weight are bound within the range of 0.0 to 1.0.  
    //
    /********************************************************************************************/


    class logical_term {

        real_type _term;
        real_type _weight;

    public:

        logical_term();
        logical_term(std::string str);
        logical_term(const int_type& n, real_type t = 1.0);
        logical_term(const bool_type& n, real_type t = 1.0);
        logical_term(const real_type& n, real_type t = 1.0);
        virtual ~logical_term();

        logical_term(logical_term&& n)                  = default;
        logical_term(const logical_term& n)             = default;
        logical_term& operator= (logical_term&& n)      = default;
        logical_term& operator= (const logical_term& n) = default;

        friend str_type      _type_(const logical_term& self);
        friend bool_type       _is_(const logical_term& self);
        friend real_type     _comp_(const logical_term& self, const let& other);
        friend void           _str_(stream_type& out, const logical_term& self);
        friend void          _repr_(stream_type& out, const logical_term& self);

        friend let          _l_and_(const logical_term& self, const let& other);
        friend let           _l_or_(const logical_term& self, const let& other);
        friend let          _l_xor_(const logical_term& self, const let& other);
        friend let            _neg_(const logical_term& self);
    };


    logical_term::logical_term() : _term(0.0), _weight(1.0) {
    }

    logical_term::logical_term(std::string str) : _term(0.0), _weight(1.0) {

        if (str == "TRUE" || str == "1") {

            _term = 1.0;
            return;
        }

        if (str == "FALSE" || str == "0") {

            _term = 0.0;
            return;
        }

        if (str == "UNDEF" || str == "UNDEFINED") {

            _term = NOT_A_NUMBER;
            _weight = NOT_A_NUMBER;
            return;
        }
    }

    logical_term::logical_term(const int_type& n, real_type t) : _term(0.0), _weight(NOT_A_NUMBER) {

        if (n) {
            _term = 1.0;
        }

        if (t <= 1.0 && t >= 0) {
            _weight = t;
        }
    }

    logical_term::logical_term(const bool_type& n, real_type t) : _term(0.0), _weight(NOT_A_NUMBER) {

        if (n) {
            _term = 1.0;
        }

        if (t <= 1.0 && t >= 0) {
            _weight = t;
        }
    }

    logical_term::logical_term(const real_type& n, real_type t) : _term(NOT_A_NUMBER), _weight(NOT_A_NUMBER) {

        if (n <= 1.0 && n >= 0) {
            _term = n;
        }

        if (t <= 1.0 && t >= 0) {
            _weight = t;
        }
    }

    logical_term::~logical_term() {
    }

    std::string _type_(const logical_term& self) {
        return "logical_term";
    }

    bool_type _is_(const logical_term& self) {
        return self._term >= self._weight;
    }

    real_type _comp_(const logical_term& self, const let& other) {

        const logical_term* b = other.cast<logical_term>();

        if (b) {

            bool_type p = _is_(self);
            bool_type q = _is_(b);

            if (p > q) {
                return 1.0;
            }

            if (p < q) {
                return -1.0;
            }

            return 0.0;
        }

        return NOT_A_NUMBER;
    }

    void _str_(stream_type& out, const logical_term& self) {

        if (self._term != self._term) {

            out << "undefined";

            return;
        }

        out << _is_(self);
    }

    void _repr_(stream_type& out, const logical_term& self) {

        out << "('" << self._term << "' '" << self._weight << "' BOOL)";
    }

    let Olly::_l_and_(const logical_term& self, const let& other) {

        const logical_term* b = other.cast<logical_term>();

        if (b) {

            real_type t = std::fmin(self._term, b->_term);

            real_type w = (self._weight + b->_weight) / 2.0;

            return logical_term(t, w);
        }

        return logical_term(NOT_A_NUMBER);
    }

    let Olly::_l_or_(const logical_term& self, const let& other) {

        const logical_term* b = other.cast<logical_term>();

        if (b) {

            real_type t = std::fmax(self._term, b->_term);

            real_type w = (self._weight + b->_weight) / 2.0;

            return logical_term(t, w);
        }

        return logical_term(NOT_A_NUMBER);
    }

    let Olly::_l_xor_(const logical_term& self, const let& other) {

        const logical_term* b = other.cast<logical_term>();

        if (b) {

            real_type t = std::fmax(self._term, b->_term);

            real_type w = (self._weight + b->_weight) / 2.0;

            logical_term res(t, w);

            
            real_type x = self._term - self._weight;

            real_type y = b->_term - b->_weight;

            if ((x < 0.0) ^ (y < 0.0)) {

                return res;
            }

            return x + y ? res : _neg_(res);
        }

        return logical_term(NOT_A_NUMBER);
    }

    let Olly::_neg_(const logical_term& self) {

        return logical_term(1.0 - self._term, self._weight);
    }

}
#endif // LOGICAL_TERM_H