#pragma once

#include <string>
#include <compare>      // std::partial_ordering
#include <utility>      // std::swap
#include <algorithm>    // std::min
#include <type_traits>  // std::is_signed
#include <cstdint>      // fixed-width integer types
#include <cctype>       // std::isalpha
#include <mutex>

#include "integer.h"
#include "numeric_settings.h"
#include "../text_support.h"

namespace Oliver {
    namespace MPA {
        
        class decimal {
            friend class math_env;
            // friend class number;

            class numeric_settings {
            public:
                friend class decimal;

                enum class rounding_t : int8_t {
                    toward_zero = 0, half_up, half_down, half_even, half_odd, ceil, floor, away_from_zero
                };

                /// Compile-time constants
                static constexpr std::size_t def_scale()   { return _def_scale; }
                static constexpr std::size_t min_scale()   { return _min_scale; }
                static constexpr std::size_t max_scale()   { return _max_scale; }
                static constexpr std::size_t max_power()   { return _def_max_pow; }
                static constexpr std::size_t max_root()    { return _def_max_root; }

                // These cannot be constexpr because they may depend on runtime state or static variables
                static std::size_t decimal_scale() {
                    std::lock_guard<std::recursive_mutex> lock(_mutex());
                    return _decimal_scale();
                }

                static void decimal_scale(const int& scale) {
                    std::lock_guard<std::recursive_mutex> lock(_mutex());
                    int scl = (scale >= static_cast<int>(_min_scale)) ? scale     : static_cast<int>(_min_scale);
                        scl = (scl   <= static_cast<int>(_max_scale)) ?   scl : static_cast<int>(_max_scale);

                    if (_scale_not_configured()) {
                        _scale_not_configured() = false;
                        _decimal_scale()        = static_cast<std::size_t>(scl);
                        _denominator()          = integer(10ll).pow(static_cast<std::size_t>(scl));

                        _integer_e();
                        _integer_pi();
                        _integer_ln2();

                        _decimal_360();
                        _decimal_180();
                        _decimal_90();
                        _decimal_45();
                    }
                }

                static std::string rounding_mode() {
                    std::lock_guard<std::recursive_mutex> lock(_mutex());
                    switch (_round_mode()) {
                        case(rounding_t::half_up):        return "half_up";
                        case(rounding_t::half_down):      return "half_down";
                        case(rounding_t::half_even):      return "half_even";
                        case(rounding_t::half_odd):       return "half_odd";
                        case(rounding_t::ceil):           return "ceil";
                        case(rounding_t::floor):          return "floor";
                        case(rounding_t::away_from_zero): return "away_from_zero";
                        case(rounding_t::toward_zero):    return "toward_zero";
                    }
                    return "toward_zero";
                }

                static void rounding_mode(const std::string& mode) {
                    std::lock_guard<std::recursive_mutex> lock(_mutex());
                    if (mode == "half_up")           {
                        _round_mode()   = rounding_t::half_up;
                    }
                    else if (mode == "half_down")    {
                        _round_mode()   = rounding_t::half_down;
                    }
                    else if (mode == "half_even")    {
                        _round_mode()   = rounding_t::half_even;
                    }
                    else if (mode == "half_odd")     {
                        _round_mode()   = rounding_t::half_odd;
                    }
                    else if (mode == "ceil")         {
                        _round_mode()   = rounding_t::ceil;
                    }
                    else if (mode == "floor")        {
                        _round_mode()   = rounding_t::floor;
                    }
                    else if (mode == "away_from_zero") {
                        _round_mode() = rounding_t::away_from_zero;
                    }
                    else  {
                        _round_mode()  = rounding_t::toward_zero;
                    }
                }

                static std::string& deg_or_rad() {
                    return _deg_or_rad();
                }

                static const integer& decimal_denominator() {
                    return _denominator();
                }

                static const decimal& e() {
                    return _decimal_e();
                }

                static const decimal& pi() {
                    return _decimal_pi();
                }

                static const decimal& ln2() {
                    return _decimal_ln2();
                }

                static const decimal& _360() {
                    return _decimal_360();
                }

                static const decimal& _180() {
                    return _decimal_180();
                }

                static const decimal& _90() {
                    return _decimal_90();
                }

                static const decimal& _45() {
                    return _decimal_45();
                }

                static bool& scale_not_configured() {
                    static bool truth = true;
                    return truth;
                }

            private:
                static constexpr std::size_t _def_view     = 16ul;
                static constexpr std::size_t _def_scale    = 32ul;
                static constexpr std::size_t _def_max_pow  = 1000ul;
                static constexpr std::size_t _def_max_root = 1000ul;
                static constexpr std::size_t _min_scale    = 0ul;
                static constexpr std::size_t _max_scale    = 10000ul;

                static std::recursive_mutex& _mutex() {
                    static std::recursive_mutex mtx;
                    return mtx;
                }

                static bool& _scale_not_configured() {
                    static bool truth = true;
                    return truth;
                }

                static std::size_t& _decimal_scale() {
                    static std::size_t scale = _def_scale + 1;
                    return scale;
                }

                static std::size_t& _max_power() {
                    static std::size_t max_power = _def_max_pow;
                    return max_power;
                }

                static std::size_t& _max_root() {
                    static std::size_t max_root = _def_max_root;
                    return max_root;
                }

