#pragma once

#include <string>

#include "integer.h"
#include "numeric_settings.h"
#include "../text_support.h"

namespace Oliver {

    class decimal {
        using word_t = std::size_t;


        class numeric_settings {
            
        public:
            enum class ROUNDING_MODE : int8_t {
                toward_zero = 0, half_up, half_down, half_even, half_odd, ceil, floor, away_from_zero
            };

            static std::size_t def_scale() {
                return _def_scale;
            }

            static std::size_t max_scale() {
                return _max_scale;
            }

            static std::size_t min_scale() {
                return _min_scale;
            }

            static int decimal_scale(){
                return _decimal_scale();
            }
            
            static void decimal_scale(const int& scale) {

                int scl = (scale >= static_cast<int>(_min_scale)) ? scale : static_cast<int>(_min_scale);
                    scl = (scl   <= static_cast<int>(_max_scale)) ?   scl : static_cast<int>(_max_scale);

                if (_scale_not_configured()) {

                    _scale_not_configured() = false;
                    _decimal_scale()        = scl;
                    _denominator()          = integer(10ul).pow(scl);

                    _integer_e();
                    _integer_pi();
                    _integer_ln2();

                    _decimal_360();
                    _decimal_180();
                    _decimal_45();
                }
            }

            static std::string rounding_mode(){

                switch (_round_mode()) {

                case(ROUNDING_MODE::half_up):
                    return "half_up";

                case(ROUNDING_MODE::half_down):
                    return "half_down";

                case(ROUNDING_MODE::half_even):
                    return "half_even";

                case(ROUNDING_MODE::half_odd):
                    return "half_odd";

                case(ROUNDING_MODE::ceil):
                    return "ceil";

                case(ROUNDING_MODE::floor):
                    return "floor";

                case(ROUNDING_MODE::away_from_zero):
                    return "away_from_zero";

                case(ROUNDING_MODE::toward_zero):
                    return "toward_zero";
                }
            }

            static void rounding_mode(const std::string& mode) {

                if (mode == "half_up") {
                    _round_mode() = ROUNDING_MODE::half_up;
                }

                else if (mode == "half_down") {
                    _round_mode() = ROUNDING_MODE::half_down;
                }

                else if (mode == "half_even") {
                    _round_mode() = ROUNDING_MODE::half_even;
                }

                else if (mode == "half_odd") {
                    _round_mode() = ROUNDING_MODE::half_odd;
                }

                else if (mode == "ceil") {
                    _round_mode() = ROUNDING_MODE::ceil;
                }

                else if (mode == "floor") {
                    _round_mode() = ROUNDING_MODE::floor;
                }

                else if (mode == "away_from_zero") {
                    _round_mode() = ROUNDING_MODE::away_from_zero;
                }

                else {
                    _round_mode() = ROUNDING_MODE::toward_zero;
                }
            }

            static integer decimal_denominator() {
                return _denominator();
            }

            static decimal e() {
                return _decimal_e();
            }

            static decimal pi() {
                return _decimal_pi();
            }

            static decimal ln2() {
                return _decimal_ln2();
            }

            static decimal _360() {
                return _decimal_360();
            }

            static decimal _180() {
                return _decimal_180();
            }

            static decimal _45() {
                return _decimal_45();
            }

            static bool& scale_not_configured() {

                static bool truth = true;

                return truth;
            }

        private:

            static const std::size_t _def_view  = 16ul;
            static const std::size_t _def_scale = 32ul;
            static const std::size_t _min_scale = 8ul;
            static const std::size_t _max_scale = 10000ul;

            static bool& _scale_not_configured() {

                static bool truth = true;

                return truth;
            }

            static std::size_t& _decimal_scale() {

                static std::size_t scale = _def_scale;

                return scale;
            }

            static integer& _denominator() {

                static integer denom = integer(10ul).pow(_def_scale); 

                return denom;
            }

            static integer& _integer_pi() {

                static integer pi(PI_STRING.substr(0, decimal_scale() + 1));

                return pi;
            }

            static integer& _integer_e() {

                static integer e(E_STRING.substr(0, decimal_scale() + 1));

                return e;
            }

            static integer& _integer_ln2() {

                static integer ln2(LN2_STRING.substr(0, decimal_scale()));

                return ln2;
            }

            static decimal& _decimal_pi() {

                static decimal e;

                e = _integer_e();

                return e;
            }

            static decimal& _decimal_e() {

                static decimal pi;

                pi = _integer_pi();

                return pi;
            }

            static decimal& _decimal_ln2() {

                static decimal ln2;

                ln2 = _integer_ln2();

                return ln2;
            }

            static decimal& _decimal_360() {

                static decimal d(360ul);

                return d;
            }

            static decimal& _decimal_180() {

                static decimal d(180ul);

                return d;
            }

            static decimal& _decimal_45() {

                static decimal d(45ul);

                return d;
            }

            static ROUNDING_MODE& _round_mode() {

                static ROUNDING_MODE mode = ROUNDING_MODE::half_even;

                return mode;
            }
        };

    public:
        static numeric_settings settings;

        constexpr decimal() :
            _numerator()
        {}

        constexpr decimal(word_t number) : 
            _numerator(integer(number))
        {}

        constexpr decimal(int64_t number) : 
            _numerator(integer(number))
        {}

        constexpr decimal(const integer& num) :
            _numerator(num)
        {}

        constexpr decimal(std::string value) :
            _numerator()
        {
            if (settings.scale_not_configured()) {
                settings.decimal_scale(decimal::settings.def_scale());
            }

            value = to_lower_case(value);

            if (value.size() > 1) {

                if (value.find('.') != std::string::npos) {
                    set_decimal(value);
                }
                else if (value.find('/') != std::string::npos) {
                    set_rational(value);
                }
                else if (value[0] == '0' && std::isalpha(value[1])) {

                    if (value[0] == '0' && value[1] == 'u') {
                        value[0] = ' ';
                        value[1] = ' ';
                        set_whole(value);
                    }
                    else if (value[0] == '0' && value[1] == 'b') {
                        value[0] = ' ';
                        value[1] = ' ';
                        set_binary(value);
                    }
                    else if (value[0] == '0' && value[1] == 'x') {
                        value[0] = ' ';
                        value[1] = ' ';
                        set_heximal(value);
                    }
                    else if (value[0] == '0' && value[1] == 'o') {
                        value[0] = ' ';
                        value[1] = ' ';
                        set_octal(value);
                    }
                    else {
                        set_integer(value);
                    }
                }
                else {
                    set_integer(value);
                }
            }
            else {
                set_integer(value);
            }
        }

