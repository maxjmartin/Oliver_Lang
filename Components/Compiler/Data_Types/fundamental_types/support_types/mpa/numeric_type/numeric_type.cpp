
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

#include "./numeric_type.h"

namespace Olly {
    namespace MPA {

        /********************************************************************************************/
        //
        //                        Static & Constant Variable Intantiation.
        //
        /********************************************************************************************/

        const whole_number numeric_type::TEN   = whole_number(10);
        const numeric_type numeric_type::UNDEF = numeric_type("undef");

        const numeric_type& numeric_type::get_min_dec_scale() {
            return minimum_decimal_scale();
        }

        void Olly::MPA::numeric_type::set_min_dec_scale(const int_type scale) {
            minimum_decimal_scale() = scale;
        }

        numeric_type& numeric_type::minimum_decimal_scale() {
            
            static numeric_type mds(16);

            return mds;
        }

        const bool_type& Olly::MPA::numeric_type::get_rational_to_lowest_terms() {
            return reduce_fractions_to_lowest_terms();
        }

        void Olly::MPA::numeric_type::set_rational_lowest_terms(const bool_type& value) {
            reduce_fractions_to_lowest_terms() = value;
        }

        bool_type& numeric_type::reduce_fractions_to_lowest_terms() {
            
            static bool_type rftlt = true;

            return rftlt;
        }

        /********************************************************************************************/
        //
        //                                  Class Implementation
        //
        /********************************************************************************************/

        numeric_type::numeric_type() : _num(), _den(1), _sign(SIGN::zero), _scale(0), _cat(NUMERIC_CATEGORY::integer), _rmode(ROUNDING_MODE::half_even) {
        }

        numeric_type::numeric_type(int_type value) : _num(value ? std::abs(value) : 0), _den(1), _sign(SIGN::zero), _scale(0), _cat(NUMERIC_CATEGORY::integer), _rmode(ROUNDING_MODE::half_even) {

            if (value < 0) {
                _sign = SIGN::negative;
            }
            else if (value > 0) {
                _sign = SIGN::positive;
            }
        }

        numeric_type::numeric_type(str_type value, str_type r_mode) : _num(), _den(1), _sign(SIGN::zero), _scale(0), _cat(NUMERIC_CATEGORY::integer), _rmode(ROUNDING_MODE::half_even) {

            if (!value.empty()) {

                lrtrim(value);

                if (std::regex_match(value, INT_REGX)) {
                    set_integer(value);
                }
                else if (std::regex_match(value, DEC_REGX)) {
                    set_decimal(value);
                }
                else if (std::regex_match(value, RAT_REGX)) {
                    set_rational(value);
                }
                else if (std::regex_match(value, WHL_REGX)) {
                    set_whole_number(value);
                }
                else if (std::regex_match(value, BINARY_REGX)) {
                    set_binary(value);
                }
                else if (std::regex_match(value, OCTAL_REGX)) {
                    set_octal(value);
                }
                else if (std::regex_match(value, HEX_REGX)) {
                    set_heximal(value);
                }
                else if (value == "undef") {
                    _cat = NUMERIC_CATEGORY::undef;
                }
                else { // nan
                    _cat = NUMERIC_CATEGORY::nan;
                }
            }
            set_rmode(r_mode);
        }

        numeric_type::numeric_type(const whole_number& num, const numeric_type& ref) : _num(num), _den(ref._den), _sign(ref._sign), _scale(ref._scale), _cat(ref._cat), _rmode(ref._rmode) {
        }

        numeric_type::numeric_type(const int_type& scale, const ROUNDING_MODE& mode) : _num(), _den(1), _sign(SIGN::zero), _scale(scale), _cat(NUMERIC_CATEGORY::decimal), _rmode(mode) {
            _den = TEN.pow(_scale);
        }

        numeric_type::~numeric_type() {
        }

        bool_type numeric_type::is() const {

            if (is_defined()) {

                return _num.is();
            }
            return false;
        }

        bool_type numeric_type::is_odd() const {

            if (is_defined()) {

                return _num.is_odd();
            }
            return false;
        }

        bool_type numeric_type::is_even() const {

            if (is_defined()) {

                return _num.is_even();
            }
            return false;
        }

        bool_type numeric_type::is_positive() const {

            if (is_defined() && _sign > SIGN::zero) {

                return true;
            }

            return false;
        }

        bool_type numeric_type::is_negative() const {

            if (is_defined() && _sign < SIGN::zero) {
                return true;
            }

            return false;
        }

        bool_type numeric_type::is_defined() const {
            return _cat > NUMERIC_CATEGORY::undef;
        }

        bool_type numeric_type::is_undefined() const {
            return _cat == NUMERIC_CATEGORY::undef;
        }

        bool_type numeric_type::is_nan() const {
            return _cat == NUMERIC_CATEGORY::nan;
        }

        bool_type numeric_type::is_decimal() const {
            return _cat == NUMERIC_CATEGORY::decimal;
        }

        bool_type numeric_type::is_rational() const {
            return _cat == NUMERIC_CATEGORY::rational;
        }

        bool_type numeric_type::is_integer() const {
            return _cat == NUMERIC_CATEGORY::integer;
        }

        bool_type numeric_type::is_whole() const {
            return _cat == NUMERIC_CATEGORY::whole;
        }

        bool_type numeric_type::is_heximal() const {
            return _cat == NUMERIC_CATEGORY::heximal;
        }

        bool_type numeric_type::is_octal() const {
            return _cat == NUMERIC_CATEGORY::octal;
        }

        bool_type numeric_type::is_binary() const {
            return _cat == NUMERIC_CATEGORY::binary;
        }

        bool_type numeric_type::is_integral_type() const {
            return (_cat <= NUMERIC_CATEGORY::integer && is_defined());
        }

        bool_type numeric_type::operator==(const numeric_type& b) const {
            return compare(b) == 0.0;
        }

        bool_type numeric_type::operator!=(const numeric_type& b) const {
            return compare(b) != 0.0;
        }

