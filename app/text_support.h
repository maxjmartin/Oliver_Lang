#pragma once

/*****************************************************************************************/
//
//                           Copyright(C) 2023 Max J Martin
//
//                            This file is part of Oliver.
//                      Oliver is program language interpreter. 
//    
//        This program is free software : you can redistribute it and /or modify
//        it under the terms of the GNU Affero General Public License as published by
//        the Free Software Foundation, either version 3 of the License, or
//        (at your option) any later version.
//    
//        This program is distributed in the hope that it will be useful,
//        but WITHOUT ANY WARRANTY; without even the implied warranty of
//        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
//        GNU Affero General Public License for more details.
//    
//        You should have received a copy of the GNU Affero General Public License
//        along with this program.If not, see <https://www.gnu.org/licenses/>.
//    
//        The author can be reached at: maxjmartin@gmail.com
//
/*****************************************************************************************/

#include <algorithm>
#include <charconv>
#include <format>
#include <locale>
#include <optional>
#include <print>
#include <ranges>
#include <regex>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#include "fmt/core.h"
#include "fmt/args.h"
#include "fmt/chrono.h"
#include "fmt/color.h"
#include "fmt/compile.h"
#include "fmt/format.h"
#include "fmt/format-inl.h"
#include "fmt/os.h"
#include "fmt/ostream.h"
#include "fmt/printf.h"
#include "fmt/ranges.h"
#include "fmt/std.h"
#include "fmt/xchar.h"

#include <boost/locale.hpp>
#include "boost/algorithm/string/trim.hpp"
#include <boost/algorithm/string/trim_all.hpp>

#ifdef _MSC_VER
    #include "Windows.h"
    // #pragma execution_character_set( "utf-8" )
#endif

namespace Oliver {

    using namespace std::string_literals;

    /**************************************************************************************************/
    //
    //                                  'Format_Args' Struct Definition
    //
    //    format_spec ::=  [[fill]align][sign]["#"]["0"][width]["." precision]["L"]["R"][type]
    //    fill        ::=  <a character other than '{' or '}'>
    //    align       ::=  "<" | ">" | "^"
    //    sign        ::=  "+" | "-" | " "
    //    width       ::=  integer | "{" [arg_id] "}"
    //    precision   ::=  integer | "{" [arg_id] "}"
    //    type        ::=  "a" | "A" | "b" | "B" | "c" | "d" | "e" | "E" | "f" | "F" | "g" | "G" |
    //                     "o" | "p" | "s" | "x" | "X"
    //
    //              NOTES: "R" option was added to to determine if a std::string representation of the 
    //                     object is to be created.  
    // 
    //                     The Format_Args struct is used to capture "fmt" formatting arguments
    //                     so that they can be applied to runtime vars.  
    //
    /**************************************************************************************************/

    struct Format_Args {
        int  base  = 10;
        char sign  = '-';
        char align = '<';
        int  width = ~0;
        int  prec  = ~0;
        char type  = 'd';
        char form  = 'g';
        char fill  = ' ';
        char pref  = '\0';
        bool pad   = false;
        bool local = false;
        bool repr  = false;

        std::string print() const {
            return std::format("base({}), sign({}), align({}), width({}), prec({}), type({}), form({}), fill({}), pref({}), pad({}), local({}), repr({})",
                                base,     sign,     align,     width,     prec,     type,     form,     fill,     pref,     pad,     local,     repr);
        }
    };

    /********************************************************************************************/
    //
    //                                Support Function Declarations
    //
    //          These definitions add a few useful and some necessary support functions.
    //          All functions are declared const expressions.  
    //
    /********************************************************************************************/

    void set_current_locale() { // Set the machine's local for proper string formating.
        boost::locale::generator gen;
        std::locale::global(gen(""));
    }

    constexpr bool is_escape_char(char c) {
        constexpr const char escape_chars[] = " \t\r\n\a\f\v\b";
        for (const char& escape_char : escape_chars) {
            if (escape_char == c) {
                return true;
            }
        }
        return false;
    }

    inline std::string to_lower_case(std::string str) noexcept {
        return boost::locale::to_lower(static_cast<const std::string&>(str));
    }

    inline std::string to_upper_case(std::string str) noexcept {
        return boost::locale::to_upper(str);
    }

    inline std::string to_title_case(std::string str) noexcept {
        return boost::locale::to_title(str);
    }

    inline std::string normalize_text(std::string str) noexcept {
        return boost::locale::normalize(str);
    }

    inline std::string left_trim_ws(std::string str) {
        boost::algorithm::trim_left(str);
        return str;
    }

    inline std::string right_trim_ws(std::string str) {
        boost::algorithm::trim_right(str);
        return str;
    }

    inline std::string trim_ws(std::string str) {
        boost::algorithm::trim(str);
        return str;
    }

    inline std::string trim_all_ws(std::string str) {
        boost::algorithm::trim_all(str);
        return str;
    }

    template<class SR>
    constexpr SR&& to_white_space(SR&& str, std::string_view delim) noexcept {

        for (auto i = delim.begin(); i != delim.end(); ++i) {
            std::replace(str.begin(), str.end(), *i, ' ');
        }

        return std::forward<SR>(str);
    }

