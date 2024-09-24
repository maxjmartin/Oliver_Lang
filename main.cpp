
/********************************************************************************************/
//
//                           Copyright(C) 2023 Max J Martin
//
//                            This file is part of Oliver.
//                      Oliver is program language interpreter. 
//    
//          This program is free software : you can redistribute it and /or modify
//          it under the terms of the GNU Affero General Public License as published by
//          the Free Software Foundation, either version 3 of the License, or
//          (at your option) any later version.
//    
//          This program is distributed in the hope that it will be useful,
//          but WITHOUT ANY WARRANTY; without even the implied warranty of
//          MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
//          GNU Affero General Public License for more details.
//    
//          You should have received a copy of the GNU Affero General Public License
//          along with this program.If not, see <https://www.gnu.org/licenses/>.
//    
//          The author can be reached at: maxjmartin@gmail.com
//
/********************************************************************************************/

#include "oliver_lang.h"

int main(int argc, char* argv[]) {

    using namespace std;
    using namespace fmt;
    using namespace Oliver;

    println("{}", to_lower_case("Hello!"s));
    println("{}", to_upper_case("Hello!"s));

    println("{}", left_trim_ws("    \t Hello!"s));
    println("{}", right_trim_ws("Hello!     \t"s));
    println("{}", trim_ws("    \t Hello!     \t"s));

    auto test_str = "1 2 3 4 5 6 7 8 9 0"s;
    auto v = split(test_str);
    println("{}", test_str);
    println("{}", v);

    var a{ nullptr };
    var b{ number(8) };
    println("Prior to sub scope:");
    println("\ta = {}",   a);
    println("\tb = {}\n", b);
    {
        auto x = new int(42);
        a = number("4");
        var c(x);
        b = move(c);
        delete x;
        println("Prior to sub scope:");
        println("\ta = {}",   a);
        println("\tb = {}",   b);
        println("\tc = {}\n", c);
    }
    a = text("I'm a text string.");
    println("After the sub scope:");
    println("\ta = {}",   a);
    println("\tb = {}", *b.cast<int>());

    println("\tb.get(a) = {}\n", b.get(a * a));  // Equivilent to operator[].

    return 0;
}
