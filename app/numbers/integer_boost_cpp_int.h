#pragma once

#include <string>
#include <algorithm>
#include <stdexcept>
#include <cstdint>
#include <compare>
#include <cctype>
#include <limits>
#include <vector>
#include <bit>
#include <utility>
#include <type_traits>
#include <span>

#if defined(_MSC_VER)
    #include <intrin.h>
#endif

#include <boost/multiprecision/cpp_int.hpp>

#include "../series/SeqVector.h"
#include "../series/SeqArray.h"
#include "../text_support.h"

namespace Oliver {
    namespace MPA {

        class integer {
            friend class decimal;
            
            enum sign_t : int8_t { 
                nan = -5, undef = -4, infinity = -3, neg_infinity = -2, negative = -1, zero = 0, positive = 1, pos_infinity = 2
            };

            using word_t   = uint64_t;
            using number_t = boost::multiprecision::cpp_int;

            sign_t   _sign;
            number_t _value;

        public:

            /*****************************************************************************************/
            //
            //                                    Sentinel Values
            //
            /*****************************************************************************************/

            static constexpr integer NaN() {
                integer result(sign_t::nan);
                return result;
            }

            static constexpr integer undefined() {
                integer result(sign_t::undef);
                return result;
            }

            static constexpr integer inf() {
                integer result(sign_t::infinity);
                return result;
            }

            static constexpr integer neg_inf() {
                integer result(sign_t::neg_infinity);
                return result;
            }

            static constexpr integer pos_inf() {
                integer result(sign_t::pos_infinity);
                return result;
            }

            /*****************************************************************************************/
            //
            //                         Public Constructors and Copy, Swap & Move
            //
            /*****************************************************************************************/

            constexpr integer() : 
                _sign(sign_t::zero), _value{0} 
            {}
            
            constexpr integer(int64_t value) : 
                _sign(value == 0 ? 
                    sign_t::zero : 
                    (value < 0   ? 
                    sign_t::negative : sign_t::positive)),
                _value{value < 0 ? 
                    static_cast<word_t>(-value) : 
                    static_cast<word_t>(value)}
            {}

            constexpr integer(const std::string& value, int base = 10) : 
                _sign(sign_t::zero), _value{0} 
            {
                set_numeric_value(value, base);
            }

            constexpr ~integer()                                 = default;
            constexpr integer(integer&& obj) noexcept            = default;
            constexpr integer(const integer& obj)                = default;
            constexpr integer& operator=(integer&& obj) noexcept = default;
            constexpr integer& operator=(const integer& obj)     = default;

            friend constexpr void swap(integer& first, integer& second) noexcept {
                std::swap(first._sign, second._sign);
                first._value.swap(second._value);
            }

            constexpr operator bool() const {
                return _sign != sign_t::zero && (is_finite() || is_infinite());
            }

            /*****************************************************************************************/
            //
            //                           Boolean Operations and Comparison
            //
            /*****************************************************************************************/

            [[nodiscard]] constexpr bool is_negative() const { return _sign == sign_t::negative; }
            [[nodiscard]] constexpr bool is_positive() const { return _sign == sign_t::positive; }
            [[nodiscard]] constexpr bool is_zero()     const { return _sign == sign_t::zero; }
            [[nodiscard]] constexpr bool is_nan()      const { return _sign == sign_t::nan; }
            [[nodiscard]] constexpr bool is_undef()    const { return _sign == sign_t::undef; }
            [[nodiscard]] constexpr bool is_neg_inf()  const { return _sign == sign_t::neg_infinity; }
            [[nodiscard]] constexpr bool is_pos_inf()  const { return _sign == sign_t::pos_infinity; }
            [[nodiscard]] constexpr bool is_finite()   const { return _sign >= sign_t::negative && _sign <= sign_t::positive; }
            [[nodiscard]] constexpr bool is_infinite() const { return _sign == sign_t::neg_infinity || _sign == sign_t::pos_infinity; }
            [[nodiscard]] constexpr bool is_odd()      const { return !is_zero() && (_value & 1); }
            [[nodiscard]] constexpr bool is_even()     const { return !is_odd(); }

