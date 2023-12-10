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

#include <complex>
#include <vector>

#include "Var.h"
#include "text_support.h"

namespace Oliver {

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
        typedef		double		                val_type;
        typedef		std::complex<double>		num_type;

    public:

        // typedef     boost::multiprecision::cpp_complex<0> test_type;

        number();
        number(std::string str);
        number(long long value);
        number(const num_type& value);

        friend std::string       _type_(const number& self);
        friend bool         _is_(const number& self);
        friend order    _comp_(const number& self, const var& other);
        friend std::string _str_(const number& self, const Format_Args& fmt);

        friend var         _add_(number& self, var& other);
        friend var         _sub_(number& self, var& other);
        friend var         _mul_(number& self, var& other);
        friend var         _div_(number& self, var& other);
        friend var         _mod_(number& self, var& other);
        friend var         _neg_(number& self);

        friend var       _f_div_(const number& self, var& other);
        friend var         _rem_(const number& self, var& other);
        friend var         _pow_(const number& self, var& other);

    private:

        num_type _value;

        bool is_nan() const;
    };

    number::number() : _value(0.0, 0.0) {
    }

    number::number(std::string str) : _value(0.0, 0.0) {

        if (str == "") {
            return;
        }

        str = to_lower(str);

        if (str == "nan") {
            _value = num_type(std::numeric_limits<val_type>::quiet_NaN(), std::numeric_limits<val_type>::quiet_NaN());
            return;
        }

        if (str == "inf" || str == "+inf") {
            _value = num_type(std::numeric_limits<val_type>::infinity(), std::numeric_limits<val_type>::infinity());
            return;
        }

        if (str == "-inf") {
            _value = num_type(std::numeric_limits<val_type>::infinity(), std::numeric_limits<val_type>::infinity());
            return;
        }

        str = to_white_space(str, ",()");

        std::vector<std::string> tokens = split(str, " ");

        val_type real = 0;
        val_type imag = 0;

        if (tokens.size()) {

            if (tokens.back().back() == 'i' || tokens.back().back() == 'j' || tokens.size() > 1) {

                std::string t = tokens.back();
                t.pop_back();

                imag = to<val_type>(t).value_or(0);

                tokens.pop_back();
            }
        }

        if (tokens.size() > 1) {

            if (tokens.back() == "-") {
                imag = -imag;
            }

            else if (tokens.back() != "+") {
                _value = num_type(std::numeric_limits<val_type>::quiet_NaN(), std::numeric_limits<val_type>::quiet_NaN());
                return;
            }
        }

        if (tokens.size()) {

            real = to<val_type>(tokens.front()).value_or(0);
        }

        _value = num_type(real, imag);
    }

    number::number(long long value) : _value(static_cast<val_type>(value)) {
    }

    number::number(const num_type& value) : _value(value) {
    }

    bool number::is_nan() const {
        return std::isnan(_value.real()) || std::isnan(_value.imag());
    }

    std::string _type_(const number& self) {
        return "number"s;
    }

    bool _is_(const number& self) {

        if (self.is_nan()) {
            return false;
        }

        return (self._value.real() || self._value.imag() ? true : false);
    }

    order _comp_(const number& self, const var& other) {

        auto ptr = other.cast<number>();

        if (ptr) {
            if (self.is_nan() || ptr->is_nan() || self._value.imag() || ptr->_value.imag()) {
                return order::unordered;
            }

            auto x = self._value.real();
            auto y = ptr->_value.real();

            if (x > y) {
                return order::greater;
            }

            if (x < y) {
                return order::less;
            }

            return order::equivalent;
        }

        return order::unordered;
    }

    std::string _str_(const number& self, const Format_Args& fmt) {

        auto real = self._value.real();
        auto imag = self._value.imag();

        if (imag && !real) {

            return fmt::format("{}j", imag);
        }

        if (!imag) {

            return fmt::format("{}", real);
        }

        return fmt::format("({},{}j)", real, imag);
    }

    var _add_(number& self, var& other) {
        
        auto ptr = other.cast<number>();

        if (!ptr) {
            return var();
        }

        return number(self._value + ptr->_value);
    }

    var _sub_(number& self, var& other) {

        auto ptr = other.cast<number>();

        if (!ptr) {
            return var();
        }

        return number(self._value - ptr->_value);
    }

    var _mul_(number& self, var& other) {

        auto ptr = other.cast<number>();

        if (!ptr) {
            return var();
        }

        return number(self._value * ptr->_value);
    }

    var _div_(number& self, var& other) {

        auto ptr = other.cast<number>();

        if (!ptr) {
            return var();
        }

        return number(self._value / ptr->_value);
    }

    var _mod_(number& self, var& other) {

        auto ptr = other.cast<number>();

        if (ptr) {

            if (self.is_nan() || ptr->is_nan() || self._value.imag() || ptr->_value.imag()) {
                return number("nan");
            }

            return number((unsigned long long)self._value.real() % (unsigned long long)ptr->_value.real());
        }

        return var();
    }

    var _neg_(number& self) {

        if (self.is_nan()) {
            return number("nan");
        }

        return number(-self._value);
    }

    var _f_div_(const number& self, var& other) {

        auto ptr = other.cast<number>();

        if (ptr) {

            if (self.is_nan() || ptr->is_nan() || self._value.imag() || ptr->_value.imag()) {
                return number("nan");
            }

            //return number(std::floor(static_cast<int64_t>(self._value.real() / ptr->_value.real())));
            return number();
        }

        return var();
    }

    var _rem_(const number& self, var& other) {

        auto ptr = other.cast<number>();

        if (ptr) {

            if (self.is_nan() || ptr->is_nan() || self._value.imag() || ptr->_value.imag()) {
                return number("nan");
            }

            auto r = self._value.real() / ptr->_value.real();

            // return number(r - static_cast<number::val_type>(std::floor(static_cast<int64_t>(r))));
            return number();
        }

        return var();
    }

    var _pow_(const number& self, var& other) {

        auto ptr = other.cast<number>();

        if (ptr) {

            if (self.is_nan() || ptr->is_nan()) {
                return number("nan");
            }

            return number(std::pow(self._value, ptr->_value));
        }

        return var();
    }
}