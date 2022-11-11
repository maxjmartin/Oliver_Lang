
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

        whole_number::whole_number() : _reg() {
        }

        whole_number::whole_number(word_type value) : _reg(1, value) {
        }

        whole_number::whole_number(str_type value, word_type base) : _reg() {

            if (base == 10) {  // Parse a decimal number.

                for (const auto n : value) {

                    // Convert each digit to a value.
                    stream_type stream;

                    word_type x = base;

                    if (!std::isspace(n)) {
                        stream << std::dec << n;
                        stream >> x;

                        // Shift the value by the base, and add the digit.
                        if (x >= 0 && x < base) {

                            operator*=(base);
                            operator+=(x);
                        }
                    }
                }
            }

            if (base == 8 || base == 16) {  // Parse an octal or hexidecimal number.

                for (const auto n : value) {

                    // Convert each digit to a value.
                    stream_type stream;

                    stream << std::hex << n;

                    word_type x;
                    stream >> x;

                    // Shift the value by the base, and add the digit.
                    operator*=(base);
                    operator+=(x);
                }

                return;
            }

            if (base == 2) {  // Parse a binary number.

                for (const auto n : value) {

                    operator<<=(1);

                    if (n == '1') {
                        operator+=(1);
                    }
                }

                return;
            }
        }

        whole_number::whole_number(const reg_type& reg) : _reg(reg) {
        }

        whole_number::~whole_number() {
        }

        bool_type whole_number::is() const {
            return _reg.is();
        }

        bool_type whole_number::is_odd() const {
            return _reg.last_reg() & 1;
        }

        bool_type whole_number::is_even() const {
            return !is_odd();
        }

        bool_type whole_number::operator==(const whole_number& b) const {
            return compare(b) == 0;
        }

        bool_type whole_number::operator!=(const whole_number& b) const {
            return compare(b) != 0;
        }

        bool_type whole_number::operator<(const whole_number& b) const {
            return compare(b) < 0;
        }

        bool_type whole_number::operator>(const whole_number& b) const {
            return compare(b) > 0;
        }

        bool_type whole_number::operator<=(const whole_number& b) const {
            return compare(b) <= 0;
        }

        bool_type whole_number::operator>=(const whole_number& b) const {
            return compare(b) >= 0;
        }

        real_type whole_number::compare(const whole_number& other) const {
            return _reg.compare(other._reg);
        }

        whole_number& whole_number::operator&=(const whole_number& other) {
            _reg &= other._reg;

            trim();

            return *this;
        }

        whole_number& whole_number::operator|=(const whole_number& other) {
            _reg |= other._reg;

            trim();

            return *this;
        }

        whole_number& whole_number::operator^=(const whole_number& other) {
            _reg ^= other._reg;

            trim();

            return *this;
        }

        whole_number& whole_number::operator<<=(size_type index) {
            _reg <<= index;

            trim();

            return *this;
        }

        whole_number& whole_number::operator>>=(size_type index) {
            _reg >>= index;

            trim();

            return *this;
        }

        whole_number whole_number::bin_comp() const {

            whole_number n;

            n._reg = _reg.bin_comp();

            return n;
        }

        whole_number whole_number::operator&(const whole_number& b) const {

            whole_number a = *this;

            a &= b;

            return a;
        }

        whole_number whole_number::operator|(const whole_number& b) const {

            whole_number a = *this;

            a |= b;

            return a;
        }

        whole_number whole_number::operator^(const whole_number& b) const {

            whole_number a = *this;

            a ^= b;

            return a;
        }

        whole_number whole_number::operator~() const {

            whole_number a;

            a._reg = ~_reg;

            return a;
        }

        whole_number whole_number::operator<<(size_type index) const {

            whole_number a = *this;

            a <<= index;

            return a;
        }

        whole_number whole_number::operator>>(size_type index) const {

            whole_number a = *this;

            a >>= index;

            return a;
        }

        whole_number& whole_number::operator+=(const whole_number& other) {

            size_type limit = _reg.size_regs() > other._reg.size_regs() ? _reg.size_regs() : other._reg.size_regs();

            d_word_type n = 0;

            for (size_type i = 0; i < limit; i += 1) {

                n = n + _reg.at_reg(i) + other._reg.at_reg(i);

                _reg.at_reg(i) = static_cast<word_type>(n);

                n >>= _reg.BITS;
            }

            if (n != 0) {

                _reg.at_reg(limit) = static_cast<word_type>(n);
            }

            trim();

            return *this;
        }

        whole_number& whole_number::operator-=(const whole_number& other) {

            if (other > *this) {
                *this = whole_number();
                return *this;
            }

            size_type limit = _reg.size_regs() > other._reg.size_regs() ? _reg.size_regs() : other._reg.size_regs();

            d_word_type n = 0;

            for (size_type i = 0; i < limit; i += 1) {

                n = n + _reg.at_reg(i) - other._reg.at_reg(i);

                _reg.at_reg(i) = static_cast<word_type>(n);

                n = ((n >> _reg.BITS) ? -1 : 0);
            }

            trim();

            return *this;
        }

        whole_number& whole_number::operator*=(const whole_number& other) {

            *this = *this * other;

            return *this;
        }

        whole_number& whole_number::operator/=(const whole_number& other) {

            *this = *this / other;

            return *this;
        }

        whole_number& whole_number::operator%=(const whole_number& other) {

            *this = *this % other;

            return *this;
        }

        whole_number whole_number::operator+(const whole_number& b) const {

            whole_number a = *this;

            a += b;

            return a;
        }

        whole_number whole_number::operator-(const whole_number& b) const {

            whole_number a = *this;

            a -= b;

            return a;
        }

        whole_number whole_number::operator*(const whole_number& b) const {

            size_type size_a = _reg.size_regs();
            size_type size_b = b._reg.size_regs();

            reg_type r((size_a + size_b + 1), 0);

            for (size_type j = 0; j < size_b; j += 1) {

                d_word_type n = 0;

                for (size_type i = 0; i < size_a; i += 1) {

                    size_type k = i + j;

                    n += static_cast<d_word_type>(_reg.at_reg(i)) * b._reg.at_reg(j) + r.at_reg(k);

                    r.at_reg(k) = static_cast<word_type>(n);

                    n >>= _reg.BITS;
                }
                r.at_reg(j + size_a) = static_cast<word_type>(n);
            }

            r.trim();

            return r;
        }

        whole_number whole_number::operator/(const whole_number& b) const {

            whole_number q;
            whole_number r;

            div_rem(b, q, r);

            return q;
        }

        whole_number whole_number::operator%(const whole_number& b) const {

            whole_number q;
            whole_number r;

            div_rem(b, q, r);

            return r;
        }

        whole_number& whole_number::operator++() {

            ++_reg;

            trim();

            return *this;
        }

        whole_number whole_number::operator++(int) {

            whole_number a(*this);

            operator++();

            return a;
        }

        whole_number& whole_number::operator--() {

            --_reg;

            trim();

            return *this;
        }

        whole_number whole_number::operator--(int) {

            whole_number a(*this);

            operator--();

            return a;
        }

        void whole_number::div_rem(const whole_number& other, whole_number& qot, whole_number& rem) const {
            qot = whole_number();
            rem = whole_number();

            if (!other.is()) {
                // Division by zero.
                return;
            }

            if (other > *this) {
                // Division by a greater value.
                qot = whole_number();
                rem = *this;
                return;
            }

            if (*this > other) {

                if (other._reg.size_regs() == 1) {

                    divide_remainder(other._reg.at_reg(0), qot, rem, 0);

                    return;
                }

                int_type stop = static_cast<int_type>(other._reg.size_regs() - 1);

                auto d = other._reg.at_reg(stop);

                for (int_type i = stop - 1; i >= 0; i -= 1) {
                    // Add one to 'd' if any other digits are defined.
                    if (other._reg.at_reg(i) != 0) {
                        d += 1;
                        break;
                    }
                }

                // Perform long division.
                auto n = *this;

                auto q = whole_number();

                auto guard = whole_number();

                while (n >= other && n != guard) {
                    
                    n.divide_remainder(d, q, rem, stop);

                    qot += q;

                    guard = n;

                    n -= (other * q);

                    q = whole_number();
                }

                q = other * qot;

                while (q < *this) {
                    qot += whole_number(1);
                    q += other;
                }

                while (q > *this) {
                    qot -= whole_number(1);
                    q -= other;
                }

                rem = *this - q;
                return;
            }

            // Return  division by two equal values.
            qot = whole_number(1);
            rem = whole_number();
        }

        whole_number whole_number::log(whole_number b) const {

            if (!is() || !b.is()) {
                return whole_number();
            }

            return log2() / b.log2();
        }

        whole_number whole_number::log2() const {

            if (!is()) {
                return whole_number();
            }

            int_type n = static_cast<int_type>(_reg.lead_bit() - 1);

            return whole_number(n);
        }

        whole_number whole_number::log10() const {

            return log(10);
        }

        whole_number whole_number::gcd(whole_number other) const {

            auto comp = compare(other);

            auto a = comp > 0.0 ? *this : other;
            auto b = comp < 0.0 ? *this : other;

            while (b.is()) {

                whole_number x = a % b;

                a = b;
                b = x;
            }

            return a;
        }

        whole_number whole_number::lcm(const whole_number& other) const {

            return (*this / gcd(other)) * other;
        }

        whole_number whole_number::pow(whole_number b) const {

            auto a = *this;

            auto one = whole_number(1);
            auto res = one;

            while (b.is()) {

                if (b.is_odd()) {

                    res *= a;
                }

                b >>= 1;

                if (b.is()) {
                    a *= a;
                }
            }

            return res;
        }

        whole_number whole_number::powmod(whole_number b, whole_number c) const {
            
            auto a = *this % c;

            auto one = whole_number(1);
            auto res = one;

            while (b.is()) {

                if (b.is_odd()) {

                    res *= a;
                    res %= c;
                }

                b >>= 1;

                if (b.is()) {
                    a *= a;
                    a %= c;
                }
            }

            return res;
        }

        whole_number whole_number::sqrt() const {
            return root(whole_number(2));
        }

        whole_number whole_number::root(const whole_number& b) const {

            whole_number low = 0;
            whole_number high = 1;

            while (high.pow(b) <= *this) {
                low = high;
                high *= 2;
            }

            while (low != high - 1) {

                whole_number step = (high - low) / 2;

                whole_number candidate = low + step;

                auto value = candidate.pow(b);

                if (value == *this) {
                    return candidate;
                }
                if (value < *this) {
                    low = candidate;
                }
                else {
                    high = candidate;
                }
            }

            return low;
        }

        whole_number whole_number::hypot(const whole_number& b) const {

            whole_number two(2);

            return (pow(two) + b.pow(two)).sqrt();
        }

        whole_number whole_number::hypot(const whole_number& b, const whole_number& c) const {

            whole_number two(2);

            return (pow(two) + b.pow(two) + c.pow(two)).sqrt();
        }

        str_type whole_number::to_string() const {

            return to_string(10);
        }

        str_type whole_number::to_string(word_type base) const {

            if (base == 10 || base == 0 || base == 2 || base == 8 || base == 16) {

                if (!is()) {
                    return "0";
                }

                whole_number radix = base != 0 ? base : 10;
                whole_number n = *this;

                stream_type stream;

                int count = 0;

                while (n.is()) {

                    whole_number q;
                    whole_number r = n;

                    n.div_rem(radix, q, r);

                    n = q;

                    stream << r._reg.at_reg(0);

                    if (base == 10) {
                        count += 1;

                        if (count == 3) {
                            stream << ',';
                            count = 0;
                        }
                    }
                }

                str_type res = stream.str();

                if (res.back() == ',') {
                    res.pop_back();
                }

                std::reverse(res.begin(), res.end());

                return res;
            }

            return "Invalid radix, radix must be 2, 8, 10, or 16.";
        }

        void whole_number::trim() {

            _reg.trim();
        }

        void whole_number::divide_remainder(word_type d, whole_number& q, whole_number& r, int_type stop) const {

            d_word_type n(0);

            for (int_type i = static_cast<int_type>(_reg.size_regs() - 1); i >= stop; i -= 1) {

                n += _reg.at_reg(i);

                q._reg.at_reg(i - stop) = static_cast<word_type>(n / d);

                n %= d;
                n <<= _reg.BITS;
            }

            r._reg.at_reg(0) = n >> _reg.BITS;

            return;
        }
    }  // end MPA
}  // end Olly