                static integer& _denominator() {
                    static integer denom = integer(10ll).pow(_def_scale);
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

                // Initialize once from integer counterparts; no reassignment on subsequent calls.
                static const decimal& _decimal_pi() {
                    static const decimal pi(_integer_pi());
                    return pi;
                }

                static const decimal& _decimal_e() {
                    static const decimal e(_integer_e());
                    return e;
                }

                static const decimal& _decimal_ln2() {
                    static const decimal ln2(_integer_ln2());
                    return ln2;
                }

                static decimal& _decimal_360() {
                    static decimal d(360ll);
                    return d;
                }

                static decimal& _decimal_180() {
                    static decimal d(180ll);
                    return d;
                }

                static decimal& _decimal_90() {
                    static decimal d(90ll);
                    return d;
                }

                static decimal& _decimal_45() {
                    static decimal d(45ll);
                    return d;
                }

                static rounding_t& _round_mode() {
                    static rounding_t mode = rounding_t::half_even;
                    return mode;
                }

                static std::string& _deg_or_rad() {
                    static std::string _deg_or_rad = "degrees";
                    return _deg_or_rad;
                }
            };

        public:
        /*
            The decimal class definition starts here.
        */
            static constexpr numeric_settings settings{};

            // Sentinel values.
            static const decimal& NaN() {
                static const decimal nan_value(integer::NaN());
                return nan_value;
            }

            static const decimal& undefined() {
                static const decimal undef_value(integer::undefined());
                return undef_value;
            }

            static const decimal& pos_inf() {
                static const decimal pos_inf_value(integer::pos_inf());
                return pos_inf_value;
            }

            static const decimal& neg_inf() {
                static const decimal neg_inf_value(integer::neg_inf());
                return neg_inf_value;
            }

            constexpr decimal() :
                _numerator()
            {}

            constexpr decimal(int64_t number) : 
                _numerator(integer(integer(number)))
            {
                _numerator *= settings.decimal_denominator();
            }

            constexpr decimal(uint64_t number) :
                _numerator(integer(number))
            {
                _numerator *= settings.decimal_denominator();
            }

            decimal(const std::string& value, int base = 10) : _numerator() 
            {
                parse_string_value(value, base);
            }

            decimal(long double number) {
                std::string str = std::to_string(number);
                parse_string_value(str, 10);
            }

            constexpr decimal(const integer& num) :
                _numerator(num)
            {}

            constexpr ~decimal()                             = default;
            constexpr decimal(decimal&& obj)                 = default;
            constexpr decimal(const decimal& obj)            = default;
            constexpr decimal& operator=(decimal&& obj)      = default;
            constexpr decimal& operator=(const decimal& obj) = default;

            friend constexpr void swap(decimal& first, decimal& second) noexcept {
                std::swap(first._numerator, second._numerator);
            }

            constexpr explicit operator bool() const {
                return static_cast<bool>(_numerator);
            }

            [[nodiscard]] constexpr bool is_decimal()  const { return !(_numerator % settings.decimal_denominator()).is_zero(); }
            [[nodiscard]] constexpr bool is_integer()  const { return !is_decimal(); }
            [[nodiscard]] constexpr bool is_negative() const { return _numerator.is_negative(); } 
            [[nodiscard]] constexpr bool is_positive() const { return _numerator.is_positive(); } 
            [[nodiscard]] constexpr bool is_zero()     const { return _numerator.is_zero(); }
            [[nodiscard]] constexpr bool is_nan()      const { return _numerator.is_nan(); }
            [[nodiscard]] constexpr bool is_undef()    const { return _numerator.is_undef(); }
            [[nodiscard]] constexpr bool is_neg_inf()  const { return _numerator.is_neg_inf(); }
            [[nodiscard]] constexpr bool is_pos_inf()  const { return _numerator.is_pos_inf(); }
            [[nodiscard]] constexpr bool is_finite()   const { return _numerator.is_finite(); }
            [[nodiscard]] constexpr bool is_infinite() const { return _numerator.is_infinite(); }
            [[nodiscard]] constexpr bool is_even()     const { return _numerator.is_even(); }
            [[nodiscard]] constexpr bool is_odd()      const { return _numerator.is_odd(); }

            constexpr bool operator==(const decimal& b) const {
                return _numerator == b._numerator;
            }

            constexpr std::partial_ordering operator<=>(const decimal& b) const {
                return _numerator <=> b._numerator;
            }

            constexpr decimal& operator&=(const decimal& other) {
                _numerator &= other._numerator;
                return *this;
            }

            constexpr decimal& operator|=(const decimal& other) {
                _numerator |= other._numerator;
                return *this;
            }

            constexpr decimal& operator^=(const decimal& other) {
                _numerator ^= other._numerator;
                return *this;
            }

            constexpr decimal& operator<<=(std::size_t shift) {
                _numerator <<= shift;
                return *this;
            }

            constexpr decimal& operator>>=(std::size_t shift) {
                _numerator >>= shift;
                return *this;
            }

            constexpr decimal& operator+=(const decimal& other) {
                _numerator += other._numerator;
                handle_rounding();
                return *this;
            }

            constexpr decimal& operator-=(const decimal& other) {
                _numerator -= other._numerator;
                handle_rounding();
                return *this;
            }

            constexpr decimal& operator*=(const decimal& other) {
                const integer& den = settings.decimal_denominator(); // cache
                _numerator *= other._numerator;
                _numerator /= den;
                handle_rounding();
                return *this;
            }

            constexpr decimal& operator/=(const decimal& other) {
                const integer& den = settings.decimal_denominator();
                _numerator *= den;
                _numerator /= other._numerator;
                handle_rounding();
                return *this;
            }

            constexpr decimal& operator%=(const decimal& other) {
                integer quotient;
                integer remainder;
                integer::divmod(_numerator, other._numerator, quotient, remainder);
                _numerator = std::move(remainder);
                handle_rounding();
                return *this;
            }

            constexpr decimal operator+() const {
                return *this;
            }

            constexpr decimal operator-() const {
                decimal result;
                result._numerator = -_numerator;
                return result;
            }

            constexpr decimal operator~() const {
                decimal result;
                result._numerator = ~_numerator;
                return result;
            }

            constexpr decimal inverse() const {
                if (is_zero()) {
                    return decimal::undefined();  // We should define complex infinity.
                }
                return decimal(1ll) / *this;
            }