        bool_type numeric_type::operator<(const numeric_type& b) const {
            return compare(b) < 0.0;
        }

        bool_type numeric_type::operator>(const numeric_type& b) const {
            return compare(b) > 0.0;
        }

        bool_type numeric_type::operator<=(const numeric_type& b) const {
            return compare(b) <= 0.0;
        }

        bool_type numeric_type::operator>=(const numeric_type& b) const {
            return compare(b) >= 0.0;
        }

        real_type numeric_type::compare(const numeric_type& other) const {

            if (_cat == other._cat) {

                if (is_defined() && other.is_defined()) {

                    if (is_decimal()) {

                        if (_sign < other._sign) {

                            return -1.0;
                        }
                        else if (_sign > other._sign) {

                            return 1.0;
                        }

                        if (_scale == other._scale) {

                            return numerator_compare(other);
                        }
                        else if (_scale > other._scale) {
                            return compare(other.set_scale(_scale));
                        }
                        else {
                            numeric_type a = set_scale(other._scale);
                            return a.compare(other);
                        }
                    }
                    else if (is_rational()) {

                        if (_den == other._den) {

                            return numerator_compare(other);
                        }
                        else {
                            numeric_type b = other;

                            b._num *= _den;
                            b._den *= _den;

                            numeric_type a = *this;

                            a._num *= other._den;
                            a._den *= other._den;

                            return a.compare(b);
                        }
                    }
                    else {
                        return numerator_compare(other);
                    }
                }
            }
            else if (_cat > other._cat) {
                return compare(other.promote_category(*this));
            }
            else {
                return promote_category(other).compare(other);
            }

            return NOT_A_NUMBER;
        }

        real_type numeric_type::numerator_compare(const numeric_type& other) const {
            /*
                Compare the absolute values of the numerators, then apply the
                sign of the numbers to the result.
            */
            real_type truth = _num.compare(other._num);

            if (_cat == other._cat) {

                if (_sign == SIGN::negative) {

                    truth *= -1.0;
                }
            }
            else {

                if (_sign == SIGN::positive && truth < 0.0) {

                    truth *= -1.0;
                }
                else if (_sign == SIGN::negative && truth > 0.0) {

                    truth *= -1.0;
                }
            }

            return truth;
        }

        str_type numeric_type::get_category() const {

            switch (_cat) {

            case NUMERIC_CATEGORY::decimal:
                return "decimal";
                break;

            case NUMERIC_CATEGORY::rational:
                return "rational";
                break;

            case NUMERIC_CATEGORY::integer:
                return "integer";
                break;

            case NUMERIC_CATEGORY::whole:
                return "whole";
                break;

            case NUMERIC_CATEGORY::heximal:
                return "heximal";
                break;

            case NUMERIC_CATEGORY::octal:
                return "octal";
                break;

            case NUMERIC_CATEGORY::binary:
                return "binary";
                break;

            case NUMERIC_CATEGORY::undef:
                return "undef";
                break;

            case NUMERIC_CATEGORY::nan:
                return "nan";
                break;

            default:
                break;
            }
            return "";
        }

        NUMERIC_CATEGORY numeric_type::get_cat_integral() const {
            return _cat;
        }

        str_type numeric_type::get_sign() const {

            switch (_sign) {

            case SIGN::negative:
                return "negative";
                break;

            case SIGN::positive:
                return "positive";
                break;

            default:
                break;
            }
            return "zero";
        }

        int_type numeric_type::get_scale() const {
            return _scale;
        }

        numeric_type& numeric_type::set_scale(const numeric_type& scale) {

            if (_cat < NUMERIC_CATEGORY::decimal) {

                *this = to_decimal(scale.abs());

                return *this;
            }

            set_decimal_scale(scale.abs());

            return *this;
        }

        numeric_type numeric_type::set_scale(const numeric_type& scale) const {

            numeric_type n = *this;

            n.set_decimal_scale(scale.abs());

            return n;
        }

        str_type numeric_type::get_rmode() const {

            str_type mode = "";

            switch (_rmode) {

            case (ROUNDING_MODE::half_up):
                mode = "half up";
                break;

            case (ROUNDING_MODE::half_down):
                mode = "half down";
                break;

            case (ROUNDING_MODE::half_even):
                mode = "half even";
                break;

            case (ROUNDING_MODE::half_odd):
                mode = "half odd";
                break;

            case (ROUNDING_MODE::ceil):
                mode = "ceil";
                break;

            case (ROUNDING_MODE::floor):
                mode = "floor";
                break;

            default:
                mode = "none";
                break;
            }

            return mode;
        }

        numeric_type& numeric_type::set_rmode(const str_type& rmode) {

            str_type mode = to_lower(rmode);

            if (!mode.empty()) {

                if (mode == "half up") {
                    _rmode = ROUNDING_MODE::half_up;
                }
                else if (mode == "half down") {
                    _rmode = ROUNDING_MODE::half_down;
                }
                else if (mode == "half even") {
                    _rmode = ROUNDING_MODE::half_even;
                }
                else if (mode == "half odd") {
                    _rmode = ROUNDING_MODE::half_odd;
                }
                else if (mode == "ceil") {
                    _rmode = ROUNDING_MODE::ceil;
                }
                else if (mode == "floor") {
                    _rmode = ROUNDING_MODE::floor;
                }
                else if (mode == "toward zero") {
                    _rmode = ROUNDING_MODE::toward_zero;
                }
                else if (mode == "away from zero") {
                    _rmode = ROUNDING_MODE::away_from_zero;
                }
            }

            return *this;
        }

        numeric_type numeric_type::set_rmode(const str_type& rmode) const {

            numeric_type a = *this;

            a.set_rmode(rmode);

            return a;
        }
        numeric_type numeric_type::get_numerator() const {

            if (_cat < NUMERIC_CATEGORY::rational) {
                return *this;
            }

            numeric_type a = *this;

            a._den = 1;

            return a;
        }

        numeric_type numeric_type::get_denominator() const {

            if (_cat < NUMERIC_CATEGORY::rational) {
                return *this;
            }

            numeric_type a = *this;

            a._num = 1;

            std::swap(a._num, a._den);

            return a;
        }