            constexpr bool operator==(const integer& b) const {
                if (_sign != b._sign) {
                    return false;
                }
                if (!is_finite()) {
                    return true; // NaN == NaN, etc.
                }
                return _value == b._value;
            }

            constexpr std::partial_ordering operator<=>(const integer& b) const {
                if (!valid_operation(b)) {
                    return std::partial_ordering::unordered;
                }
                if (_sign > b._sign) {
                    return std::partial_ordering::greater;
                }
                if (_sign < b._sign) {
                    return std::partial_ordering::less;
                }

                int cmp = compare_value(_value, b._value);
                if (_sign == sign_t::negative) {    // both negative
                    cmp = -cmp;
                }
                if (cmp > 0) {
                    return std::partial_ordering::greater;
                }
                if (cmp < 0) {
                    return std::partial_ordering::less;
                }
                return std::partial_ordering::equivalent;
            }

            [[nodiscard]] constexpr std::size_t bit_length() const {
                if (!is_finite() || is_zero()) {
                    return 0;
                }
                return static_cast<std::size_t>(std::log2(static_cast<double>(_value)) + 1);
            }

            /*****************************************************************************************/
            //
            //                                    Bitwise Operators
            //
            /*****************************************************************************************/

            constexpr integer& operator&=(const integer& other) {
                if (!valid_operation(other)) { 
                    *this = integer::NaN(); 
                    return *this; 
                }
                _value &= other._value;
                update_sign();
                return *this;
            }

            constexpr integer& operator|=(const integer& other) {
                if (!valid_operation(other)) { 
                    *this = integer::NaN(); 
                    return *this; 
                }
                _value |= other._value;
                update_sign();
                return *this;
            }

            constexpr integer& operator^=(const integer& other) {
                if (!valid_operation(other)) { 
                    *this = integer::NaN(); 
                    return *this; 
                }
                _value ^= other._value;
                update_sign();
                return *this;
            }

            constexpr integer& operator<<=(std::size_t shift) {
                if (!valid_operation()) {
                    *this = integer::NaN();
                    return *this;
                }
                _value <<= shift;
                update_sign();
                return *this;
            }

            constexpr integer& operator>>=(std::size_t shift) {
                if (!valid_operation()) {
                    *this = integer::NaN();
                    return *this;
                }
                _value >>= shift;
                update_sign();
                return *this;
            }

            /*****************************************************************************************/
            //
            //                                   Arithmetic Operators
            //
            /*****************************************************************************************/

            constexpr integer& operator+=(const integer& other) {
                if (!valid_operation(other)) {
                    *this = integer::NaN();
                    return *this;
                }
                _value += other._value;
                update_sign();
                return *this;
            }

            constexpr integer& operator-=(const integer& other) {
                if (!valid_operation(other)) {
                    *this = integer::NaN();
                    return *this;
                }
                _value -= other._value;
                update_sign();
                return *this;
            }

            constexpr integer& operator*=(const integer& other) {
                if (!valid_operation(other)) {
                    *this = integer::NaN();
                    return *this;
                }
                if (is_zero() || other.is_zero()) {
                    *this = integer{};
                    return *this;
                }
                _value *= other._value;
                _sign = (_sign == other._sign) ? sign_t::positive : sign_t::negative;
                update_sign();
                return *this;
            }

            constexpr integer& operator/=(const integer& other) {
                if (!valid_operation(other) || other.is_zero()) {
                    *this = integer::NaN();
                    return *this;
                }
                if (is_zero()) {
                    *this = other;
                    return *this;
                }
                _value /= other._value;
                _sign = (_sign == other._sign) ? sign_t::positive : sign_t::negative;
                update_sign();
                return *this;
            }

            constexpr integer& operator%=(const integer& other) {
                if (!valid_operation(other) || other.is_zero()) {
                    *this = integer::NaN();
                    return *this;
                }
                if (is_zero()) {
                    *this = other;
                    return *this;
                }
                _value %= other._value;
                _sign = (_sign == other._sign) ? sign_t::positive : sign_t::negative;
                update_sign();
                return *this;
            }