            constexpr decimal operator&(const decimal& b) const {
                decimal a(*this);
                a &= b;
                return a;
            }

            constexpr decimal operator|(const decimal& b) const {
                decimal a(*this);
                a |= b;
                return a;
            }

            constexpr decimal operator^(const decimal& b) const {
                decimal a(*this);
                a ^= b;
                return a;
            }

            constexpr decimal operator<<(std::size_t shift) const {
                decimal a(*this);
                a <<= shift;
                return a;
            }

            constexpr decimal operator>>(std::size_t shift) const {
                decimal a(*this);
                a >>= shift;
                return a;
            }

            constexpr decimal operator+(const decimal& b) const {
                decimal a(*this);
                a += b;
                return a;
            }

            constexpr decimal operator-(const decimal& b) const {
                decimal a(*this);
                a -= b;
                return a;
            }

            constexpr decimal operator*(const decimal& b) const {
                decimal a(*this);
                a *= b;
                return a;
            }

            constexpr decimal operator/(const decimal& b) const {
                decimal a(*this);
                a /= b;
                return a;
            }

            constexpr decimal operator%(const decimal& b) const {
                decimal a(*this);
                a %= b;
                return a;
            }

            friend constexpr void div_mod(const decimal& a, const decimal& b, decimal& quotient, decimal& remainder) {
                quotient  = a / b;
                remainder = a % b;
            }

            friend constexpr decimal rem(const decimal& a) {
                const integer& den = settings.decimal_denominator();
                return a._numerator % den;
            }

            friend constexpr decimal operator&(decimal& a, decimal&& b) {
                a &= b;
                return std::move(a);
            }

            friend constexpr decimal operator|(decimal& a, decimal&& b) {
                a |= b;
                return std::move(a);
            }

            friend constexpr decimal operator^(decimal& a, decimal&& b) {
                a ^= b;
                return std::move(a);
            }

            friend constexpr decimal operator+(decimal& a, decimal&& b) {
                a += b;
                return std::move(a);
            }

            friend constexpr decimal operator-(decimal& a, decimal&& b) {
                a -= b;
                return std::move(a);
            }

            friend constexpr decimal operator*(decimal& a, decimal&& b) {
                a *= b;
                return std::move(a);
            }

            friend constexpr decimal operator/(decimal& a, decimal&& b) {
                a /= b;
                return std::move(a);
            }

            friend constexpr decimal operator%(decimal& a, decimal&& b) {
                a %= b;
                return std::move(a);
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
                decimal result;
                result._numerator = _numerator.abs();
                return result;
            }

            constexpr decimal ceil() const {
                if (settings.decimal_scale() == 0) 
                    return *this;

                const integer& den = settings.decimal_denominator();
                const integer& num = _numerator;
                integer q, r;
                integer::divmod(num, den, q, r);

                if (num.is_positive() && r) 
                    q += integer(1ll);

                decimal result;
                result._numerator = q * den;
                return result;
            }

            constexpr decimal floor() const {
                if (settings.decimal_scale() == 0) 
                    return *this;

                const integer& den = settings.decimal_denominator();
                const integer& num = _numerator;
                integer q, r;
                integer::divmod(num, den, q, r);
                
                if (num.is_negative() && r) 
                    q -= integer(1ll);
                    
                decimal result;
                result._numerator = q * den;
                return result;
            }

            constexpr decimal gcd(const decimal& b) const {
                const decimal& a = *this;
                if (!a._numerator.is_zero() && !b._numerator.is_zero()) {
                    decimal result;
                    result._numerator = a._numerator.gcd(b._numerator);
                    return result;
                }
                return decimal::undefined();
            }

            constexpr decimal square() const {
                return *this * *this;
            }

            constexpr decimal cube() const {
                return *this * *this * *this;
            }

            constexpr decimal pow(const decimal& exp) const {
                if (is_zero()) 
                    return decimal(1ll);

                std::size_t exponent;
                if (exp.is_positive()) {
                    exponent = exp.to_integral<std::size_t>();
                } else {
                    return decimal::undefined();
                }

                if (exponent) {
                    const std::size_t maxp = settings.max_power();

                    if (exponent > maxp) 
                        return decimal::undefined();
                    if (exponent == 2) 
                        return *this * *this;
                    if (exponent == 1) 
                        return *this;
                    if (!exponent) 
                        return decimal(1ll);

                    decimal base = *this;
                    decimal res(1ll);

                    while (exponent) {
                        if (exponent & 1) 
                            res *= base;

                        exponent >>= 1;

                        if (exponent) 
                            base *= base;
                    }
                    res.handle_rounding();
                    return res;
                }
                return decimal(1ll);
            }

            constexpr decimal sqrt() const {
                return root(2ll);
            }

            constexpr decimal cbrt() const {
                return root(3ll);
            }

