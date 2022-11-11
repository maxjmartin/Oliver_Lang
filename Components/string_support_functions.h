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

#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

namespace Olly {

    static const std::string ESCAPE_CHAR(" \t\r\n\a\f\v\b");

    std::string to_lower(std::string str);                                              // Set all text in a std::string to lower case.
    std::string to_upper(std::string str);                                              // Set all text in a std::string to upper case.

    void ltrim(std::string& s);                                                         // Mutable remove left white space.
    void rtrim(std::string& s);                                                         // Mutable remove right white space.
    void lrtrim(std::string& s);                                                        // Mutable remove left and right white space.

    std::string left_trim(std::string s);                                               // Copy and remove left white space.
    std::string right_trim(std::string s);                                              // Copy and remove right white space.
    std::string trim(std::string s);                                                    // Copy and remove left and right white space.

    static std::vector<std::string> split(std::string str, char delim);                 // Split a string at a single character.
    static std::vector<std::string> split(const std::string& str, std::string delim);   // Split a string using a series of characters

    template<typename T> T to(std::string str);                                         // Convert a string to a specific type T.
    
    /********************************************************************************************/
    //
    //   The functions are inline defined below, since they will need to be used by the base
    //   templated data types managed by the var class.
    //
    /********************************************************************************************/

    inline std::string to_lower(std::string str) {

        std::transform(str.begin(), str.end(), str.begin(), ::tolower);

        return str;
    }

    inline std::string to_upper(std::string str) {

        std::transform(str.begin(), str.end(), str.begin(), ::toupper);

        return str;
    }

    inline void ltrim(std::string& s) {
        if (s.empty()) {
            return;
        }
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
            return !std::isspace(ch);
            }));
    }

    inline void rtrim(std::string& s) {
        if (s.empty()) {
            return;
        }
        s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
            return !std::isspace(ch);
            }).base(), s.end());
    }

    inline void lrtrim(std::string& s) {
        ltrim(s);
        rtrim(s);
    }

    inline std::string left_trim(std::string s) {
        ltrim(s);
        return s;
    }

    inline std::string right_trim(std::string s) {
        rtrim(s);
        return s;
    }

    inline std::string trim(std::string s) {
        lrtrim(s);
        return s;
    }

    inline std::vector<std::string> split(std::string str, char delim) {

        std::stringstream stream;
        stream.str(str);

        std::vector<std::string> tokens;

        while (std::getline(stream, str, delim)) {

            tokens.push_back(str);
        }

        return tokens;
    }

    inline std::vector<std::string> split(const std::string& str, std::string delim) {

        if (delim == "") {
            return split(str, ESCAPE_CHAR);
        }

        std::vector<std::string> tokens, buffer, temp;

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

    template<typename T> inline T to(std::string str) {

        T n;

        std::stringstream stream;

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