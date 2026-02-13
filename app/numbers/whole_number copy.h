#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <limits>
#include <ostream>
#include <iomanip>

#if defined(_MSC_VER)
    #include <intrin.h>
#endif

// WholeNumber: arbitrary-precision non-negative integer using radix 2^64.
// Limbs are stored little-endian: limbs[0] is least significant 64 bits.
class WholeNumber {
public:
        using limb_t = uint64_t;

        WholeNumber() = default;
        WholeNumber(uint64_t x) { if (x) limbs_.push_back(x); }
        explicit WholeNumber(const std::string& decimal) { from_decimal(decimal); }

        // Basic queries
        size_t size() const { return limbs_.size(); }

        // String conversion
        std::string to_string() const {
                if (is_zero()) return "0";
                WholeNumber tmp = *this;
                static const uint64_t BASE10 = 10000000000000000000ull; // 1e19
                std::vector<uint64_t> parts;
                while (!tmp.is_zero()) {
                        uint64_t rem = tmp.div_small(BASE10);
                        parts.push_back(rem);
                }
                std::string out;
                out.reserve(parts.size() * 19);
                // most significant part without leading zeros
                out += std::to_string(parts.back());
                for (size_t i = parts.size() - 1; i-- > 0;) {
                        // pad with zeros to 19 digits
                        std::string chunk = std::to_string(parts[i]);
                        out += std::string(19 - chunk.size(), '0');
                        out += chunk;
                }
                return out;
        }

        // Comparisons
        friend int compare(const WholeNumber& a, const WholeNumber& b) {
                if (a.limbs_.size() != b.limbs_.size())
                        return a.limbs_.size() < b.limbs_.size() ? -1 : 1;
                for (size_t i = a.limbs_.size(); i-- > 0;) {
                        if (a.limbs_[i] != b.limbs_[i])
                                return a.limbs_[i] < b.limbs_[i] ? -1 : 1;
                }
                return 0;
        }
        friend bool operator==(const WholeNumber& a, const WholeNumber& b) { return compare(a, b) == 0; }
        friend bool operator!=(const WholeNumber& a, const WholeNumber& b) { return compare(a, b) != 0; }
        friend bool operator<(const WholeNumber& a, const WholeNumber& b) { return compare(a, b) < 0; }
        friend bool operator<=(const WholeNumber& a, const WholeNumber& b) { return compare(a, b) <= 0; }
        friend bool operator>(const WholeNumber& a, const WholeNumber& b) { return compare(a, b) > 0; }
        friend bool operator>=(const WholeNumber& a, const WholeNumber& b) { return compare(a, b) >= 0; }

        constexpr bool is_zero() const { return limbs_.size() == 1 && limbs_[0] == 0; }
        constexpr bool is_even() const { return limbs_.size()  > 0 && (limbs_[0] & 1) == 0; }
        constexpr bool is_odd()  const { return limbs_.size()  > 0 && (limbs_[0] & 1) == 1; }

