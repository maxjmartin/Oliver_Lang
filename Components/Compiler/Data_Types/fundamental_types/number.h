#ifndef NUMBER_H
#define NUMBER_H

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

#include <complex>
#include "../let.h"

namespace Olly {

    /********************************************************************************************/
    //
    //                                 'number' class
    //
    //        The number class implements a mathematical number using the C++ 'complex'
    //        number type.   
    //
    //        TODO: re-implement the class using arbitrary arithmetic.
    //
    /********************************************************************************************/

    class number {
        typedef		std::complex<real_type>		num_type;

    public:

        number();
        number(const num_type& value);
        number(str_type str);
        number(const int_type& value);
        number(const real_type& value);
        number(const real_type& real, const real_type& imgn);
        number(const unsigned long long& value);
        number(const unsigned long& value);
        number(const unsigned int& value);
        virtual ~number();

        number(number&& n) = default;
        number(const number& n) = default;
        number& operator= (number&& n) = default;
        number& operator= (const number& n) = default;

        friend str_type      _type_(const number& self);
        friend bool_type       _is_(const number& self);
        friend real_type     _comp_(const number& self, const let& other);
        friend void        _str_(stream_type& out, const number& self);
        friend void       _repr_(stream_type& out, const number& self);

        friend let         _add_(const number& self, const let& other);
        friend let         _sub_(const number& self, const let& other);
        friend let         _mul_(const number& self, const let& other);
        friend let         _div_(const number& self, const let& other);
        friend let         _mod_(const number& self, const let& other);
        friend let         _neg_(const number& self);

        friend let       _f_div_(const number& self, const let& other);
        friend let         _rem_(const number& self, const let& other);
        friend let         _pow_(const number& self, const let& other);

        friend bool_type      _nan_(const number& self);
        friend bool_type  _complex_(const number& self);

        friend let             _get_pair_(const number& self);
        friend int_type     _get_integer_(const number& self);
        friend let       _get_expression_(const number& self);

        int_type integer() const;

    private:
        typedef		std::vector<str_type>		tokens_type;

        num_type _value;
    };

    number::number() : _value(0.0, 0.0) {
    }

    number::number(const num_type& value) : _value(value) {
    }

    number::number(str_type str) : _value(0.0, 0.0) {

        if (str == "") {
            return;
        }

        str = to_lower(str);

        if (str == "nan") {
            _value = num_type(std::numeric_limits<real_type>::quiet_NaN(), std::numeric_limits<real_type>::quiet_NaN());
            return;
        }

        if (str == "inf" || str == "+inf") {
            _value = num_type(std::numeric_limits<real_type>::infinity(), std::numeric_limits<real_type>::infinity());
            return;
        }

        if (str == "-inf") {
            _value = num_type(std::numeric_limits<real_type>::infinity(), std::numeric_limits<real_type>::infinity());
            return;
        }

        std::replace(str.begin(), str.end(), ',', ' ');

        tokens_type tokens = split(str, " ");

        real_type real = 0;
        real_type imgn = 0;

        if (tokens.size()) {

            if (tokens.back().back() == 'i' || tokens.back().back() == 'j' || tokens.size() > 1) {

                str_type t = tokens.back();
                t.pop_back();

                imgn = to<real_type>(t);

                tokens.pop_back();
            }
        }

        if (tokens.size()) {

            real = to<real_type>(tokens.front());
        }

        _value = num_type(real, imgn);
    }

    number::number(const int_type& value) : _value(value, 0.0) {
    }

    number::number(const real_type& value) : _value(value, 0.0) {
    }

    number::number(const real_type& real, const real_type& imgn) : _value(real, imgn) {
    }

    number::number(const unsigned long long& value) : _value(static_cast<real_type>(value), 0.0) {
    }

    number::number(const unsigned long& value) : _value(value, 0.0) {
    }

    number::number(const unsigned int& value) : _value(value, 0.0) {
    }

    number::~number() {
    }

    str_type _type_(const number& self) {
        return "number";
    }

