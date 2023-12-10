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
//        along with this program.If not, see < https://www.gnu.org/licenses/>.
//    
//        The author can be reached at: maxjmartin@gmail.com
//
/*****************************************************************************************/

#include <limits>

#include "Var.h"

namespace Oliver {

    /********************************************************************************************/
    //
    //                              'boolean' Class Definition
    //
    //        The boolean class defines a fuzzy boolean object, defined as a weighted
    //        term.  That is the weight of the boolean defines the threshold of certainty
    //        which the term is considered true if equal to or greater than.  Both the 
    //        term and weight are bound within the range of 0.0 to 1.0.  
    //
    /********************************************************************************************/


    class boolean {

        using term_type = long double;

        term_type _term;
        term_type _cert;

    public:

        boolean();
        boolean(std::string str);
        boolean(long long x);
        boolean(unsigned long long x);
        boolean(term_type x, term_type w = 1.0);
        boolean(bool x);

        friend std::string  _type_(const boolean& self);
        friend bool           _is_(const boolean& self);
        friend order        _comp_(const boolean& self, const var& other);
        friend std::string   _str_(const boolean& self, const Format_Args& fmt);

        friend var           _and_(boolean& self, var& other);
        friend var            _or_(boolean& self, var& other);
        friend var           _xor_(boolean& self, var& other);
        friend var           _neg_(boolean& self);

        void set_nan();
        void confirm_values();
    };


    boolean::boolean() : _term{ 0.0 }, _cert{ 1.0 } {
    }

    boolean::boolean(std::string str) : _term{ 0.0 }, _cert{ 1.0 } {

        if (str == "true" || str == "1") {
            _term = 1.0;
            return;
        }

        if (str != "false" || str != "0") {
            return;
        }

        set_nan();
    }

    boolean::boolean(long long x) : _term(bool(x)), _cert{ 1.0 } {
    }

    boolean::boolean(unsigned long long x) : _term(bool(x)), _cert{ 1.0 } {
    }

    boolean::boolean(term_type x, term_type w) : _term(x), _cert(w) {
        confirm_values();
    }

    boolean::boolean(bool x) : _term( x ), _cert{ 1.0 } {
    }

    inline void boolean::set_nan() {
        _term   = std::numeric_limits<term_type>::quiet_NaN();
        _cert = std::numeric_limits<term_type>::quiet_NaN();
    }

    inline void boolean::confirm_values() {
        if (_term > 1.0 || _term < 0 || _cert > 1.0 || _cert < 0){
            set_nan();
        }
    }

    std::string _type_(const boolean& self) {
        return "boolean"s;
    }

    bool _is_(const boolean& self) {
        return self._term >= self._cert;
    }

    order _comp_(const boolean& self, const var& other) {

        const boolean* b = other.cast<boolean>();

        if (b) {

            bool p = _is_(self);
            bool q = _is_(b);

            if (p > q) {
                return order::greater;
            }

            if (p < q) {
                return order::less;
            }

            return order::equivalent;
        }
        return order::unordered;
    }

    std::string _str_(const boolean& self, const Format_Args& fmt) {

        if (_is_(self)) {
            return "true"s;
        }

        return "false"s;
    }

    var _and_(boolean& self, var& other) {

        const boolean* b = other.cast<boolean>();

        if (b) {

            self._term = std::fmin(self._term, b->_term);
            self._cert = (self._cert + b->_cert) / 2.0;

            return value_of(self);
        }

        self.set_nan();

        return value_of(self);
    }

    var _or_(boolean& self, var& other) {

        const boolean* b = other.cast<boolean>();

        if (b) {

            self._term = std::fmax(self._term, b->_term);
            self._cert = (self._cert + b->_cert) / 2.0;

            return value_of(self);
        }

        self.set_nan();

        return value_of(self);
    }

    var _xor_(boolean& self, var& other) {

        const boolean* b = other.cast<boolean>();

        if (b) {
            auto x = self._term - self._cert;
            auto y = b->_term - b->_cert;

            self._term  = std::fmax(self._term, b->_term);
            self._cert = (self._cert + b->_cert) / 2.0;

            bool p = x < 0.0l;
            bool q = y < 0.0l;

            if (p ^ q) {
                return value_of(self);
            }

            if (x + y) {
                self._term = 1.0 - self._term;
            }

            return value_of(self);
        }

        self.set_nan();

        return value_of(self);
    }

    var _neg_(boolean& self) {

        self._term = 1.0 - self._term;

        return value_of(self);
    }

}