        // Arithmetic (WholeNumber)
        WholeNumber& operator+=(const WholeNumber& rhs) {
                const size_t n = std::max(limbs_.size(), rhs.limbs_.size());
                limbs_.resize(n, 0);
                uint64_t carry = 0;
                for (size_t i = 0; i < n; ++i) {
                        uint64_t a = limbs_[i];
                        uint64_t b = (i < rhs.limbs_.size() ? rhs.limbs_[i] : 0);
                        limbs_[i] = add3(a, b, carry, carry);
                }
                if (carry) limbs_.push_back(carry);
                return *this;
        }
        WholeNumber& operator-=(const WholeNumber& rhs) {
                if (*this < rhs) throw std::underflow_error("WholeNumber: negative result in subtraction");
                uint64_t borrow = 0;
                for (size_t i = 0; i < limbs_.size(); ++i) {
                        uint64_t a = limbs_[i];
                        uint64_t b = (i < rhs.limbs_.size() ? rhs.limbs_[i] : 0);
                        limbs_[i] = sub3(a, b, borrow, borrow);
                }
                normalize();
                return *this;
        }
        WholeNumber& operator*=(const WholeNumber& rhs) {
                if (is_zero() || rhs.is_zero()) { limbs_.clear(); return *this; }
                std::vector<uint64_t> res(limbs_.size() + rhs.limbs_.size(), 0);
                for (size_t i = 0; i < limbs_.size(); ++i) {
                        uint64_t carry = 0;
                        for (size_t j = 0; j < rhs.limbs_.size(); ++j) {
                                uint64_t lo, hi;
                                mul_wide(limbs_[i], rhs.limbs_[j], lo, hi);
                                // res[i+j] += lo + carry
                                uint64_t c1;
                                uint64_t t = add3(res[i + j], lo, 0, c1);
                                uint64_t c2;
                                t = add3(t, carry, 0, c2);
                                res[i + j] = t;
                                // accumulate carry into next position
                                uint64_t sum_hi;
                                uint64_t c3;
                                sum_hi = add3(hi, c1, 0, c3);
                                uint64_t c4;
                                carry = add3(sum_hi, c2, 0, c4);
                                // c3 and c4 are at most 1 each, they sum to at most 2, but we fold them via subsequent additions
                                // We'll add the leftover into res[i + j + 1] along with carry next iteration
                                uint64_t add_to_next = c3 + c4;
                                if (add_to_next) {
                                        uint64_t c5;
                                        res[i + j + 1] = add3(res[i + j + 1], add_to_next, 0, c5);
                                        carry += c5;
                                }
                        }
                        size_t k = i + rhs.limbs_.size();
                        while (carry) {
                                uint64_t c;
                                res[k] = add3(res[k], carry, 0, c);
                                carry = c;
                                ++k;
                                if (k >= res.size()) res.push_back(0);
                        }
                }
                limbs_.swap(res);
                normalize();
                return *this;
        }
        WholeNumber& operator/=(const WholeNumber& rhs) {
                WholeNumber q, r;
                divmod(*this, rhs, q, r);
                limbs_.swap(q.limbs_);
                return *this;
        }
        WholeNumber& operator%=(const WholeNumber& rhs) {
                WholeNumber q, r;
                divmod(*this, rhs, q, r);
                limbs_.swap(r.limbs_);
                return *this;
        }

        // Arithmetic (small)
        WholeNumber& operator+=(uint64_t rhs) {
                uint64_t carry = rhs;
                size_t i = 0;
                while (carry) {
                        if (i == limbs_.size()) limbs_.push_back(0);
                        uint64_t c;
                        limbs_[i] = add3(limbs_[i], carry, 0, c);
                        carry = c;
                        ++i;
                }
                return *this;
        }
        WholeNumber& operator-=(uint64_t rhs) {
                if (*this < WholeNumber(rhs)) throw std::underflow_error("WholeNumber: negative result in subtraction");
                uint64_t borrow = rhs;
                size_t i = 0;
                while (borrow && i < limbs_.size()) {
                        uint64_t b;
                        limbs_[i] = sub3(limbs_[i], (uint64_t)borrow, 0, b);
                        borrow = b;
                        ++i;
                }
                normalize();
                return *this;
        }
        WholeNumber& operator*=(uint64_t rhs) {
                if (rhs == 0 || is_zero()) { limbs_.clear(); return *this; }
                uint64_t carry = 0;
                for (size_t i = 0; i < limbs_.size(); ++i) {
                        uint64_t lo, hi;
                        mul_wide(limbs_[i], rhs, lo, hi);
                        uint64_t c;
                        limbs_[i] = add3(lo, carry, 0, c);
                        uint64_t sum = hi + c;
                        carry = sum + (sum < hi ? 1ull : 0ull); // but sum < hi shouldn't happen since c is 0/1
                }
                if (carry) limbs_.push_back(carry);
                return *this;
        }
        WholeNumber& operator/=(uint64_t rhs) {
                if (rhs == 0) throw std::domain_error("WholeNumber: division by zero");
                div_small(rhs); // returns remainder, ignores it here
                return *this;
        }
        WholeNumber& operator%=(uint64_t rhs) {
                if (rhs == 0) throw std::domain_error("WholeNumber: division by zero");
                uint64_t rem = 0;
                for (size_t i = limbs_.size(); i-- > 0;) {
                        uint64_t q, r;
                        div128by64(rem, limbs_[i], rhs, q, r);
                        limbs_[i] = q; // temporarily store quotient; we will discard after loop
                        rem = r;
                }
                // leave number unchanged; we only return remainder via constructing:
                limbs_.assign(rem ? 1 : 0, rem);
                return *this;
        }

