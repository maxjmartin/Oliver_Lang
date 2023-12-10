
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
//          along with this program.If not, see < https://www.gnu.org/licenses/>.
//    
//          The author can be reached at: maxjmartin@gmail.com
//
/********************************************************************************************/

#include <iostream>
#include <string>

#include "compiler.h"
#include "evaluater.h"
#include "OliverConfig.h"

int main(int argc, char* argv[]) {

    using namespace Oliver;

    try {
        if (argc == 1) {
            return 0;
        }

        if (argc == 2) {

            var code;
            {
                code = compiler{ TextParser{ argv[1] }.parse() }.compile();
            }

            //fmt::println("code = {}", code);
            //fmt::println("");

            {
                code = evaluator{}.eval(code);
            }

            // fmt::println("code = {}", code);
        }
    }
    catch (std::exception& e) {
        fmt::println("Error during runtime: {}", e.what());
    }
    return 0;
}
