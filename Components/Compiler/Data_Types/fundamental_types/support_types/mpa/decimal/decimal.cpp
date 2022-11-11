
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

#include "./decimal.h"

namespace Olly {
    namespace MPA {

        /********************************************************************************************/
        //
        //                        Static & Constant Variable Intantiation.
        //
        /********************************************************************************************/

        const int_type& decimal::get_scale() {
            return decimal_scale();
        }

        void Olly::MPA::decimal::set_scale(const int_type& scale) {
            if (scale_not_configured()) {
                auto s = (scale < MAX_SCALE ? scale : MAX_SCALE);

                decimal_scale() = s;
                denominator() = TEN.pow(s);
                scale_not_configured() = false;
                whole_number_pi();
                 whole_number_e();
               whole_number_ln2();
                    decimal_360();
                    decimal_180();
                     decimal_90();
                     decimal_45();
            }
        }

        const str_type decimal::get_rmode() {

            str_type res = "";

            switch (get_rmode_integral()) {

            case (ROUNDING_MODE::half_up):
                res = "half up";
                break;

            case (ROUNDING_MODE::half_down):
                res = "half down";
                break;

            case (ROUNDING_MODE::half_even):
                res = "half even";
                break;

            case (ROUNDING_MODE::half_odd):
                res = "half odd";
                break;

            case (ROUNDING_MODE::ceil):
                res = "ceil";
                break;

            case (ROUNDING_MODE::floor):
                res = "floor";
                break;

            case (ROUNDING_MODE::toward_zero):
                res = "toward zero";
                break;

            case (ROUNDING_MODE::away_from_zero):
                res = "away from zero";
                break;

            default:
                res = "none";
                break;
            }

            return res;
        }

        const decimal::ROUNDING_MODE& decimal::get_rmode_integral() {
            return rounding_mode();
        }

        void decimal::set_rmode(const str_type& mode) {

            if (mode == "none") {
                rounding_mode() = ROUNDING_MODE::none;
                return;
            }

            if (mode == "half up") {
                rounding_mode() = ROUNDING_MODE::half_up;
                return;
            }

            if (mode == "half down") {
                rounding_mode() = ROUNDING_MODE::half_down;
                return;
            }

            if (mode == "half even") {
                rounding_mode() = ROUNDING_MODE::half_even;
                return;
            }

            if (mode == "half odd") {
                rounding_mode() = ROUNDING_MODE::half_odd;
                return;
            }

            if (mode == "ceil") {
                rounding_mode() = ROUNDING_MODE::ceil;
                return;
            }

            if (mode == "floor") {
                rounding_mode() = ROUNDING_MODE::floor;
                return;
            }

            if (mode == "toward zero") {
                rounding_mode() = ROUNDING_MODE::toward_zero;
                return;
            }

            if (mode == "away from zero") {
                rounding_mode() = ROUNDING_MODE::away_from_zero;
                return;
            }
        }

        void decimal::set_rmode(const ROUNDING_MODE mode) {
            rounding_mode() = mode;
        }

        const whole_number& decimal::get_denominator() {
            return denominator();
        }

        int_type& decimal::decimal_scale() {

            static int_type dec_scale = 16;

            return dec_scale;
        }

        whole_number& decimal::denominator() {

            static whole_number den = TEN.pow(decimal_scale());

            return den;
        }

        bool_type& decimal::scale_not_configured() {

            static bool_type is_configured = true;

            return is_configured;
        }

        decimal::ROUNDING_MODE& decimal::rounding_mode() {

            static ROUNDING_MODE mode = ROUNDING_MODE::half_even;

            return mode;
        }

        whole_number& decimal::whole_number_pi() {

            static whole_number pi = whole_number(PI_STRING.substr(0, decimal_scale() + 2));

            return pi;
        }

        whole_number& decimal::whole_number_e() {

            static whole_number e = whole_number(E_STRING.substr(0, decimal_scale() + 2));

            return e;
        }

        whole_number& decimal::whole_number_ln2() {

            decimal a("2.0");

            static whole_number ln_2 = a.get_ln()._num;

            return ln_2;
        }

        decimal& decimal::decimal_360() {

            static decimal deg = decimal(denominator() * 360);

            return deg;
        }