        // Bit shifts
        WholeNumber& operator<<=(unsigned bits) {
                if (is_zero() || bits == 0) return *this;
                unsigned word_shift = bits / 64;
                unsigned bit_shift = bits % 64;
                if (word_shift) limbs_.insert(limbs_.begin(), word_shift, 0);
                if (bit_shift) {
                        uint64_t carry = 0;
                        for (size_t i = word_shift; i < limbs_.size(); ++i) {
                                uint64_t new_carry = (limbs_[i] >> (64 - bit_shift));
                                limbs_[i] = (limbs_[i] << bit_shift) | carry;
                                carry = new_carry;
                        }
                        if (carry) limbs_.push_back(carry);
                }
                return *this;
        }
        WholeNumber& operator>>=(unsigned bits) {
                if (is_zero() || bits == 0) return *this;
                unsigned word_shift = bits / 64;
                unsigned bit_shift = bits % 64;
                if (word_shift >= limbs_.size()) { limbs_.clear(); return *this; }
                if (word_shift) limbs_.erase(limbs_.begin(), limbs_.begin() + word_shift);
                if (bit_shift) {
                        uint64_t carry = 0;
                        for (size_t i = limbs_.size(); i-- > 0;) {
                                uint64_t new_carry = (limbs_[i] << (64 - bit_shift));
                                limbs_[i] = (limbs_[i] >> bit_shift) | carry;
                                carry = new_carry;
                        }
                }
                normalize();
                return *this;
        }

        // Non-member style operators
        friend WholeNumber operator+(WholeNumber a, const WholeNumber& b) { a += b; return a; }
        friend WholeNumber operator-(WholeNumber a, const WholeNumber& b) { a -= b; return a; }
        friend WholeNumber operator*(WholeNumber a, const WholeNumber& b) { a *= b; return a; }
        friend WholeNumber operator/(WholeNumber a, const WholeNumber& b) { a /= b; return a; }
        friend WholeNumber operator%(WholeNumber a, const WholeNumber& b) { a %= b; return a; }
        friend WholeNumber operator+(WholeNumber a, uint64_t b) { a += b; return a; }
        friend WholeNumber operator-(WholeNumber a, uint64_t b) { a -= b; return a; }
        friend WholeNumber operator*(WholeNumber a, uint64_t b) { a *= b; return a; }
        friend WholeNumber operator/(WholeNumber a, uint64_t b) { a /= b; return a; }
        friend WholeNumber operator%(WholeNumber a, uint64_t b) { a %= b; return a; }

        // Division: q = a / b; r = a % b
        static void divmod(const WholeNumber& a, const WholeNumber& b, WholeNumber& q, WholeNumber& r) {
                if (b.is_zero()) throw std::domain_error("WholeNumber: division by zero");
                if (a < b) { q = WholeNumber(0); r = a; return; }
                if (b.limbs_.size() == 1) {
                        q = a;
                        uint64_t rem = q.div_small(b.limbs_[0]);
                        r = WholeNumber(rem);
                        return;
                }
                // Normalize so that highest bit of divisor is set
                unsigned shift = clz64(b.limbs_.back());
                WholeNumber u = a;
                WholeNumber v = b;
                if (shift) { u <<= shift; v <<= shift; }
                const size_t n = v.limbs_.size();
                const size_t m = u.limbs_.size() - n;
                // Ensure u has an extra limb
                if (u.limbs_.size() == m + n) u.limbs_.push_back(0);

                q.limbs_.assign(m + 1, 0);

                // Pre-capture pointers for speed
                auto& U = u.limbs_;
                auto& V = v.limbs_;

                for (size_t j = m + 1; j-- > 1;) {
                        // j runs from m down to 0; using size_t safe form
                }
                // Proper loop: j from m down to 0
                for (size_t j = m + 1; j-- > 0;) {
                        // Estimate qhat = (U[j+n]*B + U[j+n-1]) / V[n-1]
                        uint64_t ujn = U[j + n];
                        uint64_t ujn1 = U[j + n - 1];
                        uint64_t qhat, rhat;
                        div128by64(ujn, ujn1, V[n - 1], qhat, rhat);
                        if (qhat == std::numeric_limits<uint64_t>::max()) {
                                // clamp to B-1
                        }
                        // Adjust with second term
                        if (n >= 2) {
                                // while qhat*V[n-2] > (rhat*B + U[j+n-2])
                                uint64_t vn2 = V[n - 2];
                                uint64_t ujn2 = U[j + n - 2];
                                uint64_t lo, hi;
                                mul_wide(qhat, vn2, lo, hi);
                                bool gt = (hi > rhat) || (hi == rhat && lo > ujn2);
                                while (gt) {
                                        --qhat;
                                        uint64_t prev_rhat = rhat;
                                        rhat += V[n - 1];
                                        if (rhat < prev_rhat) break; // overflow means stop
                                        mul_wide(qhat, vn2, lo, hi);
                                        gt = (hi > rhat) || (hi == rhat && lo > ujn2);
                                }
                        }

                        // Multiply and subtract qhat * V from U segment starting at j
                        uint64_t borrow = 0;
                        uint64_t carry = 0;
                        for (size_t i = 0; i < n; ++i) {
                                uint64_t p_lo, p_hi;
                                mul_wide(qhat, V[i], p_lo, p_hi);
                                // subtract p_lo + carry from U[j+i]
                                uint64_t t_borrow;
                                uint64_t t = sub3(U[j + i], p_lo, 0, t_borrow);
                                uint64_t t_borrow2;
                                t = sub3(t, carry, 0, t_borrow2);
                                U[j + i] = t;
                                // compute new carry/borrow: p_hi + t_borrow (+ t_borrow2) contributes to borrow on next limb
                                uint64_t sum_hi = p_hi + t_borrow + t_borrow2;
                                borrow = sum_hi;
                                carry = borrow; // since base is 2^64, high part becomes carry into next subtract
                        }
                        // subtract final borrow from U[j+n]
                        uint64_t t_borrow3;
                        uint64_t ujnp = sub3(U[j + n], carry, 0, t_borrow3);
                        U[j + n] = ujnp;

                        if (t_borrow3) {
                                // qhat was too big; add back V
                                --qhat;
                                uint64_t c = 0;
                                for (size_t i = 0; i < n; ++i) {
                                        uint64_t t_carry;
                                        uint64_t t = add3(U[j + i], V[i], c, t_carry);
                                        U[j + i] = t;
                                        c = t_carry;
                                }
                                U[j + n] += c;
                        }
                        q.limbs_[j] = qhat;
                }

                q.normalize();
                // remainder is U[0..n-1] >> shift
                r.limbs_.assign(U.begin(), U.begin() + n);
                if (shift) r >>= shift; else r.normalize();
        }

private:
        std::vector<uint64_t> limbs_;