            constexpr decimal root(const decimal& exp) const {
                // Newton-Raphson method.
                if (is_zero()) 
                    return *this;

                std::size_t exponent;
                if (exp.is_positive()) {
                    exponent = exp.to_integral<std::size_t>();
                } else {
                    return decimal::undefined();
                }

                if (exponent == 0) 
                    return decimal::undefined();

                const std::size_t max_root = settings.max_root();
                const std::size_t max_scale = settings.max_scale();
                const std::size_t max_power = settings.max_power();

                if (exponent > max_root) 
                    return decimal::undefined();

                uint64_t n = exponent;

                if (n == 1) 
                    return *this;
                // Heuristic guard: extremely large n relative to scale is not meaningful.
                if (n > max_scale || n > max_power) 
                    return decimal::undefined();

                // Handle sign and domain for negative inputs
                decimal x     = *this;
                bool make_neg = false;
                if (x.is_negative()) {
                    if (n % 2 == 0) 
                        return decimal::undefined();

                    make_neg = true;
                    x = x.abs();
                }
                // Initial guess: at least 1 to avoid division by zero in iteration
                decimal a = x;
                decimal one(1ll);

                if (a.is_zero()) 
                    return decimal();
                if (a < one) 
                    a = one;

                decimal rt(static_cast<int64_t>(exponent));
                decimal last;

                // Newton-Raphson: y_{k+1} = ((n-1)*y_k + x / y_k^{n-1}) / n
                while (last._numerator != _numerator) {
                    last = a;
                    // Compute y_k^{n-1}
                    decimal pow_of_y = one;
                    if (n > 1) {
                        pow_of_y = a; // a^(1)
                        for (uint64_t i = 2; i <= n - 1; ++i) {
                            pow_of_y *= a;
                        }
                    }
                    // Guard against division by zero (shouldn't happen with our init, but be safe)
                    if (pow_of_y.is_zero()) {
                        return decimal::undefined();
                    }
                    decimal term = x / pow_of_y;
                    a = (a * decimal(static_cast<int64_t>(exponent - 1)) + term) / rt;
                }

                if (make_neg) 
                    a = -a;

                a.handle_rounding();
                return a;
            }

            constexpr decimal hypot(const decimal& b) const {
                decimal s = *this * *this + b * b;
                return s.root(2ull);
            }

            constexpr decimal hypot(const decimal& b, const decimal& c) const {
                decimal s = *this * *this + b * b + c * c;
                return s.root(2ull);
            }

            constexpr decimal ln() const {
                // Fast path for ln(1) = 0 and ln(e) = 1
                if (_numerator == settings.decimal_denominator()) {
                    return decimal();
                }
                if (*this == settings.e()) {
                    return decimal(1ull);
                }
                if (*this < decimal(1ll) || !is_finite()) {
                    return decimal::undefined();
                }

                // Use a simple Newton-Raphson method for ln(x)
                // ln(x) = y, so x = exp(y)
                // y_{n+1} = y_n + (x - exp(y_n)) / exp(y_n)
                decimal x = *this;
                decimal y = decimal(0ll);
                decimal last_y;
                decimal exp_y;
                decimal one(1ll);
                decimal tol("0.000000000000000000000000001"); // tolerance

                // Initial guess based on bit length or digit count
                // For x >= 1, ln(x) ≈ (bit_length - 1) * ln(2)
                integer quotient = _numerator / settings.decimal_denominator();
                std::size_t bit_length = quotient.bit_length();
                
                if (bit_length > 1) {
                    // ln(x) ≈ (bit_length - 1) * ln(2)
                    y = decimal(static_cast<int64_t>(bit_length - 1)) * settings.ln2();
                } else {
                    // For values close to 1, start with a small positive guess
                    y = x - one;  // ln(1+ε) ≈ ε for small ε
                }

                int max_iter = 32;
                for (int i = 0; i < max_iter; ++i) {
                    last_y = y;
                    // Compute exp(y) using a simple Taylor series expansion for exp(y)
                    exp_y = decimal(1ll);
                    decimal term(1ll);
                    int k = 1;
                    while (term.abs() > tol && k < 64) {
                        term *= y;
                        term /= decimal(static_cast<int64_t>(k));
                        exp_y += term;
                        ++k;
                    }
                    decimal delta = (x - exp_y) / exp_y;
                    y += delta;
                    if (delta.abs() < tol) {
                        break;
                    }
                }
                y.handle_rounding();
                return y;
            }

            constexpr decimal exp() const {
                // Use Taylor series: exp(x) = 1 + x + x^2/2! + x^3/3! + ...
                decimal sum(1ll);           // Start with 1
                decimal term(1ll);          // Current term in the series
                decimal x = *this;
                int k = 1;
                decimal tol("0.000000000000000000000000001"); // tolerance

                while (term.abs() > tol && k < 64) {
                    term *= x;
                    term /= decimal(static_cast<int64_t>(k));
                    sum += term;
                    ++k;
                }
                sum.handle_rounding();
                return sum;
            }

            constexpr decimal log(const decimal& b) const {
                if (!is_zero() && is_positive()) {
                    decimal v(ln() / b.ln());
                    decimal c(v.floor());

                    if (b.pow(c) == *this) {
                        return c;
                    }

                    return v;
                }

                return decimal::undefined();
            }

            constexpr decimal log2() const {
                return log(decimal(2ull));
            }

            constexpr decimal log10() const {
                return log(decimal(10ull));
            }
            // Trigonometric functions as class methods

            constexpr decimal sin() const {
                if (!is_finite()) {
                    return decimal::undefined();
                }

                decimal x;
                bool negate = deg_or_rad(*this, x);

                // Taylor series: sin(x) = x - x^3/3! + x^5/5! - ...
                decimal term = x;
                decimal sum  = term;
                decimal xsq  = x * x;
                decimal last;
                std::size_t k = 1;

                do {
                    last = sum;
                    decimal denom = decimal(2ull * k) * decimal(2ull * k + 1);
                    term *= -xsq;
                    term /= denom;
                    sum  += term;
                    ++k;
                } while (sum != last && k <= settings.max_power());

                if (negate) {
                    sum = -sum;
                }
                return sum;
            }

            constexpr decimal cos() const {
                if (!is_finite()) {
                    return decimal::undefined();
                }

                decimal x;
                bool negate = deg_or_rad(*this, x);

                // Taylor series: cos(x) = 1 - x^2/2! + x^4/4! - ...
                decimal term(1ull);
                decimal sum = term;
                decimal xsq = x * x;
                decimal last;
                std::size_t k = 1;

                do {
                    last = sum;
                    decimal denom = decimal(2ull * k - 1) * decimal(2ull * k);
                    term *= -xsq;
                    term /= denom;
                    sum  += term;
                    ++k;
                } while (sum != last && k <= settings.max_power());

                if (negate) {
                    sum = -sum;
                }
                return sum;
            }