        numeric_type& numeric_type::operator&=(const numeric_type& other) {


            if (other._cat <= NUMERIC_CATEGORY::integer) {

                if (_cat <= NUMERIC_CATEGORY::integer) {

                    if (is_negative()) {
                        _num = _num.bin_comp();
                    }

                    auto n = other.is_negative() ? other._num.bin_comp() : other._num;

                    _num &= n;

                    return *this;
                }
            }
            *this = UNDEF;

            return *this;
        }

        numeric_type& numeric_type::operator|=(const numeric_type& other) {

            if (_cat <= NUMERIC_CATEGORY::integer) {

                if (_cat <= NUMERIC_CATEGORY::integer) {

                    if (is_negative()) {
                        _num = _num.bin_comp();
                    }

                    auto n = other.is_negative() ? other._num.bin_comp() : other._num;

                    _num |= n;

                    return *this;
                }
            }
            *this = UNDEF;

            return *this;
        }

        numeric_type& numeric_type::operator^=(const numeric_type& other) {

            if (_cat <= NUMERIC_CATEGORY::integer) {

                if (_cat <= NUMERIC_CATEGORY::integer) {

                    if (is_negative()) {
                        _num = _num.bin_comp();
                    }

                    auto n = other.is_negative() ? other._num.bin_comp() : other._num;

                    _num ^= n;

                    return *this;
                }
            }
            *this = UNDEF;

            return *this;
        }

        numeric_type& numeric_type::operator<<=(size_type index) {

            if (_cat <= NUMERIC_CATEGORY::integer) {

                _num <<= index;
            }
            else {

                _num *= TEN.pow(index);
            }

            check_for_zero();

            return *this;
        }

        numeric_type& numeric_type::operator>>=(size_type index) {

            if (_cat <= NUMERIC_CATEGORY::integer) {

                _num <<= index;
            }
            else {

                _num /= TEN.pow(index);
            }

            check_for_zero();

            return *this;
        }

        numeric_type numeric_type::operator&(const numeric_type& b) const {

            numeric_type a = *this;

            a &= b;

            return a;
        }

        numeric_type numeric_type::operator|(const numeric_type& b) const {

            numeric_type a = *this;

            a |= b;

            return a;
        }

        numeric_type numeric_type::operator^(const numeric_type& b) const {

            numeric_type a = *this;

            a ^= b;

            return a;
        }

        numeric_type numeric_type::operator~() const {

            numeric_type a;

            a._num = ~_num;

            return a;
        }

        numeric_type numeric_type::operator<<(size_type index) const {

            numeric_type a = *this;

            a <<= index;

            return a;
        }

        numeric_type numeric_type::operator>>(size_type index) const {

            numeric_type a = *this;

            a >>= index;

            return a;
        }

        numeric_type& numeric_type::operator+=(const numeric_type& other) {

            if (is_defined() && other.is_defined()) {

                if (_cat == other._cat) {

                    if (is_integral_type()) {

                        numerator_addition(other);

                        return *this;
                    }
                    else if (is_decimal()) {

                        if (_scale > other._scale) {
                            return operator+=(other.set_scale(_scale));
                        }
                        else if (_scale < other._scale) {
                            *this = set_scale(other._scale);
                            return operator+=(other);
                        }

                        numerator_addition(other);

                        return *this;
                    }
                    else {
                        // Must be rational numbers.

                        if (_den == other._den) {

                            numerator_addition(other);
                        }
                        else {
                            // Fraction addition, set both fractions to the same base.
                            // Then reinvoke the addition operator.

                            numeric_type b = other;

                            b._num *= _den;
                            b._den *= _den;

                            _num *= other._den;
                            _den *= other._den;

                            operator+=(b);
                        }

                        if (reduce_fractions_to_lowest_terms()) {
                            *this = lowest_terms();
                        }

                        check_for_zero();

                        return *this;
                    }
                }
                else if (_cat > other._cat) {
                    return operator+=(other.promote_category(*this));
                }
                else {
                    *this = promote_category(other);
                    return operator+=(other);
                }
            }

            *this = UNDEF;

            return *this;
        }

        numeric_type& numeric_type::operator-=(const numeric_type& other) {

            return operator+=(-other);
        }

        numeric_type& numeric_type::operator*=(const numeric_type& other) {

            if (is_defined() && other.is_defined()) {

                if (_cat == other._cat) {

                    if (_scale > other._scale) {
                        return operator*=(other.set_scale(_scale));
                    }
                    else if (_scale < other._scale) {
                        *this = set_scale(other._scale);
                        return operator*=(other);
                    }

                    if (_den == other._den) {

                        numerator_multiplication(other);
                        
                        if (_scale) {

                            whole_number q, r;
                            _num.div_rem(_den, q, r);

                            _num = q;
                        }

                        if (is_rational() && reduce_fractions_to_lowest_terms()) {
                            *this = lowest_terms();
                        }

                        return *this;
                    }
                    else {
                        // Perform addition between two factions of diffrent bases.
                        _num *= other._num;
                        _den *= other._den;

                        if (_sign == other._sign) {

                            _sign = SIGN::positive;
                        }
                        else {
                            _sign = SIGN::negative;
                        }

                        if (reduce_fractions_to_lowest_terms()) {
                            *this = lowest_terms();
                        }

                        check_for_zero();

                        return *this;
                    }
                }
                else if (_cat > other._cat) {
                    return operator*=(other.promote_category(*this));
                }
                else {
                    *this = promote_category(other);
                    return operator*=(other);
                }
            }

            *this = UNDEF;

            return *this;
        }

