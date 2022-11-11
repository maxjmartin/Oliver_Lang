#ifndef DECIMAL_H
#define DECIMAL_H

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
#include "../../../shared/math_const_strings.h"

namespace Olly {
    namespace MPA {

        class decimal {

            const enum class NUMERIC_CATEGORY {
                nan = 0, undef, binary, octal, heximal, whole, integer, rational, decimal, complex
                /*
                    The category of number able to be defined within the class, with the exception
                    of the complex category, which will be defined in the number class.
                */
            };

        public:

            const enum class SIGN {
                negative = 0, zero, positive, defined, undefined, nan
            };

            const enum class ROUNDING_MODE {
                none = 0, half_up, half_down, half_even, half_odd, ceil, floor, toward_zero, away_from_zero
            };

            decimal();
            decimal(SIGN value);
            decimal(whole_number value);
            decimal(int_type value);
            decimal(str_type value);
            decimal(str_type value, int_type base);
            decimal(const decimal& obj) = default;
            decimal(decimal&& obj)      = default;
            virtual ~decimal();

            decimal& operator=(const decimal& obj) = default;
            decimal& operator=(decimal&& obj)      = default;

            static const int_type& get_scale();
            static void            set_scale(const int_type& scale);

            static const      str_type  get_rmode();
            static const ROUNDING_MODE& get_rmode_integral();
            static void                 set_rmode(const str_type& mode);
            static void                 set_rmode(const ROUNDING_MODE mode);

            str_type to_string()      const;

            bool_type is()            const;
            bool_type is_not()        const;

            bool_type is_odd()        const;
            bool_type is_even()       const;
            bool_type is_zero()       const;

            bool_type is_positive()   const;
            bool_type is_negative()   const;
            bool_type is_defined()    const;
            bool_type is_undefined()  const; 
            bool_type is_nan()        const;

            str_type sign()          const;                       // Get a string representation of the numerical category of the number_type.
            SIGN     sign_integral() const;                       // Get the enumerated type assigned to the numerical_type.

            bool_type operator==(const decimal& b) const;              // Standard comparision operators.
            bool_type operator!=(const decimal& b) const;
            bool_type operator< (const decimal& b) const;
            bool_type operator> (const decimal& b) const;
            bool_type operator<=(const decimal& b) const;
            bool_type operator>=(const decimal& b) const;

            real_type compare(const decimal& other) const;             // Compare two numeric_types.
                                                                            //  0.0 = equality.
                                                                            //  1.0 = greater than.
                                                                            // -1.0 = less than.

            decimal& operator&=(const decimal& b);            // Standard const bitwise operators.
            decimal& operator|=(const decimal& b);
            decimal& operator^=(const decimal& b);
            decimal  operator& (const decimal& b) const; 
            decimal  operator| (const decimal& b) const;
            decimal  operator^ (const decimal& b) const;
            decimal  operator~ () const;

            decimal operator<<(size_type index) const;            // Standard const bitwise shift operators.
            decimal operator>>(size_type index) const;

            decimal& operator+=(const decimal& b);            // Standard arithmatic operators.
            decimal& operator-=(const decimal& b);
            decimal& operator*=(const decimal& b);
            decimal& operator/=(const decimal& b);
            decimal& operator%=(const decimal& b);

            decimal operator+(const decimal& b) const;            // Standard const arithmatic operators.
            decimal operator-(const decimal& b) const;
            decimal operator*(const decimal& b) const;
            decimal operator/(const decimal& b) const;
            decimal operator%(const decimal& b) const;

            decimal operator+() const;                            // Standard unary arithmatic operators.
            decimal operator-() const;

            decimal& operator++();                                // Standard increment operators.
            decimal  operator++(int);

            decimal& operator--();                                // Standard decrement operators.
            decimal  operator--(int);

            // Get both the quotient and the remainder of division by *this and other.
            void div_rem(const decimal& b, decimal& qot, decimal& rem) const;
            // Get both the quotient and the remainder of division by *this and other.
            void div_mod(const decimal& b, decimal& qot, decimal& rem) const;
            // Get both the quotient and the remainder of division by *this and other.
            void num_den(decimal& num, decimal& den) const;

            decimal abs()       const;
            decimal inverse()   const;
            decimal ceil()      const;                                     // Perform a ceiling operation.
            decimal floor()     const;                                     // Perform a floor operation.
            decimal factorial() const;

            decimal   gcd(const decimal& b)                     const;
            decimal   pow(const decimal& b)                     const;
            decimal  root(const decimal& b)                     const;
            decimal  sqrt()                                     const;
            decimal  cbrt()                                     const;
            decimal hypot(const decimal & b)                    const;
            decimal hypot(const decimal & b, const decimal & c) const;

            decimal    ln()                 const;
            decimal  log2()                 const;
            decimal log10()                 const;
            decimal   log(const decimal& b) const;

            decimal sin() const;
            decimal cos() const;
            decimal tan() const;

            decimal round(const decimal & scale) const;

            template<typename N>
            N to_integral() const;                                  // Cast the register to an integral of type T.

        private:
            SIGN         _sign;
            whole_number _num;

            static const whole_number ONE;
            static const whole_number TWO;
            static const whole_number TEN;

            static const int_type MAX_SCALE = 256;

            static const whole_number& get_denominator();

            static int_type&             decimal_scale();
            static whole_number&           denominator();
            static bool_type&     scale_not_configured();
            static ROUNDING_MODE&        rounding_mode();
            static whole_number&       whole_number_pi();
            static whole_number&        whole_number_e();
            static whole_number&      whole_number_ln2();
            static decimal&                decimal_360();
            static decimal&                decimal_180();
            static decimal&                 decimal_90();
            static decimal&                 decimal_45();
            static decimal&                 decimal_pi();