        constexpr ~decimal()                             = default;
        constexpr decimal(decimal&& obj)                 = default;
        constexpr decimal(const decimal& obj)            = default;
        constexpr decimal& operator=(decimal&& obj)      = default;
        constexpr decimal& operator=(const decimal& obj) = default;

        friend constexpr void swap(decimal& first, decimal& second) {
            std::swap(first._numerator, second._numerator);
        }

        constexpr operator bool() const {
            return is_zero();
        }

        constexpr bool is_decimal()  const { return _numerator[0] % settings.decimal_denominator(); }
        constexpr bool is_integer()  const { return is_decimal(); }
        constexpr bool is_negative() const { return _numerator[0].is_negative(); } 
        constexpr bool is_positive() const { return _numerator[0].is_positive(); } 
        constexpr bool is_zero()     const { return _numerator[0].is_zero(); }
        constexpr bool is_nan()      const { return _numerator[0].is_nan(); }
        constexpr bool is_undef()    const { return _numerator[0].is_undef(); }
        constexpr bool is_neg_inf()  const { return _numerator[0].is_neg_inf(); }
        constexpr bool is_pos_inf()  const { return _numerator[0].is_pos_inf(); }
        constexpr bool is_finite()   const { return _numerator[0].is_finite(); }
        constexpr bool is_infinite() const { return _numerator[0].is_infinite(); }
        constexpr bool is_even()     const { return _numerator[0].is_even(); }
        constexpr bool is_odd()      const { return _numerator[0].is_odd(); }

        constexpr bool operator==(const decimal& b) const {
            return _numerator[0] == b._numerator[0];
        }

        constexpr std::partial_ordering operator<=>(const decimal& b) const {
            return _numerator[0] <=> b._numerator[0];
        }

        decimal& operator&=(const decimal& other) {
            _numerator[0] &= other._numerator[0];
            return *this;
        }

        decimal& operator|=(const decimal& other) {
            _numerator[0] |= other._numerator[0];
            return *this;
        }

        decimal& operator^=(const decimal& other) {
            _numerator[0] ^= other._numerator[0];
            return *this;
        }

        decimal& operator<<=(std::size_t shift) {
            _numerator[0] <<= shift;
            return *this;
        }

        decimal& operator>>=(std::size_t shift) {
            _numerator[0] >>= shift;
            return *this;
        }

        decimal& operator+=(const decimal& other) {
            _numerator[0] += other._numerator[0];
            return *this;
        }

        decimal& operator-=(const decimal& other) {
            _numerator[0] -= other._numerator[0];
            return *this;
        }

        decimal& operator*=(const decimal& other) {
            _numerator[0] *= other._numerator[0];
            _numerator[0] /= settings.decimal_denominator();
            return *this;
        }

        decimal& operator/=(const decimal& other) {
            _numerator[0] *= settings.decimal_denominator();
            _numerator[0] /= other._numerator[0];
            return *this;
        }

        decimal& operator%=(const decimal& other) {
            _numerator[0] %= other._numerator[0];
            return *this;
        }

        constexpr decimal operator+() const {
            return *this;
        }

        constexpr decimal operator-() const {
            decimal result = -_numerator[0];
            return result;
        }

        constexpr decimal operator~() const {
            decimal result = ~_numerator[0];
            return result;
        }

        constexpr decimal inverse() const {
            decimal a(settings.decimal_denominator());
            a /= *this;
            return a;
        }

        constexpr decimal operator&(const decimal& b) const {
            decimal a = *this;
            a &= b;
            return a;
        }

        constexpr decimal operator|(const decimal& b) const {
            decimal a = *this;
            a |= b;
            return a;
        }

        constexpr decimal operator^(const decimal& b) const {
            decimal a = *this;
            a ^= b;
            return a;
        }

        constexpr decimal operator<<(std::size_t shift) const {
            decimal a = *this;
            a <<= shift;
            return a;
        }

        constexpr decimal operator>>(std::size_t shift) const {
            decimal a = *this;
            a >>= shift;
            return a;
        }

        constexpr decimal operator+(const decimal& b) const {
            decimal a = *this;
            a += b;
            return a;
        }

        constexpr decimal operator-(const decimal& b) const {
            decimal a = *this;
            a -= b;
            return a;
        }

        constexpr decimal operator*(const decimal& b) const {
            decimal a = *this;
            a *= b;
            return a;
        }

        constexpr decimal operator/(const decimal& b) const {
            decimal a = *this;
            a /= b;
            return a;
        }

        constexpr decimal operator%(const decimal& b) const {
            decimal a = *this;
            a %= b;
            return a;
        }

        constexpr void div_mod(const decimal& b, decimal& quotient, decimal& remainder) const {
            integer q, r;
            (_numerator[0] * settings.decimal_denominator()).div_mod(b._numerator[0], q, r);
            quotient._numerator[0] = q;
            remainder._numerator[0] = r;
        }

        friend decimal operator&(decimal& a, decimal&& b) {
            return a &= b;
        }

        friend decimal operator|(decimal& a, decimal&& b) {
            return a |= b;
        }

        friend decimal operator^(decimal& a, decimal&& b) {
            return a ^= b;
        }

        friend decimal operator+(decimal& a, decimal&& b) {
            return a += b;
        }

        friend decimal operator-(decimal& a, decimal&& b) {
            return a -= b;
        }

        friend decimal operator*(decimal& a, decimal&& b) {
            return a *= b;
        }

        friend decimal operator/(decimal& a, decimal&& b) {
            return a /= b;
        }

        friend decimal operator%(decimal& a, decimal&& b) {
            return a %= b;
        }

        constexpr decimal& operator++() {
            *this += settings.decimal_denominator();
            return *this;
        }

        constexpr decimal  operator++(int) {
            decimal a(*this);
            ++(*this);
            return a;
        }

        constexpr decimal& operator--() {
            *this -= settings.decimal_denominator();
            return *this;
        }