        decimal& decimal::decimal_180() {

            static decimal deg = decimal(denominator() * 180);

            return deg;
        }

        decimal& decimal::decimal_90() {

            static decimal deg = decimal(denominator() * 90);

            return deg;
        }

        decimal& decimal::decimal_45() {

            static decimal deg = decimal(denominator() * 45);

            return deg;
        }

        decimal& decimal::decimal_pi() {

            static decimal deg = decimal(whole_number_pi());

            return deg;
        }

        /********************************************************************************************/
        //
        //                                  Class Implementation
        //
        /********************************************************************************************/

        const whole_number decimal::ONE = whole_number(1);
        const whole_number decimal::TWO = whole_number(2);
        const whole_number decimal::TEN = whole_number(10);

        decimal::decimal() : _sign(SIGN::zero), _num(0) {
            if (scale_not_configured()) {
                scale_not_configured() = false;
            }
        }

        decimal::decimal(SIGN value) : _sign(value), _num(0) {
            if (scale_not_configured()) {
                scale_not_configured() = false;
            }
        }

        decimal::decimal(whole_number value) : _sign(SIGN::positive), _num(value) {
            if (scale_not_configured()) {
                scale_not_configured() = false;
            }

            check_for_zero();
        }

        decimal::decimal(int_type value) : _sign(SIGN::zero), _num(value ? std::abs(value) : 0) {
            if (scale_not_configured()) {
                scale_not_configured() = false;
            }

            _num *= get_denominator();

            if (value < 0) {
                _sign = SIGN::negative;
            }
            else if (value > 0) {
                _sign = SIGN::positive;
            }
        }

        decimal::decimal(str_type value) : _sign(SIGN::zero), _num(0) {
            if (scale_not_configured()) {
                scale_not_configured() = false;
            }

            lrtrim(value);

            if (!value.empty()) {

                _sign = SIGN::positive;

                if (value.front() == '-') {

                    _sign = SIGN::negative;
                    value.front() = ' ';
                }
                else if (value.front() == '+') {

                    value.front() = ' ';
                }

                auto exponent = get_sub_text_value(value, "e");

                int_type scale = find_and_set_decimal_denominator(value);

                _num = whole_number(value);

                if (scale < get_scale()) {
                    _num *= (denominator() / TEN.pow(scale));
                }

                set_decimal_exponent(exponent);

                check_for_zero();
            }
        }

        decimal::decimal(str_type value, int_type base) : _sign(SIGN::zero), _num(0) {
            if (scale_not_configured()) {
                scale_not_configured() = false;
            }

            if (!value.empty()) {

                _sign = SIGN::positive;

                if (value.front() == '-') {

                    _sign = SIGN::negative;
                    value.front() = ' ';
                }
                else if (value.front() == '+') {

                    value.front() = ' ';
                }

                _num = whole_number(value);

                check_for_zero();
            }
        }

        decimal::~decimal() {
        }

        str_type decimal::to_string() const {
            
            if (is_defined()) {

                str_type result = "";

                if (is_negative()) {
                    result += "-";
                }

                whole_number q, r;
                _num.div_rem(get_denominator(), q, r);

                result += q.to_string(0);

                if (decimal_scale()) {

                    result += ".";

                    str_type rem = r.to_string(0);

                    if (r.is()) {

                        if (static_cast<int_type>(rem.size()) < get_scale()) {

                            auto count = get_scale() - static_cast<int_type>(rem.size());

                            rem.insert(0, count, '0');
                        }

                    }
                    else {
                        while (static_cast<int_type>(rem.size()) < get_scale()) {
                            rem += "0";
                        }
                    }

                    result += rem;
                }

                return result;
            }

            if (is_nan()) {

                return "nan";
            }

            return "undef";
        }

        bool_type decimal::is() const {

            if (is_defined()) {

                return _num.is();
            }
            return false;
        }

        bool_type decimal::is_not() const {
            return !is();
        }

        bool_type decimal::is_odd() const {

            if (is_defined()) {

                return _num.is_odd();
            }
            return false;
        }

        bool_type decimal::is_even() const {

            if (is_defined()) {

                return _num.is_even();
            }
            return false;
        }