        void normalize() {
                while (!limbs_.empty() && limbs_.back() == 0) limbs_.pop_back();
        }

        void from_decimal(const std::string& s) {
                limbs_.clear();
                WholeNumber ten(10);
                for (char ch : s) {
                        if (ch >= '0' && ch <= '9') {
                                *this *= 10ull;
                                *this += static_cast<uint64_t>(ch - '0');
                        } else if (ch == '_' || ch == '\'' || std::isspace(static_cast<unsigned char>(ch))) {
                                continue;
                        } else {
                                throw std::invalid_argument("WholeNumber: invalid decimal string");
                        }
                }
        }

        // Divide by small (uint64_t). Returns remainder; updates this to quotient.
        uint64_t div_small(uint64_t d) {
                if (d == 0) throw std::domain_error("WholeNumber: division by zero");
                uint64_t rem = 0;
                for (size_t i = limbs_.size(); i-- > 0;) {
                        uint64_t q, r;
                        div128by64(rem, limbs_[i], d, q, r);
                        limbs_[i] = q;
                        rem = r;
                }
                normalize();
                return rem;
        }

        // Helpers: add/sub with carry/borrow, wide mul and 128/64 division.

        static inline uint64_t add3(uint64_t a, uint64_t b, uint64_t carry_in, uint64_t& carry_out) {
        #if defined(_MSC_VER)
                unsigned char c1 = _addcarry_u64(static_cast<unsigned char>(carry_in), a, b, &a);
                carry_out = c1;
                return a;
        #else
                unsigned __int128 sum = (unsigned __int128)a + b + carry_in;
                carry_out = (uint64_t)(sum >> 64);
                return (uint64_t)sum;
        #endif
        }

        static inline uint64_t sub3(uint64_t a, uint64_t b, uint64_t borrow_in, uint64_t& borrow_out) {
        #if defined(_MSC_VER)
                unsigned char b1 = _subborrow_u64(static_cast<unsigned char>(borrow_in), a, b, &a);
                borrow_out = b1;
                return a;
        #else
                unsigned __int128 diff = (unsigned __int128)a - b - borrow_in;
                borrow_out = (diff >> 127) & 1; // top bit set means borrow
                return (uint64_t)diff;
        #endif
        }