        numeric_type& numeric_type::operator/=(const numeric_type& other) {

            if (is_defined() && other.is_defined()) {

                if (!other.is()) {

                    *this = UNDEF;

                    return *this;
                }

                if (_cat == other._cat) {

                    if (is_integral_type()) {

                        whole_number q, r;

                        _num.div_rem(other._num, q, r);

                        _num = q;

                        if (_sign == other._sign) {

                            _sign = SIGN::positive;
                        }
                        else {
                            _sign = SIGN::negative;
                        }
                    }
                    else if (is_rational()) {

                        *this *= other.inverse();
                    }
                    else {
                        // Must be decimal numbners. 

                        if (_scale > other._scale) {
                            return operator/=(other.set_scale(_scale));
                        }
                        else if (_scale < other._scale) {
                            *this = set_scale(other._scale);
                            return operator/=(other);
                        }

                        auto a = *this;
                        auto b = other;

                        a._cat = NUMERIC_CATEGORY::rational;
                        b._cat = NUMERIC_CATEGORY::rational;
                        std::swap(b._num, b._den);

                        bool_type guard = reduce_fractions_to_lowest_terms();
                        reduce_fractions_to_lowest_terms() = false;

                        a *= b;

                        reduce_fractions_to_lowest_terms() = guard;

                        *this = a.to_decimal(_scale);
                    }

                    check_for_zero();

                    return *this;
                }
                else if (_cat > other._cat) {
                    return operator/=(other.promote_category(*this));
                }
                else {
                    *this = promote_category(other);
                    return operator/=(other);
                }
            }

            *this = UNDEF;

            return *this;
        }

        numeric_type& numeric_type::operator%=(const numeric_type& other) {

            if (is_defined() && other.is_defined()) {

                if (!other.is()) {

                    *this = UNDEF;

                    return *this;
                }

                if (_cat == other._cat) {

                    if (is_integral_type()) {

                        whole_number q, r;

                        _num.div_rem(other._num, q, r);

                        _num = r;
                    }
                    else {

                        if (_scale > other._scale) {
                            return operator%=(other.set_scale(_scale));
                        }
                        else if (_scale < other._scale) {
                            *this = set_scale(other._scale);
                            return operator%=(other);
                        }

                        *this /= other;

                        _num %= _den;

                        if (!_scale) {
                            _den = other._den;
                        }
                    }

                    if (_sign == other._sign) {

                        _sign = SIGN::positive;
                    }
                    else {
                        _sign = SIGN::negative;
                    }

                    check_for_zero();

                    return *this;
                }
                else if (_cat > other._cat) {
                    return operator%=(other.promote_category(*this));
                }
                else {
                    *this = promote_category(other);
                    return operator%=(other);
                }
            }

            *this = UNDEF;

            return *this;
        }

        numeric_type numeric_type::operator+(const numeric_type& b) const {

            numeric_type a = *this;

            a += b;

            return a;
        }

        numeric_type numeric_type::operator-(const numeric_type& b) const {

            numeric_type a = *this;

            a -= b;

            return a;
        }

        numeric_type numeric_type::operator*(const numeric_type& b) const {

            numeric_type a = *this;

            a *= b;

            return a;
        }

        numeric_type numeric_type::operator/(const numeric_type& b) const {

            numeric_type a = *this;

            a /= b;

            return a;
        }

        numeric_type numeric_type::operator%(const numeric_type& b) const {

            numeric_type a = *this;

            a %= b;

            return a;
        }

        numeric_type numeric_type::operator+() const {

            return *this;
        }

        numeric_type numeric_type::operator-() const {

            numeric_type a = *this;
            a.invert_sign();

            return a;
        }

        numeric_type& numeric_type::operator++() {

            if (is_defined()) {

                if (is_negative()) {

                    if (is_integral_type()) {
                        --_num;
                    }
                    else {
                        _num -= _den;
                    }

                    check_for_zero();
                }
                else {

                    if (is_integral_type()) {
                        ++_num;
                    }
                    else {
                        _num += _den;
                    }

                    if (_sign == SIGN::zero) {
                        _sign = SIGN::positive;
                    }
                }
            }

            return *this;
        }

        numeric_type numeric_type::operator++(int) {

            numeric_type a(*this);

            operator++();

            return a;
        }

        numeric_type& numeric_type::operator--() {

            if (is_defined()) {

                if (is_negative()) {

                    if (is_integral_type()) {
                        ++_num;
                    }
                    else {
                        _num += _den;
                    }

                    if (_sign == SIGN::zero) {
                        _sign = SIGN::negative;
                    }
                }
                else {

                    if (is_integral_type()) {
                        --_num;
                    }
                    else {
                        _num -= _den;
                    }

                    check_for_zero();
                }
            }

            return *this;
        }

        numeric_type numeric_type::operator--(int) {

            numeric_type a(*this);

            operator--();

            return a;
        }

        void numeric_type::div_rem(const numeric_type& other, numeric_type& qot, numeric_type& rem) const {

            if (is_defined() && other.is_defined()) {

                if (!other.is()) {

                    qot = UNDEF;
                    rem = UNDEF;

                    return;
                }

                if (_cat == other._cat) {

                    if (is_decimal()) {

                        qot = *this / other;

                        rem = qot;

                        rem._num %= rem._den;

                        qot -= rem;
                    }
                    else if (is_rational()) {

                        qot = *this * other.inverse();

                        rem = qot;

                        rem._num %= rem._den;
                        rem._den = other._den;
                    }
                    else {

                        whole_number q, r;

                        _num.div_rem(other._num, q, r);

                        qot._num = q;
                        rem._num = r;

                    }

                    if (_sign == other._sign) {

                        qot._sign = SIGN::positive;
                        rem._sign = SIGN::positive;
                    }
                    else {
                        qot._sign = SIGN::negative;
                        rem._sign = SIGN::negative;
                    }

                    qot.check_for_zero();
                    rem.check_for_zero();

                    return;
                }
                else if (_cat > other._cat) {
                    return div_rem(other.promote_category(*this), qot, rem);
                }
                else {
                    numeric_type a = promote_category(other);
                    return a.div_rem(other, qot, rem);
                }
            }

            if (is_nan() || other.is_nan()) {

                qot = numeric_type("nan");
                rem = numeric_type("nan");
            }
            else {
                qot = UNDEF;
                rem = UNDEF;
            }

        }