        bool_type decimal::is_zero() const {

            if (_sign == SIGN::zero) {

                return true;
            }
            return false;
        }

        bool_type decimal::is_positive() const {

            if (is_defined() && _sign > SIGN::zero) {

                return true;
            }

            return false;
        }

        bool_type decimal::is_negative() const {

            if (is_defined() && _sign < SIGN::zero) {
                return true;
            }

            return false;
        }

        bool_type decimal::is_defined() const {
            return _sign < SIGN::defined;
        }

        bool_type decimal::is_undefined() const {
            return _sign == SIGN::undefined;
        }

        bool_type decimal::is_nan() const {
            return _sign == SIGN::nan;
        }

        str_type decimal::sign() const {

            str_type res;

            switch (_sign) {

            case (SIGN::negative):
                res = "negative";
                break;

            case (SIGN::zero):
                res = "zero";
                break;

            case (SIGN::positive):
                res = "positive";
                break;

            case (SIGN::undefined):
                res = "undefined";
                break;

            case (SIGN::nan):
                res = "nan";
                break;
            }

            return res;
        }

        decimal::SIGN decimal::sign_integral() const {
            return _sign;
        }

        bool_type decimal::operator==(const decimal& b) const {
            return compare(b) == 0.0;
        }

        bool_type decimal::operator!=(const decimal& b) const {
            return compare(b) != 0.0;
        }

        bool_type decimal::operator<(const decimal& b) const {
            return compare(b) < 0.0;
        }

        bool_type decimal::operator>(const decimal& b) const {
            return compare(b) > 0.0;
        }

        bool_type decimal::operator<=(const decimal& b) const {
            return compare(b) <= 0.0;
        }

        bool_type decimal::operator>=(const decimal& b) const {
            return compare(b) >= 0.0;
        }

        real_type decimal::compare(const decimal& other) const {

            if (is_defined() && other.is_defined()) {

                if (_sign < other._sign) {

                    return -1.0;
                }
                else if (_sign > other._sign) {

                    return 1.0;
                }

                real_type truth = _num.compare(other._num);

                if (_sign == SIGN::negative) {

                    truth *= -1.0;
                }

                return truth;
            }

            return NOT_A_NUMBER;
        }

        decimal& decimal::operator&=(const decimal& b) {

            if (is_negative()) {

                _num = _num.bin_comp();
            }
            _num.operator&=((b.is_negative() ? b._num.bin_comp() : b._num));

            return *this;
        }

        decimal& decimal::operator|=(const decimal& b) {
            
            if (is_negative()) {

                _num = _num.bin_comp();
            }
            _num.operator|=((b.is_negative() ? b._num.bin_comp() : b._num));

            return *this;
        }

        decimal& decimal::operator^=(const decimal& b) {
            
            if (is_negative()) {

                _num = _num.bin_comp();
            }
            _num.operator^=((b.is_negative() ? b._num.bin_comp() : b._num));

            return *this;
        }

        decimal decimal::operator&(const decimal& b) const {
            
            decimal a(*this);

            a &= b;

            return a;
        }

        decimal decimal::operator|(const decimal& b) const {
            
            decimal a(*this);

            a |= b;

            return a;
        }

        decimal decimal::operator^(const decimal& b) const {
            
            decimal a(*this);

            a ^= b;

            return a;
        }

        decimal decimal::operator~() const {

            decimal a = -*this;

            a += ONE;

            return a;
        }

        decimal decimal::operator<<(size_type index) const {

            decimal a(*this);

            a._num <<= index;

            return a;
        }

        decimal decimal::operator>>(size_type index) const {
            
            decimal a(*this);

            a._num >>= index;

            return a;
        }

        decimal& decimal::operator+=(const decimal& b) {

            if (_sign == b._sign) {

                _num += b._num;

                return *this;
            }

            real_type n = _num.compare(b._num);

            if (n > 0.0) {
                _num -= b._num;
            }
            else if (n < 0.0) {
                _num = b._num - _num;
                _sign = b._sign;
            }
            else {
                _num = whole_number();
            }

            check_for_zero();

            return *this;
        }