            void     invert_sign();

            decimal  get_sub_text_value(str_type& value, str_type del) const;
            int_type find_and_set_decimal_denominator(str_type& value);
            void     set_decimal_exponent(decimal& exponent);
            void     check_for_zero();

            void num_den(decimal& num, decimal& den, bool_type run) const;

            void  to_pow(const decimal& b);
            void to_root(const decimal& b);

            void set_round(int_type scale, const ROUNDING_MODE& mode);

            decimal get_ln() const;

            void perform_taylor_series_sin_cos(const decimal& x, decimal& n, int_type i) const;
        };

        /********************************************************************************************/
        //
        //                             Template Function Implementation
        //
        /********************************************************************************************/

        template<typename N>
        inline N decimal::to_integral() const {

            return is_negative() ? -_num.to_integral<N>() : _num.to_integral<N>();
        }

    } // end MPA
}
#endif // DECIMAL_H

/*
    decimal::set_scale(64);
    std::cout << "scale = " << decimal::get_scale() << std::endl;
    std::cout << std::endl;

    decimal a("0.0042");
    decimal b("3");
    decimal c("2");

    std::cout << "a = " << a.to_string() << std::endl;
    std::cout << "b = " << b.to_string() << std::endl;
    std::cout << "c = " << c.to_string() << std::endl;
    std::cout << std::endl;

    std::cout << "a == b = " << (a == b) << std::endl;
    std::cout << "a != b = " << (a != b) << std::endl;
    std::cout << "a  > b = " << (a >  b) << std::endl;
    std::cout << "a >= b = " << (a >= b) << std::endl;
    std::cout << "a  < b = " << (a <  b) << std::endl;
    std::cout << "a <= b = " << (a <= b) << std::endl;
    std::cout << std::endl;

    std::cout << "a.sign() = " << a.sign() << std::endl;
    std::cout << "b.sign() = " << b.sign() << std::endl;
    std::cout << "c.sign() = " << c.sign() << std::endl;
    std::cout << std::endl;

    std::cout << "a & b = " << (a & b).to_string() << std::endl;
    std::cout << "a | b = " << (a | b).to_string() << std::endl;
    std::cout << "a ^ b = " << (a ^ b).to_string() << std::endl;
    std::cout << "~a    = " <<    (~a).to_string() << std::endl;
    std::cout << std::endl;

    auto t = a << 6;
    std::cout << "a << 6 = " << t.to_string() << std::endl;
    t = t >> 6;
    std::cout << "a >> 6 = " << t.to_string() << std::endl;
    std::cout << std::endl;

    std::cout << "a + b = " << (a + b).to_string() << std::endl;
    std::cout << "a - b = " << (a - b).to_string() << std::endl;
    std::cout << "a * b = " << (a * b).to_string() << std::endl;
    std::cout << "a / b = " << (a / b).to_string() << std::endl;
    std::cout << "a % b = " << (a % b).to_string() << std::endl;
    std::cout << "++a   = " << (++a).to_string() << std::endl;
    std::cout << "--a   = " << (--a).to_string() << std::endl;
    std::cout << std::endl;

    decimal q, r;
    a.div_rem(b, q, r);
    std::cout << "a.div_rem(b, q, r): " << "q = " << q.to_string() << ", r = " << r.to_string() << std::endl;
    a.div_mod(b, q, r);
    std::cout << "a.div_mod(b, q, r): " << "q = " << q.to_string() << ", r = " << r.to_string() << std::endl;
    a.num_den(q, r);
    std::cout << "a.num_den(q, r): " << "q = " << q.to_string() << ", r = " << r.to_string() << std::endl;
    std::cout << std::endl;

    std::cout << "a.abs()       = " << a.abs().to_string() << std::endl;
    std::cout << "a.inverse()   = " << a.inverse().to_string() << std::endl;
    std::cout << "a.ceil()      = " << a.ceil().to_string() << std::endl;
    std::cout << "a.floor()     = " << a.floor().to_string() << std::endl;
    // std::cout << "a.factorial() = " << a.factorial().to_string() << std::endl;
    std::cout << "a.gcd(b)      = " << a.gcd(b).to_string() << std::endl;
    std::cout << "a.pow(b)      = " << a.pow(b).to_string() << std::endl;
    std::cout << "a.root(b)     = " << a.root(b).to_string() << std::endl;
    std::cout << "a.sqrt()      = " << a.sqrt().to_string() << std::endl;
    std::cout << "a.cbrt()      = " << a.cbrt().to_string() << std::endl;
    std::cout << "a.hypot(b)    = " << a.hypot(b).to_string() << std::endl;
    std::cout << "a.hypot(b, c) = " << a.hypot(b, c).to_string() << std::endl;
    std::cout << std::endl;

    a = decimal("0.05");
    std::cout << "a.ln()     = " << a.ln().to_string() << std::endl;
    std::cout << "a.log2()   = " << a.log2().to_string() << std::endl;
    std::cout << "a.log10()  = " << a.log10().to_string() << std::endl;
    std::cout << "a.log(5)   = " << a.log(5).to_string() << std::endl;
    std::cout << "a.log(50)  = " << a.log(50).to_string() << std::endl;
    std::cout << std::endl;

    a = decimal("5");
    std::cout << "a.sin() = " << a.sin().to_string() << std::endl;
    std::cout << "a.cos() = " << a.cos().to_string() << std::endl;
    std::cout << "a.tan() = " << a.tan().to_string() << std::endl;

    return 0;
*/