        void numeric_type::numerator_addition(const numeric_type& other) {

            if (_sign == other._sign) {

                _num += other._num;

                return;
            }

            real_type n = _num.compare(other._num);

            if (n > 0.0) {
                _num -= other._num;
            }
            else if (n < 0.0) {
                _num = other._num - _num;
                _sign = other._sign;
            }
            else {
                _num = whole_number();
            }

            check_for_zero();
        }

        void numeric_type::numerator_multiplication(const numeric_type& other) {

            _num = _num * other._num;

            if (_sign == other._sign) {

                _sign = SIGN::positive;
            }
            else {
                _sign = SIGN::negative;
            }

            check_for_zero();
        }

        numeric_type numeric_type::to_decimal(numeric_type scale) const {

            if (!scale.is_integer() || scale.is_negative()) {
                return UNDEF;
            }

            if (scale < get_min_dec_scale()) {
                scale = get_min_dec_scale();
            }

            if (_cat <= NUMERIC_CATEGORY::rational) {

                numeric_type a(1);
                a._cat = NUMERIC_CATEGORY::decimal;
                a._den = TEN.pow(scale._num);
                a._num *= a._den;
                a._scale = scale.to_integral<int_type>();

                numeric_type b = *this;

                b *= a;

                return b;
            }
            return set_scale(scale);
        }

        numeric_type numeric_type::to_decimal() const {

            if (_cat <= NUMERIC_CATEGORY::rational) {

                whole_number scale = _num.log10() + _den.log10();

                auto a = to_decimal(scale.to_integral<int_type>());

                return a;
            }

            return *this;
        }

        numeric_type numeric_type::to_rational() const {

            numeric_type a = *this;

            if (_cat > NUMERIC_CATEGORY::rational) {

                whole_number q, r;
                a._num.div_rem(a._den, q, r);


                whole_number d = (r % 3).is() ? a._den : a._den - 1;

                auto gcd = r.gcd(d);

                a._num = r / gcd;
                a._den = d / gcd;

                a._num += (q * a._den);

                a.check_for_zero();
            }
            a._cat = NUMERIC_CATEGORY::rational;
            a._scale = 0;

            return a;
        }

        numeric_type numeric_type::to_integer() const {

            numeric_type a = *this;

            if (_cat >= NUMERIC_CATEGORY::rational) {

                a._num /= a._den;
                a._den = 1;
            }
            a._cat = NUMERIC_CATEGORY::integer;
            a._scale = 0;

            a.check_for_zero();

            return a;
        }

        numeric_type numeric_type::to_whole() const {

            numeric_type a = *this;

            if (_cat >= NUMERIC_CATEGORY::rational) {

                a._num /= a._den;
                a._den = 1;
            }
            a._sign = SIGN::positive;
            a._cat = NUMERIC_CATEGORY::whole;
            a._scale = 0;

            a.check_for_zero();

            return a;
        }

        numeric_type numeric_type::to_heximal() const {

            numeric_type a = *this;

            if (_cat >= NUMERIC_CATEGORY::rational) {

                a._num /= a._den;
                a._den = 1;
            }
            a._cat = NUMERIC_CATEGORY::heximal;
            a._scale = 0;

            a.check_for_zero();

            return a;
        }

        numeric_type numeric_type::to_octal() const {

            numeric_type a = *this;

            if (_cat >= NUMERIC_CATEGORY::rational) {

                a._num /= a._den;
                a._den = 1;
            }
            a._cat = NUMERIC_CATEGORY::octal;
            a._scale = 0;

            a.check_for_zero();

            return a;
        }

        numeric_type numeric_type::to_binary() const {

            numeric_type a = *this;

            if (_cat >= NUMERIC_CATEGORY::rational) {

                a._num /= a._den;
                a._den = 1;
            }
            a._cat = NUMERIC_CATEGORY::binary;
            a._scale = 0;

            a.check_for_zero();

            return a;
        }

        numeric_type numeric_type::promote_category(const numeric_type& other) const {

            // First check if we need to demote to an undefined number.
            if (is_undefined() || other.is_undefined()) {
                if (_cat < other._cat) {

                    return *this;
                }
                else {
                    return other;
                }
            }

            numeric_type res;

            switch (other._cat) {

            case NUMERIC_CATEGORY::decimal:   // Promote res to a decimal number.
                res = *this;

                res._cat = NUMERIC_CATEGORY::decimal;
                res._scale = other._scale;
                res._num *= other._den;
                res._num /= res._den;
                res._den = other._den;
                break;

            case NUMERIC_CATEGORY::rational:  // Promote res to a rational number.
                res = to_rational();
                break;

            case NUMERIC_CATEGORY::integer:  // Promote res to a integer number.
                res = to_integer();
                break;

            case NUMERIC_CATEGORY::whole:    // Promote res to a whole number.
                res = to_whole();
                break;

            case NUMERIC_CATEGORY::heximal:  // Promote res to a heximal number.
                res = to_heximal();
                break;

            case NUMERIC_CATEGORY::octal:    // Promote res to a octal number.
                res = to_octal();
                break;

            default:    // Binary number need no other promotion.
                break;
            }

            return res;
        }

        numeric_type numeric_type::abs() const {

            if (is_negative()) {

                return -*this;
            }
            return *this;
        }

        numeric_type numeric_type::inverse() const {

            if (_cat == NUMERIC_CATEGORY::rational) {

                numeric_type a = *this;

                std::swap(a._num, a._den);

                return a;
            }

            numeric_type a = to_rational();

            return a.inverse();
        }

        numeric_type numeric_type::factorial() const {

            numeric_type a = *this;
            numeric_type r = 1;
            numeric_type one = 1;

            while (a > one) {
                r *= a;
                a -= one;
            }

            return r;
        }

        numeric_type numeric_type::log(numeric_type b) const {

            if (is_defined() && b.is_defined() && is_positive() && b.is_positive()) {

                return numeric_type(_num.log(b._num), *this);
            }

            return UNDEF;
        }

        numeric_type numeric_type::log2() const {

            if (is_defined() && is_positive()) {

                return numeric_type(_num.log2(), *this);
            }

            return UNDEF;
        }

