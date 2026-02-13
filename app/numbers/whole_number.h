#pragma once

#include <string>
#include <algorithm>
#include <stdexcept>
#include <compare>
#include <cctype>
#include <limits>
#include <vector>
#include <iostream>

#if defined(_MSC_VER)
    #include <intrin.h>
#endif

#include <boost/multiprecision/cpp_int.hpp>

#include "../series/SeqVector.h"
#include "../text_support.h"

namespace Oliver {
    namespace MPA {

        class whole_number {
        public:
            using limb_t = uint64_t;

            whole_number() = default;
            whole_number(uint64_t x) { if (x) limbs_.push_back(x); }
            explicit whole_number(const std::string& decimal) { from_decimal(decimal); }

            // Basic queries
            size_t size() const { return limbs_.size(); }

            // String conversion
            std::string to_string() const {
                if (is_zero()) return "0";
                whole_number tmp = *this;
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
            friend int compare(const whole_number& a, const whole_number& b) {
                if (a.limbs_.size() != b.limbs_.size())
                    return a.limbs_.size() < b.limbs_.size() ? -1 : 1;
                for (size_t i = a.limbs_.size(); i-- > 0;) {
                    if (a.limbs_[i] != b.limbs_[i])
                        return a.limbs_[i] < b.limbs_[i] ? -1 : 1;
                }
                return 0;
            }
            friend bool operator==(const whole_number& a, const whole_number& b) { return compare(a, b) == 0; }
            friend bool operator!=(const whole_number& a, const whole_number& b) { return compare(a, b) != 0; }
            friend bool operator<(const whole_number& a, const whole_number& b) { return compare(a, b) < 0; }
            friend bool operator<=(const whole_number& a, const whole_number& b) { return compare(a, b) <= 0; }
            friend bool operator>(const whole_number& a, const whole_number& b) { return compare(a, b) > 0; }
            friend bool operator>=(const whole_number& a, const whole_number& b) { return compare(a, b) >= 0; }

            constexpr bool is_zero() const { return limbs_.empty() || (limbs_.size() == 1 && limbs_[0] == 0); }
            constexpr bool is_even() const { return limbs_.empty() || (limbs_[0] & 1) == 0; }
            constexpr bool is_odd() const { return !limbs_.empty() && (limbs_[0] & 1) == 1; }

            // Arithmetic (whole_number)
            whole_number& operator+=(const whole_number& rhs) {
                const size_t n = std::max(limbs_.size(), rhs.limbs_.size());
                limbs_.resize(n, 0);
                uint64_t carry = 0;
                for (size_t i = 0; i < n; ++i) {
                    uint64_t a = i < limbs_.size() ? limbs_[i] : 0;
                    uint64_t b = i < rhs.limbs_.size() ? rhs.limbs_[i] : 0;
                    limbs_[i] = add3(a, b, carry, carry);
                }
                if (carry) limbs_.push_back(carry);
                return *this;
            }
            whole_number& operator-=(const whole_number& rhs) {
                if (*this < rhs) throw std::underflow_error("whole_number: negative result in subtraction");
                uint64_t borrow = 0;
                for (size_t i = 0; i < limbs_.size(); ++i) {
                    uint64_t a = limbs_[i];
                    uint64_t b = (i < rhs.limbs_.size() ? rhs.limbs_[i] : 0);
                    limbs_[i] = sub3(a, b, borrow, borrow);
                }
                normalize();
                return *this;
            }
            whole_number& operator*=(const whole_number& rhs) {
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
            whole_number& operator/=(const whole_number& rhs) {
                whole_number q, r;
                divmod(*this, rhs, q, r);
                limbs_.swap(q.limbs_);
                return *this;
            }
            whole_number& operator%=(const whole_number& rhs) {
                whole_number q, r;
                divmod(*this, rhs, q, r);
                limbs_.swap(r.limbs_);
                return *this;
            }

            // Arithmetic (small)
            whole_number& operator+=(uint64_t rhs) {
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
            whole_number& operator-=(uint64_t rhs) {
                if (*this < whole_number(rhs)) throw std::underflow_error("whole_number: negative result in subtraction");
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
            whole_number& operator*=(uint64_t rhs) {
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
            whole_number& operator/=(uint64_t rhs) {
                if (rhs == 0) throw std::domain_error("whole_number: division by zero");
                div_small(rhs); // returns remainder, ignores it here
                return *this;
            }
            whole_number& operator%=(uint64_t rhs) {
                if (rhs == 0) throw std::domain_error("whole_number: division by zero");
                uint64_t rem = div_small(rhs);
                limbs_.assign(rem ? 1 : 0, rem);
                return *this;
            }

            // Bit shifts
            whole_number& operator<<=(unsigned bits) {
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
            whole_number& operator>>=(unsigned bits) {
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

            // Bitwise operations
            whole_number& operator&=(const whole_number& rhs) {
                const size_t min_size = std::min(limbs_.size(), rhs.limbs_.size());
                for (size_t i = 0; i < min_size; ++i) {
                    limbs_[i] &= rhs.limbs_[i];
                }
                limbs_.resize(min_size);
                normalize();
                return *this;
            }

            whole_number& operator|=(const whole_number& rhs) {
                const size_t max_size = std::max(limbs_.size(), rhs.limbs_.size());
                limbs_.resize(max_size, 0);
                for (size_t i = 0; i < rhs.limbs_.size(); ++i) {
                    limbs_[i] |= rhs.limbs_[i];
                }
                return *this;
            }

            whole_number& operator^=(const whole_number& rhs) {
                const size_t max_size = std::max(limbs_.size(), rhs.limbs_.size());
                limbs_.resize(max_size, 0);
                for (size_t i = 0; i < rhs.limbs_.size(); ++i) {
                    limbs_[i] ^= rhs.limbs_[i];
                }
                normalize();
                return *this;
            }

            whole_number operator~() const {
                whole_number result = *this;
                for (auto& limb : result.limbs_) {
                    limb = ~limb;
                }
                return result;
            }

            // Mathematical functions
            whole_number gcd(const whole_number& other) const {
                if (is_zero()) return other;
                if (other.is_zero()) return *this;
                
                whole_number a = *this;
                whole_number b = other;
                
                while (!b.is_zero()) {
                    whole_number temp = b;
                    b = a % b;
                    a = temp;
                }
                return a;
            }

            whole_number lcm(const whole_number& other) const {
                if (is_zero() || other.is_zero()) return whole_number(0);
                whole_number gcd_val = gcd(other);
                return (*this / gcd_val) * other;
            }

            whole_number pow(int exponent) const {
                if (exponent < 0) throw std::invalid_argument("whole_number: negative exponent not supported");
                if (exponent == 0) return whole_number(1);
                if (is_zero()) return whole_number(0);
                if (exponent == 1) return *this;
                
                whole_number base = *this;
                whole_number result(1);
                
                while (exponent > 0) {
                    if (exponent & 1) result *= base;
                    base *= base;
                    exponent >>= 1;
                }
                return result;
            }

            whole_number root(int n) const {
                if (n <= 0) throw std::invalid_argument("whole_number: non-positive root not supported");
                if (n == 1) return *this;
                if (is_zero()) return whole_number(0);

                // Binary search for the nth root
                whole_number low(1);
                whole_number high = *this;
                whole_number mid, one(1), two(2);

                while (low <= high) {
                    mid = (low + high) / two;
                    whole_number mid_pow = mid.pow(n);
                    whole_number next_pow = (mid + one).pow(n);
                    
                    if ((mid_pow == *this) || (mid_pow < *this && next_pow > *this))
                        return mid;
                        
                    if (mid_pow < *this)
                        low = mid + one;
                    else
                        high = mid - one;
                }
                return high;
            }

            // Non-member style operators
            friend whole_number operator+(whole_number a, const whole_number& b) { a += b; return a; }
            friend whole_number operator-(whole_number a, const whole_number& b) { a -= b; return a; }
            friend whole_number operator*(whole_number a, const whole_number& b) { a *= b; return a; }
            friend whole_number operator/(whole_number a, const whole_number& b) { a /= b; return a; }
            friend whole_number operator%(whole_number a, const whole_number& b) { a %= b; return a; }
            friend whole_number operator+(whole_number a, uint64_t b) { a += b; return a; }
            friend whole_number operator-(whole_number a, uint64_t b) { a -= b; return a; }
            friend whole_number operator*(whole_number a, uint64_t b) { a *= b; return a; }
            friend whole_number operator/(whole_number a, uint64_t b) { a /= b; return a; }
            friend whole_number operator%(whole_number a, uint64_t b) { a %= b; return a; }
            
            // Bitwise operators
            friend whole_number operator&(whole_number a, const whole_number& b) { a &= b; return a; }
            friend whole_number operator|(whole_number a, const whole_number& b) { a |= b; return a; }
            friend whole_number operator^(whole_number a, const whole_number& b) { a ^= b; return a; }
            friend whole_number operator<<(whole_number a, unsigned bits) { a <<= bits; return a; }
            friend whole_number operator>>(whole_number a, unsigned bits) { a >>= bits; return a; }

            // Division: q = a / b; r = a % b
            static void divmod(const whole_number& a, const whole_number& b, whole_number& q, whole_number& r) {
                if (b.is_zero()) throw std::domain_error("whole_number: division by zero");
                if (a < b) { q = whole_number(0); r = a; return; }
                if (b.limbs_.size() == 1) {
                    q = a;
                    uint64_t rem = q.div_small(b.limbs_[0]);
                    r = whole_number(rem);
                    return;
                }
                // Normalize so that highest bit of divisor is set
                unsigned shift = clz64(b.limbs_.back());
                whole_number u = a;
                whole_number v = b;
                if (shift) { u <<= shift; v <<= shift; }
                const size_t n = v.limbs_.size();
                const size_t m = u.limbs_.size() - n;
                // Ensure u has an extra limb
                if (u.limbs_.size() == m + n) u.limbs_.push_back(0);

                q.limbs_.assign(m + 1, 0);

                // Pre-capture pointers for speed
                auto& U = u.limbs_;
                auto& V = v.limbs_;

                // Proper loop: j from m down to 0
                for (size_t j = m + 1; j-- > 0;) {
                    // Estimate qhat = (U[j+n]*B + U[j+n-1]) / V[n-1]
                    uint64_t ujn = U[j + n];
                    uint64_t ujn1 = U[j + n - 1];
                    uint64_t qhat, rhat;
                    div128by64(ujn, ujn1, V[n - 1], qhat, rhat);
                    
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
                whole_number ten(10);
                for (char ch : s) {
                    if (ch >= '0' && ch <= '9') {
                        *this *= 10ull;
                        *this += static_cast<uint64_t>(ch - '0');
                    } else if (ch == '_' || ch == '\'' || std::isspace(static_cast<unsigned char>(ch))) {
                        continue;
                    } else {
                        throw std::invalid_argument("whole_number: invalid decimal string");
                    }
                }
            }

            // Divide by small (uint64_t). Returns remainder; updates this to quotient.
            uint64_t div_small(uint64_t d) {
                if (d == 0) throw std::domain_error("whole_number: division by zero");
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

        /****************************************************************************************************************/

        bool whole_number_tests() {
            using namespace Oliver;
            using namespace Oliver::MPA;
        
            
            namespace mp = boost::multiprecision;
            using boost_int = mp::cpp_int;
            
            // Test values - a mix of large and small numbers
            std::vector<std::string> test_numbers = {
                "0",
                "1",
                "42",
                "123456789",
                "987654321098765432109876543210",
                "123456789098765432112345678909876543211234567890987654321",
                "9999999999999999999999999999999999999999999999999999999"
            };
            
            bool all_tests_passed = true;
            
            // Helper function to test and compare operations
            auto test_operation = [&all_tests_passed](
                const std::string& op_name, 
                const std::string& a_str, 
                const std::string& b_str, 
                const auto& operation, 
                const auto& boost_operation) {
                
                whole_number a_wn(a_str);
                whole_number b_wn(b_str);
                boost_int a_boost(a_str);
                boost_int b_boost(b_str);
                
                std::string wn_result, boost_result;
                
                try {
                    wn_result = operation(a_wn, b_wn).to_string();
                    boost_result = boost_int(boost_operation(a_boost, b_boost)).str();
                    
                    if (wn_result != boost_result) {
                        std::cout << "FAILED: " << op_name << " for " << a_str << " and " << b_str << "\n";
                        std::cout << "  whole_number: " << wn_result << "\n";
                        std::cout << "  boost::cpp_int: " << boost_result << "\n";
                        all_tests_passed = false;
                    } else {
                        std::cout << "PASSED: " << op_name << " for " << a_str << " and " << b_str << "\n";
                    }
                } catch (const std::exception& e) {
                    std::cout << "Exception during " << op_name << " operation: " << e.what() << "\n";
                    all_tests_passed = false;
                }
            };
            
            // Test single-operand operations
            auto test_unary_operation = [&all_tests_passed](
                const std::string& op_name, 
                const std::string& a_str, 
                const auto& operation, 
                const auto& boost_operation) {
                
                whole_number a_wn(a_str);
                boost_int a_boost(a_str);
                
                std::string wn_result, boost_result;
                
                try {
                    wn_result = operation(a_wn).to_string();
                    boost_result = boost_int(boost_operation(a_boost)).str();
                    
                    if (wn_result != boost_result) {
                        std::cout << "FAILED: " << op_name << " for " << a_str << "\n";
                        std::cout << "  whole_number: " << wn_result << "\n";
                        std::cout << "  boost::cpp_int: " << boost_result << "\n";
                        all_tests_passed = false;
                    } else {
                        std::cout << "PASSED: " << op_name << " for " << a_str << "\n";
                    }
                } catch (const std::exception& e) {
                    std::cout << "Exception during " << op_name << " operation: " << e.what() << "\n";
                    all_tests_passed = false;
                }
            };
            
            // Iterate through pairs of numbers to test operations
            for (size_t i = 0; i < test_numbers.size(); i++) {
                for (size_t j = 0; j < test_numbers.size(); j++) {
                    const auto& a_str = test_numbers[i];
                    const auto& b_str = test_numbers[j];
                    
                    std::cout << "\nTesting with: a = " << a_str << ", b = " << b_str << "\n";
                    
                    // Test binary operations
                    test_operation("Addition", a_str, b_str, 
                        [](const whole_number& a, const whole_number& b) { return a + b; },
                        [](const boost_int& a, const boost_int& b) { return a + b; });
                    
                    test_operation("Subtraction", a_str, b_str,
                        [](const whole_number& a, const whole_number& b) { return a - b; },
                        [](const boost_int& a, const boost_int& b) { return a - b; });
                    
                    test_operation("Multiplication", a_str, b_str,
                        [](const whole_number& a, const whole_number& b) { return a * b; },
                        [](const boost_int& a, const boost_int& b) { return a * b; });
                    
                    if (b_str != "0") {
                        test_operation("Division", a_str, b_str,
                            [](const whole_number& a, const whole_number& b) { return a / b; },
                            [](const boost_int& a, const boost_int& b) { return a / b; });
                        
                        test_operation("Modulus", a_str, b_str,
                            [](const whole_number& a, const whole_number& b) { return a % b; },
                            [](const boost_int& a, const boost_int& b) { return a % b; });
                        
                        test_operation("GCD", a_str, b_str,
                            [](const whole_number& a, const whole_number& b) { return a.gcd(b); },
                            [](const boost_int& a, const boost_int& b) { return boost::multiprecision::gcd(a, b); });
                        
                        test_operation("LCM", a_str, b_str,
                            [](const whole_number& a, const whole_number& b) { return a.lcm(b); },
                            [](const boost_int& a, const boost_int& b) { return boost::multiprecision::lcm(a, b); });
                    }
                    
                    test_operation("Bitwise AND", a_str, b_str,
                        [](const whole_number& a, const whole_number& b) { return a & b; },
                        [](const boost_int& a, const boost_int& b) { return a & b; });
                    
                    test_operation("Bitwise OR", a_str, b_str,
                        [](const whole_number& a, const whole_number& b) { return a | b; },
                        [](const boost_int& a, const boost_int& b) { return a | b; });
                    
                    test_operation("Bitwise XOR", a_str, b_str,
                        [](const whole_number& a, const whole_number& b) { return a ^ b; },
                        [](const boost_int& a, const boost_int& b) { return a ^ b; });
                }
                
                // Test unary operations
                const auto& a_str = test_numbers[i];
                
                // test_unary_operation("Bitwise NOT", a_str,
                //     [](const whole_number& a) { return ~a; },
                //     [](const boost_int& a) { return ~a; });
                    
                // Test power and root operations
                if (a_str != "0") {
                    for (int exp = 1; exp <= 4; exp++) {
                        test_unary_operation("Power " + std::to_string(exp), a_str,
                            [exp](const whole_number& a) { return a.pow(exp); },
                            [exp](const boost_int& a) { return boost::multiprecision::pow(a, exp); });
                        
                        if (a_str != "0") {
                            test_unary_operation("Root " + std::to_string(exp), a_str,
                                [exp](const whole_number& a) { return a.root(exp); },
                                [exp](const boost_int& a) { 
                                    if (a == 0) return boost_int(0);
                                    boost_int low(1), high(a), mid, one(1), two(2);
                                    while (low <= high) {
                                        mid = (low + high) / two;
                                        boost_int mid_pow = boost::multiprecision::pow(mid, exp);
                                        boost_int next_pow = boost::multiprecision::pow(mid + one, exp);
                                        if (mid_pow == a || (mid_pow < a && next_pow > a))
                                            return mid;
                                        if (mid_pow < a)
                                            low = mid + one;
                                        else
                                            high = mid - one;
                                    }
                                    return high;
                                });
                        }
                    }
                }
            }
            
            std::cout << "\n" << (all_tests_passed ? "All tests passed!" : "Some tests failed!") << "\n";
            return all_tests_passed;
        }
    }  // end namespace MPA
}  // end namespace Oliver

