
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

#include "./real.h"

namespace Olly {
	namespace MPA {

		real::real() : _num(), _den(1), _sign(SIGN::zero), _scale(0), _cat(NUMERIC_CATEGORY::integer), _rmode(ROUNDING_MODE::half_even), _set_lt(true) {
		}

		real::real(int_type value) : _num(value ? std::abs(value) : 0), _den(1), _sign(SIGN::zero), _scale(0), _cat(NUMERIC_CATEGORY::integer), _rmode(ROUNDING_MODE::half_even), _set_lt(true) {

			if (value < 0) {
				_sign = SIGN::negative;
			}
			else if (value > 0) {
				_sign = SIGN::positive;
			}
		}

		real::real(str_type value, str_type r_mode) : _num(), _den(1), _sign(SIGN::zero), _scale(0), _cat(NUMERIC_CATEGORY::integer), _rmode(ROUNDING_MODE::half_even), _set_lt(true) {

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

		real::real(const whole_number& num, const SIGN s) : _num(num), _den(1), _sign(s), _scale(0), _cat(NUMERIC_CATEGORY::integer), _rmode(ROUNDING_MODE::half_even), _set_lt(true) {
			check_for_zero();
		}

		real::real(const int_type& scale, const ROUNDING_MODE& mode) : _num(), _den(1), _sign(SIGN::zero), _scale(scale), _cat(NUMERIC_CATEGORY::decimal), _rmode(mode), _set_lt(true) {
			_den = whole_number(10).pow(_scale);
		}

		void real::set_integer(str_type& value) {

			initialize_value_of_the_sign(value);

			_num = whole_number(value);

			check_for_zero();

			confirm_category();
		}

		void real::set_decimal(str_type& value) {

			initialize_value_of_the_sign(value);

			auto scale = get_sub_text_value(value, "s").abs();

			auto exponent = get_sub_text_value(value, "e");

			find_and_set_decimal_denominator(value);

			_num = whole_number(value);

			set_decimal_exponent(exponent);

			set_decimal_scale(scale);

			check_for_zero();

			confirm_category();
		}

		void Olly::MPA::real::set_rational(str_type& value) {

			// Locate and set the denominator.
			auto denominator = get_sub_text_value(value, "/");

			if (!denominator.is()) {
				*this = real("undef");
				return;
			}

			while (!value.empty() && std::isspace(value.back())) {
				value.pop_back();
			}

			// Look if a leading integer is present and get it.
			str_type lead_val_str = "";

			auto found = value.find(' ');

			if (found != std::string::npos) {

				lead_val_str = value.substr(0, found);

				value.erase(0, found);
			}

			// Define the numerator.
			auto numerator = real(value);

			// Define the leading integer.
			auto lead_value = real(lead_val_str);

			// If only integral values have been passed, set the values of the number.  
			if (denominator._cat <= NUMERIC_CATEGORY::integer && numerator._cat <= NUMERIC_CATEGORY::integer) {

				_num = numerator._num;
				_den = denominator._num;

				if (numerator._sign != denominator._sign) {
					_sign = SIGN::negative;
				}
				else {
					_sign = SIGN::positive;
				}

				check_for_zero();

				confirm_category();

				if (lead_value.is() && lead_value._cat <= NUMERIC_CATEGORY::integer) {

					if (_sign != lead_value._sign) {
						invert_sign();
					}

					_num += (lead_value._num * _den);

					check_for_zero();

					confirm_category();
				}
			}
			//  Else create a decimal value of the rational argument provided.  
			else {

				real base("0.0");

				base += lead_value;

				base += (numerator / denominator);

				*this = base;

				check_for_zero();

				confirm_category();
			}

			if (_set_lt) {
				lowest_terms();
			}
		}

		void real::set_binary(str_type& value) {

			value[0] = ' ';
			value[1] = ' ';

			_num = whole_number(value, 2);

			_sign = SIGN::positive;

			check_for_zero();

			confirm_category();

			if (_cat == NUMERIC_CATEGORY::integer) {
				_cat = NUMERIC_CATEGORY::binary;
			}
		}

		void real::set_heximal(str_type& value) {

			value[0] = ' ';
			value[1] = ' ';

			_num = whole_number(value, 16);

			_sign = SIGN::positive;

			check_for_zero();

			confirm_category();

			if (_cat == NUMERIC_CATEGORY::integer) {
				_cat = NUMERIC_CATEGORY::heximal;
			}
		}

		void real::set_octal(str_type& value) {

			value[0] = ' ';

			_num = whole_number(value, 8);

			_sign = SIGN::positive;

			check_for_zero();

			confirm_category();

			if (_cat == NUMERIC_CATEGORY::integer) {
				_cat = NUMERIC_CATEGORY::octal;
			}
		}

		real::~real() {
		}

		bool_type real::is() const {

			if (is_defined()) {

				return _num.is();
			}
			return false;
		}

		bool_type real::is_odd() const {

			if (is_defined()) {

				return _num.is_odd();
			}
			return false;
		}

		bool_type real::is_even() const {

			if (is_defined()) {

				return _num.is_even();
			}
			return false;
		}

		bool_type real::is_positive() const {

			if (is_defined() && _sign > SIGN::zero) {

				return _num.is_even();
			}

			return false;
		}

		bool_type real::is_negative() const {

			if (is_defined() && _sign < SIGN::zero) {
				return true;
			}

			return false;
		}

		bool_type real::is_defined() const {
			return _cat > NUMERIC_CATEGORY::undef;
		}

		bool_type real::is_undefined() const {
			return _cat == NUMERIC_CATEGORY::undef;
		}

		bool_type real::is_nan() const {
			return _cat == NUMERIC_CATEGORY::nan;
		}

		str_type real::get_category() const {

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
		}

		NUMERIC_CATEGORY real::get_cat_integral() const {
			return _cat;
		}

		int_type real::get_scale() const {
			return _scale;
		}

		real& real::set_scale(const real& scale) {

			if (_cat < NUMERIC_CATEGORY::decimal) {

				*this = to_decimal();
			}

			set_decimal_scale(scale.abs());

			return *this;
		}

		real real::set_scale(const real& scale) const {

			real n = *this;

			n.set_decimal_scale(scale.abs());

			return n;
		}

		str_type real::get_rmode() const {

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

		real& real::set_rmode(const str_type& rmode) {
		
			if (!rmode.empty()) {

				if (rmode == "half up") {
					_rmode = ROUNDING_MODE::half_up;
				}
				else if (rmode == "half down") {
					_rmode = ROUNDING_MODE::half_down;
				}
				else if (rmode == "half even") {
					_rmode = ROUNDING_MODE::half_even;
				}
				else if (rmode == "half odd") {
					_rmode = ROUNDING_MODE::half_odd;
				}
				else if (rmode == "ceil") {
					_rmode = ROUNDING_MODE::ceil;
				}
				else if (rmode == "floor") {
					_rmode = ROUNDING_MODE::floor;
				}
				else if (rmode == "toward zero") {
					_rmode = ROUNDING_MODE::toward_zero;
				}
				else if (rmode == "away from zero") {
					_rmode = ROUNDING_MODE::away_from_zero;
				}
			}

			return *this;
		}

		real real::set_rmode(const str_type& rmode) const {

			real a = *this;

			a.set_rmode(rmode);

			return a;
		}
		real real::get_numerator() const {
			return real(_num, _sign);
		}

		real real::get_denominator() const {
			return real(_den, SIGN::positive);
		}

		bool_type real::is_decimal() const {
			return _cat == NUMERIC_CATEGORY::decimal;
		}

		bool_type real::is_rational() const {
			return _cat == NUMERIC_CATEGORY::rational;
		}

		bool_type real::is_integer() const {
			return _cat == NUMERIC_CATEGORY::integer;
		}

		bool_type real::is_heximal() const {
			return _cat == NUMERIC_CATEGORY::heximal;
		}

		bool_type real::is_octal() const {
			return _cat == NUMERIC_CATEGORY::octal;
		}

		bool_type real::is_binary() const {
			return _cat == NUMERIC_CATEGORY::binary;
		}

		bool_type real::operator==(const real& b) const {
			return compare(b) == 0.0;
		}

		bool_type real::operator!=(const real& b) const {
			return compare(b) != 0.0;
		}

		bool_type real::operator<(const real& b) const {
			return compare(b) < 0.0;
		}

		bool_type real::operator>(const real& b) const {
			return compare(b) > 0.0;
		}

		bool_type real::operator<=(const real& b) const {
			return compare(b) <= 0.0;
		}

		bool_type real::operator>=(const real& b) const {
			return compare(b) >= 0.0;
		}

		real_type real::compare(const real& other) const {

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
							real a = set_scale(other._scale);
							return a.compare(other);
						}
					}
					else if (is_rational()) {

						if (_den == other._den) {

							return numerator_compare(other);
						}
						else {
							real b = other;

							b._num *= _den;
							b._den *= _den;

							real a = *this;

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

		real& real::operator&=(const real& other) {


			if (other._cat <= NUMERIC_CATEGORY::integer) {

				if (_cat <= NUMERIC_CATEGORY::integer) {

					if (is_negative()) {
						_num = _num.bin_comp();
					}

					auto n = other.is_negative() ? other._num.bin_comp() : other._num;

					_num &= n;
				}
			}
			return *this;
		}

		real& real::operator|=(const real& other) {

			if (_cat <= NUMERIC_CATEGORY::integer) {

				if (_cat <= NUMERIC_CATEGORY::integer) {

					if (is_negative()) {
						_num = _num.bin_comp();
					}

					auto n = other.is_negative() ? other._num.bin_comp() : other._num;

					_num |= n;
				}
			}
			return *this;
		}

		real& real::operator^=(const real& other) {

			if (_cat <= NUMERIC_CATEGORY::integer) {

				if (_cat <= NUMERIC_CATEGORY::integer) {

					if (is_negative()) {
						_num = _num.bin_comp();
					}

					auto n = other.is_negative() ? other._num.bin_comp() : other._num;

					_num ^= n;
				}
			}
			return *this;
		}

		real& real::operator<<=(size_type index) {

			if (_cat <= NUMERIC_CATEGORY::integer) {

				_num <<= index;
			}
			else {
				whole_number ten(10);

				_num *= ten.pow(index);
			}

			check_for_zero();

			return *this;
		}

		real& real::operator>>=(size_type index) {

			if (_cat <= NUMERIC_CATEGORY::integer) {

				_num <<= index;
			}
			else {
				whole_number ten(10);

				_num /= ten.pow(index);
			}

			check_for_zero();

			return *this;
		}

		real real::operator&(const real& b) const {

			real a = *this;

			a &= b;

			return a;
		}

		real real::operator|(const real& b) const {

			real a = *this;

			a |= b;

			return a;
		}

		real real::operator^(const real& b) const {

			real a = *this;

			a ^= b;

			return a;
		}

		real real::operator~() const {

			real a;

			a._num = ~_num;

			return a;
		}

		real real::operator<<(size_type index) const {

			real a = *this;

			a <<= index;

			return a;
		}

		real real::operator>>(size_type index) const {

			real a = *this;

			a >>= index;

			return a;
		}

		real& real::operator+=(const real& other) {

			if (is_defined() && other.is_defined()) {

				if (_cat == other._cat) {

					if (is_rational()) {

						if (_den == other._den) {

							numerator_addition(other);

							if (_set_lt) {
								lowest_terms();
							}

							return *this;
						}
						else {
							// Fraction addition.

							real b = other;

							b._num *= _den;
							b._den *= _den;

							_num *= other._den;
							_den *= other._den;

							return operator+=(b);
						}
					}
					else {

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
				}
				else if (_cat > other._cat) {
					return operator+=(other.promote_category(*this));
				}
				else {
					*this = promote_category(other);
					return operator+=(other);
				}
			}
		
			*this = real("undef");

			return *this;
		}

		real& real::operator-=(const real& other) {

			return operator+=(-other);
		}

		real& real::operator*=(const real& other) {

			if (is_defined() && other.is_defined()) {

				if (_cat == other._cat) {

					if (is_rational()) {

						if (_den == other._den) {

							numerator_multiplication(other);

							if (_set_lt) {
								lowest_terms();
							}

							check_for_zero();

							return *this;
						}
						else {

							_num *= other._num;
							_den *= other._den;

							if (_sign == other._sign) {

								_sign = SIGN::positive;
							}
							else {
								_sign = SIGN::negative;
							}

							if (_set_lt) {
								lowest_terms();
							}

							check_for_zero();

							return *this;
						}
					}
					else {

						if (_scale > other._scale) {
							return operator*=(other.set_scale(_scale));
						}
						else if (_scale < other._scale) {
							*this = set_scale(other._scale);
							return operator*=(other);
						}

						numerator_multiplication(other);

						if (_scale) {
							_num /= _den;
						}

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
			
			*this = real("undef");

			return *this;
		}

		real& real::operator/=(const real& other) {

			if (is_defined() && other.is_defined()) {

				if (!other.is()) {

					*this = real("undef");

					return *this;
				}

				if (_cat == other._cat) {

					if (is_rational()) {

						return operator*=(other.inverse());
					}
					else {

						if (_scale > other._scale) {
							return operator/=(other.set_scale(_scale));
						}
						else if (_scale < other._scale) {
							*this = set_scale(other._scale);
							return operator/=(other);
						}

						numerator_division(other);

						return *this;

					}
				}
				else if (_cat > other._cat) {
					return operator/=(other.promote_category(*this));
				}
				else {
					*this = promote_category(other);
					return operator/=(other);
				}
			}
			
			*this = real("undef");

			return *this;
		}

		real& real::operator%=(const real& other) {

			if (is_defined() && other.is_defined()) {

				if (_cat == other._cat) {

					if (is_rational()) {

						*this = *this - (other * (*this / other).floor());

						if (_set_lt) {
							lowest_terms();
						}

						check_for_zero();

						return *this;
					}
					else {

						if (_scale > other._scale) {
							return operator%=(other.set_scale(_scale));
						}
						else if (_scale < other._scale) {
							*this = set_scale(other._scale);
							return operator%=(other);
						}

						numerator_modulus(other);

						return *this;

					}
				}
				else if (_cat > other._cat) {
					return operator%=(other.promote_category(*this));
				}
				else {
					*this = promote_category(other);
					return operator%=(other);
				}
			}

			*this = real("undef");

			return *this;
		}

		real real::operator+(const real& b) const {

			real a = *this;

			a += b;

			return a;
		}

		real real::operator-(const real& b) const {

			real a = *this;

			a -= b;

			return a;
		}

		real real::operator*(const real& b) const {

			real a = *this;

			a *= b;

			return a;
		}

		real real::operator/(const real& b) const {

			real a = *this;

			a /= b;

			return a;
		}

		real real::operator%(const real& b) const {

			real a = *this;

			a %= b;

			return a;
		}

		real real::operator+() const {

			return *this;
		}

		real real::operator-() const {

			real a = *this;
			a.invert_sign();

			return a;
		}

		real& real::operator++() {

			if (is_defined()) {

				if (is_negative()) {

					--_num;

					check_for_zero();
				}
				else {

					++_num;

					if (_sign == SIGN::zero) {
						_sign = SIGN::positive;
					}
				}
			}

			return *this;
		}

		real real::operator++(int) {

			real a(*this);

			operator++();

			return a;
		}

		real& real::operator--() {

			if (is_defined()) {

				if (is_negative()) {

					++_num;

					if (_sign == SIGN::zero) {
						_sign = SIGN::negative;
					}
				}
				else {

					--_num;

					check_for_zero();
				}
			}

			return *this;
		}

		real real::operator--(int) {

			real a(*this);

			operator--();

			return a;
		}

		void real::div_rem(const real& other, real& qot, real& rem) const {

			if (is_defined() && other.is_defined()) {

				if (!other.is()) {

					qot = real("undef");
					rem = real("undef");

					return;
				}

				if (_cat == other._cat) {

					if (_cat <= NUMERIC_CATEGORY::integer) {

						_num.div_rem(other._num, qot._num, rem._num);

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
					if (is_decimal()) {

						qot = *this / other;

						rem = qot;

						rem._num %= rem._den;
						qot._num -= rem._num;

						return;
					}

					qot = *this / other;
					rem = *this % other;

					return;
				}
				else if (_cat > other._cat) {
					return div_rem(other.promote_category(*this), qot, rem);
				}
				else {
					real a = promote_category(other);
					return a.div_rem(other, qot, rem);
				}
			}

			if (is_nan() || other.is_nan()) {

				qot = real("nan");
				rem = real("nan");
			}
			else {
				qot = real("undef");
				rem = real("undef");
			}

		}

		real real::to_decimal(const real& scale) const {

			if (!scale.is_integer() || scale.is_negative() || is_negative()) {
				return real("undef");
			}

			if (_cat <= NUMERIC_CATEGORY::rational) {

				int_type s = scale.to_integral<int_type>();

				real a(s, _rmode);

				a += *this;

				return a;
			}

			return *this;
		}

		real real::to_decimal() const {

			auto a = to_decimal(real(_num.log10() + _den.log10(), SIGN::positive));

			return a;
		}

		real real::to_rational() const {

			real a = *this;

			if (_cat > NUMERIC_CATEGORY::rational) {

				auto gcd = a._num.gcd(a._den);

				a._num /= gcd;
				a._den /= gcd;
			}
			a._cat = NUMERIC_CATEGORY::rational;
			a._scale = 0;

			return a;
		}

		real real::to_integer() const {

			real a = *this;

			if (_cat >= NUMERIC_CATEGORY::rational) {

				a._num /= a._den;
				a._den = 1;
			}
			a._cat = NUMERIC_CATEGORY::integer;
			a._scale = 0;

			return a;
		}

		real real::to_heximal() const {

			real a = *this;

			if (_cat >= NUMERIC_CATEGORY::rational) {

				a._num /= a._den;
				a._den = 1;
			}
			a._cat = NUMERIC_CATEGORY::heximal;
			a._scale = 0;

			return a;
		}

		real real::to_octal() const {

			real a = *this;

			if (_cat >= NUMERIC_CATEGORY::rational) {

				a._num /= a._den;
				a._den = 1;
			}
			a._cat = NUMERIC_CATEGORY::octal;
			a._scale = 0;

			return a;
		}

		real real::to_binary() const {

			real a = *this;

			if (_cat >= NUMERIC_CATEGORY::rational) {

				a._num /= a._den;
				a._den = 1;
			}
			a._cat = NUMERIC_CATEGORY::binary;
			a._scale = 0;

			return a;
		}

		real real::abs() const {

			if (is_negative()) {

				return -*this;
			}
			return *this;
		}

		real real::inverse() const {

			if (is_decimal()) {

				return *this / 1;
			}

			real a = _cat == NUMERIC_CATEGORY::rational ? *this : to_rational();

			std::swap(a._num, a._den);

			return a;
		}

		real real::log(real b) const {

			if (is_defined() && b.is_defined() && is_positive() && b.is_positive()) {

				return real(_num.log(b._num), _sign);
			}

			return real("undef");
		}

		real real::log2() const {

			if (is_defined() && is_positive()) {

				return real(_num.log2(), _sign);
			}

			return real("undef");
		}

		real real::log10() const {

			if (is_defined() && is_positive()) {

				return real(_num.log10(), _sign);
			}

			return real("undef");
		}

		real real::gcd(real& other) const {

			if (is_defined() && other.is_defined()) {

				return real(_num.gcd(other._num), SIGN::positive);
			}
			return real("undef");
		}

		real real::pow(real b) const {

			if (is_defined() && b.is_defined()) {

				if (b.is_negative()) {

					return root(b.abs());
				}

				real n;

				n._num = _num.pow(b._num);

				if (n._num.is()) {

					n._sign = _sign;

					if (is_negative() && b.is_odd()) {

						n = -n;
					}
				}
				return n;
			}
			return *this;
		}

		real real::ceil() const {

			real a = *this;

			whole_number q, r;
			a._num.div_rem(a._den, q, r);

			a._num = q;

			if (r.is()) {

				if (a.is_positive()) {
					++a._num;
				}
				else {
					--a._num;
				}
			}
			if (is_rational()) {
				a._den = 1;
			}

			return a;
		}

		real real::floor() const {

			real a = *this;

			whole_number q, r;
			a._num.div_rem(a._den, q, r);

			a._num = q;

			if (r.is()) {

				if (a.is_positive()) {
					--a._num;
				}
				else {
					++a._num;
				}
			}
			if (is_rational()) {
				a._den = 1;
			}

			return a;
		}

		real real::sqrt() const {
			return root(real(2));
		}

		real real::root(real b) const {

			if (is_defined() && b.is_defined()) {

				if (b.is_negative()) {

					return pow(b.abs());
				}

				if (is_negative()) {
					return real("nan");
				}

				return real(_num.root(b._num), _sign);
			}
			return *this;
		}

		real real::hypot(real& b) const {

			real two(2);

			return (pow(two) + b.pow(two)).sqrt();
		}

		real real::hypot(real& b, real& c) const {

			real two(2);

			return (pow(two) + b.pow(two) + c.pow(two)).sqrt();
		}

		real real::lowest_terms() const {

			real a = *this;

			if (is_rational()) {

				whole_number n = a._num.gcd(a._den);

				a._num /= n;
				a._den /= n;
			}

			return a;
		}

		real& real::round_to(const real& index) {

			int_type i = 0;

			if (index.is_integer()) {

				i = index.to_integral<int_type>();
			}

			switch (_cat) {

			case (NUMERIC_CATEGORY::decimal): {

				if (i > 0) {

					i = _scale - i;

					whole_number ten(10);
					whole_number power = ten.pow(i);

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

					whole_number ten(10);
					whole_number power = ten.pow(i);

					whole_number q, r;
					_num.div_rem(power, q, r);

					_num = q;

					if (r.is()) {
						set_round(r);
					}

					power = ten.pow(i);

					_num *= power;
				}

			} break;

			case (NUMERIC_CATEGORY::rational): {

				real a = to_decimal();

				a.round_to(index);

				*this = a.to_rational();

			} break;

			default:
				break;
			}

			return *this;
		}

		real real::round_to(const real& index) const {
		
			real a = *this;

			a.round_to(index);

			return a;
		}

		str_type real::to_string() const {

			if (is_defined()) {

				str_type result = "";

				if (is_negative()) {
					result += "-";
				}

				if (_cat == NUMERIC_CATEGORY::decimal) {

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
				}
				else if (_cat == NUMERIC_CATEGORY::rational) {
					result += (_den == 1 ? _num.to_string(0) : _num.to_string(0) + "/" + _den.to_string(0));
				}
				else if (_cat == NUMERIC_CATEGORY::integer) {
					result += _num.to_string(0);
				}
				else if (_cat == NUMERIC_CATEGORY::heximal) {
					result += _num.to_string(16);
				}
				else if (_cat == NUMERIC_CATEGORY::octal) {
					result += _num.to_string(8);
				}
				else if (_cat == NUMERIC_CATEGORY::binary) {
					result += _num.to_string(2);
				}

				return result;
			}

			if (NUMERIC_CATEGORY::undef == _cat) {

				return "undef";
			}

			return "nan";
		}

		real real::promote_category(const real& other) const {

			// First check if we need to demote to an undefined number.
			if (is_undefined() || other.is_undefined()) {
				if (_cat < other._cat) {

					return *this;
				}
				else {
					return other;
				}
			}

			real res(*this);

			switch (other._cat) {

			case NUMERIC_CATEGORY::decimal:  // Promote res to a decimal number.
				res._cat = NUMERIC_CATEGORY::decimal;
				res._scale = other._scale;
				res._num *= other._den;
				res._num /= res._den;
				res._den = other._den;
				break;

			case NUMERIC_CATEGORY::rational:  // Promote res to a rational number.
				res._cat = NUMERIC_CATEGORY::rational;
				res._num *= other._den;
				res._den *= other._den;
				break;

			case NUMERIC_CATEGORY::integer:  // Promote res to a integer number.
				res._cat = NUMERIC_CATEGORY::integer;
				break;

			case NUMERIC_CATEGORY::heximal:  // Promote res to a heximal number.
				res._cat = NUMERIC_CATEGORY::heximal;
				break;

			case NUMERIC_CATEGORY::octal:  // Promote res to a octal number.
				res._cat = NUMERIC_CATEGORY::octal;
				break;

			default:    // Binary number need no other promotion.
				break;
			}

			return res;
		}

		void real::invert_sign() {

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

		real::SIGN real::get_invert_sign() const {

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

		void real::check_for_zero() {

			if (is_defined()) {

				if (!_num.is() && _sign != SIGN::zero) {
					_sign = SIGN::zero;
				}
			}
		}

		void real::confirm_category() {
			
			whole_number one(1);

			if (_scale > 0 && _den > one) {
				_cat = NUMERIC_CATEGORY::decimal;
			}
			else if (_scale == 0 && _den > one) {
				_cat = NUMERIC_CATEGORY::rational;
			}
			else if (_scale == 0 && _den == one) {
				_cat = NUMERIC_CATEGORY::integer;
			}
			else if (!_den.is()) {
				*this = real("undef");
			}
		}

		real real::get_sub_text_value(str_type& value, str_type del) const {

			auto found = value.find(del);
			if (found == std::string::npos) {

				found = value.find(to_upper(del));
			}

			str_type sub_string_value = "";

			real derived_value;

			if (found != std::string::npos) {

				if (value.begin() + found < value.end()) {
					sub_string_value = value.substr(found + 1);
				}
				value.erase(found);

				derived_value = real(sub_string_value);
			}

			return derived_value;
		}

		void real::find_and_set_decimal_denominator(str_type& value) {

			bool_type dec = false;


			auto found = value.find(".");
			if (found != std::string::npos) {

				dec = true;
				value.at(found) = ' ';
			}

			if (dec) {
				/*
					Note only a decimal place defines a numerical scale.  If no
					decimal is present in the string, an integer is being defined.
				*/
				_scale = value.size() - found - 1;
				if (_scale < 1) {
					_scale = 1;
				}
				_den = whole_number(10).pow(_scale);
				_cat = NUMERIC_CATEGORY::decimal;
			}
		}

		void real::set_decimal_exponent(real& exponent) {

			if (exponent.is()) {

				whole_number ten(10);

				if (exponent.is_negative()) {
					/*
						If an integer is being defined, but a negative exponent is
						provided, the integer will be defined as a decimal number.
					*/

					if (whole_number(_scale) < exponent._num) {
						_den = ten.pow(exponent._num);
					}
					else {
						_den *= ten.pow(exponent._num);
					}
				}
				else if (exponent.is_positive()) {
					/*
						If an integer is being defined, but a positive exponent is
						provided, the integer will be powered to the exponent and
						remain an integer.
					*/
					_num *= ten.pow(exponent._num);
				}

				set_internal_value_of_scale();
			}
		}

		void real::set_decimal_scale(const real& scale) {

			auto test = scale.compare(_scale);

			if (scale.is() && test) {

				whole_number ten(10);

				if (test > 0) {
					// Set the scale of the decimal number to be larger.

					_num *= ten.pow(scale._num - _scale);
					_den = ten.pow(scale._num);
				}
				else if (test < 0) {
					// Set the scale of the decimal number to be smaller.

					_num /= ten.pow(whole_number(_scale) - scale._num);
					_den = ten.pow(scale._num);
				}

				set_internal_value_of_scale();
			}
		}

		void real::set_internal_value_of_scale() {

			whole_number ten(10);

			_scale = _den.log10().to_integral<int_type>();

			whole_number test = ten.pow(_scale);

			while (test > _den) {
				test = test / ten;
				_scale -= 1;
			}
		}

		void real::initialize_value_of_the_sign(str_type& value) {

			_sign = SIGN::positive;

			if (value.front() == '-') {

				_sign = SIGN::negative;
				value.front() = ' ';
			}
			else if (value.front() == '+') {

				value.front() = ' ';
			}
		}

		real_type real::numerator_compare(const real& other) const {

			real_type truth = _num.compare(other._num);

			if (_sign == SIGN::negative) {

				truth *= -1.0;
			}

			return truth;
		}

		void real::numerator_addition(const real& other) {

			if (_sign == other._sign) {

				_num += other._num;

				return;
			}

			real_type n = _num.compare(other._num);

			if (n == 0.0) {

				if (is_decimal()) {
					_sign = SIGN::zero;
					_num = whole_number();
				}
				else {
					*this = real();
				}
			}
			else if (n > 0.0) {
				_num -= other._num;
			}
			else {
				_num = other._num - _num;
				_sign = other._sign;
			}

			check_for_zero();
		}

		void real::numerator_multiplication(const real& other) {

			_num = _num * other._num;

			if (_sign == other._sign) {

				_sign = SIGN::positive;
			}
			else {
				_sign = SIGN::negative;
			}

			check_for_zero();
		}

		void real::numerator_division(const real& other) {

			if (_scale) {
				_num *= _den;
			}

			whole_number q, r;

			_num.div_rem(other._num, q, r);

			_num = q;

			if (_sign == other._sign) {

				_sign = SIGN::positive;
			}
			else {
				_sign = SIGN::negative;
			}

			set_round(r);

			check_for_zero();
		}

		void real::numerator_modulus(const real& other) {

			whole_number q, r;

			_num.div_rem(other._num, q, r);

			_num = r;

			if (_sign == other._sign) {

				_sign = SIGN::positive;
			}
			else {
				_sign = SIGN::negative;
			}

			check_for_zero();
		}

		void real::set_round(whole_number& rem) {

			switch (_rmode) {

			case ROUNDING_MODE::half_even: {

				whole_number five(5);

				whole_number mod = _num % whole_number(10);

				if (mod > five) {
					_num += 1;
				}
				if (mod == five) {

					if (is_odd()) {
						_num += 1;
					}
				}
			} break;

			case ROUNDING_MODE::half_odd: {

				whole_number five(5);

				whole_number mod = _num % whole_number(10);

				if (mod > five) {
					_num += 1;
				}
				if (mod == five) {

					if (is_even()) {
						_num += 1;
					}
				}
			} break;

			case ROUNDING_MODE::half_up: {

				whole_number five(5);
				if (is_positive() && _num % whole_number(10) >= five) {
					_num += 1;
				}
				else if (is_negative() && _num % whole_number(10) > five) {
					_num += 1;
				}
			} break;

			case ROUNDING_MODE::half_down: {

				whole_number five(5);
				if (is_positive() && _num % whole_number(10) > five) {
					_num += 1;
				}
				else if (is_negative() && _num % whole_number(10) >= five) {
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
		real real::set_rational_lowest_terms(const bool_type& value) const {
			
			real a = *this;

			a.set_rational_lowest_terms(value);

			return a;
		}

		real& real::set_rational_lowest_terms(const bool_type& value) {
			
			_set_lt = value;

			return *this;
		}

		bool_type real::get_rational_lowest_terms() const {
			return _set_lt;
		}
	}  // end MPA
} // end Olly
