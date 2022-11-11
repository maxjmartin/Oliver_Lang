#ifndef BIT_REGISTER_H
#define BIT_REGISTER_H

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

#include "../system_fundamentals.h"

namespace Olly {

        /********************************************************************************************/
        //
        //                                 'bit_register' class
        //
        //        The bit_register class implements a series of binary registers size to
        //        the integral type passed during to the template at definition.
        //
        //        Support for all of the binary operation is provides, along with binary
        //        based mathematical operations.  The implimentation is little endian.
        //
        //        The class is defined using explicit inclusion instantiation, a class
        //        declaration is defined, followed by the class definition.
        //
        /********************************************************************************************/

    template<typename N>
    class bit_register {
        static_assert(std::numeric_limits<N>::is_integer,                  "The bit_register template argument T must be an integral.");
        static_assert(std::numeric_limits<N>::is_signed    ? false : true, "The bit_register template argument T must be an unsigned integral.");
        static_assert(std::numeric_limits<N>::digits >= 16 ? true : false, "The bit_register template argument T must be a minimum of 16 bits wide.");

    public:
        typedef N             word_type;
        typedef std::vector<N> reg_type;

        static const N         MASK = ~N(0);
        static const size_type BITS = std::numeric_limits<N>::digits;

        bit_register();
        bit_register(str_type value, str_type base = "10");

        bit_register(const size_type& size, word_type value = 0);
        bit_register(const bit_register& obj) = default;
        bit_register(bit_register&& obj)      = default;
        virtual ~bit_register();

        bit_register& operator=(const bit_register& obj) = default;
        bit_register& operator=(bit_register&& obj)      = default;

        bool_type       is() const;                             // Boolean conversion.
        bool_type      all() const;                             // Boolean test for all bits being set to 1.
        size_type    count() const;                             // The count of bits set to 1.
        size_type lead_bit() const;                             // Return the lead bit.
        size_type last_bit() const;                             // Return the last bit.
        word_type lead_reg() const;                             // Return the leading register.
        word_type last_reg() const;                             // Return the last register.

        bool_type at_bit(size_type index) const;                // Return the value of a bit at the index.

        word_type& at_reg(size_type index);                     // Return the word at the indexed register.
        word_type  at_reg(size_type index) const;

        str_type to_string() const;                             // Return a string representation at radix 10.
        str_type to_string(N base) const;                       // Return a string representation at radix 'base'.
        void     to_string(stream_type& stream) const;          // Send a string representation to a stream_type.

        size_t size_bits() const;                               // Get the total size in bits of the register.
        size_t size_regs() const;                               // Get the total size of words in the register.

        bit_register& set();                                    // Set all bits to true.
        bit_register& set(size_type index);                     // Set a bit at 'index' to true.

        bit_register& reset();                                  // Set all bits to false.
        bit_register& reset(size_type index);                   // Set a bit at 'index' to false.

        bit_register& flip();                                   // Flip the truth of every bit in the register.
        bit_register& flip(size_type index);                    // Flip the truth of a bit at 'index'.

        bool_type operator==(const bit_register& b) const;      // Standard comparison operators.
        bool_type operator!=(const bit_register& b) const;
        bool_type operator< (const bit_register& b) const;
        bool_type operator> (const bit_register& b) const;
        bool_type operator<=(const bit_register& b) const;
        bool_type operator>=(const bit_register& b) const;

        bit_register& operator&=(const bit_register& other);    // Standard mutable bitwise operators.
        bit_register& operator|=(const bit_register& other);
        bit_register& operator^=(const bit_register& other);

        bit_register& operator<<=(size_type index);             // Standard mutable shift operators.
        bit_register& operator>>=(size_type index);

        bit_register operator&(const bit_register& b) const;    // Standard const bitwise operators.
        bit_register operator|(const bit_register& b) const;
        bit_register operator^(const bit_register& b) const;
        bit_register operator~() const;

        bit_register operator<<(size_type index) const;         // Standard const shift operators.
        bit_register operator>>(size_type index) const;

