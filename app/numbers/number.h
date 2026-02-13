#pragma once

#include <string>
#include <compare>      // std::partial_ordering
#include <utility>      // std::swap
#include <algorithm>    // std::min
#include <type_traits>  // std::is_signed
#include <cstdint>      // fixed-width integer types
#include <cctype>       // std::isalpha
#include <mutex>

#include "decimal.h"
#include "numeric_settings.h"
#include "../text_support.h"

namespace Oliver {
    namespace MPA {

        class number {
            decimal _real;
            decimal _imag;

        public:

            // Constructors
            constexpr number() : _real(), _imag() {}
            constexpr number(long long value) : _real(value), _imag(0ll) {}
            constexpr number(long long r_value, long long i_value) : _real(r_value), _imag(i_value) {}

            number(const std::string& r_value, 
                   const std::string& i_value,
                   int base = 10) 
                : _real(r_value, base), _imag(i_value, base) 
                {}
            number(const std::string& value, int base = 10) : _real(), _imag(){
                parse_from_string(value, base);
            }

            static constexpr number from_polar(const decimal& r, const decimal& theta) {
                // z = r * (cos(theta) + i*sin(theta))
                return number(r * theta.cos(), r * theta.sin());
            }

            constexpr ~number()                            = default;
            constexpr number(number&& obj)                 = default;
            constexpr number(const number& obj)            = default;
            constexpr number& operator=(number&& obj)      = default;
            constexpr number& operator=(const number& obj) = default;

            friend constexpr void swap(number& first, number& second) noexcept {
                std::swap(first._real, second._real);
                std::swap(first._imag, second._imag);
            }

            constexpr explicit operator bool() const {
                return static_cast<bool>(_real) || static_cast<bool>(_imag);
            }

            constexpr bool operator==(const number& other) const {
                return _real == other._real && _imag == other._imag;
            }

            constexpr std::partial_ordering operator<=>(const number& other) const {
                if (_imag.is_zero() && other._imag.is_zero()) {
                    return _real <=> other._real;
                }
                if (_real.is_zero() && other._real.is_zero()) {
                    return _imag <=> other._imag;
                }
                return abs()._real <=> other.abs()._real;
            }

            [[nodiscard]] constexpr bool is_complex()  const { return _real.is_zero()     && !_imag.is_zero(); }
            [[nodiscard]] constexpr bool is_decimal()  const { return _real.is_decimal()  ||  _imag.is_decimal(); }
            [[nodiscard]] constexpr bool is_integer()  const { return _real.is_integer()  &&  _imag.is_integer(); }
            [[nodiscard]] constexpr bool is_negative() const { return _real.is_negative() ||  _imag.is_negative(); }
            [[nodiscard]] constexpr bool is_positive() const { return _real.is_positive() ||  _imag.is_positive(); }
            [[nodiscard]] constexpr bool is_zero()     const { return _real.is_zero()     &&  _imag.is_zero(); }
            [[nodiscard]] constexpr bool is_nan()      const { return _real.is_nan()      ||  _imag.is_nan(); }
            [[nodiscard]] constexpr bool is_undef()    const { return _real.is_undef()    ||  _imag.is_undef(); }
            [[nodiscard]] constexpr bool is_neg_inf()  const { return _real.is_neg_inf()  ||  _imag.is_neg_inf(); }
            [[nodiscard]] constexpr bool is_pos_inf()  const { return _real.is_pos_inf()  ||  _imag.is_pos_inf(); }
            [[nodiscard]] constexpr bool is_finite()   const { return _real.is_finite()   ||  _imag.is_finite(); }
            [[nodiscard]] constexpr bool is_infinite() const { return _real.is_infinite() ||  _imag.is_infinite(); }
            [[nodiscard]] constexpr bool is_even()     const { return _real.is_even()     ||  _imag.is_even(); }
            [[nodiscard]] constexpr bool is_odd()      const { return _real.is_odd()      ||  _imag.is_odd(); }

            constexpr number& operator&=(const number& other) {
                if (real_xor_imag()) {
                    _real &= other._real;
                    _imag &= other._imag;
                }
                else {
                    *this = abs();
                    *this &= other.abs();
                }
                return *this;
            }

            constexpr number& operator|=(const number& other) {
                if (real_xor_imag()) {
                    _real |= other._real;
                    _imag |= other._imag;
                }
                else {
                    *this = abs();
                    *this |= other.abs();
                }
                return *this;
            }