        numeric_type numeric_type::log10() const {

            if (is_defined() && is_positive()) {

                return numeric_type(_num.log10(), *this);
            }

            return UNDEF;
        }

        numeric_type numeric_type::loge() const {
            
            if (!is_decimal()) {
                return to_decimal().loge();
            }

            if (is_negative()) {
                return UNDEF;
            }

            numeric_type one(1);
            numeric_type two(2);

            one = one.to_decimal(_scale);
            two = two.to_decimal(_scale);

            numeric_type r = (*this - one) / (*this + one);

            numeric_type a = r;

            numeric_type t = r;

            numeric_type p(3);

            numeric_type l;

            while (l != t) {
                l = t;

                a *= r;
                a *= r;

                t += (one / p) * a;

                p += two;
            }

            return t * 2;
        }

        numeric_type numeric_type::gcd(numeric_type& other) const {

            if (is_defined() && other.is_defined()) {

                if (!other.is()) {

                    return *this;
                }

                numeric_type a(*this);

                if (_cat == other._cat) {

                    if (_scale > other._scale) {
                        return gcd(other.set_scale(_scale));
                    }
                    else if (_scale < other._scale) {
                        a = set_scale(other._scale);
                        return a.gcd(other);
                    }
                    else {
                        // Start GCD algorithm.

                        if (a.is_integral_type()) {

                            a._num = a._num.gcd(other._num);
                        }
                        else if (is_decimal()) {

                            whole_number q, r;

                            _num.div_rem(_den, q, r);

                            if (r.is()) {
                                a = numeric_type(1, *this);
                                a._num *= a._den;

                                return a;
                            }

                            a._num = q;

                            other._num.div_rem(other._den, q, r);

                            if (r.is()) {
                                a = numeric_type(1, *this);
                                a._num *= a._den;

                                return a;
                            }

                            a._num = a._num.gcd(q);
                            a._num *= a._den;
                        }
                        else {
                            // Must be rational.

                            a._num = a._num.gcd(other._num);
                            a._den = a._den.lcm(other._den);
                        }
                    }

                    a.check_for_zero();

                    if (a.is_negative()) {
                        a.invert_sign();
                    }
                    
                    return a;
                    // End GCD algorithm.
                }
                else if (_cat > other._cat) {
                    return gcd(other.promote_category(*this));
                }
                else {
                    a = promote_category(other);
                    return a.gcd(other);
                }
            }

            return UNDEF;
        }

        numeric_type numeric_type::pow(numeric_type b) const {

            if (!b.is_integral_type()) {

                if (b.is_negative()) {

                    return inverse().pow(b.abs());
                }

                if (b.is_decimal()) {

                    b = b.to_rational();
                }

                // Else b must be a rational number.

                numeric_type x(1);
                x._num = b._num;

                numeric_type y(1);
                y._num = b._den;

                return pow(x).root(y);
            }

            numeric_type a(*this);
            b = b.to_rational();

            if (is_defined() && b.is_defined()) {

                if (b.is_negative()) {

                    return inverse().pow(b.abs());
                }

                a._num = a._num.pow(b._num);

                if (a._scale) {
                    a >>= (a._scale * (b._num.to_integral<int_type>() - 1));
                }
                else if (is_rational()) {
                    a._den = a._den.pow(b._num);
                }
            }

            return a;
        }

        numeric_type numeric_type::ceil() const {

            if (is_integral_type()) {
                return *this;
            }

            numeric_type a = *this;

            whole_number q, r;
            a._num.div_rem(a._den, q, r);

            a._num = q * a._den;

            if (r.is()) {

                if (a.is_positive()) {
                    a._num += a._den;
                }
                else {
                    a._num -= a._den;
                }
            }
            if (is_rational()) {
                a._den = 1;
            }

            return a;
        }

        numeric_type numeric_type::floor() const {

            numeric_type a = *this;

            whole_number q, r;
            a._num.div_rem(a._den, q, r);

            a._num = q * a._den;

            if (r.is()) {

                if (a.is_positive()) {
                    a._num -= a._den;
                }
                else {
                    a._num += a._den;
                }
            }
            if (is_rational()) {
                a._den = 1;
            }

            return a;
        }

        numeric_type numeric_type::sqrt() const {
            numeric_type a(2);
            return root(a);
        }

        numeric_type numeric_type::root(numeric_type b) const {

            if (!b.is_integral_type()) {

                if (b.is_negative()) {

                    return inverse().root(b.abs());
                }

                if (b.is_decimal()) {

                    b = b.to_rational();
                }
                // Else b must be a rational number.

                numeric_type x(1);
                x._num = b._num;

                numeric_type y(1);
                y._num = b._den;

                return pow(x).root(y);
            }

            if (is_integral_type()) {
                
                return numeric_type(_num.root(b._num), *this);
            }
            else if (is_decimal()) {

                numeric_type a((_num / _den).root(b._num), *this);
                a._num *= a._den;

                //  Define a 0.1 decimal place incrementor.
                numeric_type inc(1, *this);
                inc <<= _scale - 1;

                if (a.pow(b) == *this) {
                    return a;  // Return a perfect square.
                }

                //  Perform one iteration for each decimal place.
                for (int_type i = 0; i < _scale; i += 1) {

                    while ((a.pow(b)) <= *this) {

                        a += inc;
                    }

                    a = a - inc;
                    inc >>= 1;  //  Right shift the incrementor to the next decimal place.  
                }
                return a;
            }
            else {
                // It must be a rational number.
                numeric_type a(*this);

                a._num = a._num.root(b._num);
                a._den = a._den.root(b._num);

                return a;
            }

            return *this;
        }

        numeric_type numeric_type::hypot(numeric_type& b) const {

            numeric_type two(2);

            return (pow(two) + b.pow(two)).sqrt();
        }

        numeric_type numeric_type::hypot(numeric_type& b, numeric_type& c) const {

            numeric_type two(2);

            return (pow(two) + b.pow(two) + c.pow(two)).sqrt();
        }

