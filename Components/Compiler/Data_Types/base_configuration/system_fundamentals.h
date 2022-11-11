#ifndef SYSTEM_FUNDAMENTALS_H
#define SYSTEM_FUNDAMENTALS_H

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
#include <bitset>
#include <cctype>
#include <deque>
#include <functional>
#include <iostream>
#include <map>
#include <regex>
#include <sstream>
#include <utility>
#include <vector>

#ifdef _MSC_VER
#include "Windows.h"
#endif

#include "regex_strings.h"

/*  Place holder for porting over to another operating system.
#ifdef _MSC_VER

#else

#endif

*/

namespace Olly {

#if _WIN32 || _WIN64
#if _WIN64
    using   int_type = long int;
    using  long_type = long long int;
    using  real_type = long double;
    using  uint_type = unsigned long int;
    using ulong_type = unsigned long long int;
#else
    using   int_type = int;
    using  long_type = long int;
    using  real_type = double;
    using  uint_type = unsigned int;
    using ulong_type = unsigned long int;
#endif
#endif

#if __GNUC__
#if __x86_64__ || __ppc64__
    using   int_type = long int;
    using  long_type = long long int;
    using  real_type = long double;
    using  uint_type = unsigned long int;
    using ulong_type = unsigned long long int;
#else
    using   int_type = int;
    using  long_type = long int;
    using  real_type = double;
    using  uint_type = unsigned int;
    using ulong_type = unsigned long int;
#endif
#endif

    /********************************************************************************************/
    //
    //                             Fundamental System Declarations
    //
    //          The declarations below should be used throughout the entire project.
    //
    /********************************************************************************************/

    typedef     bool                        bool_type;
    typedef     std::size_t                 size_type;
    typedef     std::string                 str_type;
    typedef     std::stringstream           stream_type;
    typedef     std::vector<str_type>       tokens_type;
    typedef     std::regex                  regex_type;

    static const real_type           NOT_A_NUMBER = std::numeric_limits<real_type>::quiet_NaN();

    static const regex_type          WHL_REGX(WHL_REG_STR, std::regex::optimize);
    static const regex_type          INT_REGX(INT_REG_STR, std::regex::optimize);
    static const regex_type          DEC_REGX(DEC_REG_STR, std::regex::optimize);
    static const regex_type          HEX_REGX(HEX_REG_STR, std::regex::optimize);
    static const regex_type          OCTAL_REGX(OCT_REG_STR, std::regex::optimize);
    static const regex_type          BINARY_REGX(BIN_REG_STR, std::regex::optimize);
    static const regex_type          RAT_REGX(RAT_REG_STR, std::regex::optimize);

    static const std::hash<str_type> DEFAULT_HASH_FUNCTION;

    static const tokens_type         ENCLOSURE_WORDS{
                                            "(", ")", "'", "'", "\"", "\"", "[", "]", "{", "}", "`", "`"
                                        };

    // TODO: conform string to the regex string header.
    static const regex_type   REAL_REGEX("((\\+|-)?[[:digit:]]+)(\\.(([[:digit:]]+)?))?((e|E)((\\+|-)?)[[:digit:]]+)?", std::regex_constants::ECMAScript | std::regex_constants::optimize);
}
#endif // SYSTEM_FUNDAMENTALS_H