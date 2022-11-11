#ifndef NUMERIC_TYPE_H
#define NUMERIC_TYPE_H

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

#include "../whole_number/whole_number.h"

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
            nan = 0, undef, binary, octal, heximal, whole, integer, rational, decimal, complex
            /*
                The category of number able to be defined within the class, with the exception
                of the complex category, which will be defined in the number class.
            */
        };

        class numeric_type {

        public:

            const enum class SIGN {
                negative = 0, zero, positive
            };

            const enum class ROUNDING_MODE {
                half_up = 0, half_down, half_even, half_odd, ceil, floor, toward_zero, away_from_zero
            };

            numeric_type();
            numeric_type(int_type value);
            numeric_type(str_type value, str_type r_mode = "");
            numeric_type(const numeric_type& obj) = default;
            numeric_type(numeric_type&& obj)      = default;
            virtual ~numeric_type();

            numeric_type& operator=(const numeric_type& obj) = default;
            numeric_type& operator=(numeric_type&& obj)      = default;

            bool_type is() const;                                           // Boolean conversion.

            bool_type is_odd()  const;                                      // Is the numeric_type odd?
            bool_type is_even() const;                                      // Is the numeric_type even?

            bool_type is_positive()   const;                                // Is the numeric_type positive?
            bool_type is_negative()   const;                                // Is the numeric_type negative?
            bool_type is_defined()    const;                                // Is the numeric_type defined?
            bool_type is_undefined()  const;                                // Is the numeric_type undefined?
            bool_type is_nan()        const;                                // Is the numeric_type not a number?

            bool_type is_decimal()       const;                             // Check if an instance of a numerical_type
            bool_type is_rational()      const;                             // is of a specific category.
            bool_type is_integer()       const;
            bool_type is_whole()         const;
            bool_type is_heximal()       const;
            bool_type is_octal()         const;
            bool_type is_binary()        const;
            bool_type is_integral_type() const;                             // Check if this is a defined numeric_type
                                                                            // which is not a decimal or rational type.

            bool_type operator==(const numeric_type& b) const;              // Standard comparision operators.
            bool_type operator!=(const numeric_type& b) const;
            bool_type operator< (const numeric_type& b) const;
            bool_type operator> (const numeric_type& b) const;
            bool_type operator<=(const numeric_type& b) const;
            bool_type operator>=(const numeric_type& b) const;

            real_type compare(const numeric_type& other) const;             // Compare two numeric_types.
                                                                            //  0.0 = equality.
                                                                            //  1.0 = greater than.
                                                                            // -1.0 = less than.

            str_type         get_category()     const;                      // Get a string representation of the numerical category of the number_type.
            NUMERIC_CATEGORY get_cat_integral() const;                      // Get the enumerated type assigned to the numerical_type.

            str_type get_sign() const;                                      // Get a string representation of the numeric_type's sign.

            int_type      get_scale() const;                                // Get or set the decimal scale.
            numeric_type& set_scale(const numeric_type& scale);             // Any numeric_type not a decimal is
            numeric_type  set_scale(const numeric_type& scale) const;       // converted to a decimal first.

            str_type      get_rmode() const;                                // Get and set the rounding mode.
            numeric_type& set_rmode(const str_type& rmode);
            numeric_type  set_rmode(const str_type& rmode) const;

            numeric_type get_numerator() const;                             // Get the numerator.
            numeric_type get_denominator() const;                           // Get the denominator.

            static const bool_type& get_rational_to_lowest_terms();         // Get or set if a rational numbers should always reduce to lowest terms.  
            static void set_rational_lowest_terms(const bool_type& value);

            static const numeric_type& get_min_dec_scale();                 // Get the minimum decimal scale pf decimal numbers.
            static void  set_min_dec_scale(const int_type scale);           // Set the minimum decimal scale of decimal numbers.

            numeric_type& operator&=(const numeric_type& other);            // Standard bitwise operators.
            numeric_type& operator|=(const numeric_type& other);            // They will only manipulate integral
            numeric_type& operator^=(const numeric_type& other);            // numeric_types.

            numeric_type& operator<<=(size_type index);                     // Standard bitwise shift operators.
            numeric_type& operator>>=(size_type index);                     // Integral numeric_types are bit shifted.
                                                                            // Decimal and rational types are shifted
                                                                            // by a power of 10.

            numeric_type operator&(const numeric_type& b) const;            // Standard const bitwise operators.
            numeric_type operator|(const numeric_type& b) const;
            numeric_type operator^(const numeric_type& b) const;
            numeric_type operator~() const;

            numeric_type operator<<(size_type index) const;                 // Standard const bitwise shift operators.
            numeric_type operator>>(size_type index) const;

            numeric_type& operator+=(const numeric_type& other);            // Standard arithmatic operators.
            numeric_type& operator-=(const numeric_type& other);
            numeric_type& operator*=(const numeric_type& other);
            numeric_type& operator/=(const numeric_type& other);
            numeric_type& operator%=(const numeric_type& other);

            numeric_type operator+(const numeric_type& b) const;            // Standard const arithmatic operators.
            numeric_type operator-(const numeric_type& b) const;
            numeric_type operator*(const numeric_type& b) const;
            numeric_type operator/(const numeric_type& b) const;
            numeric_type operator%(const numeric_type& b) const;

            numeric_type operator+() const;                                 // Standard unary arithmatic operators.
            numeric_type operator-() const;

            numeric_type& operator++();                                     // Standard increment operators.
            numeric_type  operator++(int);

            numeric_type& operator--();                                     // Standard decrement operators.
            numeric_type  operator--(int);

            // Get both the quotient and the remainder of division by *this and other.
            void div_rem(const numeric_type& other, numeric_type& qot, numeric_type& rem) const;

            numeric_type to_decimal(numeric_type scale)  const;             // Convert a numeric_type to a decimal at a scale of 'scale'.
            numeric_type to_decimal()  const;                               // Convert a numeric_type to a decimal at a scale of the log10 of the numerator and denominator.
            numeric_type to_rational() const;                               // Convert a numeric_type to a rational.
            numeric_type to_integer()  const;                               // Convert a numeric_type to a integer.
            numeric_type to_whole()    const;                               // Convert a numeric_type to a whole.
            numeric_type to_heximal()  const;                               // Convert a numeric_type to a heximal.
            numeric_type to_octal()    const;                               // Convert a numeric_type to a octal.
            numeric_type to_binary()   const;                               // Convert a numeric_type to a binary.
            numeric_type promote_category(const numeric_type& other) const; // Promote this numeric type to the same numeric category of 'other'.

            numeric_type abs()     const;                                   // Get the absolute value of a numeric_type.
            numeric_type inverse() const;                                   // Get the inverse of a numeric type.
                                                                            // Integral types are promoted to rationals.

            numeric_type factorial() const;                                 // Get the factorial of the numeric_type.

            numeric_type log(numeric_type b) const;                         // Get the log of base 'b'.
            numeric_type log2()      const;                                 // Get the log of 2.
            numeric_type log10()     const;                                 // get the log of 10.
            numeric_type loge()      const;                                 // get the log of e.

            numeric_type  gcd(numeric_type& b) const;                       // Get the greates common denominator between two numeric_types.
            numeric_type  pow(numeric_type  b) const;                       // Raise a numeric_type to the power of 'b'.
            
            numeric_type  ceil() const;                                     // Perform a ceiling operation.
            numeric_type floor() const;                                     // Perform a floor operation.

            numeric_type  sqrt()       const;                               // Get the square root of a numeric_type.
            numeric_type  root(numeric_type b) const;                       // Get the root of 'b' of a numeric_type.

            numeric_type hypot(numeric_type& b)          const;             // Get the hypotenuse of two numeric_types.
            numeric_type hypot(numeric_type& b, numeric_type& c) const;     // Get the hypotenuse of three numeric_types.

            numeric_type lowest_terms() const;                              // Set a rational to its lowest terms.

            numeric_type& round_to(const numeric_type& index);              // Round a numeric_type to a specific 'index'.
            numeric_type  round_to(const numeric_type& index) const;        // Integral and decimals are rounded normally.
                                                                            // Rationals are converted to decimal then rounded
                                                                            // and converted back to rationals.

            str_type to_string() const;                                     // Get a string representation of the numeric_type.

            template<typename N>
            N to_integral() const;                                          // Convert the this to system integral.
                                                                            // The conversion does not account for
                                                                            // overflow precision lose.

        private:

            whole_number        _num;
            whole_number        _den;
            SIGN                _sign;
            int_type            _scale;
            NUMERIC_CATEGORY    _cat;
            ROUNDING_MODE       _rmode;

            static const whole_number TEN;
            static const numeric_type UNDEF;

            static numeric_type& minimum_decimal_scale();
            static bool_type& reduce_fractions_to_lowest_terms();

            numeric_type(const whole_number& num, const numeric_type& ref);
            numeric_type(const int_type& scale, const ROUNDING_MODE& mode);

            void set_integer(str_type& value);
            void set_decimal(str_type& value);
            void set_rational(str_type& value);
            void set_whole_number(str_type& value);
            void set_binary(str_type& value);
            void set_heximal(str_type& value);
            void set_octal(str_type& value);

            void invert_sign();
            SIGN get_invert_sign() const;
            void check_for_zero();

            numeric_type get_sub_text_value(str_type& value, str_type del) const;
            void find_and_set_decimal_denominator(str_type& value);
            void set_decimal_exponent(numeric_type& exponent);
            void set_decimal_scale(numeric_type scale);
            void set_internal_value_of_scale();
            void initialize_value_of_the_sign(str_type& value);

            real_type numerator_compare(const numeric_type& other) const;

            void numerator_addition(const numeric_type& other);
            void numerator_multiplication(const numeric_type& other);

            void     round();
            void set_round(whole_number& rem);

            str_type get_potential_prefix() const;
        };

        /********************************************************************************************/
        //
        //                             Template Function Implementation
        //
        /********************************************************************************************/

        template<typename N>
        inline N numeric_type::to_integral() const {

            if (is_integral_type()) {

                N n = _num.to_integral<N>();

                if (is_negative()) {

                    n *= -1;
                }

                return n;
            }

            return 0;
        }
    } // end MPA
}
#endif // NUMERIC_TYPE_H
