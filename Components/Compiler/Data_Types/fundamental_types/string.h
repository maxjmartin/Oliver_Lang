#ifndef STRING_H
#define STRING_H

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
    //                               'string' Class Definition
    //
    //        The string class provides a wrapper around normal C++ strings.  Why
    //        re-invent the wheel?
    //
    /********************************************************************************************/


    class string {

        str_type _value;

    public:

        string();
        string(str_type str);
        string(const char& c);
        virtual ~string();

        string(string&& n)                  = default;
        string(const string& n)             = default;
        string& operator= (string&& n)      = default;
        string& operator= (const string& n) = default;

        friend str_type    _type_(const string& self);
        friend bool          _is_(const string& self);
        friend real_type   _comp_(const string& self, const let& other);
        friend void         _str_(stream_type& out, const string& self);
        friend void        _repr_(stream_type& out, const string& self);

        friend int_type    _size_(const string& self);
        friend let         _lead_(const string& self);
        friend let        _place_(const string& self, const let& other);
        friend let        _shift_(const string& self);
        friend let      _reverse_(const string& self);

        friend bool_type  _iterable_(const string& self);
    };


    string::string() : _value("") {
    }

    string::string(str_type str) : _value(str) {
    }

    string::string(const char& c) : _value(str(c)) {
    }

    string::~string() {
    }

    str_type _type_(const string& self) {
        return "string";
    }

    bool _is_(const string& self) {
        return !self._value.empty();
    }

    real_type _comp_(const string& self, const let& other) {

        const string* s = other.cast<string>();

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

    void _str_(stream_type& out, const string& self) {
        out << self._value;
    }

    void _repr_(stream_type& out, const string& self) {
        out << "\"";
        _str_(out, self);
        out << "\"";
    }

    int_type _size_(const string& self) {
        return (int_type)self._value.size();
    }

    let _lead_(const string& self) {

        return string(self._value.front());
    }

    let _place_(const string& self, const let& other) {

        const string* s = other.cast<string>();

        if (s) {

            string r;

            r._value = s->_value + self._value;

            return r;
        }

        return nothing();
    }

    let _shift_(const string& self) {

        if (self._value.empty()) {
            return self;
        }

        string t = self;
        t._value.erase(0, 1);

        return t;
    }

    let _reverse_(const string& self) {

        string l = self;

        std::reverse(l._value.begin(), l._value.end());

        return l;
    }

    bool_type _iterable_(const string& self) {
        return true;
    }

}
#endif // STRING_H