        decimal& decimal::operator-=(const decimal& b) {

            return operator+=(-b);
        }

        decimal& decimal::operator*=(const decimal& b) {

            _num *= b._num;

            _num /= denominator();

            if (_sign == b._sign) {

                _sign = SIGN::positive;
            }
            else {
                _sign = SIGN::negative;
            }

            check_for_zero();

            return *this;
        }

        decimal& decimal::operator/=(const decimal& b) {

            _num *= denominator();

            _num /= b._num;

            if (_sign == b._sign) {

                _sign = SIGN::positive;
            }
            else {
                _sign = SIGN::negative;
            }

            check_for_zero();

            return *this;
        }

        decimal& decimal::operator%=(const decimal& b) {

            _num %= b._num;

            if (_sign == b._sign) {

                _sign = SIGN::positive;
            }
            else {
                _sign = SIGN::negative;
            }

            check_for_zero();

            return *this;
        }

        decimal decimal::operator+(const decimal& b) const {

            decimal a(*this);

            a += b;

            return a;
        }

        decimal decimal::operator-(const decimal& b) const {

            decimal a(*this);

            a -= b;

            return a;
        }

        decimal decimal::operator*(const decimal& b) const {

            decimal a(*this);

            a *= b;

            return a;
        }

        decimal decimal::operator/(const decimal& b) const {

            decimal a(*this);

            a /= b;

            return a;
        }

        decimal decimal::operator%(const decimal& b) const {

            decimal a(*this);

            a %= b;

            return a;
        }

        decimal decimal::operator+() const {
            return *this;
        }

        decimal decimal::operator-() const {

            decimal a(*this);

            a.invert_sign();

            return a;
        }

        decimal& decimal::operator++() {
            
            if (is_defined()) {

                if (is_negative()) {

                    _num -= denominator();

                    check_for_zero();
                }
                else {

                    _num += denominator();

                    if (_sign == SIGN::zero) {
                        _sign = SIGN::positive;
                    }
                }
            }

            return *this;
        }

        decimal decimal::operator++(int) {

            decimal a(*this);

            operator++();

            return a;
        }

        decimal& decimal::operator--() {
            
            if (is_defined()) {

                if (is_negative()) {

                    _num += denominator();

                    if (_sign == SIGN::zero) {
                        _sign = SIGN::negative;
                    }
                }
                else {

                    _num -= denominator();

                    check_for_zero();
                }
            }

            return *this;
        }

        decimal decimal::operator--(int) {
            
            decimal a(*this);

            operator--();

            return a;
        }

        void decimal::div_rem(const decimal& b, decimal& qot, decimal& rem) const {
            qot = decimal();
            rem = decimal();

            if (is_defined() && b.is_defined()) {

                if (!b.is()) {

                    qot = decimal(SIGN::undefined);
                    rem = decimal(SIGN::undefined);

                    return;
                }

                qot = *this / b;

                rem = qot;

                rem._num %= denominator();

                qot -= rem;
            }

            else if (is_nan() || b.is_nan()) {

                qot = decimal(SIGN::nan);
                rem = decimal(SIGN::nan);
            }

            else {
                qot = decimal(SIGN::undefined);
                rem = decimal(SIGN::undefined);
            }
        }

        void decimal::div_mod(const decimal& b, decimal& qot, decimal& rem) const {
            qot = decimal();
            rem = decimal();

            if (is_defined() && b.is_defined()) {

                if (!b.is()) {

                    qot = decimal(SIGN::undefined);
                    rem = decimal(SIGN::undefined);

                    return;
                }

                _num.div_rem(b._num, qot._num, rem._num);

                qot._num *= denominator();

                if (_sign == b._sign) {

                    qot._sign = SIGN::positive;
                    rem._sign = SIGN::positive;
                }
                else {
                    qot._sign = SIGN::negative;
                    rem._sign = SIGN::negative;
                }

                qot.check_for_zero();
                rem.check_for_zero();
            }

            else if (is_nan() || b.is_nan()) {

                qot = decimal(SIGN::nan);
                rem = decimal(SIGN::nan);
            }

            else {
                qot = decimal(SIGN::undefined);
                rem = decimal(SIGN::undefined);
            }
        }