        numeric_type numeric_type::lowest_terms() const {

            numeric_type a = *this;

            if (is_rational()) {

                whole_number n = a._num.gcd(a._den);

                a._num /= n;
                a._den /= n;
            }

            return a;
        }

        void numeric_type::invert_sign() {

            switch (_sign) {

            case (SIGN::negative):
                _sign = SIGN::positive;
                break;

            case (SIGN::positive):
                _sign = SIGN::negative;
                break;

            default:
                break;
            }
        }

        numeric_type::SIGN numeric_type::get_invert_sign() const {

            if (is_defined()) {

                SIGN n = SIGN::zero;

                switch (_sign) {

                case (SIGN::negative):
                    n = SIGN::positive;
                    break;

                case (SIGN::positive):
                    n = SIGN::negative;
                    break;

                default:
                    break;
                }

                return n;
            }

            return _sign;
        }

        void numeric_type::round() {

            whole_number q, r;
            _num.div_rem(TEN, q, r);

            _num = q;

            set_round(r);
        }

        void numeric_type::set_round(whole_number& rem) {

            if (rem.is()) {

                switch (_rmode) {

                case ROUNDING_MODE::half_even: {

                    whole_number five(5);

                    if (rem > five) {
                        _num += 1;
                    }
                    if (rem == five) {

                        if (is_odd()) {
                            _num += 1;
                        }
                    }
                } break;

                case ROUNDING_MODE::half_odd: {

                    whole_number five(5);

                    if (rem > five) {
                        _num += 1;
                    }
                    if (rem == five) {

                        if (is_even()) {
                            _num += 1;
                        }
                    }
                } break;

                case ROUNDING_MODE::half_up: {

                    whole_number five(5);
                    if (is_positive() && rem >= five) {
                        _num += 1;
                    }
                    else if (is_negative() && rem > five) {
                        _num += 1;
                    }
                } break;

                case ROUNDING_MODE::half_down: {

                    whole_number five(5);
                    if (is_positive() && rem > five) {
                        _num += 1;
                    }
                    else if (is_negative() && rem >= five) {
                        _num += 1;
                    }
                } break;

                case ROUNDING_MODE::ceil: {

                    if (is_positive() && rem.is()) {
                        _num += 1;
                    }
                } break;

                case ROUNDING_MODE::floor: {

                    if (is_negative() && rem.is()) {
                        _num += 1;
                    }
                } break;

                case ROUNDING_MODE::away_from_zero: {

                    if (rem.is()) {
                        _num += 1;
                    }
                } break;

                default:
                    break;
                }
            }
        }

        numeric_type& numeric_type::round_to(const numeric_type& index) {

            int_type i = 0;

            if (index.is_integer()) {

                i = index.to_integral<int_type>();
            }

            switch (_cat) {

            case (NUMERIC_CATEGORY::decimal): {

                if (i > 0) {

                    i = _scale - i;

                    whole_number power = TEN.pow(i);

                    whole_number q, r;
                    _num.div_rem(power, q, r);

                    _num = q;

                    if (r.is()) {
                        set_round(r);
                    }

                    _num *= power;
                }
                else {  // Round to zero decimal spaces.
                    whole_number q, r;
                    _num.div_rem(_den, q, r);

                    _num = q * _den;

                    if (r.is()) {
                        set_round(r);
                    }
                }

            } break;

            case (NUMERIC_CATEGORY::integer): {

                if (i > 1) {

                    i -= 1;

                    whole_number power = TEN.pow(i);

                    whole_number q, r;
                    _num.div_rem(power, q, r);

                    _num = q;

                    if (r.is()) {
                        set_round(r);
                    }

                    power = TEN.pow(i);

                    _num *= power;
                }

            } break;

            case (NUMERIC_CATEGORY::rational): {

                numeric_type a = to_decimal();

                a.round_to(index);

                *this = a.to_rational();

            } break;

            default:
                break;
            }

            return *this;
        }

        numeric_type numeric_type::round_to(const numeric_type& index) const {

            numeric_type a = *this;

            a.round_to(index);

            return a;
        }

        str_type numeric_type::to_string() const {

            if (is_defined()) {

                str_type result = "";

                if (is_negative()) {
                    result += "-";
                }

                result += get_potential_prefix();

                switch (_cat) {
                case (NUMERIC_CATEGORY::decimal): {

                    whole_number q, r;
                    _num.div_rem(_den, q, r);

                    result += q.to_string(0);
                    result += ".";

                    str_type rem = r.to_string(0);

                    if (r.is()) {

                        if (static_cast<int_type>(rem.size()) < _scale) {

                            auto count = _scale - static_cast<int_type>(rem.size());

                            rem.insert(0, count, '0');
                        }

                    }
                    else {
                        while (static_cast<int_type>(rem.size()) < _scale) {
                            rem += "0";
                        }
                    }
                    result += rem;
                } break;

                case (NUMERIC_CATEGORY::rational): {
                    result += (_den == 1 ? _num.to_string(0) : _num.to_string(0) + "/" + _den.to_string(0));
                } break;

                case (NUMERIC_CATEGORY::integer):
                case (NUMERIC_CATEGORY::whole): {
                    result += _num.to_string(0);
                } break;

                case (NUMERIC_CATEGORY::heximal): {
                    result += _num.to_string(16);
                } break;

                case (NUMERIC_CATEGORY::octal): {
                    result += _num.to_string(8);
                } break;

                case (NUMERIC_CATEGORY::binary): {
                    result += _num.to_string(2);
                } break;

                default:
                    break;
                }

                return result;
            }

            if (NUMERIC_CATEGORY::undef == _cat) {

                return "undef";
            }

            return "nan";
        }

        str_type numeric_type::get_potential_prefix() const {

            switch (_cat) {

            case NUMERIC_CATEGORY::whole:
                return "u";
                break;

            case NUMERIC_CATEGORY::heximal:
                return "0x";
                break;

            case NUMERIC_CATEGORY::octal:
                return "0";
                break;

            case NUMERIC_CATEGORY::binary:
                return "0b";
                break;

            default:
                break;
            }
            return "";
        }