            /*****************************************************************************************/
            //
            //                                    Unary Operators
            //
            /*****************************************************************************************/

            constexpr integer operator+() const noexcept {
                return *this;
            }

            constexpr integer operator-() const {
                if (is_finite()) {
                    integer result;
                    result._value = -_value;
                    result._sign = static_cast<sign_t>(-static_cast<int>(result._sign));
                    return result;
                }
                return integer::NaN();
            }

            constexpr integer operator~() const {
                if (!is_finite()) {
                    return integer::NaN();
                }
                
                integer result;
                result._value = ~_value;
                result._sign = static_cast<sign_t>(-static_cast<int>(result._sign));
                result.update_sign();
                
                return result;
            }

            /*****************************************************************************************/
            //
            //                                    Binary Operators
            //
            /*****************************************************************************************/

            constexpr integer operator&(const integer& b) const {
                integer a(*this);
                a &= b;
                return a;
            }

            constexpr integer operator|(const integer& b) const {
                integer a(*this);
                a |= b;
                return a;
            }

            constexpr integer operator^(const integer& b) const {
                integer a(*this);
                a ^= b;
                return a;
            }

            constexpr integer operator<<(std::size_t shift) const {
                integer a(*this);
                a <<= shift;
                return a;
            }

            constexpr integer operator>>(std::size_t shift) const {
                integer a(*this);
                a >>= shift;
                return a;
            }

            constexpr integer operator+(const integer& b) const {
                integer a(*this);
                a += b;
                return a;
            }

            constexpr integer operator-(const integer& b) const {
                integer a(*this);
                a -= b;
                return a;
            }

            constexpr integer operator*(const integer& b) const {
                integer a(*this);
                a *= b;
                return a;
            }

            constexpr integer operator/(const integer& b) const {
                integer a(*this);
                a /= b;
                return a;
            }

            constexpr integer operator%(const integer& b) const {
                integer a(*this);
                a %= b;
                return a;
            }

            static constexpr void divmod(const integer& a, const integer& b, integer& quotient, integer& remainder) {
                if (!a.valid_operation() || !b.valid_operation() || b.is_zero()) {
                    quotient = integer::NaN();
                    remainder = integer::NaN();
                    return;
                }
                boost::multiprecision::divide_qr(a._value, b._value, quotient._value, remainder._value);
                quotient._sign = (a._sign == b._sign) ? sign_t::positive : sign_t::negative;
                remainder._sign = a._sign; // remainder has same sign as dividend
                quotient.update_sign();
                remainder.update_sign();
            }

            /*****************************************************************************************/
            //
            //                                    Move Operators
            //
            /*****************************************************************************************/

            friend constexpr integer operator~(integer&& a) {
                a = ~a;
                return std::move(a);
            }

            friend constexpr integer operator&(integer&& a, const integer& b) {
                a &= b;
                return std::move(a);
            }

            friend constexpr integer operator|(integer&& a, const integer& b) {
                a |= b;
                return std::move(a);
            }

            friend constexpr integer operator^(integer&& a, const integer& b) {
                a ^= b;
                return std::move(a);
            }

            friend constexpr integer operator<<(integer&& a, std::size_t index) {
                a <<= index;
                return std::move(a);
            }

            friend constexpr integer operator>>(integer&& a, std::size_t index) {
                a >>= index;
                return std::move(a);
            }

            friend constexpr integer operator+(integer&& a, const integer& b) {
                a += b;
                return std::move(a);
            }

            friend constexpr integer operator-(integer&& a, const integer& b) {
                a -= b;
                return std::move(a);
            }

            friend constexpr integer operator*(integer&& a, const integer& b) {
                a *= b;
                return std::move(a);
            }

            friend constexpr integer operator/(integer&& a, const integer& b) {
                a /= b;
                return std::move(a);
            }

            friend constexpr integer operator%(integer&& a, const integer& b) {
                a %= b;
                return std::move(a);
            }

            /*****************************************************************************************/
            //
            //                                 Mathematical Functions
            //
            /*****************************************************************************************/