        bit_register& operator+=(const bit_register& other);    // Standard mutable math operators.
        bit_register& operator-=(const bit_register& other);
        bit_register& operator*=(const bit_register& other);
        bit_register& operator/=(const bit_register& other);
        bit_register& operator%=(const bit_register& other);

        bit_register operator+(const bit_register& b) const;    // Standard const math operators.
        bit_register operator-(const bit_register& b) const;
        bit_register operator*(const bit_register& b) const;
        bit_register operator/(const bit_register& b) const;
        bit_register operator%(const bit_register& b) const;

        bit_register& operator++();                             // Standard increment operators.
        bit_register  operator++(int);

        bit_register& operator--();                             // Standard decremant operators.
        bit_register  operator--(int);

        template<typename I>
        N to_integral() const;                                  // Cast the register to an integral of type T.

        bit_register  bin_comp() const;                         // Return the binary compliment of the register.

        // Get both the qotient and the remainder of the regester divided by 'other'.
        void div_rem(bit_register& other, bit_register& qot, bit_register& rem) const;

        real_type compare(const bit_register& other) const;     // Compare two registers.
                                                                //  0.0 = equality.
                                                                //  1.0 = greater than.
                                                                // -1.0 = less than.

        bit_register& trim();                                   // Trim all words of 0 from the end of the register, until
                                                                // a set word is encounter, or the last word is encountered.

    private:
        typedef std::bitset<BITS>           single_prc_bitset;
        typedef std::bitset<BITS + BITS>    double_prc_bitset;

        static const N ONE = 1;

        reg_type _reg;

        void get_shift_index(size_type& index, size_type& reg_index, size_type& bit_index) const;

        void divide_remainder(const bit_register& x, bit_register y, bit_register& q, bit_register& r) const;

        str_type get_string(N base) const;

        void  left_shift_bits(size_type& word_index, size_type& bit_index);
        void right_shift_bits(size_type& word_index, size_type& bit_index);
    };

    /********************************************************************************************/
    //
    //                              'bit_register' implimentation
    //
    /********************************************************************************************/

    template<typename N>
    inline bit_register<N>::bit_register() : _reg(1, 0) {
    }

    template<typename N>
    inline bit_register<N>::bit_register(const size_type& size, word_type value) : _reg((size > 0 ? size : 1), value) {
    }

    template<typename N>
    inline bit_register<N>::bit_register(str_type value, str_type base) : _reg(1, 0) {

        N base_radix = to<N>(base);             // Get the base radix to use.

        bit_register<N> radix(1, base_radix);   // Define a bit_register to act as the radix.

        for (auto i : value) {                  // Loop through each digit and add it to the bit_register.

            str_type digit_str = "";

            digit_str.push_back(i);

            N n = to<N>(digit_str);

            if (n < base_radix) {
                bit_register<N> digit(1, n);

                operator*=(radix);
                operator+=(digit);
            }
        }
    }

    template<typename N>
    inline bit_register<N>::~bit_register() {
    }

    template<typename N>
    inline bool_type bit_register<N>::is() const {

        for (auto i : _reg) {

            if (i) {
                return true;
            }
        }
        return false;
    }

    template<typename N>
    inline bool_type bit_register<N>::all() const {

        for (auto i : _reg) {

            if (i != MASK) {
                return false;
            }
        }
        return true;
    }

    template<typename N>
    inline size_type bit_register<N>::count() const {

        size_type count = 0;

        for (const auto i : _reg) {

            auto n = i;

            while (n > 0) {

                if (n & 1) {
                    count += 1;
                }
                n >>= 1;
            }
        }

        return count;
    }

    template<typename N>
    inline size_type bit_register<N>::lead_bit() const {

        size_type word_index = _reg.size();

        word_type mask = (ONE << (BITS - ONE));

        for (auto i = _reg.crbegin(); i != _reg.crend(); ++i) {
            word_index -= 1;

            auto a = *i;

            size_type bit_index = BITS;

            while (a) {

                if (a & mask) {
                    return bit_index + (word_index * BITS);
                }
                a <<= 1;
                bit_index -= 1;
            }
        }

        return 0;
    }