        void decimal::num_den(decimal& num, decimal& den) const {
            num_den(num, den, true);
        }

        decimal decimal::abs() const {
            
            decimal a(*this);

            if (a._sign < SIGN::zero) {
                a.invert_sign();
            }

            return a;
        }

        decimal decimal::inverse() const {

            decimal a(*this);

            whole_number d = denominator() * denominator();

            a._num = d / a._num;

            return a;
        }

        decimal decimal::ceil() const {

            if (decimal_scale() == 0) {
                return *this;
            }

            decimal a(*this);

            whole_number q, r;
            a._num.div_rem(denominator(), q, r);

            a._num = q * denominator();

            if (r.is()) {

                if (a.is_positive()) {
                    a._num += denominator();
                }
                else {
                    a._num -= denominator();
                }
            }

            return a;
        }

        decimal decimal::floor() const {

            if (decimal_scale() == 0) {
                return *this;
            }

            decimal a(*this);

            whole_number q, r;
            a._num.div_rem(denominator(), q, r);

            a._num = q * denominator();

            if (r.is()) {

                if (a.is_positive()) {
                    a._num -= denominator();
                }
                else {
                    a._num += denominator();
                }
            }

            return a;
        }

        decimal decimal::factorial() const {

            whole_number q, r;
            _num.div_rem(denominator(), q, r);

            if (!r.is()) {
                whole_number a = q;
                whole_number b = 1;

                while (a > ONE) {
                    b *= a;
                    a -= ONE;
                }

                return decimal(b * denominator());
            }

            decimal num, den;
            num_den(num, den);

            q = num._num;
            r = den._num;

            whole_number n = 1;
            whole_number d = 1;

            while (q > ONE) {
                n *= q;
                d *= r;
                q -= den._num;
            }

            n = ((n * denominator()) / d) * whole_number_pi().sqrt();
            
            return decimal(n);
        }

        decimal decimal::gcd(const decimal& b) const {

            if (is_defined() && b.is_defined()) {

                decimal a(*this);

                a._num = a._num.gcd(b._num);

                a.check_for_zero();

                if (a.is_negative()) {
                    a.invert_sign();
                }

                return a;                   
            }

            return decimal(SIGN::undefined);
        }

        decimal decimal::pow(const decimal& b) const {
            
            if (is_defined() && b.is_defined()) {

                if (b.is_negative()) {

                    return inverse().root(b.abs());
                }

                decimal num, den;
                b.num_den(num, den, false);

                decimal a(*this);

                if (den == ONE) {
                    a.to_pow(num);
                }
                else {
                    a.to_root(den);
                    a.to_pow(num);
                }

                return a;
            }

            return decimal(SIGN::undefined);
        }

        decimal decimal::root(const decimal& b) const {

            if (is_defined() && b.is_defined()) {

                if (is_negative()) {

                    return decimal(SIGN::undefined);
                }

                if (b.is_negative()) {

                    return inverse().root(b.abs());
                }

                decimal num, den;
                b.num_den(num, den, false);

                // std::cout << "num = " << num.to_string() << std::endl;
                // std::cout << "den = " << den.to_string() << std::endl;

                decimal a(*this);

                if (den == ONE) {
                    a.to_root(num);
                }
                else {
                    a.to_root(num);
                    a.to_pow(den);
                }

                return a;
            }

            return decimal(SIGN::undefined);
        }

        decimal decimal::sqrt() const {
            return root(2);
        }

        decimal decimal::cbrt() const {
            return root(3);
        }

        decimal decimal::hypot(const decimal& b) const {
            return (pow(2) + b.pow(2)).sqrt();
        }

        decimal decimal::hypot(const decimal& b, const decimal& c) const {
            return (pow(2) + b.pow(2) + b.pow(2)).sqrt();
        }

        decimal decimal::ln() const {

            if (_sign < SIGN::positive) {
                return decimal(SIGN::undefined);
            }

            if ((_num > denominator())) {
                /*
                    Reduce the number to less than the denominator.
                    Then get the matural logarithm of the reduced value.  
                    This greatly improves speed of convergance.
                */
                whole_number x(_num);
                whole_number exp;
                while (x >= denominator()) {
                    exp += ONE;
                    x /= TWO;
                }

                // return ln(x) + ln(2) * exp
                return (decimal(x).get_ln() + decimal(whole_number_ln2()) * decimal(exp * denominator()));
            }
            else if ((_num == denominator())) {
                return decimal();
            }
            else if ((_num == whole_number_e())) {
                return decimal(denominator());
            }

            return get_ln();
        }

