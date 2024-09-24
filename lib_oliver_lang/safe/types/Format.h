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

#include "Var.h"
#include "Number.h"

namespace Oliver {


    /********************************************************************************************/
    //
    //                               'format' Class Definition
    //
    //        The format class provides a wrapper around normal C++ formats.  Why
    //        re-invent the wheel?
    //
    /********************************************************************************************/


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
    //                   "o" | "p" | "s" | "x" | "X"
    //
    //              NOTES: "R" option was added to to determine if a std::string representation of the 
    //                     object is to be created.  
    // 
    //                     The Format_Args struct is used to capture "fmt" formatting arguments
    //                     so that they can be applied to runtime vars.  
    //
    /**************************************************************************************************/


    class format {

        std::string _value;
        Format_Args _args;

    public:

        format();
        format(std::string_view str);

        friend std::string _type_(const format& self);
        friend bool          _is_(const format& self);
        friend order       _comp_(const format& self, const var& other);
        friend std::string  _str_(const format& self, const Format_Args& fmt);

        Format_Args get_args();
    };

    format::format() : _value{}, _args{} {
    }

    format::format(std::string_view str) : _value{ str }, _args{} {

        bool padding_not_set = true;

        bool prec_definition = false;

        int arg_buffer = 0;

        auto pos = str.begin();

        char last_c = ' ';
        char      c = ' ';

        while (pos != str.end() && *pos != '}') {  // The parsing loop for the fmt arguments.

            last_c = c;
                 c = *pos;

            switch (c) {

            case '<':
            case '^':
            case '>':
                _args.align = c;
                _args.fill = last_c;
                break;

            case '+':
            case '-':
            case ' ':
                _args.sign = c;
                break;

            case '#':
                _args.pref = c;
                break;

            case '0':
                if (padding_not_set) {
                    _args.pad = true;
                    padding_not_set = !padding_not_set;
                }
                arg_buffer *= 10;
                break;

            case '.':
                prec_definition = true;
                _args.width = arg_buffer / 10;
                arg_buffer = 0;
                break;

            case 'L':
                _args.local = true;
                break;

            case 'R':
                _args.repr = true;
                break;

            case 'b':
            case 'B':
                _args.type = c;
                _args.base = 2;
                break;
            case 'o':
            case 'O':
                _args.type = c;
                _args.base = 8;
                break;
            case 'x':
            case 'X':
                _args.type = c;
                _args.base = 16;
                break;

            case 'd':
            case 'D':
            case 'g':
            case 'G':
                _args.type = c;
                _args.base = 10;
                break;

            case 'a':
            case 'A':
            case 'e':
            case 'E':
            case 'f':
            case 'F':
                _args.type = c;
                _args.base = 0;
                break;

            case 'c':  // Number conversion to utf character
            case 'C':
                _args.type = c;
                _args.base = -1;
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

        if (_args.pref == '#') {
            _args.pref = _args.type;
        }

        if (prec_definition) {
            _args.prec = arg_buffer / 10;
        }
        else {
            _args.width = arg_buffer / 10;
        }

        fmt::println("\n_args = {}\n", _args.print());
    }

    inline Format_Args format::get_args() {
        return _args;
    }

    std::string _type_(const format& self) {
        return "format"s;
    }

    bool _is_(const format& self) {
        return !self._value.empty();
    }

    order _comp_(const format& self, const var& other) {

        const format* s = other.cast<format>();

        if (s) {

            if (self._value > s->_value) {
                return order::greater;
            }

            if (self._value < s->_value) {
                return order::less;
            }

            return order::equivalent;
        }
        return order::unordered;
    }

    std::string _str_(const format& self, const Format_Args& fmt) {
        return "`" + self._value + "`";
    }
}