        constexpr decimal  operator--(int) {
            decimal a(*this);
            --(*this);
            return a;
        }

        constexpr decimal abs() const {
            return _numerator[0].abs();
        }

        constexpr decimal ceil() const {

            if (settings.decimal_scale() == 0) {
                return *this;
            }

            decimal q, r;
            decimal a = *this;
            decimal b = settings.decimal_denominator();

            a.div_mod(b, q, r);
            
            if (r) {
                if (q.is_positive()) {
                    q._numerator[0] += b._numerator[0];
                }
                else {
                    q._numerator[0] -= b._numerator[0];
                }
            }

            return q;
        }

        constexpr decimal floor() const {

            if (settings.decimal_scale() == 0) {
                return *this;
            }

            decimal q, r;
            decimal a = *this;
            decimal b = settings.decimal_denominator();

            a.div_mod(b, q, r);
            
            if (r) {
                if (q.is_positive()) {
                    q._numerator[0] -= b._numerator[0];
                }
                else {
                    q._numerator[0] += b._numerator[0];
                }
            }

            return q;
        }

        constexpr decimal gcd(const decimal& b) const {
            if (_numerator[0] && b._numerator[0]) {
                return _numerator[0].gcd(b._numerator[0]);
            }
            return integer("undefined");
        }

        constexpr decimal pow(const decimal& b) const {
            if (*this && b) {

                std::size_t n = b.to_integral<std::size_t>();

                if (n == 2) {
                    return *this * *this;
                }

                if (n == 1) {
                    return *this;
                }

                if (!b) {
                    return decimal(1ul);
                }

                decimal a = *this;
                decimal res = 1ul;

                while (n) {

                    if (n & 1) {
                        res *= a;
                    }
                    n >>= 1;

                    if (n) {
                        a *= a;
                    }
                }
                return res;
            }

            return integer("undefined");
        }

        constexpr decimal root(const decimal& b) const {
            // e^(ln(*this)/b) - Will need to see if this is faster, in the future.
            if (*this && b) {
                std::size_t n = b.to_integral<std::size_t>();

                if (n > settings.max_scale() || is_negative()) {
                    return integer("undefined");
                }
                decimal a = *this * settings.decimal_denominator().pow(n - 1);
                a._numerator[0] = a._numerator[0].root(n);
                return a;
            }
            return integer("undefined");
        }

        decimal hypot(const decimal& b) const {
            return ((*this * *this) += (b * b)).root(2ul);
        }

        decimal hypot(const decimal& b, const decimal& c) const {
            return ((*this * *this) += (b * b) += (c * c)).root(2ul);
        }

        decimal ln() const {
            if ((_numerator[0] == settings.decimal_denominator())) {
                return decimal();
            }
            else if ((_numerator[0] == settings.e().get_integer())) {
                return decimal(1ul);
            }

            if ((_numerator[0] > settings.decimal_denominator())) {
                /*
                    Factor out the power of 2.  Then get ln().
                    This greatly improves speed of convergence.
                */

                std::size_t exp = (_numerator[0] / settings.decimal_denominator()).lead_bit() - 1ul;
                integer x = _numerator[0] >> exp;

                decimal r;
                r._numerator[0] = x;

                // return ln(x) + ln(2) * exp
                return (r.get_ln() + decimal(settings.ln2()) * decimal(exp));
            }

            return -get_ln();
        }

        decimal log2() const {
            return log(decimal(2ul));
        }

        decimal log10() const {
            return log(decimal(10ul));
        }

        decimal log(const decimal& b) const {
            
            if (*this && is_positive()) {

                decimal a = ln() / b.ln();

                decimal c(a);

                if (b.pow(c) == *this) {
                    return c;
                }

                return a;
            }

            return integer("undefined");
        }

        decimal sin() const {
            
            decimal sin_x = abs();

            bool neg = is_negative() ? true : false;

            /*
                First reduce value closure to 0.5
                to improve speed of convergence.  
            */

            if (sin_x > settings._360()) {
                sin_x %= settings._360();
            }

            if (sin_x > settings._180()) {
                sin_x %= settings._180();
                neg = !neg;
            }

            sin_x *= (settings.pi() / settings._180());  // Convert to radians. 

            sin_x = sin_x.get_sin();

            return neg ? -sin_x : sin_x;
        }

        decimal cos() const {

            decimal sin_x = sin();
            decimal one(1ul);

            return (one - (sin_x * sin_x)).root(2ul); 
        }

        decimal tan() const {

            decimal one(1ul);
            decimal sin_x = sin();
            decimal cos_x = (one - (sin_x * sin_x)).root(2ul);

            return sin_x / cos_x;
        }


        decimal asin() const {

            decimal asin_x = abs();

            bool neg = is_negative() ? true : false;

            /*
                First reduce value closure to 0.5
                to improve speed of convergence.
            */

            int power_of_2 = 0;

            decimal limit("0.5");
            decimal one(1ul);
            decimal two(2ul);

            while (asin_x > limit) {

                asin_x = asin_x / (one + (one + asin_x * asin_x).root(two));

                power_of_2 += 1;
            }

            asin_x = asin_x.get_asin() * two.pow(static_cast<std::size_t>(power_of_2));

            return neg ? -asin_x : asin_x;
        }

        decimal acos() const {
            return (settings.pi() / decimal(2ul)) - asin();
        }

        decimal atan() const {

            decimal sinh_x = abs();

            bool neg = is_negative() ? true : false;

            /*
                First reduce value closure to 0.5
                to improve speed of convergence.
            */

            int power_of_2 = 0;

            decimal limit("0.1");
            decimal one(1ul);
            decimal two(2ul);

            while (sinh_x > limit) {

                sinh_x = sinh_x / (one + (one + sinh_x * sinh_x).root(two));

                power_of_2 += 1;
            }

            sinh_x = sinh_x.get_atan() * two.pow(static_cast<std::size_t>(power_of_2));

            return neg ? -sinh_x : sinh_x;
        }

        decimal sinh() const {
            
            decimal sinh_x = abs();

            bool neg = is_negative() ? true : false;

            sinh_x = sinh_x.get_sinh();

            return neg ? -sinh_x : sinh_x;
        }