            constexpr integer gcd(const integer& other) const {
                if (!valid_operation() || !other.valid_operation()) {
                    return integer::NaN();
                }
                integer a = abs();
                integer b = other.abs();

                while (!b.is_zero()) {
                    integer temp = b;
                    b = a % b;
                    a = temp;
                }
                return a;
            }

            constexpr integer lcm(const integer& other) const {
                if (!valid_operation(other)) {
                    return integer::NaN();
                }
                if (is_zero() || other.is_zero()) {
                    return integer{};
                }

                integer result = gcd(other);

                if (result.is_zero()) {
                    return result;
                }

                result = (*this / result) * other;
                result._sign = sign_t::positive;
                return result;
            }

            constexpr integer pow(std::size_t exponent) const {
                if (!valid_operation()) {
                    return integer::NaN();
                }
                integer result(*this);
                if (exponent == 0) {
                    result._value = 1;
                    result._sign = sign_t::positive;
                    return result;
                }
                if (is_zero()) {
                    return *this;
                }

                result._value = boost::multiprecision::pow(_value, exponent);
                result._sign = (is_negative() && (exponent & 1)) ? sign_t::negative : sign_t::positive;
                result.update_sign();
                return result;
            }

            constexpr integer abs() const {
                if (is_negative()) {
                    return -(*this);
                }
                return *this;
            }

            /*****************************************************************************************/
            //
            //                                   String Conversion
            //
            /*****************************************************************************************/

            constexpr std::string to_string(std::size_t base = 10) const {
                if (_sign == sign_t::zero) {
                    return "0";
                }
                if (_sign == sign_t::nan) {
                    return "NaN";
                }
                if (_sign == sign_t::undef) {
                    return "undef";
                }
                if (_sign == sign_t::neg_infinity) {
                    return "-Inf";
                }
                if (_sign == sign_t::pos_infinity) {
                    return "+Inf";
                }
            
                std::string result;
                if (is_negative()) {
                    result = "-";
                }
                result += words_to_string(_value, base);
                return result;
            }

            template<typename T>
            constexpr T to_integral() const {
                static_assert(std::is_integral_v<T>, "T must be an integral type");
                
                // Return 0 for non-finite values
                if (!is_finite()) {
                    return T{0};
                }
                
                // Handle zero case
                if (is_zero()) {
                    return T{0};
                }
                
                // Check if T can represent negative values
                constexpr bool T_is_signed = std::is_signed_v<T>;
                
                // If T is unsigned but our value is negative, return 0
                if (!T_is_signed && is_negative()) {
                    return T{0};
                }
                
                // Get the absolute value for range checking
                number_t abs_value = _value;
                if (is_negative()) {
                    abs_value = -abs_value;
                }
                
                // Check against T's limits to prevent UB
                constexpr auto T_max = std::numeric_limits<T>::max();
                constexpr auto T_min = std::numeric_limits<T>::lowest();
                
                // For unsigned types, check against max only
                if constexpr (!T_is_signed) {
                    if (abs_value > static_cast<number_t>(T_max)) {
                        return T_max;  // Clamp to max value
                    }
                    return static_cast<T>(abs_value);
                }
                else {
                    // For signed types, check both bounds
                    if (is_negative()) {
                        // Check against minimum (most negative) value
                        number_t min_abs = static_cast<number_t>(-static_cast<std::make_signed_t<T>>(T_min));
                        if (abs_value > min_abs) {
                            return T_min;  // Clamp to min value
                        }
                        return -static_cast<T>(abs_value);
                    }
                    else {
                        // Check against maximum positive value
                        if (abs_value > static_cast<number_t>(T_max)) {
                            return T_max;  // Clamp to max value
                        }
                        return static_cast<T>(abs_value);
                    }
                }
            }

        private:

            /*****************************************************************************************/
            //
            //                                   Private Constructors
            //
            /*****************************************************************************************/


            constexpr integer(sign_t sign) : 
                _sign(sign), _value{0} 
            {}

            /*****************************************************************************************/
            //
            //                                General Private Methods
            //
            /*****************************************************************************************/


