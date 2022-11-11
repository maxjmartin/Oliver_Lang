#ifndef WHOLE_NUMBER_H
#define WHOLE_NUMBER_H

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

#include "../../../shared/bit_register/bit_register.h"

namespace Olly {
    namespace MPA {

        /********************************************************************************************/
        //
        //                                 'whole_number' class
        //
        //        The whole_number class implements a mathematical whole_number using the a
        //        'bit_register' class the 'uint_type' and the 'ulong_type' defined within
        //        the 'system_fundamentals' header.  The implimentation is big endian.
        //
        /********************************************************************************************/

        class whole_number {

        public:
            typedef uint_type      word_type;
            typedef ulong_type   d_word_type;

            whole_number();
            whole_number(word_type value);
            whole_number(str_type value, word_type base = 10);
            whole_number(const whole_number& obj) = default;
            whole_number(whole_number&& obj)      = default;
            virtual ~whole_number();

            whole_number& operator=(const whole_number& obj) = default;
            whole_number& operator=(whole_number&& obj)      = default;

            bool_type is() const;                                   // Boolean conversion.

            bool_type is_odd()  const;                              // Return true is the whole number is odd.
            bool_type is_even() const;                              // Return true is the whole number is even.

            bool_type operator==(const whole_number& b) const;      // Standard comparision operators.
            bool_type operator!=(const whole_number& b) const;
            bool_type operator< (const whole_number& b) const;
            bool_type operator> (const whole_number& b) const;
            bool_type operator<=(const whole_number& b) const;
            bool_type operator>=(const whole_number& b) const;

            real_type compare(const whole_number& other) const;     // Compare two whole numbers.
                                                                    //  0.0 = equality.
                                                                    //  1.0 = greater than.
                                                                    // -1.0 = less than.

            whole_number& operator&=(const whole_number& other);    // Standard mutable bitwise operators.
            whole_number& operator|=(const whole_number& other);
            whole_number& operator^=(const whole_number& other);

            whole_number& operator<<=(size_type index);             // Standard mutable shift operators.
            whole_number& operator>>=(size_type index);

            whole_number bin_comp() const;

            whole_number operator&(const whole_number& b) const;    // Standard const bitwise operators.
            whole_number operator|(const whole_number& b) const;
            whole_number operator^(const whole_number& b) const;
            whole_number operator~() const;

            whole_number operator<<(size_type index) const;         // Standard const shift operators.
            whole_number operator>>(size_type index) const;

            whole_number& operator+=(const whole_number& other);    // Standard mutable math operators.
            whole_number& operator-=(const whole_number& other);
            whole_number& operator*=(const whole_number& other);
            whole_number& operator/=(const whole_number& other);
            whole_number& operator%=(const whole_number& other);

            whole_number operator+(const whole_number& b) const;    // Standard const math operators.
            whole_number operator-(const whole_number& b) const;
            whole_number operator*(const whole_number& b) const;
            whole_number operator/(const whole_number& b) const;
            whole_number operator%(const whole_number& b) const;

            whole_number& operator++();                             // Standard increment operators.
            whole_number  operator++(int);

            whole_number& operator--();                             // Standard decremant operators.
            whole_number  operator--(int);

            // Get both the qotient and the remainder of the whole number divided by 'other'.
            void div_rem(const whole_number& other, whole_number& qot, whole_number& rem) const;

            whole_number log(whole_number b) const;                 // Basic logorithmic functions.
            whole_number log2() const;
            whole_number log10() const;

            whole_number gcd(whole_number        b) const;                    // Greatest common denominator of two whole numbers.
            whole_number lcm(const whole_number& b) const;                    // Least common multiple of two whole numbers.
            whole_number pow(whole_number        b) const;                    // Raise the whole number to the power of 'b'.
            whole_number powmod(whole_number  b, whole_number c) const; // Raise the whole number to the power of 'b'.

            whole_number  sqrt() const;                                 // The square root of a whole number.
            whole_number  root(const whole_number& b) const;                   // The whole number root of 'b'.
            whole_number hypot(const whole_number& b) const;                  // The hypotenuse of two whole numbers.
            whole_number hypot(const whole_number& b, const whole_number& c) const; // The hypotenuse of three whole numbers.

            str_type to_string() const;                             // Return the string representation of the whole number at base 10.
            str_type to_string(word_type base) const;               // Return the string representation of the whole number at radix 'base'.
                                                                    // The only valid radix are 0, 2, 8, 16, and 10.
                                                                    // Radix 0 is a radix 10 representation without the commas.

            template<typename N>
            N to_integral() const;                                  // Cast the register to an integral of type T.

        private:
            typedef bit_register<word_type>  reg_type;

            reg_type _reg;

            whole_number(const reg_type& reg);

            void trim();

            void divide_remainder(word_type d, whole_number& q, whole_number& r, int_type stop) const;
        };

        /********************************************************************************************/
        //
        //                             Template Function Implementation
        //
        /********************************************************************************************/

        template<typename N>
        inline N whole_number::to_integral() const {

            return _reg.to_integral<N>();
        }
    } // end MPA
}
#endif // WHOLE_NUMBER_H