    template<typename N>
    inline size_type bit_register<N>::last_bit() const {

        size_type word_index = 0;

        word_type mask = 1;

        for (auto i = _reg.cbegin(); i != _reg.cend(); ++i) {

            auto a = *i;

            size_type bit_index = 1;

            while (a) {

                if (a & mask) {
                    return bit_index + (word_index * BITS);
                }
                a >>= 1;
                bit_index += 1;
            }
            word_index += 1;
        }

        return 0;
    }

    template<typename N>
    inline N bit_register<N>::lead_reg() const {

        if (_reg.empty()) {
            return N(0);
        }

        return _reg.back();
    }

    template<typename N>
    inline N bit_register<N>::last_reg() const {

        if (_reg.empty()) {
            return N(0);
        }

        return _reg.front();
    }

    template<typename N>
    inline bool_type bit_register<N>::at_bit(size_type index) const {

        size_type reg_index, bit_index;
        get_shift_index(index, reg_index, bit_index);

        if (reg_index < _reg.size()) {

            return _reg[reg_index] & (ONE << (bit_index - ONE));
        }

        return false;
    }

    template<typename N>
    inline N& bit_register<N>::at_reg(size_type index) {

        while (index >= _reg.size()) {

            _reg.push_back(0);
        }

        return _reg[index];
    }

    template<typename N>
    inline N bit_register<N>::at_reg(size_type index) const {

        if (index < _reg.size()) {

            return _reg[index];
        }

        return word_type(0);
    }

    template<typename N>
    inline str_type bit_register<N>::to_string() const {

        return to_string(10);
    }

    template<typename N>
    inline str_type bit_register<N>::to_string(N base) const {

        if (base > 360) {
            return "Radix must be between 0 and 360.";
        }

        if (base == 0) {
            stream_type stream;

            to_string(stream);

            return stream.str();
        }

        if (!is()) {
            return "0";
        }

        return get_string(base);
    }

    template<typename N>
    inline void bit_register<N>::to_string(stream_type& stream) const {

        size_type i = _reg.size();

        while (i-- > 1) {
            stream << "word[" << i << "] = " << single_prc_bitset(_reg[i]) << "\n";
        }
        stream << "word[" << 0 << "] = " << single_prc_bitset(_reg[i]);
    }

    template<typename N>
    inline size_t bit_register<N>::size_bits() const {
        return _reg.size() * BITS;
    }

    template<typename N>
    inline size_t bit_register<N>::size_regs() const {
        return _reg.size();
    }

    template<typename N>
    inline bit_register<N>& bit_register<N>::set() {

        for (auto i = _reg.begin(); i != _reg.end(); ++i) {
            *i = MASK;
        }

        return *this;
    }

    template<typename N>
    inline bit_register<N>& bit_register<N>::set(size_type index) {

        size_type reg_index, bit_index;
        get_shift_index(index, reg_index, bit_index);

        while (reg_index >= _reg.size()) {

            _reg.push_back(0);
        }

        _reg[reg_index] |= (ONE << (bit_index - ONE));

        return *this;
    }

    template<typename N>
    inline bit_register<N>& bit_register<N>::reset() {

        for (size_type i = 0, end = _reg.size(); i < end; i += 1) {
            _reg[i] = word_type(0);
        }

        return *this;
    }

    template<typename N>
    inline bit_register<N>& bit_register<N>::reset(size_type index) {

        size_type reg_index, bit_index;
        get_shift_index(index, reg_index, bit_index);

        while (reg_index >= _reg.size()) {

            _reg.push_back(0);
        }

        _reg[reg_index] &= ~(1 << (bit_index - 1));

        return *this;
    }

    template<typename N>
    inline bit_register<N>& bit_register<N>::flip() {

        for (size_type i = 0, end = _reg.size(); i < end; i += 1) {
            _reg[i] = ~_reg[i];
        }

        return *this;
    }

    template<typename N>
    inline bit_register<N>& bit_register<N>::flip(size_type index) {

        size_type reg_index, bit_index;
        get_shift_index(index, reg_index, bit_index);

        while (reg_index >= _reg.size()) {

            _reg.push_back(0);
        }

        _reg[reg_index] ^= (1 << (bit_index - 1));

        return *this;
    }