        decimal cosh() const {

            decimal cosh_x = abs();

            bool neg = is_negative() ? true : false;

            cosh_x = cosh_x.get_cosh();

            return neg ? -cosh_x : cosh_x;
        }

        decimal tanh() const {
            return sinh() / cosh();
        }

        decimal asinh() const {
            return (*this + ( *this * *this + decimal(1ul)).root(2ul)).ln();
        }

        decimal acosh() const {
            return (*this + (*this * *this - decimal(1ul)).root(2ul)).ln();
        }

        decimal atanh() const {
            return decimal("0.5") * ((decimal(1ul) + *this) / (decimal(1ul) - *this)).ln();
        }

        template<typename N>
        constexpr N to_integral() const {
            N a = (_numerator[0] / settings.decimal_denominator()).to_integral<N>();

            if (std::is_signed<N>::value && is_negative()) {
                a *= -1;
            }
            return a;
        }

        constexpr integer& get_integer() {
            return _numerator[0];
        }

        constexpr integer get_integer() const {
            return _numerator[0];
        }

        constexpr integer::sign_t get_sign() const {
            return _numerator[0].get_sign();
        }

        std::string to_string(std::size_t base = 10) const {
            std::string result;

            if (is_finite()) {
                integer qot, rem;
                _numerator[0].div_mod(settings.decimal_denominator(), qot, rem);

                result = qot.to_string(base);

                std::string decimal_string = rem.abs().to_string(base);

                int decimal_dif = static_cast<int>(settings.decimal_scale()) - static_cast<int>(decimal_string.size());

                if (decimal_dif) {
                    decimal_string.insert(0, decimal_dif, '0');
                }

                if (rem) {
                    result += "." + decimal_string;
                    while (result.back() == '0') {
                        result.pop_back();
                    }
                }
                else {
                    result += ".0";
                }
            }
            else {
                switch (get_sign()) {
                    case integer::sign_t::pos_infinity:
                        result = "Infinity";
                        break;
                    case integer::sign_t::neg_infinity:
                        result = "-Infinity";
                        break;
                    case integer::sign_t::undef:
                        result = "undefined";
                        break;
                    case integer::sign_t::nan:
                        result = "NaN";
                        break;
                    default:
                        break;
                }
            }
            return result;
        }

        std::string to_string(std::size_t base, bool sign) const {
            std::string result = to_string(base);

            if (sign && _numerator[0].is_positive()) {
                result = "+" + result;
            }
            return result;
        }

    private:
        scalar<integer> _numerator;

        decimal get_ln() const {
            /*
                Perform a Taylor Series estimation of
                the natural logarithm of this number.
            */
            decimal one(1ul);
            decimal two(2ul);

            decimal x = (*this - one) / (*this + one); 

            decimal ln_x_iter = x;
            decimal ln_x      = x;

            decimal denominator(3ul);

            decimal last_iter;

            // Perform the calculation until the last 'last_iter' decimal
            // is the same as the current calculation 'ln_x'.  

            while (last_iter != ln_x) {
                last_iter = ln_x;

                ln_x_iter *= x;
                ln_x_iter *= x;

                ln_x += (one / denominator) * ln_x_iter;

                denominator += two;
            }

            return ln_x * two;
        }

        decimal get_sin() const {

            decimal one   = 1ul;
            decimal x     = *this;
            decimal sin_x = x;

            decimal sin_x_iter = x;

            decimal sin_power(3ul);
            decimal sin_factorial(6ul);

            decimal last;

            // Perform the calculation until the last 'last_iter' decimal
            // is the same as the current calculation 'term'.  

            while (last.get_integer() != sin_x.get_integer()) {

                last = sin_x;

                sin_x = -sin_x;

                sin_x_iter *= x;
                sin_x_iter *= x;

                sin_x += (one / sin_factorial) * sin_x_iter;

                sin_power     += one;
                sin_factorial *= sin_power;

                sin_power     += one;
                sin_factorial *= sin_power;
            }

            return sin_x;
        }

        decimal get_asin() const {
            
            /*
                Perform a Taylor Series estimation of
                the natural logarithm of this number.
            */

            decimal one(1ul);
            decimal two(2ul);

            decimal x = *this;
            decimal asin_x = x;

            decimal asin_x_iter = x;

            decimal asin_power(3ul);

            decimal num_coef(1ul);
            decimal den_coef(2ul);

            decimal last;

            // Perform the calculation until the last 'last_iter' decimal
            // is the same as the current calculation 'term'.  

            while (last.get_integer() != asin_x.get_integer()) {

                last = asin_x;

                // asin_x = -asin_x;

                asin_x_iter *= x;
                asin_x_iter *= x;

                asin_x += (one / (den_coef * asin_power)) * (num_coef * asin_x_iter);

                num_coef   *= asin_power;
                asin_power += one;
                den_coef   *= asin_power;
                asin_power += one;
            }

            return asin_x;
        }

        decimal get_atan() const {

            /*
                Perform a Taylor Series estimation of
                the natural logarithm of this number.
            */

            decimal one(1ul);
            decimal two(2ul);

            decimal x = *this;
            decimal sinh_x = x;

            decimal sinh_x_iter = x;

            decimal atan_power(3ul);

            decimal last;

            // Perform the calculation until the last 'last_iter' decimal
            // is the same as the current calculation 'term'.  

            while (last.get_integer() != sinh_x.get_integer()) {

                last = sinh_x;

                sinh_x = -sinh_x;

                sinh_x_iter *= x;
                sinh_x_iter *= x;

                sinh_x += (one / atan_power) * sinh_x_iter;

                atan_power += two;
            }

            return sinh_x;
        }

        decimal get_sinh() const {

            decimal one = 1ul;
            decimal x = *this;
            decimal sinh_x = x;

            decimal sinh_x_iter = x;

            decimal sin_power(3ul);
            decimal sin_factorial(6ul);

            decimal last;

            // Perform the calculation until the last 'last_iter' decimal
            // is the same as the current calculation 'term'.  

            while (last.get_integer() != sinh_x.get_integer()) {

                last = sinh_x;

                sinh_x_iter *= x;
                sinh_x_iter *= x;

                sinh_x += (one / sin_factorial) * sinh_x_iter;

                sin_power += one;
                sin_factorial *= sin_power;

                sin_power += one;
                sin_factorial *= sin_power;
            }

            return sinh_x;
        }