        static inline void mul_wide(uint64_t a, uint64_t b, uint64_t& lo, uint64_t& hi) {
        #if defined(_MSC_VER)
                lo = _umul128(a, b, &hi);
        #else
                unsigned __int128 p = (unsigned __int128)a * b;
                lo = (uint64_t)p;
                hi = (uint64_t)(p >> 64);
        #endif
        }

        static inline void div128by64(uint64_t hi, uint64_t lo, uint64_t d, uint64_t& q, uint64_t& r) {
                // Compute ((hi<<64) | lo) / d
        #if defined(_MSC_VER)
                q = _udiv128(hi, lo, d, &r);
        #else
                unsigned __int128 num = ((unsigned __int128)hi << 64) | lo;
                q = (uint64_t)(num / d);
                r = (uint64_t)(num % d);
        #endif
        }

        static inline unsigned clz64(uint64_t x) {
                if (x == 0) return 64;
        #if defined(_MSC_VER)
                unsigned long idx;
        #  if defined(_M_X64) || defined(_M_ARM64)
                _BitScanReverse64(&idx, x);
                return 63u - idx;
        #  else
                // 32-bit target: split
                uint32_t hi = static_cast<uint32_t>(x >> 32);
                if (hi) { _BitScanReverse(&idx, hi); return 31u - idx; }
                uint32_t lo = static_cast<uint32_t>(x);
                _BitScanReverse(&idx, lo);
                return 63u - (32u + idx);
        #  endif
        #else
                return __builtin_clzll(x);
        #endif
        }
};

// Stream output
inline std::ostream& operator<<(std::ostream& os, const WholeNumber& n) {
        return os << n.to_string();
}