        decimal decimal::log2() const {
            
            return log(decimal(2));
        }

        decimal decimal::log10() const {

            return log(decimal(10));
        }

        decimal decimal::log(const decimal& b) const {

            if (is_defined() && is_positive()) {

                decimal a = ln() / b.ln();

                decimal c(a);

                c.set_round(0, ROUNDING_MODE::away_from_zero);

                if (b.pow(c) == *this) {
                    return c;
                }

                return a;
            }

            return decimal(SIGN::undefined);
        }

        decimal decimal::sin() const {

            decimal a(*this);

            bool_type neg = a.is_negative() ? true : false;

            if (neg) {
                a._sign = SIGN::positive;
            }

            if (a > decimal_360()) {
                a %= decimal_360();
            }

            if (a > decimal_180()) {
                a %= decimal_180();
                neg = !neg;
            }

            a *= (decimal_pi() / decimal_180());

            decimal y = a;
            perform_taylor_series_sin_cos(a, y, 3);

            return neg ? -y : y;
        }

        decimal decimal::cos() const {

            decimal a(*this);

            bool_type neg = a.is_negative() ? true : false;

            if (neg) {
                a._sign = SIGN::positive;
            }

            if (a > decimal_360()) {
                a %= decimal_360();
            }

            if (a > decimal_180()) {
                a %= decimal_180();
                neg = !neg;
            }

            a *= (decimal_pi() / decimal_180());

            decimal y(denominator());
            perform_taylor_series_sin_cos(a, y, 2);

            return neg ? -y : y;
        }

        decimal decimal::tan() const {

            return sin() / cos();
        }

        decimal decimal::round(const decimal& scale) const {
        
            int_type scl = scale.to_integral<int_type>();

            decimal a(*this);

            a.set_round(scl, rounding_mode());

            return a;
        }