        decimal get_cosh() const {

            decimal one(1ul);
            decimal x = *this;
            decimal cosh_x = one;

            decimal cosh_x_iter = one;

            decimal cos_power(2ul);
            decimal cos_factorial(2ul);

            decimal last;

            // Perform the calculation until the last 'last_iter' decimal
            // is the same as the current calculation 'term'.  

            while (last.get_integer() != cosh_x.get_integer()) {

                last = cosh_x;

                cosh_x_iter *= x;
                cosh_x_iter *= x;

                cosh_x += (one / cos_factorial) * cosh_x_iter;

                cos_power += one;
                cos_factorial *= cos_power;

                cos_power += one;
                cos_factorial *= cos_power;
            }

            return cosh_x;
        }


        void set_integer(std::string& value) {
            _numerator = integer(value);
            _numerator *= settings.decimal_denominator();
        }

        void set_decimal(std::string& value) {

            trim_ws(value);

            if (!value.empty()) {

                integer exponent = get_sub_text_value(value, "e");

                int scale = find_and_set_scale(value);

                _numerator = integer(value);

                if (scale < settings.decimal_scale()) {
                    _numerator = _numerator * (settings.decimal_denominator() / integer(10ul).pow(scale));
                }

                set_decimal_exponent(exponent);
            }
        }

        void set_rational(std::string& value) {

            // Locate and set the denominator.
            integer den(get_sub_text_value(value, "/"));

            if (!den) {
                _numerator = integer("undefined");
                return;
            }

            trim_ws(value);

            // Look if a leading integer is present and get it.
            std::string lead_val_str = "";

            auto found = value.find_last_of(' ');

            if (found != std::string::npos) {

                lead_val_str = value.substr(0, found);

                value.erase(0, found);
            }

            // Define the leading integer.
            integer lead_value(lead_val_str);

            _numerator = integer(value);

            // Add any leading value to the number.
            if (lead_value) {

                _numerator = _numerator + (lead_value * den);
            }

            _numerator *= settings.decimal_denominator();
            _numerator /= den;
        }

        void set_whole(std::string& value) {
            _numerator = integer(value, 10);
        }

        void set_binary(std::string& value) {
            _numerator = integer(value, 2);
        }

        void set_octal(std::string& value) {
            _numerator = integer(value, 8);
        }

        void set_heximal(std::string& value) {
            _numerator = integer(value, 16);
        }

        void set_decimal_exponent(integer& exponent) {
            /*
                Test if an exponent was defined then apply the exponent.
                Then reset the _scale of the decimal number.
            */
            if (exponent) {

                std::size_t n = (exponent / settings.decimal_denominator()).to_integral<std::size_t>();

                integer exp = integer(10ul).pow(n);

                if (exponent.is_negative()) {

                    _numerator = _numerator / exp;
                }
                else if (exponent.is_positive()) {

                    _numerator = _numerator * exp;
                }
            }
        }

        integer get_sub_text_value(std::string& value, std::string del) const {
            /*
                See if a value is defined for a specific deliminator.  If found get and
                return the integer after the delimeter, preserving the remaining text.
            */
            auto found = value.find(del);

            std::string sub_string_value = "";

            integer derived_value;

            if (found != std::string::npos) {

                if (value.begin() + found < value.end()) {
                    sub_string_value = value.substr(found + 1);
                }
                value.erase(found);

                derived_value = integer(sub_string_value);
            }

            return derived_value;
        }

        std::size_t find_and_set_scale(std::string& value) const {
            /*
                Find the decimal point which is required to for all
                Decimal number text input.
            */

            int scale = 0;

            auto found = value.find(".");

            if (found != std::string::npos) {
                value.at(found) = ' ';

                scale = static_cast<int>(value.size() - found) - 1;
                if (scale < 1) {
                    scale = 0;
                }
            }

            if (scale > settings.decimal_scale()) {

                value.resize(value.size() - (scale - settings.decimal_scale()));
            }

            return scale;
        }
    };