            constexpr number& operator^=(const number& other) {
                if (real_xor_imag()) {
                    _real ^= other._real;
                    _imag ^= other._imag;
                }
                else {
                    *this = abs();
                    *this ^= other.abs();
                }
                return *this;
            }

            constexpr number& operator<<=(std::size_t shift) {
                if (real_xor_imag()) {
                    _real <<= shift;
                    _imag <<= shift;
                }
                else {
                    *this = abs();
                    *this <<= shift;
                }
                return *this;
            }

            constexpr number& operator>>=(std::size_t shift) {
                if (real_xor_imag()) {
                    _real >>= shift;
                    _imag >>= shift;
                }
                else {
                    *this = abs();
                    *this >>= shift;
                }
                return *this;
            }

            constexpr number& operator+=(const number& other) {
                _real += other._real;
                _imag += other._imag;
                return *this;
            }

            constexpr number& operator-=(const number& other) {
                _real -= other._real;
                _imag -= other._imag;
                return *this;
            }

            constexpr number& operator*=(const number& other) {
                if (real_xor_imag()) {
                    _real *= other._real;
                    _imag *= other._imag;
                }
                else {
                    decimal r = _real * other._real - _imag * other._imag;
                    decimal i = _real * other._imag + _imag * other._real;
                    _real = r;
                    _imag = i;
                }
                return *this;
            }
            
            constexpr number& operator/=(const number& other) {
                if (other._imag.is_zero()) {
                    if (other._real.is_zero()) {
                        _real = decimal::undefined();
                        _imag = decimal(0ll);
                    } else {
                        _real /= other._real;
                        _imag = decimal(0ll);
                    }
                } else if (other._real.is_zero()) {
                    if (other._imag.is_zero()) {
                        _real = decimal::undefined();
                        _imag = decimal(0ll);
                    } else {
                        _imag /= other._imag;
                        _real = decimal(0ll);
                    }
                } else {
                    decimal denom = other._real * other._real + other._imag * other._imag;
                    if (denom.is_zero()) {
                        _real = decimal(0ll);
                        if (is_negative()) 
                            _imag = decimal::neg_inf();
                        else 
                            _imag = decimal::pos_inf();
                    } else {
                        decimal r = (_real * other._real + _imag * other._imag) / denom;
                        decimal i = (_imag * other._real - _real * other._imag) / denom;
                        _real = r;
                        _imag = i;
                    }
                }
                return *this;
            }
            
            constexpr number& operator%=(const number& other) {
                if (other._imag.is_zero()) {
                    if (other._real.is_zero()) {
                        _real = decimal::undefined();
                        _imag = decimal(0ll);
                    } else {
                        _real %= other._real;
                        _imag = decimal(0ll);
                    }
                } else if (other._real.is_zero()) {
                    if (other._imag.is_zero()) {
                        _real = decimal::undefined();
                        _imag = decimal(0ll);
                    } else {
                        _imag %= other._imag;
                        _real = decimal(0ll);
                    }
                } else {
                    decimal denom = other._real * other._real + other._imag * other._imag;
                    if (denom.is_zero()) {
                        _real = decimal(0ll);
                        if (is_negative()) 
                            _imag = decimal::neg_inf();
                        else 
                            _imag = decimal::pos_inf();
                    } else {
                        *this = abs();
                        *this %= other.abs();
                    }
                }
                return *this;
            }

            constexpr number operator+() const {
                return *this;
            }

            constexpr number operator-() const {
                number result;
                result._real = -_real;
                result._imag = -_imag;
                return result;
            }

            constexpr number operator~() const {
                number result;
                result._real = ~_real;
                result._imag = ~_imag;
                return result;
            }

            constexpr number inverse() const {
                return number(1ll) / *this;
            }

            constexpr number operator&(const number& b) const {
                number a(*this);
                a &= b;
                return a;
            }

            constexpr number operator|(const number& b) const {
                number a(*this);
                a |= b;
                return a;
            }

            constexpr number operator^(const number& b) const {
                number a(*this);
                a ^= b;
                return a;
            }

            constexpr number operator<<(std::size_t shift) const {
                number a(*this);
                a <<= shift;
                return a;
            }

            constexpr number operator>>(std::size_t shift) const {
                number a(*this);
                a >>= shift;
                return a;
            }

            constexpr number operator+(const number& b) const {
                number a(*this);
                a += b;
                return a;
            }

            constexpr number operator-(const number& b) const {
                number a(*this);
                a -= b;
                return a;
            }