    template<typename N>
    inline bool_type bit_register<N>::operator==(const bit_register<N>& b) const {
        return compare(b) == 0;
    }

    template<typename N>
    inline bool_type bit_register<N>::operator!=(const bit_register<N>& b) const {
        return compare(b) != 0;
    }

    template<typename N>
    inline bool_type bit_register<N>::operator<(const bit_register<N>& b) const {
        return compare(b) < 0;
    }

    template<typename N>
    inline bool_type bit_register<N>::operator>(const bit_register<N>& b) const {
        return compare(b) > 0;
    }

    template<typename N>
    inline bool_type bit_register<N>::operator<=(const bit_register<N>& b) const {
        return compare(b) <= 0;
    }

    template<typename N>
    inline bool_type bit_register<N>::operator>=(const bit_register<N>& b) const {
        return compare(b) >= 0;
    }

    template<typename N>
    inline bit_register<N>& bit_register<N>::operator&=(const bit_register<N>& other) {

        while (_reg.size() < other._reg.size()) {

            _reg.push_back(0);
        }

        for (size_type i = 0, end = _reg.size(); i < end; i += 1) {
            _reg[i] &= other.at_reg(i);
        }

        return *this;
    }

    template<typename N>
    inline bit_register<N>& bit_register<N>::operator|=(const bit_register<N>& other) {

        while (_reg.size() < other._reg.size()) {

            _reg.push_back(0);
        }

        for (size_type i = 0, end = _reg.size(); i < end; i += 1) {
            _reg[i] |= other.at_reg(i);
        }

        return *this;
    }

    template<typename N>
    inline bit_register<N>& bit_register<N>::operator^=(const bit_register<N>& other) {

        while (_reg.size() < other._reg.size()) {

            _reg.push_back(0);
        }

        for (size_type i = 0, end = _reg.size(); i < end; i += 1) {
            _reg[i] ^= other.at_reg(i);
        }

        return *this;
    }

    template<typename N>
    inline bit_register<N>& bit_register<N>::operator<<=(size_type index) {

        size_type word_index, bit_index;

        get_shift_index(index, word_index, bit_index);

        if (word_index) {
            _reg.insert(_reg.begin(), word_index, 0);
        }

        if (bit_index) {

            left_shift_bits(word_index, bit_index);
        }

        return *this;
    }

    template<typename N>
    inline bit_register<N>& bit_register<N>::operator>>=(size_type index) {

        size_type word_index, bit_index;

        get_shift_index(index, word_index, bit_index);

        if (word_index) {

            if (word_index < _reg.size()) {

                _reg.erase(_reg.begin(), _reg.begin() + word_index);
            }
            else {
                for (auto i = _reg.begin(), end = _reg.end(); i != end; ++i) {
                    *i = 0;
                }

                return *this;
            }
        }

        if (bit_index) {

            right_shift_bits(word_index, bit_index);
        }

        return *this;
    }

    template<typename N>
    inline bit_register<N> bit_register<N>::operator&(const bit_register<N>& b) const {

        bit_register<N> a(*this);

        a &= b;

        return a;
    }

    template<typename N>
    inline bit_register<N> bit_register<N>::operator|(const bit_register<N>& b) const {

        bit_register<N> a(*this);

        a |= b;

        return a;
    }

    template<typename N>
    inline bit_register<N> bit_register<N>::operator^(const bit_register<N>& b) const {

        bit_register<N> a(*this);

        a ^= b;

        return a;
    }

    template<typename N>
    inline bit_register<N> bit_register<N>::operator~() const {

        bit_register<N> a = *this;

        for (size_type i = 0, end = a._reg.size(); i < end; i += 1) {
            a._reg[i] = ~a._reg[i];
        }

        return a;
    }

    template<typename N>
    inline bit_register<N> bit_register<N>::operator<<(size_type index) const {

        bit_register<N> a(*this);

        a <<= index;

        return a;
    }