    bool decimal_tests() {

        decimal::settings.decimal_scale(100);

        auto a_str = "1234567890987654321";
        auto b_str = "9876543210.125";
        auto c_str = "0";

        auto a = scalar<decimal>(decimal{a_str});
        auto b = scalar<decimal>(decimal{b_str});
        auto c = scalar<decimal>(decimal{c_str});

        fmt::println("a.integer_value() = {}", a[0].get_integer().to_string());
        fmt::println("b.integer_value() = {}", b[0].get_integer().to_string());
        fmt::println("c.integer_value() = {}", c[0].get_integer().to_string());
        fmt::println("");

        fmt::println("a = {}", a[0].to_string());  // = 1234567890987654321
        fmt::println("b = {}", b[0].to_string());  // = 9876543210
        fmt::println("c = {}", c[0].to_string());  // = 0
        fmt::println("");

        fmt::println("a == b : {}", a[0] == b[0]);         // = false
        fmt::println("a != b : {}", a[0] != b[0]);         // = true
        fmt::println("a < b : {}", a[0] < b[0]);           // = false
        fmt::println("a > b : {}", a[0] > b[0]);           // = true
        fmt::println("a <= b : {}", a[0] <= b[0]);         // = false
        fmt::println("a >= b : {}", a[0] >= b[0]);         // = true
        fmt::println("");

        fmt::println("a++ = {}", (a++)[0].to_string());  // = 1234567890987654322
        fmt::println("++a = {}", (++a)[0].to_string());  // = 1234567890987654321
        fmt::println("b++ = {}", (b++)[0].to_string());  // = 9876543211
        fmt::println("++b = {}", (++b)[0].to_string());  // = 9876543210
        fmt::println("");

        a = scalar<decimal>(decimal{a_str});
        b = scalar<decimal>(decimal{b_str});

        fmt::println("a = {}", a[0].to_string());  // = 1234567890987654321
        fmt::println("b = {}", b[0].to_string());  // = 9876543210
        fmt::println("c = {}", c[0].to_string());  // = 0
        fmt::println("");

        fmt::println("a-- = {}", (a--)[0].to_string());  // = 1234567890987654322
        fmt::println("--a = {}", (--a)[0].to_string());  // = 1234567890987654321
        fmt::println("b-- = {}", (b--)[0].to_string());  // = 9876543210
        fmt::println("--b = {}", (--b)[0].to_string());  // = 9876543210
        fmt::println("");

        a = scalar<decimal>(decimal{a_str});
        b = scalar<decimal>(decimal{b_str});
        c = scalar<decimal>(decimal{c_str});

        fmt::println("a = {}", a[0].to_string());  // = 1234567890987654321
        fmt::println("b = {}", b[0].to_string());  // = 9876543210
        fmt::println("c = {}", c[0].to_string());  // = 0
        fmt::println("");

        fmt::println("a.inverse() = {}", a[0].inverse().to_string());  // = 1234567890987654321
        fmt::println("b.inverse() = {}", b[0].inverse().to_string());  // = 9876543210
        fmt::println("c.inverse() = {}", c[0].inverse().to_string());  // = 0
        fmt::println("");

        fmt::println("a = {}", a[0].to_string());  // = 1234567890987654321
        fmt::println("b = {}", b[0].to_string());  // = 9876543210
        fmt::println("c = {}", c[0].to_string());  // = 0
        fmt::println("");

        fmt::println("a + b = {}", (a + b)[0].to_string());  // = -1234567900864197531
        fmt::println("a - b = {}", (a - b)[0].to_string());  // = -1234567881111111111
        fmt::println("a * b = {}", (a * b)[0].get_integer().to_string());  // = 12193263121018137477899710410
        fmt::println("a / b = {}", (a / b)[0].to_string());  // = 124999998
        fmt::println("a % b = {}", (a[0] % b[0]).to_string());  // = 9490740741
        fmt::println("");

        // fmt::println("a & b = {}", (a & b)[0].to_string());  // = 2102432
        // fmt::println("a | b = {}", (a | b)[0].to_string());  // = 1234567900862095099
        // fmt::println("a ^ b = {}", (a ^ b)[0].to_string());  // = 1234567900859992667
        // fmt::println("~a = {}", (~a)[0].to_string());        // = -1234567890987654322
        // fmt::println("~b = {}", (~b)[0].to_string(16));        // = -9876543211
        // fmt::println("");

        return true;
    }
}