        void numeric_type::set_integer(str_type& value) {

            initialize_value_of_the_sign(value);

            _num = whole_number(value);

            check_for_zero();
        }

        void numeric_type::set_decimal(str_type& value) {

            initialize_value_of_the_sign(value);

            auto scale = get_sub_text_value(value, "s").abs();

            auto exponent = get_sub_text_value(value, "e");

            find_and_set_decimal_denominator(value);

            _num = whole_number(value);

            set_decimal_exponent(exponent);

            set_decimal_scale(scale);

            check_for_zero();
        }

        void Olly::MPA::numeric_type::set_rational(str_type& value) {

            initialize_value_of_the_sign(value);

            _cat = NUMERIC_CATEGORY::rational;

            // Locate and set the denominator.
            auto denominator = get_sub_text_value(value, "/");

            if (!denominator.is()) {
                *this = UNDEF;
                return;
            }

            rtrim(value);

            // Look if a leading integer is present and get it.
            str_type lead_val_str = "";

            auto found = value.find(' ');

            if (found != std::string::npos) {

                lead_val_str = value.substr(0, found);

                value.erase(0, found);
            }

            // Define the numerator.
            auto numerator = numeric_type(value);

            // Define the leading integer.
            auto lead_value = numeric_type(lead_val_str);

            _num = numerator._num;
            _den = denominator._num;

            *this += lead_value;

            if (reduce_fractions_to_lowest_terms()) {
                *this = lowest_terms();
            }

            check_for_zero();
        }

        void numeric_type::set_whole_number(str_type& value) {

            value[0] = ' ';

            _num = whole_number(value);

            _sign = SIGN::positive;

            check_for_zero();

            if (_cat == NUMERIC_CATEGORY::integer) {
                _cat = NUMERIC_CATEGORY::whole;
            }
        }

        void numeric_type::set_binary(str_type& value) {

            value[0] = ' ';
            value[1] = ' ';

            _num = whole_number(value, 2);

            _sign = SIGN::positive;

            check_for_zero();

            if (_cat == NUMERIC_CATEGORY::integer) {
                _cat = NUMERIC_CATEGORY::binary;
            }
        }

        void numeric_type::set_heximal(str_type& value) {

            value[0] = ' ';
            value[1] = ' ';

            _num = whole_number(value, 16);

            _sign = SIGN::positive;

            check_for_zero();

            if (_cat == NUMERIC_CATEGORY::integer) {
                _cat = NUMERIC_CATEGORY::heximal;
            }
        }

        void numeric_type::set_octal(str_type& value) {

            value[0] = ' ';

            _num = whole_number(value, 8);

            _sign = SIGN::positive;

            check_for_zero();

            if (_cat == NUMERIC_CATEGORY::integer) {
                _cat = NUMERIC_CATEGORY::octal;
            }
        }

        void numeric_type::initialize_value_of_the_sign(str_type& value) {

            _sign = SIGN::positive;

            if (value.front() == '-') {

                _sign = SIGN::negative;
                value.front() = ' ';
            }
            else if (value.front() == '+') {

                value.front() = ' ';
            }
        }

        numeric_type numeric_type::get_sub_text_value(str_type& value, str_type del) const {
            /*
                See if a value is defined for a specific deliminator.  If found get and
                return that value, and preserve the remainder of the string.
            */
            auto found = value.find(del);
            if (found == std::string::npos) {

                found = value.find(to_upper(del));
            }

            str_type sub_string_value = "";

            numeric_type derived_value;

            if (found != std::string::npos) {

                if (value.begin() + found < value.end()) {
                    sub_string_value = value.substr(found + 1);
                }
                value.erase(found);

                derived_value = numeric_type(sub_string_value);
            }

            return derived_value;
        }

        void numeric_type::find_and_set_decimal_denominator(str_type& value) {
            /*
                Find the decimal point which is required to pass the regex test
                applied during the object constructer.  Then set the denominator.
            */
            auto found = value.find(".");
            value.at(found) = ' ';

            _scale = value.size() - found - 1;
            if (_scale < 1) {
                _scale = 1;
            }
            _den = TEN.pow(_scale);
            _cat = NUMERIC_CATEGORY::decimal;
        }

        void numeric_type::set_decimal_exponent(numeric_type& exponent) {
            /*
                Test if an exponent was defined then apply the exponent.
                Then reset the _scale of the decimal number.
            */
            if (exponent.is()) {

                if (exponent.is_negative()) {

                    _den *= TEN.pow(exponent._num);
                }
                else if (exponent.is_positive()) {

                    _num *= TEN.pow(exponent._num);
                }

                set_internal_value_of_scale();
            }
        }

        void numeric_type::set_decimal_scale(numeric_type scale) {
            /*
                This helper function accepts a scale to be applied the this
                object.  Then either increases or decreases the scale of the
                decimal number.
            */

            if (scale < get_min_dec_scale()) {
                scale = get_min_dec_scale();
            }

            auto test = scale.compare(_scale);

            if (scale.is() && test) {

                if (test > 0) {
                    // Set the scale of the decimal number to be larger.

                    _num *= TEN.pow(scale._num - _scale);
                    _den = TEN.pow(scale._num);
                }
                else if (test < 0) {
                    // Set the scale of the decimal number to be smaller.

                    _num /= TEN.pow(whole_number(_scale) - scale._num);
                    _den = TEN.pow(scale._num);
                }

                set_internal_value_of_scale();
            }
        }

        void numeric_type::set_internal_value_of_scale() {
            /*
                This helper function confirms the scale of the
                denominator and the integral scale of this instance
                match.
            */

            _scale = _den.log10().to_integral<int_type>();

            whole_number test = TEN.pow(_scale);

            while (test > _den) {
                test = test / TEN;
                _scale -= 1;
            }
        }

        void numeric_type::check_for_zero() {

            if (is_defined()) {

                if (!_num.is() && _sign != SIGN::zero) {
                    _sign = SIGN::zero;
                }
            }
        }

    }  // end MPA
} // end Olly