    template<typename N>
    inline bit_register<N> bit_register<N>::operator>>(size_type index) const {

        bit_register<N> a(*this);

        a >>= index;

        return a;
    }

    template<typename N>
    inline bit_register<N>& bit_register<N>::operator+=(const bit_register<N>& other) {

        bit_register<N> b(other);
        bit_register<N> c;

        while (b.is()) {

            c = (*this & b) << 1;

            *this ^= b;

            b = c;
        }

        return *this;
    }

    template<typename N>
    inline bit_register<N>& bit_register<N>::operator-=(const bit_register<N>& other) {

        if (other >= *this) {
            return reset();
        }

        bit_register<N> b = other;

        while (b.size_regs() < size_regs()) {
            b._reg.push_back(0);
        }

        b = b.bin_comp();

        b._reg.push_back(0);  // Add a word to handle the two's compliment overflow.

        *this += b;

        _reg.pop_back(); // Get rid of the two's compliment overflow.

        return *this;
    }

    template<typename N>
    inline bit_register<N>& bit_register<N>::operator*=(const bit_register<N>& other) {

        *this = *this * other;

        return *this;
    }

    template<typename N>
    inline bit_register<N>& bit_register<N>::operator/=(const bit_register<N>& other) {

        *this = *this / other;

        return *this;
    }

    template<typename N>
    inline bit_register<N>& bit_register<N>::operator%=(const bit_register<N>& other) {

        *this = *this % other;

        return *this;
    }

    template<typename N>
    inline bit_register<N> bit_register<N>::operator+(const bit_register<N>& b) const {

        bit_register<N> a = *this;

        a += b;

        return a;
    }

    template<typename N>
    inline bit_register<N> bit_register<N>::operator-(const bit_register<N>& b) const {

        bit_register<N> a = *this;

        a -= b;

        return a;
    }

    template<typename N>
    inline bit_register<N> bit_register<N>::operator*(const bit_register<N>& b) const {

        size_type count = 0;

        bit_register<N> x;
        bit_register<N> y = b;

        while (y.is()) {

            if (y.at_bit(1)) {
                x += (*this << count);
            }

            count += 1;
            y >>= 1;
        }

        return x;
    }

    template<typename N>
    inline bit_register<N> bit_register<N>::operator/(const bit_register<N>& b) const {

        bit_register<N> q;
        bit_register<N> r = *this;

        divide_remainder(*this, b, q, r);

        return q;
    }

    template<typename N>
    inline bit_register<N> bit_register<N>::operator%(const bit_register<N>& b) const {

        bit_register<N> q;
        bit_register<N> r = *this;

        divide_remainder(*this, b, q, r);

        return r;
    }

    template<typename N>
    inline bit_register<N>& bit_register<N>::operator++() {

        bit_register<N> one(1, 1);

        operator+=(one);

        return *this;
    }

    template<typename N>
    inline bit_register<N> bit_register<N>::operator++(int) {

        bit_register<N> a(*this);

        operator++();

        return a;
    }

    template<typename N>
    inline bit_register<N>& bit_register<N>::operator--() {

        bit_register<N> one(1, 1);

        operator-=(one);

        return *this;
    }

    template<typename N>
    inline bit_register<N> bit_register<N>::operator--(int) {

        bit_register<N> a(*this);

        operator--();

        return a;
    }

    template<typename N>
    inline bit_register<N> bit_register<N>::bin_comp() const {

        bit_register<N> a = ~*this;
        bit_register<N> one(1, 1);

        a += one;

        return a;
    }

    template<typename N>
    inline void bit_register<N>::div_rem(bit_register& other, bit_register& qot, bit_register& rem) const {

        rem = *this;

        divide_remainder(*this, other, qot, rem);
    }

    template<typename N>
    inline real_type bit_register<N>::compare(const bit_register<N>& other) const {

        size_type i = size_regs() > other.size_regs() ? size_regs() : other.size_regs();

        while (i-- > 0) {

            auto x = at_reg(i);
            auto y = other.at_reg(i);

            if (x > y) {
                return 1.0;
            }

            if (x < y) {
                return -1.0;
            }
        }

        return 0;
    }