            constexpr decimal tan() const {
                if (!is_finite()) {
                    return decimal::undefined();
                }
                decimal c = this->cos();
                if (c.is_zero() || !c.is_finite()) {
                    return decimal::undefined();
                }
                return this->sin() / c;
            }

            constexpr decimal cot() const {
                if (!is_finite()) {
                    return decimal::undefined();
                }
                decimal s = this->sin();
                if (s.is_zero() || !s.is_finite()) {
                    return decimal::undefined();
                }
                return this->cos() / s;
            }

            constexpr decimal sec() const {
                if (!is_finite()) {
                    return decimal::undefined();
                }
                decimal c = this->cos();
                if (c.is_zero() || !c.is_finite()) {
                    return decimal::undefined();
                }
                return decimal(1ull) / c;
            }

            constexpr decimal csc() const {
                if (!is_finite()) {
                    return decimal::undefined();
                }
                decimal s = this->sin();
                if (s.is_zero() || !s.is_finite()) {
                    return decimal::undefined();
                }
                return decimal(1ull) / s;
            }

            // Inverse trigonometric functions (respect settings.deg_or_rad())
            constexpr decimal asin() const {
                const decimal& x = *this;
                if (!x.is_finite()) {
                    return decimal::undefined();
                }
                decimal one(1ull);
                if (x > one || x < -one) {
                    return decimal::undefined();
                }

                // Endpoints
                if (x == one) {
                    if (settings.deg_or_rad() == std::string("degrees")) {
                        return settings._90();
                    }
                    return settings.pi() / decimal(2ull);
                }
                if (x == -one) {
                    if (settings.deg_or_rad() == std::string("degrees")) {
                        return -settings._90();
                    }
                    return -settings.pi() / decimal(2ull);
                }

                // Initial guess
                decimal angle;
                if (settings.deg_or_rad() == std::string("degrees")) {
                    angle = x * settings._90();
                } else {
                    angle = x;
                }

                // Clamp initial guess to principal range
                if (settings.deg_or_rad() == std::string("degrees")) {
                    decimal ninety(90ull);
                    if (angle > ninety) angle = ninety;
                    else if (angle < -ninety) angle = -ninety;
                } else {
                    decimal half_pi = settings.pi() / decimal(2ull);
                    if (angle > half_pi) angle = half_pi;
                    else if (angle < -half_pi) angle = -half_pi;
                }

                // Newton-Raphson on f(angle) = sin(angle) - x
                decimal last;
                std::size_t iter = 0;
                decimal scale_factor = (settings.deg_or_rad() == std::string("degrees"))
                    ? (settings.pi() / settings._180())
                    : decimal(1ull);

                do {
                    last = angle;
                    decimal s = angle.sin();
                    decimal c = angle.cos();
                    if (c.is_zero()) {
                        break;
                    }
                    angle -= scale_factor * (s - x) / c;

                    // Keep within principal range
                    if (settings.deg_or_rad() == std::string("degrees")) {
                        decimal ninety(90ull);
                        if (angle > ninety) angle = ninety;
                        else if (angle < -ninety) angle = -ninety;
                    } else {
                        decimal half_pi = settings.pi() / decimal(2ull);
                        if (angle > half_pi) angle = half_pi;
                        else if (angle < -half_pi) angle = -half_pi;
                    }
                    ++iter;
                } while (angle != last && iter <= settings.max_power());

                return angle;
            }

            constexpr decimal acos() const {
                const decimal& x = *this;
                if (!x.is_finite()) {
                    return decimal::undefined();
                }
                decimal one(1ull);
                if (x > one || x < -one) {
                    return decimal::undefined();
                }

                // Endpoints
                if (x == one)  return decimal(0ull);
                if (settings.deg_or_rad() == std::string("degrees")) {
                    if (x == -one) return settings._180();
                } else {
                    if (x == -one) return settings.pi();
                }

                // For |x| <= 0.7 use acos(x) = pi/2 - asin(x) (or 90° - asin(x))
                decimal absx = x.abs();
                if (absx <= decimal(7ull) / decimal(10ull)) {
                    if (settings.deg_or_rad() == std::string("degrees")) {
                        return settings._90() - x.asin();
                    } else {
                        return settings.pi() / decimal(2ull) - x.asin();
                    }
                } else {
                    decimal y = (decimal(1ull) - x).sqrt();
                    decimal z = (decimal(1ull) + x).sqrt();
                    // atan2 returns in the configured unit; multiply by 2 to get acos
                    return decimal(2ull) * y.atan2(z);
                }
            }

            constexpr decimal atan() const {
                const decimal& x = *this;
                if (!x.is_finite()) {
                    return decimal::undefined();
                }
                // atan(x) = asin( x / sqrt(1 + x^2) )
                decimal one(1ull);
                decimal denom = (one + x * x).sqrt();
                if (denom.is_zero()) {
                    return decimal::undefined();
                }
                decimal t = x / denom;
                // Clamp numerically to [-1, 1] if tiny drift
                decimal abs_t = t.abs();
                if (abs_t > one) {
                    t = t.is_negative() ? -one : one;
                }
                return t.asin();
            }

            constexpr decimal atan2(const decimal& x) const {
                // this = y, x = x
                const decimal& y = *this;
                if (!x.is_finite() || !y.is_finite()) {
                    return decimal::undefined();
                }

                bool degrees = settings.deg_or_rad() == std::string("degrees");
                decimal zero(0ull);
                decimal ang;

                if (x.is_zero()) {
                    if (y.is_zero()) {
                        return zero;
                    }
                    if (degrees) {
                        return y.is_positive() ? settings._90() : -settings._90();
                    } else {
                        return y.is_positive() ? settings.pi() / decimal(2ull) : -settings.pi() / decimal(2ull);
                    }
                }

                ang = (y / x).atan();

                if (x.is_positive()) {
                    return ang;
                } else { // x < 0
                    if (y.is_positive() || y.is_zero()) {
                        if (degrees) {
                            return ang + settings._180();
                        } else {
                            return ang + settings.pi();
                        }
                    } else {
                        if (degrees) {
                            return ang - settings._180();
                        } else {
                            return ang - settings.pi();
                        }
                    }
                }
            }

