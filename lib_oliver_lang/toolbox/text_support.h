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
#include <locale>
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

#ifdef _MSC_VER
    #include "Windows.h"
    #pragma execution_character_set( "utf-8" )
#endif

using namespace std::string_literals;

namespace Oliver {

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
            return fmt::format("base({}), sign({}), align({}), width({}), prec({}), type({}), form({}), fill({}), pref({}), pad({}), local({}), repr({})",
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

    static const std::string EscapeChars(" \t\r\n\a\f\v\b");

    constexpr bool is_escape_char(char c);

    template<class SR>                                  // A string refrence is used so both string and string_views are compatable.
    constexpr SR&& to_lower_case(SR&& str) noexcept;    // A move operation is used as views typically are not be passed by refrence.
    template<class SR>
    constexpr SR&& to_upper_case(SR&& str) noexcept;

    constexpr std::string_view  left_trim_ws(std::string_view str);
    constexpr std::string_view right_trim_ws(std::string_view str);
    constexpr std::string_view       trim_ws(std::string_view str);

    template<class SR>
    constexpr SR&& to_white_space(SR&& str, std::string_view delim) noexcept;

    constexpr std::vector<std::string> split(std::string str, std::string_view delim = "");

    constexpr auto parse_fmt_args(fmt::format_parse_context& ctx, Format_Args& fmt_args);

    /********************************************************************************************/
    //
    //                              Support Function Implimentations
    //
    /********************************************************************************************/

    constexpr bool is_escape_char(char c) {
        return EscapeChars.find_first_of(c) != std::string_view::npos;
    }

    template<class SR>
    constexpr SR&& to_lower_case(SR&& str) noexcept {  // TODO: add concenpt qualifiers.

        std::transform(str.begin(), str.end(), str.begin(),
            [](unsigned char c) -> unsigned char { return std::tolower(c);
        });

        return std::forward<SR>(str);
    }

    template<class SR>
    constexpr SR&& to_upper_case(SR&& str) noexcept {

        std::transform(str.begin(), str.end(), str.begin(),
            [](unsigned char c) -> unsigned char { return std::toupper(c);
            });

        return std::forward<SR>(str);
    }

    constexpr std::string_view left_trim_ws(std::string_view str) {
        if (!str.empty()) {
            auto i = str.find_first_not_of(EscapeChars);

            if (i != str.npos) {
                if (i > 0) {
                    str.remove_prefix(i);
                }
            }
        }
        return str;
    }

    constexpr std::string_view right_trim_ws(std::string_view str) {
        if (!str.empty()) {
            auto i = str.find_last_not_of(EscapeChars);

            if (i != str.npos) {
                i += 1;
                if (i < str.size()) {
                    str.remove_suffix(str.size() - i);
                }
            }
        }
        return str;
    }

    constexpr std::string_view trim_ws(std::string_view str) {
        return left_trim_ws(right_trim_ws(str));
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
            return split(str, EscapeChars);
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
    std::optional<T> to(std::string_view str) {

        T value{};

#if __cpp_lib_to_chars >= 202306L
        if (std::from_chars(str.data(), str.data() + str.size(), value))
#else
        if (std::from_chars(str.data(), str.data() + str.size(), value).ec == std::errc{})
#endif
            return value;
        else
            return std::nullopt;
    };

    constexpr auto parse_fmt_args(fmt::format_parse_context& ctx, Format_Args& fmt_args) {

        bool padding_not_set = true;

        bool prec_definition = false;

        int arg_buffer = 0;

        auto pos = ctx.begin();

        char last_c = ' ';
        char      c = ' ';

        while (pos != ctx.end() && *pos != '}') {  // The parsing loop for the fmt arguments.

            last_c = c;
            c = *pos;

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
            }

            ++pos;
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