            constexpr number operator*(const number& b) const {
                number a(*this);
                a *= b;
                return a;
            }

            constexpr number operator/(const number& b) const {
                number a(*this);
                a /= b;
                return a;
            }

            constexpr number operator%(const number& b) const {
                number a(*this);
                a %= b;
                return a;
            }

            friend constexpr void div_mod(const number& a, const number& b, number& quotient, number& remainder) {
                quotient  = a / b;
                remainder = a % b;
            }

            friend constexpr number rem(const number& a) {
                number result;
                result._real = rem(a._real);
                result._imag = rem(a._imag);
                return result;
            }

            friend constexpr number operator&(number& a, number&& b) {
                a &= b;
                return std::move(a);
            }

            friend constexpr number operator|(number& a, number&& b) {
                a |= b;
                return std::move(a);
            }

            friend constexpr number operator^(number& a, number&& b) {
                a ^= b;
                return std::move(a);
            }

            friend constexpr number operator+(number& a, number&& b) {
                a += b;
                return std::move(a);
            }

            friend constexpr number operator-(number& a, number&& b) {
                a -= b;
                return std::move(a);
            }

            friend constexpr number operator*(number& a, number&& b) {
                a *= b;
                return std::move(a);
            }

            friend constexpr number operator/(number& a, number&& b) {
                a /= b;
                return std::move(a);
            }

            friend constexpr number operator%(number& a, number&& b) {
                a %= b;
                return std::move(a);
            }

            constexpr number& operator++() {
                ++_real;
                ++_imag;
                return *this;
            }

            constexpr number  operator++(int) {
                number a(*this);
                ++(*this);
                return a;
            }

            constexpr number& operator--() {
                --_real;
                --_imag;
                return *this;
            }

            constexpr number  operator--(int) {
                number a(*this);
                --(*this);
                return a;
            }

            constexpr number conjugate() const { return number(_real, -_imag); }

            constexpr number abs() const { 
                return number((_real * _real + _imag * _imag).sqrt(), decimal(0ll)); 
            }

            constexpr number ceil(const number&a) const {
                number result;
                result._real = a._real.ceil();
                result._imag = a._imag.ceil();
                return result;
            }

            constexpr number floor(const number&a) const {
                number result;
                result._real = a._real.floor();
                result._imag = a._imag.floor();
                return result;
            }

            constexpr number gcd(const number& b) const {
                number result;
                if (is_complex() || b.is_complex()) {
                    result._real = decimal(0ll);
                    result._imag = decimal(0ll);
                }
                else {
                    result._real = _real.gcd(b._real);
                    result._imag = _imag.gcd(b._imag);
                }
                return result;
            }

            constexpr number square() const {
                return *this * *this;
            }

            constexpr number cube() const {
                return *this * *this * *this;
            }

            constexpr number pow(const number& exp) const {
                if (is_zero() && exp.is_zero())
                    return number(1ll); // 0^0 = 1 by convention
                if (is_zero())
                    return number(0ll);
                if (exp.is_zero())
                    return number(1ll);

                if (is_complex()) {
                    // z = this, w = exp
                    // result = exp(w * ln(z))
                    number z = ln();         // You need to implement a complex ln() if not present
                    number w = exp * z;
                    number result = w.exp();      // You need to implement a complex exp() if not present
                    return result;
                }

                number result;
                result._real = _real.pow(exp._real);
                result._imag = _imag.pow(exp._imag);
                return result;
            }
            // Square root
            constexpr number sqrt() const {
                if (is_complex()) {
                    // sqrt(z) = sqrt((|z| + Re(z))/2) + sign(Im(z)) * i * sqrt((|z| - Re(z))/2)
                    decimal modulus = abs()._real;
                    decimal real_part = ((_real + modulus) / decimal(2ll)).sqrt();
                    decimal imag_part = ((_real - modulus) / decimal(2ll)).abs().sqrt();
                    if (_imag.is_negative())
                        imag_part = -imag_part;
                    return number(real_part, imag_part);
                }
                return number(_real.sqrt(), _imag.sqrt());
            }

            // Cube root
            constexpr number cbrt() const {
                if (is_complex()) {
                    // cbrt(z) = cbrt(r) * (cos(theta/3) + i*sin(theta/3)), where z = r*e^{i*theta}
                    decimal modulus = abs()._real;
                    decimal theta = arg();
                    decimal cbrt_mod = modulus.cbrt();
                    decimal real_part = cbrt_mod * (theta / decimal(3ll)).cos();
                    decimal imag_part = cbrt_mod * (theta / decimal(3ll)).sin();
                    return number(real_part, imag_part);
                }
                return number(_real.cbrt(), _imag.cbrt());
            }