            constexpr decimal acot() const {
                const decimal& x = *this;
                if (!x.is_finite()) {
                    return decimal::undefined();
                }
                bool degrees = settings.deg_or_rad() == std::string("degrees");

                if (x.is_zero()) {
                    return degrees ? settings._90() : settings.pi() / decimal(2ull);
                }

                decimal ang = (decimal(1ull) / x).atan();
                if (x.is_positive()) {
                    if (ang.is_negative()) {
                        if (degrees) ang += settings._180(); else ang += settings.pi();
                    }
                    return ang;
                } else {
                    if (degrees) {
                        return ang + settings._180();
                    } else {
                        return ang + settings.pi();
                    }
                }
            }

            constexpr decimal asec() const {
                const decimal& x = *this;
                if (!x.is_finite()) {
                    return decimal::undefined();
                }
                decimal one(1ull);
                decimal ax = x.abs();
                if (ax < one || x.is_zero()) {
                    return decimal::undefined();
                }
                return (decimal(1ull) / x).acos();
            }

            constexpr decimal acsc() const {
                const decimal& x = *this;
                if (!x.is_finite()) {
                    return decimal::undefined();
                }
                decimal one(1ull);
                decimal ax = x.abs();
                if (ax < one || x.is_zero()) {
                    return decimal::undefined();
                }
                return (decimal(1ull) / x).asin();
            }

            template<typename N>
            constexpr N to_integral() const {
                integer n = _numerator / settings.decimal_denominator();
                n = n.abs();
                integer limit(static_cast<int64_t>(std::numeric_limits<N>::max()));

                if (n > limit) {
                    return 0;
                }

                N a = n.to_integral<N>();
                if (std::is_signed<N>::value && is_negative()) {
                    a = static_cast<N>(-a);
                }
                return a;
            }

            constexpr decimal sinh() const {
                // sinh(x) = (exp(x) - exp(-x)) / 2
                return (this->exp() - (-(*this)).exp()) / decimal(2ll);
            }

            constexpr decimal cosh() const {
                // cosh(x) = (exp(x) + exp(-x)) / 2
                return (this->exp() + (-(*this)).exp()) / decimal(2ll);
            }

            constexpr decimal tanh() const {
                // tanh(x) = sinh(x) / cosh(x)
                decimal c = this->cosh();
                if (c.is_zero() || !c.is_finite()) {
                    return decimal::undefined();
                }
                return this->sinh() / c;
            }

            constexpr decimal asinh() const {
                // asinh(x) = ln(x + sqrt(x^2 + 1))
                decimal inside = (*this) * (*this) + decimal(1ll);
                if (!inside.is_finite() || inside.is_negative()) {
                    return decimal::undefined();
                }
                return ((*this) + inside.sqrt()).ln();
            }

            constexpr decimal acosh() const {
                // acosh(x) = ln(x + sqrt(x^2 - 1)), x >= 1
                const decimal& x = *this;
                decimal one(1ll);
                if (x < one) {
                    return decimal::undefined();
                }
                decimal inside = x * x - one;
                if (!inside.is_finite() || inside.is_negative()) {
                    return decimal::undefined();
                }
                return (x + inside.sqrt()).ln();
            }

            constexpr decimal atanh() const {
                // atanh(x) = 0.5 * ln((1 + x) / (1 - x)), |x| < 1
                const decimal& x = *this;
                decimal one(1ll);
                if (x >= one || x <= -one) {
                    return decimal::undefined();
                }
                return ((one + x) / (one - x)).ln() / decimal(2ll);
            }

            std::string to_string(std::size_t base = 10) const {
                if (!is_finite()) {
                    return _numerator.to_string(base);
                }

                integer qot, rem;
                integer::divmod(_numerator, settings.decimal_denominator(), qot, rem);

                std::string result = qot.to_string(base);

                // Only show decimal part if scale > 0
                std::size_t scale = settings.decimal_scale();
                if (scale > 0) {
                    // Scale the remainder to the correct number of digits
                    integer abs_rem = rem.abs();
                    integer scaled_rem = abs_rem * integer(base).pow(scale) / settings.decimal_denominator();
                    std::string decimal_string = scaled_rem.to_string(base);

                    // Pad with leading zeros if necessary
                    if (decimal_string.length() < scale) {
                        decimal_string.insert(0, scale - decimal_string.length(), '0');
                    }

                    // Remove trailing zeros
                    while (!decimal_string.empty() && decimal_string.back() == '0') {
                        decimal_string.pop_back();
                    }

                    if (!decimal_string.empty()) {
                        result += "." + decimal_string;
                    } else {
                        result += ".0";
                    }
                }

                return result;
            }

            std::string to_string(std::size_t base, bool sign) const {
                std::string result = to_string(base);
                if (sign && _numerator.is_positive()) {
                    result = "+" + result;
                }
                return result;
            }

        private:
            integer _numerator;