        void decimal::invert_sign() {

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

        decimal decimal::get_sub_text_value(str_type& value, str_type del) const {
            /*
                See if a value is defined for a specific deliminator.  If found get and
                return that value, and preserve the remainder of the string.
            */
            auto found = value.find(del);
            if (found == std::string::npos) {

                found = value.find(to_upper(del));
            }

            str_type sub_string_value = "";

            decimal derived_value;

            if (found != std::string::npos) {

                if (value.begin() + found < value.end()) {
                    sub_string_value = value.substr(found + 1);
                }
                value.erase(found);

                derived_value = decimal(sub_string_value);
            }

            return derived_value;
        }

        int_type decimal::find_and_set_decimal_denominator(str_type& value) {
            /*
                Find the decimal point which is required to pass the regex test
                applied during the object constructer.  Then set the denominator.
            */
            int_type scale = 0;

            auto found = value.find(".");

            if (found != std::string::npos) {
                value.at(found) = ' ';

                scale = static_cast<int_type>(value.size() - found) - 1;
                if (scale < 1) {
                    scale = 0;
                }
            }

            if (scale > get_scale()) {

                value.resize(value.size() - (scale - get_scale()));
            }
            
            return scale;
        }

        void decimal::set_decimal_exponent(decimal& exponent) {
            /*
                Test if an exponent was defined then apply the exponent.
                Then reset the _scale of the decimal number.
            */
            if (exponent.is()) {

                if (exponent.is_negative()) {

                    _num /= TEN.pow(exponent._num / denominator());
                }
                else if (exponent.is_positive()) {

                    _num *= TEN.pow(exponent._num / denominator());
                }
            }
        }

        void decimal::check_for_zero() {

            if (is_defined()) {

                if (!_num.is() && _sign != SIGN::zero) {
                    _sign = SIGN::zero;
                }
            }
        }

        void decimal::num_den(decimal& num, decimal& den, bool_type run) const {
            num = decimal();
            den = decimal();

            if (is_defined()) {

                if (decimal_scale() == 0) {
                    num = *this;
                    den = ONE;
                    return;
                }

                whole_number q, r;
                _num.div_rem(denominator(), q, r);

                whole_number d = (r % 3).is() ? denominator() : denominator() - ONE;

                whole_number gcd = r.gcd(d);

                if (run) {
                    num = (r / gcd) * denominator();
                    den = (d / gcd) * denominator();
                }
                else {
                    num = (r / gcd);
                    den = (d / gcd);
                }

                num += (q * den._num);

                num.check_for_zero();
                den.check_for_zero();
            }

            else if (is_nan()) {

                num = decimal(SIGN::nan);
                den = decimal(SIGN::nan);
            }

            else {
                num = decimal(SIGN::undefined);
                den = decimal(SIGN::undefined);
            }
        }

        void decimal::to_pow(const decimal& b) {

            _num = _num.pow(b._num);


            operator/=(denominator().pow(b._num.to_integral<int_type>()));

            check_for_zero();

            if (is_negative() && b.is_odd()) {
                _sign = SIGN::negative;
            }
            else {
                _sign = SIGN::positive;
            }
        }

        void decimal::to_root(const decimal& b) {

            operator*=(denominator().pow(b._num.to_integral<int_type>()));

            _num = _num.root(b._num);

            check_for_zero();

            if (is_negative() && b.is_odd()) {
                _sign = SIGN::negative;
            }
            else {
                _sign = SIGN::positive;
            }
        }

        void decimal::set_round(int_type scale, const ROUNDING_MODE& mode) {

            scale = decimal_scale() - scale;

            whole_number scalar = TEN.pow(scale);

            whole_number qot, rem;
            _num.div_rem(scalar, qot, rem);

            rem /= (scalar / 10);

            switch (mode) {

            case ROUNDING_MODE::half_even: {

                whole_number five(5);

                if (rem > five) {
                    qot += 1;
                }
                if (rem == five) {

                    if (is_odd()) {
                        qot += 1;
                    }
                }
            } break;

            case ROUNDING_MODE::half_odd: {

                whole_number five(5);

                if (rem > five) {
                    qot += 1;
                }
                if (rem == five) {

                    if (is_even()) {
                        qot += 1;
                    }
                }
            } break;

            case ROUNDING_MODE::half_up: {

                whole_number five(5);
                if (is_positive() && rem >= five) {
                    qot += 1;
                }
                else if (is_negative() && rem > five) {
                    qot += 1;
                }
            } break;

            case ROUNDING_MODE::half_down: {

                whole_number five(5);
                if (is_positive() && rem > five) {
                    qot += 1;
                }
                else if (is_negative() && rem >= five) {
                    qot += 1;
                }
            } break;

            case ROUNDING_MODE::ceil: {

                if (is_positive() && rem.is()) {
                    qot += 1;
                }
            } break;

            case ROUNDING_MODE::floor: {

                if (is_negative() && rem.is()) {
                    qot += 1;
                }
            } break;

            case ROUNDING_MODE::away_from_zero: {

                if (rem.is()) {
                    qot += 1;
                }
            } break;

            default:
                break;
            }

            _num = qot * scalar;
        }

        decimal decimal::get_ln() const {
            /*
                Perform a Taylor Series estimation of 
                the natural logorithm of this number.
            */

            decimal one(1);
            decimal two(2);

            decimal r = (*this - one) / (*this + one);

            decimal a = r;

            decimal t = r;

            decimal p(3);

            decimal l;

            // Perform the calculation until the last 'l' decimal
            // is the same as the current calculation 't'.  
            while (l != t) {
                l = t;

                a *= r;
                a *= r;

                t += (one / p) * a;

                p += two;
            }

            return t * 2;
        }

        void decimal::perform_taylor_series_sin_cos(const decimal& x, decimal& n, int_type i) const {

            decimal mult(denominator());

            int_type limit = i + decimal_scale();

            while (i < limit) {
                
                mult = -mult;

                decimal exp(denominator() * i);

                decimal t = x.pow(exp) / exp.factorial();

                n += mult * t;

                i += 2;
            }
        }

    }  // end MPA
} // end Olly