namespace numbers_tests {

inline WholeNumber pow2(unsigned bits) {
    WholeNumber x(1);
    x <<= bits;
    return x;
}

inline WholeNumber pow2_minus_1(unsigned bits) {
    assert(bits > 0);
    WholeNumber x = pow2(bits);
    x -= 1ull;
    return x;
}

inline void test_whole_number_basic() {
    // Zero and small construction
    WholeNumber a; // 0
    assert(a.is_zero());
    assert(a.to_string() == "0");

    WholeNumber b(0);
    assert(b.is_zero());

    WholeNumber c(1);
    assert(!c.is_zero());
    assert(c.to_string() == "1");

    WholeNumber d(1234567890123456789ull);
    assert(d.to_string() == "1234567890123456789");

    // Decimal string parsing (ignores underscore, quote, whitespace)
    WholeNumber e(std::string("  000_001'234 567'890"));
    assert(e.to_string() == "1234567890");

    // Comparisons
    assert(WholeNumber(5) > WholeNumber(4));
    assert(WholeNumber(5) >= WholeNumber(5));
    assert(WholeNumber(5) == WholeNumber(5));
    assert(WholeNumber(4) < WholeNumber(5));
    assert(WholeNumber(4) <= WholeNumber(5));
    assert(WholeNumber(4) != WholeNumber(5));
}

inline void test_whole_number_add_sub_carries() {
    // Force multi-limb carry: (2^(64*k) - 1) + 1 = 2^(64*k)
    unsigned limbs = 3;
    WholeNumber all_ones = pow2_minus_1(64u * limbs); // (1 << (64*limbs)) - 1
    WholeNumber tmp = all_ones;
    tmp += 1ull;
    WholeNumber expect = pow2(64u * limbs);
    assert(tmp == expect);

    // Subtraction underflow guard
    WholeNumber x(100);
    WholeNumber y(99);
    WholeNumber z = x - y;
    assert(z.to_string() == "1");

    // Sub by small with borrows across limbs
    WholeNumber big = pow2(128);
    big -= 1ull;
    assert(big == pow2_minus_1(128));
    big += 1ull;
    assert(big == pow2(128));
}

inline void test_whole_number_shifts() {
    // Shift left then right should recover original
    WholeNumber a(123456789ull);
    WholeNumber b = a;
    b <<= 130;
    b >>= 130;
    assert(b == a);

    // Large shifts
    WholeNumber one(1);
    WholeNumber c = one;
    c <<= 256;
    assert(c > one);
    c >>= 256;
    assert(c == one);
}

inline void test_whole_number_mul() {
    // (2^k - 1) * (2^m - 1) = 2^(k+m) - 2^k - 2^m + 1
    unsigned k = 130;
    unsigned m = 64;
    WholeNumber A = pow2_minus_1(k);
    WholeNumber B = pow2_minus_1(m);
    WholeNumber P = A * B;

    WholeNumber E = pow2(k + m);
    E -= pow2(k);
    E -= pow2(m);
    E += 1ull;

    assert(P == E);

    // Multiplying by 0 and 1
    WholeNumber z;
    assert((A * z).is_zero());
    assert((A * WholeNumber(1)) == A);
}

inline void test_whole_number_divmod_small() {
    // Small divisor
    WholeNumber a(std::string("123456789012345678901234567890"));
    uint64_t d = 97;
    WholeNumber q = a;
    q /= d;
    WholeNumber r = a;
    r %= d;

    // Check: q * d + r == a and r < d
    WholeNumber recomposed = q * WholeNumber(d) + r;
    assert(recomposed == a);
    assert(r < WholeNumber(d));
}

inline void test_whole_number_divmod_large() {
    // Large divisor (multi-limb): test identity a = q*b + r and known remainder for Mersenne-like numbers
    // For A = 2^k - 1, B = 2^m - 1, remainder is 2^(k mod m) - 1
    unsigned k = 250;
    unsigned m = 100; // > 64 ensures multi-limb divisor
    WholeNumber A = pow2_minus_1(k);
    WholeNumber B = pow2_minus_1(m);

    WholeNumber q, r;
    WholeNumber::divmod(A, B, q, r);

    // r should be 2^(k % m) - 1
    unsigned r_exp_bits = k % m;
    WholeNumber r_expected = r_exp_bits ? pow2_minus_1(r_exp_bits) : WholeNumber(0);
    assert(r == r_expected);

    // Recomposition check
    WholeNumber recomposed = q * B + r;
    assert(recomposed == A);
}

inline void test_whole_number_string_io_large() {
    // Build a large decimal string with ignored separators and ensure round-trip via to_string
    std::string raw;
    raw.reserve(5000);
    for (int i = 0; i < 300; ++i) raw += "1234567890"; // 3000 digits
    // Insert some ignored separators
    std::string with_seps;
    with_seps.reserve(raw.size() + 300);
    for (size_t i = 0; i < raw.size(); ++i) {
        with_seps.push_back(raw[i]);
        if (i % 50 == 49) with_seps.push_back('_');
        if (i % 77 == 76) with_seps.push_back(' ');
        if (i % 93 == 92) with_seps.push_back('\'');
    }

    // Remove leading zeros by adding a non-zero at front if necessary
    if (!raw.empty() && raw[0] == '0') raw[0] = '1';

    WholeNumber big(with_seps);
    std::string out = big.to_string();
    assert(out == raw);

    // Arithmetic sanity on large number (multiply then divide)
    WholeNumber two(2);
    WholeNumber prod = big * two;
    WholeNumber q, r;
    WholeNumber::divmod(prod, two, q, r);
    assert(q == big);
    assert(r.is_zero());
}

inline void test_whole_number_large() {
    // Construct very large numbers and validate operations:
    // A = 2^1024 - 1, B = 2^512 - 1
    WholeNumber A = pow2_minus_1(1024);
    WholeNumber B = pow2_minus_1(512);

    // Check addition overflow into new limb
    WholeNumber A_plus_1 = A;
    A_plus_1 += 1ull;
    assert(A_plus_1 == pow2(1024));

    // Multiplication against power-of-two (shift check)
    WholeNumber shifted = A;
    shifted <<= 256;
    WholeNumber expect = pow2(256) * A;
    assert(shifted == expect);

    // Large divmod property: (2^1024 - 1) mod (2^512 - 1) == 0
    WholeNumber q, r;
    WholeNumber::divmod(A, B, q, r);
    assert(r.is_zero());
    // Recomposition
    WholeNumber recomposed = q * B + r;
    assert(recomposed == A);

    // Another large divmod where remainder is (2^(1024 % 768) - 1) = (2^256 - 1)
    WholeNumber C = pow2_minus_1(768);
    WholeNumber D = pow2_minus_1(256);
    WholeNumber q2, r2;
    WholeNumber::divmod(A, C, q2, r2);
    assert(r2 == D);
    WholeNumber recomposed2 = q2 * C + r2;
    assert(recomposed2 == A);
}

inline void test_whole_number_all() {
    test_whole_number_basic();
    test_whole_number_add_sub_carries();
    test_whole_number_shifts();
    test_whole_number_mul();
    test_whole_number_divmod_small();
    test_whole_number_divmod_large();
    test_whole_number_string_io_large();
    test_whole_number_large();
}

} // namespace numbers_tests