            constexpr bool valid_operation() const {
                return is_finite();
            }

            constexpr bool valid_operation(const integer& other) const {
                return is_finite() && other.is_finite();
            }

            constexpr void update_sign() {
                if (_value == 0) {
                    _sign = sign_t::zero;
                } else if (_value < 0) {
                    _sign = sign_t::negative;
                } else {
                    _sign = sign_t::positive;
                }
            }

            static constexpr int compare_value(const number_t& a, const number_t& b) {
                return (a < b) ? -1 : ((a > b) ? 1 : 0);
            }

            static constexpr std::string words_to_string(const number_t& value, std::size_t base = 10) {
                if (value == 0) {
                    return "0";
                }
                if (base < 2 || base > 36) {
                    return "Invalid base conversion.";
                }
                std::string result;
                number_t temp = value;

                // Handle conversion to different bases
                if (base == 10) {
                    // Use boost's built-in string conversion for base 10
                    result = temp.str();
                } else {
                    // Manual conversion for other bases
                    if (temp == 0) {
                        return "0";
                    }
                    
                    const char digits[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
                    
                    while (temp > 0) {
                        number_t remainder = temp % base;
                        result = digits[static_cast<std::size_t>(remainder)] + result;
                        temp /= base;
                    }
                }

                return result;
            }

            constexpr void set_numeric_value(std::string text, const int& base) {
                if (check_text_for_non_numeric_signs(text)) {
                    return;
                }
                if (base < 2 || base > 36) {
                    *this = integer::NaN();
                    return;
                }
                
                bool negative = false;
                if (!text.empty()) {
                    if (text[0] == '-') {
                        negative = true;
                        text[0] = ' ';
                    }
                    if (text[0] == '+') {
                        text[0] = ' ';
                    }
                }
                
                from_decimal(text);
                if (negative) {
                    _sign = sign_t::negative;
                } else {
                    _sign = sign_t::positive;
                }
                update_sign();
            }

            constexpr bool check_text_for_non_numeric_signs(std::string text) {
                text = to_lower_case(trim_ws(text));
                if (text == "+infinity" || text == "+inf") {
                    _sign = sign_t::pos_infinity;
                    return true;
                }
                else if (text == "-infinity" || text == "-inf") {
                    _sign = sign_t::neg_infinity;
                    return true;
                }
                else if (text == "undefined") {
                    _sign = sign_t::undef;
                    return true;
                }
                else if (text == "nan") {
                    _sign = sign_t::nan;
                    return true;
                }
                return false;
            }

            constexpr void from_decimal(const std::string& s) {
                _value = 0;
                for (char ch : s) {
                    if (ch >= '0' && ch <= '9') {
                        _value *= 10;
                        _value += static_cast<word_t>(ch - '0');
                    } 
                    else if (ch == '_' || ch == '\'' || std::isspace(static_cast<unsigned char>(ch))) {
                        continue;
                    } 
                    else {
                        *this = integer::NaN();
                        return;
                    }
                }
            }
        };