            void parse_string_value(std::string value, int base) {
                value = to_lower_case(value);

                if (value.size() > 1) {
                    auto e_pos = value.find('e');
                    if (e_pos != std::string::npos) {
                        if (value.find('.') == std::string::npos) {
                            value.insert(e_pos, ".0");
                        }
                    }
                    if (value.find('.') != std::string::npos) { 
                        set_decimal(value, base);
                    }
                    else if (value.find('/') != std::string::npos) {
                        set_rational(value, base);
                    }
                    else if (value[0] == '0' && std::isalpha(static_cast<unsigned char>(value[1]))) {
                        if (value[0] == '0' && value[1] == 'u') {
                            value[0] = ' ';
                            value[1] = ' ';
                            set_whole(value, base);
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
                            set_integer(value, base);
                        }
                    }
                    else {
                        set_integer(value, base);
                    }
                }
                else {
                    set_integer(value, base);
                }
            }

            void set_integer(std::string_view value, int base) {
                _numerator = integer(std::string(value), base);
                _numerator *= settings.decimal_denominator();
            }

            void set_decimal(std::string& value, int base) {
                trim_ws(value);
                to_lower_case(value);

                if (!value.empty()) {
                    long long exponent = get_sub_text_value(value, "e", base);

                    int scale = find_and_set_scale(value);

                    _numerator = integer(value);

                    if (scale < static_cast<int>(settings.decimal_scale())) {
                        _numerator = _numerator * (settings.decimal_denominator() / integer(static_cast<int64_t>(base)).pow(static_cast<std::size_t>(scale)));
                    }

                    set_decimal_exponent(exponent);
                }
            }

            void set_rational(std::string& value, int base) {
                long long den_text = get_sub_text_value(value, "/", base);
                integer den(den_text);

                if (den.is_zero()) {
                    _numerator = integer::undefined();
                    return;
                }
                
                trim_ws(value);

                std::string lead_val_str = "";

                auto found = value.find_last_of(' ');

                if (found != std::string::npos) {
                    lead_val_str = value.substr(0, found);
                    value.erase(0, found);
                }

                integer lead_value(lead_val_str, base);

                _numerator = integer(value, base);

                if (!lead_value.is_zero()) {
                    _numerator = _numerator + (lead_value * den);
                }

                _numerator *= settings.decimal_denominator();
                _numerator /= den;
            }

            void set_whole(std::string& value, int base) {
                _numerator = integer(value, base);
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

            constexpr void set_decimal_exponent(long long exponent) {
                if (exponent != 0) {
                    integer exp = integer(10ll).pow(exponent < 0 ? static_cast<std::size_t>(-exponent) : static_cast<std::size_t>(exponent));

                    if (exponent < 0) {
                        _numerator = _numerator / exp;
                    }
                    else if (exponent > 0) {
                        _numerator = _numerator * exp;
                    }
                }
            }
            
            long long get_sub_text_value(std::string& value, std::string del, int base) const {
                auto found = value.find(del);

                std::string sub_string_value = "";
                long long derived_value = 0;

                if (found != std::string::npos) {
                    if (value.begin() + found < value.end()) {
                        sub_string_value = value.substr(found + 1);
                    }
                    value.erase(found);

                    derived_value = string_to_number<long long>(sub_string_value, base).value_or(0);
                }

                return derived_value;
            }

            std::size_t find_and_set_scale(std::string& value) const {
                int scale = 0;

                auto found = value.find(".");

                if (found != std::string::npos) {
                    value.at(found) = ' ';

                    scale = static_cast<int>(value.size() - found) - 1;
                    if (scale < 1) {
                        scale = 0;
                    }
                }

                if (scale > static_cast<int>(settings.decimal_scale())) {
                    value.resize(value.size() - (static_cast<std::size_t>(scale) - settings.decimal_scale()));
                }

                return static_cast<std::size_t>(scale);
            }

            friend constexpr bool deg_or_rad(const decimal& a, decimal& x) {
                bool negate = false;

                if (settings.deg_or_rad() == std::string("degrees")) {
                    decimal ang = a % settings._360();
                    if (ang > settings._180()) {
                        ang -= settings._360();
                    } else if (ang <= -settings._180()) {
                        ang += settings._360();
                    }

                    if (ang.is_negative()) {
                        ang = -ang;
                    }
                    decimal ninety(90ll);
                    if (ang > ninety) {
                        ang = settings._180() - ang;
                        negate = true;
                    }

                    x = ang * settings.pi() / settings._180();
                } else {
                    decimal pi = settings.pi();
                    decimal two_pi = pi * decimal(2ll);
                    decimal ang = a % two_pi;
                    if (ang > pi) {
                        ang -= two_pi;
                    } else if (ang <= -pi) {
                        ang += two_pi;
                    }

                    if (ang.is_negative()) {
                        ang = -ang;
                    }
                    decimal half_pi = pi / decimal(2ll);
                    if (ang > half_pi) {
                        ang = pi - ang;
                        negate = true;
                    }

                    x = ang;
                }
                return negate;
            }

            void handle_rounding() {
                const integer& den = settings.decimal_denominator();
                if (den.is_zero()) {
                    return;
                }

                integer q, r;
                integer::divmod(_numerator, den, q, r);
                if (r.is_zero()) { 
                    _numerator = q * den; return; 
                }

                // Compare 2*r with den to detect ties and halves
                const integer twice = r + r;

                switch (settings._round_mode()) {
                    case numeric_settings::rounding_t::toward_zero: {
                        _numerator = q * den;
                        break;
                    }
                    case numeric_settings::rounding_t::away_from_zero: {
                        if (is_zero()) { 
                            _numerator = q * den; break; 
                        }
                        _numerator = (is_positive() ? (q + integer(1ll)) : (q - integer(1ll))) * den;
                        break;
                    }
                    case numeric_settings::rounding_t::ceil: {
                        _numerator = is_positive() ? (q + integer(1ll)) * den : q * den;
                        break;
                    }
                    case numeric_settings::rounding_t::floor: {
                        _numerator = is_negative() ? (q - integer(1ll)) * den : q * den;
                        break;
                    }
                    case numeric_settings::rounding_t::half_up: {
                        const bool up = (twice >= den);
                        if (!up) { 
                            _numerator = q * den; break; 
                        }
                        _numerator = (is_negative() ? (q - integer(1ll)) : (q + integer(1ll))) * den;
                        break;
                    }
                    case numeric_settings::rounding_t::half_down: {
                        const bool up = (twice > den);
                        if (!up) { 
                            _numerator = q * den; break; 
                        }
                        _numerator = (is_negative() ? (q - integer(1ll)) : (q + integer(1ll))) * den;
                        break;
                    }
                    case numeric_settings::rounding_t::half_even: {
                        const bool tie = (twice == den);
                        if (tie) {
                            const bool q_odd = q.is_odd();
                            if (!q_odd) { 
                                _numerator = q * den; break; 
                            }
                            _numerator = (is_negative() ? (q - integer(1ll)) : (q + integer(1ll))) * den;
                        } else {
                            const bool up = (twice > den);
                            if (!up) { 
                                _numerator = q * den; break; 
                            }
                            _numerator = (is_negative() ? (q - integer(1ll)) : (q + integer(1ll))) * den;
                        }
                        break;
                    }
                    case numeric_settings::rounding_t::half_odd: {
                        const bool tie = (twice == den);
                        if (tie) {
                            const bool q_even = q.is_even();
                            if (!q_even) { 
                                _numerator = q * den; break; 
                            }
                            _numerator = (is_negative() ? (q - integer(1ll)) : (q + integer(1ll))) * den;
                        } else {
                            const bool up = (twice > den);
                            if (!up) { 
                                _numerator = q * den; break; 
                            }
                            _numerator = (is_negative() ? (q - integer(1ll)) : (q + integer(1ll))) * den;
                        }
                        break;
                    }
                    default:
                        _numerator = q * den;
                        break;
                }
            }
        };