    template<typename N>
    inline bit_register<N>& bit_register<N>::trim() {

        while (!_reg.empty() && _reg.back() == 0) {

            _reg.pop_back();
        }

        if (_reg.empty()) {

            _reg.push_back(0);
        }

        return *this;
    }

    template<typename N>
    inline void bit_register<N>::get_shift_index(size_type& index, size_type& reg_index, size_type& bit_index) const {

        if (index) {

            if (index >= BITS) {

                reg_index = index / BITS;
                bit_index = index % BITS;

                if (!bit_index) {
                    --reg_index;
                    bit_index = BITS;
                }

                return;
            }

            reg_index = 0;
            bit_index = index;

            return;
        }

        reg_index = 0;
        bit_index = 0;
    }

    template<typename N>
    inline void bit_register<N>::divide_remainder(const bit_register<N>& x, bit_register<N> y, bit_register<N>& q, bit_register<N>& r) const {

        if (!y.is() || !x.is() || x < y) {
            return;
        }

        size_type lead_x = x.lead_bit();
        size_type lead_y = y.lead_bit();

        size_type bit_dif = (lead_x - lead_y);

        y <<= bit_dif;

        bit_dif += 2;

        while (bit_dif-- > 1) {

            if (r >= y) {
                q.set(bit_dif);
                r -= y;
            }
            y >>= 1;
        }
    }

    template<typename N>
    inline str_type bit_register<N>::get_string(N base) const {

        bit_register<N> radix(1, base);
        bit_register<N> n = *this;

        stream_type stream;

        while (n.is()) {

            bit_register<N> q;
            bit_register<N> r = n;

            divide_remainder(n, radix, q, r);

            n = q;

            stream << r.at_reg(0);
        }

        str_type res = stream.str();
        std::reverse(res.begin(), res.end());

        return res;
    }

    template<typename N>
    inline void bit_register<N>::left_shift_bits(size_type& word_index, size_type& bit_index) {

        size_type i = _reg.size();

        _reg.push_back(0);

        auto bit_mask = double_prc_bitset(MASK);

        while (i-- > 0) {

            auto buffer = double_prc_bitset();
            buffer |= double_prc_bitset(_reg[i]);

            buffer <<= bit_index;

            _reg[i] = static_cast<N>((buffer & bit_mask).to_ullong());

            buffer >>= BITS;
            buffer |= double_prc_bitset(_reg[i + 1]);

            _reg[i + 1] = static_cast<N>(buffer.to_ullong());
        }

        if (_reg.back() == 0) {
            _reg.pop_back();
        }
    }

    template<typename N>
    inline void bit_register<N>::right_shift_bits(size_type& word_index, size_type& bit_index) {

        bool_type pop_back = word_index ? false : true;

        if (word_index) {
            word_index -= 1;
        }
        _reg.push_back(0);

        auto inv_index = BITS - bit_index;

        size_type end = (_reg.size() - 1);

        auto bit_mask = double_prc_bitset(MASK);

        for (size_type i = 0; i < end; i += 1) {

            auto buffer = double_prc_bitset();
            buffer |= double_prc_bitset(_reg[i + 1]);

            buffer <<= inv_index;

            _reg[i] >>= bit_index;
            _reg[i] |= static_cast<N>((buffer & bit_mask).to_ullong());
        }
        _reg[end] >>= bit_index;


        while (word_index-- > 0) {
            _reg.push_back(0);
        }

        if (pop_back) {
            _reg.pop_back();
        }
    }

    template<typename N>
    template<typename I>
    inline N bit_register<N>::to_integral() const {
        static_assert(std::numeric_limits<I>::is_integer, "Integral required.");

        if (!_reg.empty()) {

            auto bits_of_I = std::numeric_limits<I>::digits;

            if (bits_of_I >= BITS && !_reg.empty()) {

                return I(_reg.front());
            }

            I n = 0;

            for (int_type i = BITS / bits_of_I; i >= 0; i -= 1) {

                n <<= bits_of_I;
                n += at_reg(i);
            }

            return n;
        }

        return I(0);
    }
}
#endif // BIT_REGISTER_H