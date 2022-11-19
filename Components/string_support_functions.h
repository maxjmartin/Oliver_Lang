#ifndef STRING_SUPPORT_FUNCTIONS_H
#define STRING_SUPPORT_FUNCTIONS_H

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

//#include <algorithm>
//#include <sstream>
//#include <string>
//#include <vector>

#include "Compiler/Data_Types/base_configuration/system_fundamentals.h"

namespace Olly {

    static const str_type ESCAPE_CHAR(" \t\r\n\a\f\v\b");

    str_type to_lower(str_type str);                                  // Set all text in a str_type to lower case.
    str_type to_upper(str_type str);                                  // Set all text in a str_type to upper case.

    void ltrim(str_type& s);                                          // Mutable remove left white space.
    void rtrim(str_type& s);                                          // Mutable remove right white space.
    void lrtrim(str_type& s);                                         // Mutable remove left and right white space.

    str_type left_trim(str_type s);                                   // Copy and remove left white space.
    str_type right_trim(str_type s);                                  // Copy and remove right white space.
    str_type trim(str_type s);                                        // Copy and remove left and right white space.

    static tokens_type split(str_type str, char delim);               // Split a string at a single character.
    static tokens_type split(const str_type& str, str_type delim);    // Split a string using a series of characters

    template<typename T> T to(str_type str);                          // Convert a string to a specific type T.
    
    /********************************************************************************************/
    //
    //   The functions are inline defined below, since they will need to be used by the base
    //   templated data types managed by the var class.
    //
    /********************************************************************************************/

    inline str_type to_lower(str_type str) {

        std::transform(str.begin(), str.end(), str.begin(), ::tolower);

        return str;
    }

    inline str_type to_upper(str_type str) {

        std::transform(str.begin(), str.end(), str.begin(), ::toupper);

        return str;
    }

    inline void ltrim(str_type& s) {
        if (s.empty()) {
            return;
        }
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
            return !std::isspace(ch);
            }));
    }

    inline void rtrim(str_type& s) {
        if (s.empty()) {
            return;
        }
        s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
            return !std::isspace(ch);
            }).base(), s.end());
    }

    inline void lrtrim(str_type& s) {
        ltrim(s);
        rtrim(s);
    }

    inline str_type left_trim(str_type s) {
        ltrim(s);
        return s;
    }

    inline str_type right_trim(str_type s) {
        rtrim(s);
        return s;
    }

    inline str_type trim(str_type s) {
        lrtrim(s);
        return s;
    }

    inline tokens_type split(str_type str, char delim) {

        stream_type stream;
        stream.str(str);

        tokens_type tokens;

        while (std::getline(stream, str, delim)) {

            tokens.push_back(str);
        }

        return tokens;
    }

    inline tokens_type split(const str_type& str, str_type delim) {

        if (delim == "") {
            return split(str, ESCAPE_CHAR);
        }

        tokens_type tokens, buffer, temp;

        char d;

        d = delim.back();
        delim.pop_back();

        tokens = split(str, d);

        while (delim.size()) {

            d = delim.back();
            delim.pop_back();

            buffer = tokens;
            tokens.clear();

            for (std::size_t i = 0, stop = buffer.size(); i < stop; i += 1) {

                temp = split(buffer.at(i), d);

                for (std::size_t j = 0, stop = temp.size(); j < stop; j += 1) {

                    tokens.push_back(temp.at(j));
                }

                temp.clear();
            }
        }

        return tokens;
    }

    template<typename T> inline T to(str_type str) {

        T n;

        stream_type stream;

        try {
            stream << trim(str);
            stream >> n;
        }
        catch (...) {
            n = T();
        }

        return n;
    }

}
#endif // STRING_SUPPORT_FUNCTIONS_H