            // General root
            constexpr number root(const decimal& exp) const {
                if (is_complex()) {
                    // z^(1/exp) = exp(ln(z)/exp)
                    return (ln() / number(exp, decimal(0ll))).exp();
                }
                return number(_real.root(exp), _imag.root(exp));
            }

            // Hypotenuse (2 arguments)
            constexpr number hypot(const number& b) const {
                if (is_complex() || b.is_complex()) {
                    // For complex numbers, hypot is not well-defined; return abs() of both and use decimal hypot
                    decimal h = abs()._real.hypot(b.abs()._real);
                    return number(h, decimal(0ll));
                }
                return number(_real.hypot(b._real), _imag.hypot(b._imag));
            }

            // Hypotenuse (3 arguments)
            constexpr number hypot(const number& b, const number& c) const {
                if (is_complex() || b.is_complex() || c.is_complex()) {
                    decimal h = abs()._real.hypot(b.abs()._real, c.abs()._real);
                    return number(h, decimal(0ll));
                }
                return number(_real.hypot(b._real, c._real), _imag.hypot(b._imag, c._imag));
            }

            constexpr number ln() const {
                if (is_complex()) {
                    // ln(z) = ln|z| + i*arg(z)
                    decimal modulus = (_real * _real + _imag * _imag).sqrt();
                    decimal real_part = modulus.ln();
                    decimal imag_part = arg(); // already implemented as atan2(imag, real)
                    return number(real_part, imag_part);
                }
                number result;
                result._real = _real.ln();
                result._imag = _imag.ln();
                return result;
            }

            constexpr number exp() const {
                if (is_complex()) {
                    // exp(a + bi) = exp(a) * (cos(b) + i*sin(b))
                    decimal exp_real = _real.exp();
                    decimal cos_imag = _imag.cos();
                    decimal sin_imag = _imag.sin();
                    return number(exp_real * cos_imag, exp_real * sin_imag);
                }
                number result;
                result._real = _real.exp();
                result._imag = _imag.exp();
                return result;
            }

            constexpr number log(const number& b) const {
                if (is_complex() || b.is_complex()) {
                    // log_b(z) = ln(z) / ln(b)
                    return ln() / b.ln();
                }
                if (!is_zero() && is_positive() && !b.is_zero() && b.is_positive()) {
                    return number(_real.log(b._real), decimal(0ll));
                }
                return number(decimal::undefined());
            }

            // Base-2 logarithm
            constexpr number log2() const {
                if (is_complex()) {
                    return ln() / number(decimal(2ll), decimal(0ll)).ln();
                }
                if (!is_zero() && is_positive()) {
                    return number(_real.log2(), decimal(0ll));
                }
                return number(decimal::undefined());
            }

            // Base-10 logarithm
            constexpr number log10() const {
                if (is_complex()) {
                    return ln() / number(decimal(10ll), decimal(0ll)).ln();
                }
                if (!is_zero() && is_positive()) {
                    return number(_real.log10(), decimal(0ll));
                }
                return number(decimal::undefined());
            }

            // Trigonometric functions

            constexpr number sin() const {
                if (is_complex()) {
                    // sin(a + bi) = sin(a)cosh(b) + i*cos(a)sinh(b)
                    decimal a = _real;
                    decimal b = _imag;
                    decimal real_part = a.sin() * b.cosh();
                    decimal imag_part = a.cos() * b.sinh();
                    return number(real_part, imag_part);
                }
                return number(_real.sin(), _imag.sin());
            }

            constexpr number cos() const {
                if (is_complex()) {
                    // cos(a + bi) = cos(a)cosh(b) - i*sin(a)sinh(b)
                    decimal a = _real;
                    decimal b = _imag;
                    decimal real_part = a.cos() * b.cosh();
                    decimal imag_part = -a.sin() * b.sinh();
                    return number(real_part, imag_part);
                }
                return number(_real.cos(), _imag.cos());
            }