    bool_type _is_(const number& self) {

        if (_nan_(self)) {
            return false;
        }

        return (self._value.real() || self._value.imag() ? true : false);
    }

    real_type _comp_(const number& self, const let& other) {

        const number* n = other.cast<number>();

        if (n) {
            if (_nan_(self) || _nan_(*n) || _complex_(self) || _complex_(*n)) {
                return NOT_A_NUMBER;
            }

            real_type x = self._value.real();
            real_type y = n->_value.real();

            if (x > y) {
                return 1.0;
            }

            if (x < y) {
                return -1.0;
            }

            return 0.0;
        }

        return NOT_A_NUMBER;
    }

    void _str_(stream_type& out, const number& self) {

        // TODO: support stream numerical conversion flags.

        real_type real = self._value.real();
        real_type imag = self._value.imag();

        if (imag && !real) {

            out << imag << "j";
            return;
        }

        if (!imag && real) {

            out << real;
            return;
        }

        if (!real && !imag) {

            out << 0.0;
            return;
        }

        out << "(" << real << ",";

        // out.setf(std::ios::showpos);

        out << imag << "j)";

        // out.unsetf(std::ios::showpos);
    }

    void _repr_(stream_type& out, const number& self) {
        out << "\'";
        _str_(out, self);
        out << "\'";
    }

    let _add_(const number& self, const let& other) {

        const number* n = other.cast<number>();

        if (n) {
            return number(self._value + n->_value);
        }
        return nothing();
    }

    let _sub_(const number& self, const let& other) {

        const number* n = other.cast<number>();

        if (n) {
            return number(self._value - n->_value);
        }
        return nothing();
    }

    let _mul_(const number& self, const let& other) {

        const number* n = other.cast<number>();

        if (n) {
            return number(self._value * n->_value);
        }
        return nothing();
    }

    let _div_(const number& self, const let& other) {

        const number* n = other.cast<number>();

        if (n) {
            return number(self._value / n->_value);
        }
        return nothing();
    }

    let _mod_(const number& self, const let& other) {

        const number* n = other.cast<number>();

        if (n) {

            if (_nan_(self) || _nan_(*n) || _complex_(self) || _complex_(*n)) {
                return number("nan");
            }

            return number((unsigned long long)self._value.real() % (unsigned long long)n->_value.real());
        }

        return nothing();
    }

    let _neg_(const number& self) {

        if (_nan_(self)) {
            return number("nan");
        }

        return number(-self._value);
    }

    let _f_div_(const number& self, const let& other) {

        const number* n = other.cast<number>();

        if (n) {

            if (_nan_(self) || _nan_(*n) || _complex_(self) || _complex_(*n)) {
                return number("nan");
            }

            return number(std::floor(self._value.real() / n->_value.real()));
        }

        return nothing();
    }

    let _rem_(const number& self, const let& other) {

        const number* n = other.cast<number>();

        if (n) {

            if (_nan_(self) || _nan_(*n) || _complex_(self) || _complex_(*n)) {
                return number("nan");
            }

            real_type r = self._value.real() / n->_value.real();

            return number(r - std::floor(r));
        }

        return nothing();
    }

    let _pow_(const number& self, const let& other) {

        const number* n = other.cast<number>();

        if (n) {

            if (_nan_(self) || _nan_(*n)) {
                return number("nan");
            }

            return number(std::pow(self._value, n->_value));
        }

        return nothing();
    }

    bool_type _nan_(const number& self) {
        return std::isnan(self._value.real()) || std::isnan(self._value.imag());
    }

    bool_type _complex_(const number& self) {

        if (self._value.imag()) {
            return true;
        }

        return false;
    }

    let _get_pair_(const number& self) {

        let exp = expression();

        exp = exp.join(self._value.imag());
        exp = exp.join(self._value.real());

        return exp;
    }

    int_type _get_integer_(const number& self) {
        return self.integer();
    }

    let _get_expression_(const number& self) {
        return _get_pair_(self);
    }

    int_type number::integer() const {

        return static_cast<int_type>(_value.real());
    }

}
#endif // NUMBER_H