/*
        friend constexpr decimal sin(const decimal& v) {
            decimal sin_x = abs(v);
            bool neg = v.is_negative();

            // Reduce argument to improve convergence
            if (sin_x > settings._360()) {
                sin_x %= settings._360();
            }

            if (sin_x > settings._180()) {
                sin_x %= settings._180();
                neg = !neg;
            }

            // Convert degrees to radians and use series
            sin_x *= (settings.pi() / settings._180());
            sin_x = sin_x.get_sin();

            return neg ? -sin_x : sin_x;
        }

        friend constexpr decimal cos(const decimal& v) {
            if (!v.is_finite()) {
                return integer("undefined");
            }
            // Use identity cos(x) = sin(90° - x) to avoid sqrt cancellation and sign ambiguity.
            decimal ninety = settings._180() / decimal(2ull);
            return sin(ninety - v);
        }

        friend constexpr decimal tan(const decimal& v) {
            if (!v.is_finite()) {
            return integer("undefined");
            }

            // tan(x) = sin(x) / cos(x)
            decimal s = sin(v);
            decimal c = cos(v);

            if (!c.is_finite() || c.is_zero()) {
            return integer("undefined");
            }

            return s / c;
        }

    friend constexpr decimal asin(const decimal& v) {

            decimal asin_x = abs(v);

            bool neg = v.is_negative() ? true : false;

            /*
                First reduce value closure to 0.5
                to improve speed of convergence.
            */

            int power_of_2 = 0;

            decimal limit("0.5");
            decimal one(1ull);
            decimal two(2ull);

            while (asin_x > limit) {

                asin_x = asin_x / (one + root((one + asin_x * asin_x), two));

                power_of_2 += 1;
            }

            asin_x = asin_x.get_asin() * pow(two, decimal(static_cast<uint64_t>(power_of_2)));

            return neg ? -asin_x : asin_x;
        }


        friend constexpr decimal acos(const decimal& v) {
                    // Handle non-finite and domain checks
                    if (!v.is_finite()) {
                        return integer("undefined");
                    }
                    decimal one(1ull);
                    if (v > one || v < -one) {
                        return integer("undefined");
                    }

                    // Exact cases
                    if (v.is_zero()) {
                        return settings.pi() / decimal(2ull);
                    }
                    if (v == one) {
                        return decimal(); // 0
                    }
                    if (v == -one) {
                        return settings.pi();
                    }

                    // Use stable formula away from 0: acos(x) = 2 * atan( sqrt((1-x)/(1+x)) )
                    // For small |x|, use acos(x) = pi/2 - asin(x) to avoid extra sqrt.
                    decimal x = v;
                    decimal limit("0.5");
                    if (abs(x) <= limit) {
                        return settings.pi() / decimal(2ull) - asin(x);
                    }

                    decimal num = (one - x);
                    decimal den = (one + x);

                    // Guard against rounding causing slight negatives
                    if (num.is_negative()) {
                        num = decimal();
                    }
                    if (den.is_negative()) {
                        return integer("undefined");
                    }

                    decimal s = root(num / den, decimal(2ull));
                    if (!s.is_finite() || s.is_negative()) {
                        return integer("undefined");
                    }

                    return decimal(2ull) * s.atan();
                }

        constexpr decimal atan() const {
            // Handle non-finite and trivial cases
            if (!is_finite()) {
            return integer("undefined");
            }
            if (is_zero()) {
            return decimal();
            }

            // Odd symmetry: atan(-x) = -atan(x)
            bool neg = is_negative();
            decimal x = neg ? -(*this) : *this;

            // atan(+/-Infinity) = +/- pi/2
            if (is_pos_inf()) {
            return settings.pi() / decimal(2ull);
            }
            if (is_neg_inf()) {
            return -(settings.pi() / decimal(2ull));
            }

            // Helper: power series for small |z|
            auto atan_series = [](const decimal& z) -> decimal {
            // atan(z) = z - z^3/3 + z^5/5 - ...
            decimal sum = z;
            decimal term = z;
            decimal z2 = z * z;

            decimal three(3ull);
            decimal two(2ull);
            decimal denom = three;

            decimal last;
            while (last.get_integer() != sum.get_integer()) {
                last = sum;
                term *= -(z2);        // next factor: -z^2
                sum  += term / denom; // add term/(2k+1)
                denom += two;         // next odd denominator
                if (!term) break;
            }
            return sum;
            };

            // Argument reduction loop using half-/quarter-angle identities:
            // Better threshold than 0.5: tan(pi/8) = sqrt(2) - 1 ~ 0.4142
            decimal one(1ull);
            decimal two(2ull);
            decimal four(4ull);
            decimal sqrt2 = root(two, two);
            decimal tan_pi_over_8 = sqrt2 - one;

            // Accumulator for multiples of pi/4
            decimal acc;

            // Reduce until |x| <= tan(pi/8) for fast series convergence
            while (x > tan_pi_over_8) {
            if (x > one) {
                // atan(x) = pi/2 - atan(1/x)
                acc += (settings.pi() / two);
                x = one / x;
            } else {
                // atan(x) = pi/4 + atan((x-1)/(x+1))
                acc += (settings.pi() / four);
                decimal t = (x - one) / (x + one);
                // If t is not reduced (numerical issues), break
                if (!(t < x)) {
                x = t; // still proceed once to avoid infinite loop
                break;
                }
                x = t;
            }
            }

            // Final small-argument evaluation
            decimal result = acc + atan_series(x);

            return neg ? -result : result;
        }

        decimal sinh() const {
            // Handle non-finite inputs consistently with other trig funcs
            if (!is_finite()) {
                return integer("undefined");
            }
            if (is_zero()) {
                return decimal();
            }

            // sinh is odd: compute on |x|, restore sign at the end
            decimal x = abs(*this);
            bool neg = is_negative();

            // Argument reduction via halving to speed up series convergence:
            // compute t = x / 2^k small enough, then rebuild using
            //   sinh(2y) = 2*sinh(y)*cosh(y)
            //   cosh(2y) = 2*cosh(y)^2 - 1
            decimal two(2ull);
            decimal limit("0.5");
            int k = 0;
            int max_k = static_cast<int>(std::min<std::size_t>(64, settings.decimal_scale()));
            decimal t = x;
            while (t > limit && k < max_k) {
                t /= two;
                ++k;
            }

            // Evaluate series at reduced argument
            decimal s = t.get_sinh();
            decimal c = t.get_cosh();

            // Reconstruct sinh(x) by k doublings
            for (int i = 0; i < k; ++i) {
                // s' = 2*s*c
                decimal s2 = s + s;
                decimal s_next = s2 * c;

                // c' = 2*c^2 - 1
                decimal c_next = (c + c) * c - decimal(1ull);

                s = s_next;
                c = c_next; 
            }

            return neg ? -s : s;
        }

        decimal cosh() const {
            // Handle non-finite inputs
            if (!is_finite()) {
                return integer("undefined");
            }
            // cosh(0) = 1
            if (is_zero()) {
                return decimal(1ull);
            }

            // cosh is even; use |x|
            decimal x = abs(*this);

            // Argument reduction via halving for faster series convergence:
            // evaluate at t = x / 2^k <= limit, then rebuild with
            //   cosh(2y) = 2*cosh(y)^2 - 1
            decimal two(2ull);
            decimal limit("0.5");
            int k = 0;
            int max_k = static_cast<int>(std::min<std::size_t>(64, settings.decimal_scale()));
            decimal t = x;
            while (t > limit && k < max_k) {
                t /= two;
                ++k;
            }

            // Series at reduced argument
            decimal c = t.get_cosh();

            // Reconstruct by k doublings
            for (int i = 0; i < k; ++i) {
                c = (c + c) * c - decimal(1ull); // 2*c^2 - 1
            }

            return c;
        }

        decimal tanh() const {
            // Handle non-finite and trivial cases
            if (!is_finite()) {
                return integer("undefined");
            }
            if (is_zero()) {
                return decimal();
            }

            // Odd symmetry: tanh(-x) = -tanh(x)
            decimal x = abs(*this);
            bool neg = is_negative();

            // Compute via sinh/cosh which already do argument reduction
            decimal s = x.sinh();
            decimal c = x.cosh();

            if (!s.is_finite() || !c.is_finite() || c.is_zero()) {
                return integer("undefined");
            }

            decimal r = s / c;

            // Clamp tiny numeric drift outside [-1, 1]
            decimal one(1ull);
            if (r > one)  {
                r = one;
            }
            if (r < -one) {
                r = -one;
            }

            return neg ? -r : r;
        }

        decimal asinh() const {
            // Handle non-finite inputs
            if (is_nan() || is_undef()) {
                return integer("undefined");
            }
            if (is_pos_inf() || is_neg_inf()) {
                return *this;
            }
            if (is_zero()) {
                return decimal();
            }

            // Use odd symmetry: asinh(-x) = -asinh(x)
            decimal x = abs(*this);
            bool neg = is_negative();

            decimal one(1ull);
            decimal two(2ull);

            // Piecewise for numerical stability:
            // - small |x|: use ln(1 + x + x^2 / (1 + sqrt(1 + x^2))) to reduce cancellation
            // - large |x|: use ln(x) + ln(2) to avoid overflow and cancellation
            // - otherwise: standard ln(x + sqrt(x^2 + 1))
            decimal result;
            decimal small("0.5");
            decimal large("1e8");

            if (x <= small) {
                decimal rad = root((one + x * x), two);
                if (!rad.is_finite() || rad.is_negative()) {
                    return integer("undefined");
                }
                decimal z = x + (x * x) / (one + rad);
                result = ln(one + z);
            } 
            else if (x >= large) {
                    result = ln(x) + decimal(Oliver::decimal::settings.ln2());
                } 
                else {
                    decimal rad = root((x * x + one), two);
                    if (!rad.is_finite() || rad.is_negative()) {
                        return integer("undefined");
                    }
                    result = ln(x + rad);
            }

            return neg ? -result : result;
        }

        decimal acosh() const {
            // Handle non-finite and domain
            if (is_nan() || is_undef()) {
                return integer("undefined");
            }
            if (is_pos_inf()) {
                return *this;
            }
            if (is_neg_inf()) {
                return integer("undefined");
            }

            decimal x = *this;
            decimal one(1ull);
            if (x < one) {
                return integer("undefined");
            }
            if (x == one) {
                return decimal(); // 0
            }

            // For very large x: acosh(x) ~ ln(2x) = ln(x) + ln(2)
            decimal large("1e8");
            if (x >= large) {
                return ln(x) + decimal(Oliver::decimal::settings.ln2());
            }

            // Numerically stable form near 1:
            // acosh(x) = ln(x + sqrt(x-1) * sqrt(x+1))
            decimal a = root((x - one), decimal(2ull));
            decimal b = root((x + one), decimal(2ull));
            if (!a.is_finite() || !b.is_finite() || a.is_negative() || b.is_negative()) {
                return integer("undefined");
            }
            return ln(x + a * b);
        }

        decimal atanh() const {
            // Domain and finiteness checks
            if (is_nan() || is_undef() || is_pos_inf() || is_neg_inf()) {
                return integer("undefined");
            }

            decimal x = *this;
            decimal one(1ull);
            if (x >= one || x <= -one) {
                return integer("undefined");
            }
            if (x.is_zero()) {
                return decimal();
            }

            // Odd function: work with |x|, restore sign
            bool neg = x.is_negative();
            x = abs(x);

            // Use series for small |x| to avoid cancellation:
            // atanh(x) = x + x^3/3 + x^5/5 + ...
            decimal limit("0.25");
            decimal result;

            if (x <= limit) {
                decimal sum = x;
                decimal term = x;
                decimal x2 = x * x;
                decimal denom(3ull);
                decimal two(2ull);
                decimal last;

                while (last.get_integer() != sum.get_integer()) {
                    last = sum;
                    term *= x2;              // x^{2k+1}
                    sum += term / denom;     // + x^{2k+1} / (2k+1)
                    denom += two;            // next odd denominator
                }
                result = sum;
            }
            else {
                // For moderate |x| use a numerically stable form:
                // atanh(x) = 0.5 * ln(1 + (2x)/(1 - x))
                decimal two(2ull);
                decimal y = (two * x) / (one - x);
                result = ln(one + y) / two;
            }

            return neg ? -result : result;   
        }

        decimal get_sin() const {
            decimal one = 1ull;
            decimal two = 2ull;

            decimal x = *this;

            // Taylor series with stable term recurrence:
            // sin(x) = x - x^3/3! + x^5/5! - ...
            decimal sum  = x;      // accumulated sum
            decimal term = x;      // current term, starts at x
            decimal n    = one;    // term index for denominator: uses 2n and 2n+1

            decimal last;

            while (sum.get_integer() != last.get_integer()) {
                last = sum;

                // next term: -term * x^2 / ((2n)*(2n+1))
                decimal a = two * n;
                decimal next = -(term * x * x) / (a * (a + one));

                sum += next;

                // Advance
                term = next;
                n += one;

                // Safety: if term becomes zero at current precision, stop
                if (!term) {
                    break;
                }
            }

            return sum;
        }

        decimal get_asin() const {
            
            /*
                Perform a Taylor Series estimation of
                the natural logarithm of this number.
            */

            decimal one(1ull);
            decimal two(2ull);

            decimal x = *this;
            decimal asin_x = x;

            decimal asin_x_iter = x;

            decimal asin_power(3ull);

            decimal num_coef(1ull);
            decimal den_coef(2ull);

            decimal last;

            // Perform the calculation until the last 'last_iter' decimal
            // is the same as the current calculation 'term'.  

            while (last.get_integer() != asin_x.get_integer()) {

                last = asin_x;

                // asin_x = -asin_x;

                asin_x_iter *= x;
                asin_x_iter *= x;

                asin_x += (one / (den_coef * asin_power)) * (num_coef * asin_x_iter);

                num_coef   *= asin_power;
                asin_power += one;
                den_coef   *= asin_power;
                asin_power += one;
            }

            return asin_x;
        }

        decimal get_atan() const {
            /*
                Perform a Taylor Series estimation of
                arctan(x).
            */
            decimal one(1ull);
            decimal two(2ull);

            decimal x = *this;
            decimal sinh_x = x;

            decimal sinh_x_iter = x;

            decimal atan_power(3ull);

            decimal last;

            // Perform the calculation until the last 'last_iter' decimal
            // is the same as the current calculation 'term'.  
            while (last.get_integer() != sinh_x.get_integer()) {
                last = sinh_x;

                sinh_x = -sinh_x;

                sinh_x_iter *= x;
                sinh_x_iter *= x;

                sinh_x += (one / atan_power) * sinh_x_iter;

                atan_power += two;
            }

            return sinh_x;
        }

        decimal get_sinh() const {
            decimal one = 1ull;
            decimal x = *this;
            decimal sinh_x = x;

            decimal sinh_x_iter = x;

            decimal sin_power(3ull);
            decimal sin_factorial(6ull);

            decimal last;

            // Perform the calculation until the last 'last_iter' decimal
            // is the same as the current calculation 'term'.  
            while (last.get_integer() != sinh_x.get_integer()) {
                last = sinh_x;

                sinh_x_iter *= x;
                sinh_x_iter *= x;

                sinh_x += (one / sin_factorial) * sinh_x_iter;

                sin_power += one;
                sin_factorial *= sin_power;

                sin_power += one;
                sin_factorial *= sin_power;
            }

            return sinh_x;
        }

        decimal get_cosh() const {
            decimal one(1ull);
            decimal x = *this;
            decimal cosh_x = one;

            decimal cosh_x_iter = one;

            decimal cos_power(2ull);
            decimal cos_factorial(2ull);

            decimal last;

            // Perform the calculation until the last 'last_iter' decimal
            // is the same as the current calculation 'term'.  
            while (last.get_integer() != cosh_x.get_integer()) {
                last = cosh_x;

                cosh_x_iter *= x;
                cosh_x_iter *= x;

                cosh_x += (one / cos_factorial) * cosh_x_iter;

                cos_power += one;
                cos_factorial *= cos_power;

                cos_power += one;
                cos_factorial *= cos_power;
            }

            return cosh_x;
        }
*/