            constexpr number tan() const {
                if (is_complex()) {
                    // tan(a + bi) = (sin(2a) + i*sinh(2b)) / (cos(2a) + cosh(2b))
                    decimal a = _real;
                    decimal b = _imag;
                    decimal two_a = a + a;
                    decimal two_b = b + b;
                    decimal denom = two_a.cos() + two_b.cosh();
                    decimal real_part = two_a.sin() / denom;
                    decimal imag_part = two_b.sinh() / denom;
                    return number(real_part, imag_part);
                }
                return number(_real.tan(), _imag.tan());
            }

            constexpr number asin() const {
                if (is_complex()) {
                    // asin(z) = -i * ln(iz + sqrt(1 - z^2))
                    number iz(-_imag, _real); // i*z = -imag + i*real
                    number one(1ll);
                    number sqrt_term = (one - (*this) * (*this)).sqrt();
                    number ln_arg = iz + sqrt_term;
                    number result = (-number(0ll, 1ll)) * ln_arg.ln();
                    return result;
                }
                return number(_real.asin(), _imag.asin());
            }

            constexpr number acos() const {
                if (is_complex()) {
                    // acos(z) = -i * ln(z + i*sqrt(1 - z^2))
                    number one(1ll);
                    number sqrt_term = (one - (*this) * (*this)).sqrt();
                    number ln_arg = (*this) + number(0ll, 1ll) * sqrt_term;
                    number result = (-number(0ll, 1ll)) * ln_arg.ln();
                    return result;
                }
                return number(_real.acos(), _imag.acos());
            }

            constexpr number atan() const {
                if (is_complex()) {
                    // atan(z) = (i/2) * [ln(1 - i*z) - ln(1 + i*z)]
                    number iz_neg = number(0ll, -1ll) * (*this);
                    number iz_pos = number(0ll, 1ll) * (*this);
                    number one(1ll);
                    number ln1 = (one - iz_pos).ln();
                    number ln2 = (one + iz_pos).ln();
                    number result = number(decimal(0ll), decimal("0.5")) * (ln1 - ln2);
                    return result;
                }
                return number(_real.atan(), _imag.atan());
            }

            // Argument (angle)
            decimal arg() const {
                // atan2(_imag, _real)
                if (_real.is_zero() && _imag.is_zero()) return decimal(0ll);
                // Use std::atan2 for double, then convert to decimal
                double r = std::stod(_real.to_string());
                double i = std::stod(_imag.to_string());
                return decimal(static_cast<long double>(std::atan2(i, r)));
            }

            // String representation
            std::string to_string(std::size_t base = 10) const {
                std::string r = _real.to_string(base);
                std::string i = _imag.to_string(base);
                if (_imag.is_zero()) return r;
                if (_real.is_zero()) return i + "i";
                // if (_imag.is_negative()) return r + " - " + (-_imag).to_string(base) + "i";
                return r + " + " + i + "i";
            }

            constexpr bool real_xor_imag() const {
                return (_real.is_zero() != _imag.is_zero()) && !(_real.is_zero() && _imag.is_zero());
            }

            constexpr const decimal& real() const { return _real; }
            constexpr const decimal& imag() const { return _imag; }
            constexpr void set_real(const std::string& value) { _real = decimal(value); }
            constexpr void set_imag(const std::string& value) { _imag = decimal(value); }

        private:
            constexpr number(decimal r) : _real(r), _imag(decimal(0ll)) {}
            constexpr number(decimal r, decimal i) : _real(r), _imag(i) {}

            void parse_from_string(std::string value, int base) {
                value = trim_ws(value);

                value = to_white_space(value, ",()");
                std::vector<std::string> tokens = split(value, " ");

                decimal real{};
                decimal imag{};

                bool neg = false;

                std::size_t limit = tokens.size();
                for (std::size_t i = 0; i < limit; ++i) {
                    std::string token = tokens[i];

                    if (token == "-") {
                        neg = true;
                        continue;
                    } else if (token == "+") {
                        neg = false;
                        continue;
                    }

                    if (token.back() == 'i' || token.back() == 'j') {
                        if (token == "i" || token == "j") {
                            imag += neg ? decimal(-1ll) :  decimal(1ll);
                        } else if (token == "-i" || token == "-j") {
                            imag += neg ? decimal(-1ll) :  decimal(1ll);
                        } else {
                            token.pop_back();
                            imag += neg ? -decimal(token, base) : decimal(token, base);
                        }
                    } else {
                        real += neg ? -decimal(token, base) : decimal(token, base);
                        fmt::println("value = {}", decimal(token, base).to_string());
                    }
                }
                _real = real;
                _imag = imag;
            }
        };
    }  // namespace MPA
}  // namespace Oliver