        class math_env {
        public:
            ~math_env() = default;

            std::string rounding_mode() const { return decimal::settings.rounding_mode(); }

            void rounding_mode(std::string_view mode) const { decimal::settings.rounding_mode(std::string(mode)); }

            std::size_t decimal_scale() const { return decimal::settings.decimal_scale(); }

            void decimal_scale(const int& scale) const { decimal::settings.decimal_scale(scale); }


            std::string_view deg_or_rad() const { return decimal::settings.deg_or_rad(); }

            void deg_or_rad(std::string_view mode) const { decimal::settings.deg_or_rad() = std::string(mode); }

            const decimal& e()    const { return decimal::numeric_settings::e(); }
            const decimal& pi()   const { return decimal::numeric_settings::pi(); }
            const decimal& ln2()  const { return decimal::numeric_settings::ln2(); }
            const decimal& _360() const { return decimal::numeric_settings::_360(); }
            const decimal& _180() const { return decimal::numeric_settings::_180(); }
            const decimal& _90()  const { return decimal::numeric_settings::_90(); }
            const decimal& _45()  const { return decimal::numeric_settings::_45(); }

            integer decimal_denominator() const { return decimal::numeric_settings::decimal_denominator(); }

            static constexpr std::size_t min_scale()  { return decimal::settings.min_scale(); }
            static constexpr std::size_t max_scale()  { return decimal::settings.max_scale(); }
            static constexpr std::size_t max_power()  { return decimal::settings.max_power(); }
            static constexpr std::size_t max_root()   { return decimal::settings.max_root(); }
        };


    bool decimal_tests() {

            using namespace Oliver;
            using namespace MPA;

            std::string num = "11";

            // std::cout << "Enter a number string: ";
            // std::cin >> num; // Reads an integer

            math_env env;
            env.deg_or_rad("rad");
            env.decimal_scale(16);

            fmt::println("math_env: decimal scale = {}", env.decimal_scale());
            fmt::println("math_env: deg_or_rad    = {}", env.deg_or_rad());

            env.deg_or_rad("degrees");
            fmt::println("math_env: deg_or_rad    = {}", env.deg_or_rad());

            std::string a_str = "19";
            std::string b_str = num;
            std::string c_str = "0";

            auto a = scalar<decimal>(decimal{a_str, 16});
            auto b = scalar<decimal>(decimal{b_str});
            auto c = scalar<decimal>(decimal{c_str});

            fmt::println("decimal scale: {}", decimal::settings.decimal_scale());
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

            fmt::println("a = {}", a[0].to_string());  
            fmt::println("b = {}", b[0].to_string());  
            fmt::println("c = {}", c[0].to_string()); 
            fmt::println("");

            fmt::println("a + b = {}", (a + b)[0].to_string()); 
            fmt::println("a - b = {}", (a - b)[0].to_string()); 
            fmt::println("a * b = {}", (a * b)[0].to_string()); 
            fmt::println("a / b = {}", (a / b)[0].to_string()); 
            fmt::println("a % b = {}", (a[0] % b[0]).to_string());  
            fmt::println("");

            fmt::println("a.inverse() = {}", a[0].inverse().to_string());  
            fmt::println("b.inverse() = {}", b[0].inverse().to_string());  
            fmt::println("c.inverse() = {}", c[0].inverse().to_string());  
            fmt::println("");

            fmt::println("a = {}", a[0].to_string()); 
            fmt::println("b = {}", b[0].to_string());  
            fmt::println("c = {}", c[0].to_string());  
            fmt::println("");

            fmt::println("a.ln() = {}", a[0].ln().to_string());
            fmt::println("b.ln() = {}", b[0].ln().to_string());
            fmt::println("c.ln() = {}", c[0].ln().to_string());
            fmt::println("");



            // for (std::size_t i = 1; i < 23; ++i) {
            //     decimal n = decimal(static_cast<uint64_t>(i));
            //     fmt::println("{} % {}.ln() = {}", i, i, (n % n.ln()).to_string());
            // }
            // fmt::println("");

            return true;
        }
    }  // end namespace MPA
}  // end namespace Oliver