        bool integer_tests() {
            using namespace Oliver;
            using namespace Oliver::MPA;

            namespace mp = boost::multiprecision;
            using boost_int = mp::cpp_int;
            
            // Test values - a mix of large and small numbers
            std::vector<std::string> test_numbers = {
            // Zero and basic numbers
            "0",
            "1",
            "-1",
            "42",
            "-42",
            
            // Edge cases for arithmetic
            "9223372036854775807",  // INT64_MAX
            "-9223372036854775808", // INT64_MIN
            "18446744073709551615", // UINT64_MAX
            
            // Medium sized numbers
            "123456789",
            "-123456789",
            
            // Powers of 2 for bitwise operations
            "2",
            "4",
            "8",
            "65536",
            "4294967296",          // 2^32
            "18446744073709551616", // 2^64
            
            // Large numbers
            "987654321098765432109876543210",
            "-987654321098765432109876543210",
            "123456789098765432112345678909876543211234567890987654321",
            "-123456789098765432112345678909876543211234567890987654321",
            
            // Specific bit patterns
            "340282366920938463463374607431768211455", // 2^128 - 1, all bits set in 128 bits
            "170141183460469231731687303715884105728", // 2^127, just one bit set
            
            // Prime numbers
            "17",
            "997",
            "104729", // 10,000th prime
            
            // Numbers with lots of 9's (testing carrying operations)
            "9",
            "99",
            "999",
            "9999999999",
            "9999999999999999999999999999999999999999999999999999999",
            
            // Numbers with alternating bit patterns
            "1010101010101010101010101010101010101010",
            "12345678901234567890123456789012345678901234567890",
            
            // Numbers near multiples of powers of base (10)
            "999999999999999999999999999999999999999",
            "1000000000000000000000000000000000000000",
            "1000000000000000000000000000000000000001",
            
            // Special cases that might trigger edge cases in math operations
            "18446744073709551617", // 2^64 + 1
            "36893488147419103233", // 2^65 + 1
            "115792089237316195423570985008687907853269984665640564039457584007913129639935", // 2^256 - 1
            
            // Numbers that might cause division issues
            "115792089237316195423570985008687907852589419931798687112530834" // Large prime-like number
            };
            
            bool all_tests_passed = true;
            
            // Timing variables
            double total_integer_time = 0.0;
            double total_boost_time = 0.0;
            
            // Helper function to test and compare operations
            auto test_operation = [&all_tests_passed, &total_integer_time, &total_boost_time](
            const std::string& op_name, 
            const std::string& a_str, 
            const std::string& b_str, 
            const auto& operation, 
            const auto& boost_operation) {
            
            integer a_int(a_str);
            integer b_int(b_str);
            boost_int a_boost(a_str);
            boost_int b_boost(b_str);
            
            std::string int_result, boost_result;
            
            try {
                // Time integer operation
                auto start_int = std::chrono::high_resolution_clock::now();
                auto result_int = operation(a_int, b_int);
                auto end_int = std::chrono::high_resolution_clock::now();
                int_result = result_int.to_string();
                auto duration_int = std::chrono::duration_cast<std::chrono::nanoseconds>(end_int - start_int);
                total_integer_time += duration_int.count();
                
                // Time boost operation
                auto start_boost = std::chrono::high_resolution_clock::now();
                auto result_boost = boost_operation(a_boost, b_boost);
                auto end_boost = std::chrono::high_resolution_clock::now();
                boost_result = boost_int(result_boost).str();
                auto duration_boost = std::chrono::duration_cast<std::chrono::nanoseconds>(end_boost - start_boost);
                total_boost_time += duration_boost.count();
                
                if (int_result != boost_result) {
                std::cout << "FAILED: " << op_name << " for " << a_str << " and " << b_str << "\n";
                std::cout << "  integer: " << int_result << "\n";
                std::cout << "  boost::cpp_int: " << boost_result << "\n";
                all_tests_passed = false;
                // } else {
                //     std::cout << "PASSED: " << op_name << " for " << a_str << " and " << b_str << "\n";
                }
            } catch (const std::exception& e) {
                std::cout << "Exception during " << op_name << " operation: " << e.what() << "\n";
                all_tests_passed = false;
            }
            };
            
            // Test single-operand operations
            auto test_unary_operation = [&all_tests_passed, &total_integer_time, &total_boost_time](
            const std::string& op_name, 
            const std::string& a_str, 
            const auto& operation, 
            const auto& boost_operation) {
            
            integer a_int(a_str);
            boost_int a_boost(a_str);
            
            std::string int_result, boost_result;
            
            try {
                // Time integer operation
                auto start_int = std::chrono::high_resolution_clock::now();
                auto result_int = operation(a_int);
                auto end_int = std::chrono::high_resolution_clock::now();
                int_result = result_int.to_string();
                auto duration_int = std::chrono::duration_cast<std::chrono::nanoseconds>(end_int - start_int);
                total_integer_time += duration_int.count();
                
                // Time boost operation
                auto start_boost = std::chrono::high_resolution_clock::now();
                auto result_boost = boost_operation(a_boost);
                auto end_boost = std::chrono::high_resolution_clock::now();
                boost_result = boost_int(result_boost).str();
                auto duration_boost = std::chrono::duration_cast<std::chrono::nanoseconds>(end_boost - start_boost);
                total_boost_time += duration_boost.count();
                
                if (int_result != boost_result) {
                std::cout << "FAILED: " << op_name << " for " << a_str << "\n";
                std::cout << "  integer: " << int_result << "\n";
                std::cout << "  boost::cpp_int: " << boost_result << "\n";
                all_tests_passed = false;
                // } else {
                //     std::cout << "PASSED: " << op_name << " for " << a_str << "\n";
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
                [](const integer& a, const integer& b) { return a + b; },
                [](const boost_int& a, const boost_int& b) { return a + b; });
                
                test_operation("Subtraction", a_str, b_str,
                [](const integer& a, const integer& b) { return a - b; },
                [](const boost_int& a, const boost_int& b) { return a - b; });
                
                test_operation("Multiplication", a_str, b_str,
                [](const integer& a, const integer& b) { return a * b; },
                [](const boost_int& a, const boost_int& b) { return a * b; });
                
                if (b_str != "0") {
                test_operation("Division", a_str, b_str,
                    [](const integer& a, const integer& b) { return a / b; },
                    [](const boost_int& a, const boost_int& b) { return a / b; });
                
                test_operation("Modulus", a_str, b_str,
                    [](const integer& a, const integer& b) { return a % b; },
                    [](const boost_int& a, const boost_int& b) { return a % b; });
                
                test_operation("GCD", a_str, b_str,
                    [](const integer& a, const integer& b) { return a.gcd(b); },
                    [](const boost_int& a, const boost_int& b) { return boost::multiprecision::gcd(a, b); });
                
                test_operation("LCM", a_str, b_str,
                    [](const integer& a, const integer& b) { return a.lcm(b); },
                    [](const boost_int& a, const boost_int& b) { return boost::multiprecision::lcm(a, b); });
                }
                
                test_operation("Bitwise AND", a_str, b_str,
                [](const integer& a, const integer& b) { return a & b; },
                [](const boost_int& a, const boost_int& b) { return a & b; });
                
                test_operation("Bitwise OR", a_str, b_str,
                [](const integer& a, const integer& b) { return a | b; },
                [](const boost_int& a, const boost_int& b) { return a | b; });
                
                test_operation("Bitwise XOR", a_str, b_str,
                [](const integer& a, const integer& b) { return a ^ b; },
                [](const boost_int& a, const boost_int& b) { return a ^ b; });
            }
            
            // Test unary operations
            const auto& a_str = test_numbers[i];
            
            // Test power operations - note: root operation commented out as it may not be available in integer class
            if (a_str != "0") {
                for (int exp = 1; exp <= 4; exp++) {
                test_unary_operation("Power " + std::to_string(exp), a_str,
                    [exp](const integer& a) { return a.pow(exp); },
                    [exp](const boost_int& a) { return boost::multiprecision::pow(a, exp); });
                
                // Root operation commented out since it's not part of the integer class implementation
                /*
                if (a_str != "0") {
                    test_unary_operation("Root " + std::to_string(exp), a_str,
                    [exp](const integer& a) { return a.root(exp); },
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
                */
                }
            }
            }
            
            // Performance comparison
            std::cout << "\n" << (all_tests_passed ? "All tests passed!" : "Some tests failed!") << "\n";
            std::cout << "\nPerformance Comparison:\n";
            
            if (total_integer_time < total_boost_time) {
            std::cout << "integer class was faster!\n";
            } else if (total_boost_time < total_integer_time) {
            std::cout << "boost::cpp_int was faster!\n";
            } else {
            std::cout << "Both implementations performed equally!\n";
            }
            
            std::cout << "Total integer class time: " << (total_integer_time / 1000000.0) << " ms\n";
            std::cout << "Total boost::cpp_int time: " << (total_boost_time / 1000000.0) << " ms\n";
            
            return all_tests_passed;
        }

    }  // end namespace MPA
}  // end namespace Oliver
