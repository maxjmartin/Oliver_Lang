#ifndef REGEX_STRINGS_H
#define REGEX_STRINGS_H

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

#include <string>

// A match for a single goute string "(['])(?:(?=(\\?))\\2.)*?\\1",
// Just change the begining quotation for a double quote.

namespace Olly {

    static const std::string HEX_REG_STR = "^0[xX][0-9a-fA-F]*$";
                                        /*
                                          ^               The string must begin with pattern.
                                          0               A zero must preceed other characters.
                                          [xX]            Followed by either an x or an X.
                                          [0-9a-fA-F]*    Then any valid hexidecimal digit.
                                          $               The string must end with pattern.
                                        */

    static const std::string OCT_REG_STR = "^0[1-7][0-7]*$";
                                        /*
                                          ^               The string must begin with pattern.
                                          0               A zero must preceed other characters.
                                          [1-7]           Followed by an octal digit greater than 0.
                                          [0-7]*          Followed by any valid octal digit.
                                          $               The string must end with pattern.
                                        */

    static const std::string BIN_REG_STR = "^0[bB][0-1]*$";
                                        /*
                                          ^               The string must begin with pattern.
                                          0               A zero must preceed other characters.
                                          [bB]            Followed by either an b or an B.
                                          [0-1]*          Folloed by any valid binary digit.
                                          $               The string must end with pattern.
                                        */

    static const std::string BASE_INT_STR = "[1-9][0-9]*";
                                        /*
                                          [1-9]           Any single digit greater than zero.
                                          [0-9]*          Followed by any number of digits.
                                        */

    static const std::string WHL_REG_STR = "^[Uu]" + BASE_INT_STR + "$";
                                        /*
                                          ^               The string must begin with pattern.
                                          [Uu]            A unsigned number identifier.
                                          BASE_INT_STR    An integral number pattern.
                                          $               The string must end with pattern.
                                        */

    static const std::string INT_REG_STR = "^[+-]?" + BASE_INT_STR + "$";
                                        /*
                                          ^               The string must begin with pattern.
                                          [+-]?           Optional plus or minus.
                                          BASE_INT_STR    An integral number pattern.
                                          $               The string must end with pattern.
                                        */

    static const std::string BASE_DEC_STR = "[+-]?([0-9]+\\.[0-9]*|\\.[0-9]+)([Ee][+-]?[0-9])?([Ss][0-9]+)?";
                                        /*
                                          [+-]?           Optional plus or minus.
                                          (
                                            [0-9]+        One or more combination of digits.
                                            \\.           A required decimal place.
                                            [0-9]*        Zero or more combination of digits.
                                            |             Or....
                                            \\.           A required decimal place
                                            [0-9]+        One or more digits.
                                          )
                                          (
                                            [Ee]          Exponent symbol.
                                            [+-]?         Optional plus or minus.
                                            [0-9]         Any combination of digits.
                                          )?              Exponent group is optional.
                                          (
                                            [Ss]          Scale symbol.
                                            [0-9]+        One or more combination of digits.
                                          )?              Scale group is optional.
                                        */

    static const std::string DEC_REG_STR = "^" + BASE_DEC_STR + "$";
                                        /*
                                          ^               The string must begin with pattern.
                                          BASE_DEC_STR    A decimal number pattern.
                                          $               The string must end with pattern.
                                        */

    static const std::string RAT_REG_STR = "^[+-]?(" + BASE_INT_STR + ")?([\\s+])?" + BASE_INT_STR + "([\\s+]?[/][\\s+]?)" + BASE_INT_STR + "$";
                                        /*
                                          [+-]?           Optional plus or minus.
                                          ^               The string must begin with pattern.
                                          (
                                            DEC_REG_STR   Decimal number pattern.
                                          )?              Decimal number is optional.
                                          (
                                            [\\s+]        Any whitespace characters.
                                          )?              Whitespace is optional.
                                          DEC_REG_STR     Numerator, which is required.
                                          (
                                            [\\s+]?       Optional whitespace.
                                            [/]           Rational deliminator.
                                            [\\s+]?       Optional whitespace.
                                          )
                                          DEC_REG_STR     Denominator, which is required.
                                          $               The string must end with pattern.
                                        */
}
#endif // REGEX_STRINGS_H