    constexpr std::vector<std::string> split(std::string str, std::string_view delim) {
        if (delim.empty()) {
            return split(str, " \t\r\n\a\f\v\b");
        }
        for (const char c : delim) {
            for (auto& i : str) {
                if (i == c) {
                    i = ' ';
                }
            }
        }
        auto result = std::vector<std::string>();
        for (std::string& val : std::ranges::split_view(str, ' ') | std::ranges::to<std::vector<std::string>>()) {
            if (not val.empty()) {
                result.emplace_back(std::move(val));
            }
        }
        return result;
    }
    template<typename T>
    constexpr std::optional<T> string_to_number(std::string_view str, int base) {
        // Only allow integral types
        if constexpr (!std::is_integral_v<T>) {
            return std::nullopt;
        }

        T value = 0;
        bool negative = false;
        std::size_t start = 0;

        if (str.empty()) { 
            return std::nullopt; 
        }

        if (str[0] == '-') {
            negative = true;
            ++start;
        }
        else if (str[0] == '+') {
            ++start;
        }

        std::size_t limit = str.size();
        for (std::size_t i = start; i < limit; ++i) {
            if (std::isspace(static_cast<unsigned char>(str[i]))) {
                continue;
            }
            int digit = 0;
            if (str[i] >= '0' && str[i] <= '9') {
                digit = str[i] - '0';
            }
            else if (str[i] >= 'A' && str[i] <= 'Z') {
                digit = str[i] - 'A' + 10;
            }
            else if (str[i] >= 'a' && str[i] <= 'z') {
                digit = str[i] - 'a' + 10;
            }
            else {
                return std::nullopt; 
            }
            if (digit < base) {
                // Prevent overflow/underflow
                if (value > (std::numeric_limits<T>::max() - digit) / base) {
                    return std::nullopt;
                }
                value = value * base + digit;
            } else {
                return std::nullopt;
            }
        }
        if (negative) {
            if constexpr (std::is_signed_v<T>) {
                if (static_cast<std::make_unsigned_t<T>>(value) > static_cast<std::make_unsigned_t<T>>(std::numeric_limits<T>::max()) + 1) {
                    return std::nullopt;
                }
                // Check for overflow when negating value
                if (static_cast<std::make_unsigned_t<T>>(value) == static_cast<std::make_unsigned_t<T>>(std::numeric_limits<T>::max()) + 1) {
                    return std::numeric_limits<T>::min();
                }
                return static_cast<T>(-static_cast<std::make_signed_t<T>>(value));
            } else {
                // Negative sign with unsigned type is invalid
                return std::nullopt;
            }
        }
        return value;
    }

    constexpr auto parse_fmt_args(std::format_parse_context& ctx, Format_Args& fmt_args) {

        bool padding_not_set = true;

        bool prec_definition = false;

        int arg_buffer = 0;

        auto pos = ctx.begin();
        auto end = ctx.end();
        auto lmt = '}';

        char last_c = ' ';
        char      c = ' ';

        while (pos != end && *pos != lmt) {  // The parsing loop for the fmt arguments.

            last_c = c;
            c = std::string{ *pos }.at(0);

            switch (c) {

            case '<':
            case '^':
            case '>':
                fmt_args.align = c;
                fmt_args.fill = last_c;
                break;

            case '+':
            case '-':
            case ' ':
                fmt_args.sign = c;
                break;

            case '#':
                fmt_args.pref = c;
                break;

            case '0':
                if (padding_not_set) {
                    fmt_args.pad = true;
                    padding_not_set = !padding_not_set;
                }
                arg_buffer *= 10;
                break;

            case '.':
                prec_definition = true;
                fmt_args.width = arg_buffer / 10;
                arg_buffer = 0;
                break;

            case 'L':
                fmt_args.local = true;
                break;

            case 'R':
                fmt_args.repr = true;
                break;

            case 'b':
            case 'B':
                fmt_args.type = c;
                fmt_args.base = 2;
                break;
            case 'o':
            case 'O':
                fmt_args.type = c;
                fmt_args.base = 8;
                break;
            case 'x':
            case 'X':
                fmt_args.type = c;
                fmt_args.base = 16;
                break;

            case 'd':
            case 'D':
            case 'g':
            case 'G':
                fmt_args.type = c;
                fmt_args.base = 10;
                break;

            case 'a':
            case 'A':
            case 'e':
            case 'E':
            case 'f':
            case 'F':
                fmt_args.type = c;
                fmt_args.base = 0;
                break;

            case 'c':  // Number conversion to utf character
            case 'C':
                fmt_args.type = c;
                fmt_args.base = -1;
                break;

            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                arg_buffer += c - '0';
                arg_buffer *= 10;
                padding_not_set = false;
                break;
            default:
            }

            std::advance(pos, 1);
        }

        if (fmt_args.pref == '#') {
            fmt_args.pref = fmt_args.type;
        }

        if (prec_definition) {
            fmt_args.prec = arg_buffer / 10;
        }
        else {
            fmt_args.width = arg_buffer / 10;
        }

        return pos;
    }
}
