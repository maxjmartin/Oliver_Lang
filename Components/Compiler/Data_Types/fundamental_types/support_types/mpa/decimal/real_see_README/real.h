#pragma once

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

#include "whole_number.h"

namespace Olly {
	namespace MPA {

		/********************************************************************************************/
		//
		//                                 'integer' class
		//
		//        The integer class implements a mathematical integer using the a
		//        'bit_numister' class using a 32 bit word for songle precision terms, and a 
		//        64 bit word to perform arithmatic between terms.  
		//
		/********************************************************************************************/

		const enum class NUMERIC_CATEGORY {
			nan = 0, undef, binary, octal, heximal, integer, rational, decimal, complex
		};

		class real {

		public:

			const enum class SIGN {
				negative = 0, zero, positive
			};

			const enum class ROUNDING_MODE {
				half_up = 0, half_down, half_even, half_odd, ceil, floor, toward_zero, away_from_zero
			};

			real();
			real(int_type value);
			real(str_type value, str_type r_mode = "");
			real(const real& obj) = default;
			real(real&& obj) = default;
			virtual ~real();

			real& operator=(const real& obj) = default;
			real& operator=(real&& obj) = default;

			bool_type is() const;

			bool_type is_odd()  const;
			bool_type is_even() const;

			bool_type is_positive()   const;
			bool_type is_negative()   const;
			bool_type is_defined()    const;
			bool_type is_undefined()  const;
			bool_type is_nan()        const;

			str_type         get_category()     const;
			NUMERIC_CATEGORY get_cat_integral() const;

			int_type get_scale() const;
			real&    set_scale(const real& scale);
			real     set_scale(const real& scale) const;

			str_type get_rmode() const;
			real&    set_rmode(const str_type& rmode);
			real     set_rmode(const str_type& rmode) const;

			real get_numerator() const;
			real get_denominator() const;

			bool_type is_decimal()  const;
			bool_type is_rational() const;
			bool_type is_integer()  const;
			bool_type is_heximal()  const;
			bool_type is_octal()    const;
			bool_type is_binary()   const;

			bool_type operator==(const real& b) const;
			bool_type operator!=(const real& b) const;
			bool_type operator< (const real& b) const;
			bool_type operator> (const real& b) const;
			bool_type operator<=(const real& b) const;
			bool_type operator>=(const real& b) const;

			real_type compare(const real& other) const;

			real& operator&=(const real& other);
			real& operator|=(const real& other);
			real& operator^=(const real& other);

			real& operator<<=(size_type index);
			real& operator>>=(size_type index);

			real operator&(const real& b) const;
			real operator|(const real& b) const;
			real operator^(const real& b) const;
			real operator~() const;

			real operator<<(size_type index) const;
			real operator>>(size_type index) const;

			real& operator+=(const real& other);
			real& operator-=(const real& other);
			real& operator*=(const real& other);
			real& operator/=(const real& other);
			real& operator%=(const real& other);

			real operator+(const real& b) const;
			real operator-(const real& b) const;
			real operator*(const real& b) const;
			real operator/(const real& b) const;
			real operator%(const real& b) const;

			real operator+() const;
			real operator-() const;

			real& operator++();
			real  operator++(int);

			real& operator--();
			real  operator--(int);

			void div_rem(const real& other, real& qot, real& rem) const;

			real to_decimal(const real& scale)  const;
			real to_decimal()  const;
			real to_rational() const;
			real to_integer()  const;
			real to_heximal()  const;
			real to_octal()    const;
			real to_binary()   const;

			real abs()     const;
			real inverse() const;

			real log(real b) const;
			real log2()      const;
			real log10()     const;

			real gcd(real& b) const;
			real pow(real  b) const;

			real  ceil() const;
			real floor() const;

			real  sqrt()       const;
			real  root(real b) const;

			real hypot(real& b)          const;
			real hypot(real& b, real& c) const;

			real lowest_terms() const;

			real& round_to(const real& index);
			real  round_to(const real& index) const;

			str_type to_string() const;

			real promote_category(const real& other) const;

			template<typename N>
			N to_integral() const;

		private:

			whole_number        _num;
			whole_number        _den;
			SIGN                _sign;
			int_type            _scale;
			NUMERIC_CATEGORY    _cat;
			ROUNDING_MODE       _rmode;
			bool_type           _set_lt;

			real(const whole_number& num, const SIGN s);
			real(const int_type& scale, const ROUNDING_MODE& mode);

			void set_integer(str_type& value);
			void set_decimal(str_type& value);
			void set_rational(str_type& value);
			void set_binary(str_type& value);
			void set_heximal(str_type& value);
			void set_octal(str_type& value);

			void invert_sign();
			SIGN get_invert_sign() const;
			void check_for_zero();
			void confirm_category();

			real get_sub_text_value(str_type& value, str_type del) const;
			void find_and_set_decimal_denominator(str_type& value);
			void set_decimal_exponent(real& exponent);
			void set_decimal_scale(const real& scale);
			void set_internal_value_of_scale();
			void initialize_value_of_the_sign(str_type& value);

			real_type numerator_compare(const real& other) const;

			void numerator_addition(const real& other);
			void numerator_multiplication(const real& other);
			void numerator_division(const real& other);
			void numerator_modulus(const real& other);

			void set_round(whole_number& rem);

			real  set_rational_lowest_terms(const bool_type& value) const;
			real& set_rational_lowest_terms(const bool_type& value);

			bool_type get_rational_lowest_terms() const;
		};

		template<typename N>
		inline N real::to_integral() const {

			N n = _num.to_integral<N>();

			if (is_negative()) {

				n *= -1;
			}

			return n;
		}
	} // end MPA
} // end Olly
