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
//        along with this program.If not, see < https://www.gnu.org/licenses/>.
//    
//        The author can be reached at: maxjmartin@gmail.com
//
/*****************************************************************************************/

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <sstream>
#include <string>
#include <string_view>

#include "text_support.h"

namespace Oliver {

    /********************************************************************************************/
    //
    //                              'TextReader' class definition
    //
    //        The TextReader class opens a file and then passes each individual character
    //        to the TextReader for evaluation.  If a valid file is not opened then the string
    //        itself is iterated over for reading.  
    //
    /********************************************************************************************/

    class TextReader {

        char                    _c;
        std::fstream            _input;
        bool                    _is_file;
        std::stringstream       _stream;
        std::recursive_mutex    _mutex;

    public:

        TextReader(const std::string_view inp);
        ~TextReader();

        char next();
        char peek() const;

        std::string get_line();

        bool is();

        bool is_file() const;

    private:
        TextReader()                      = delete;
        TextReader(const TextReader& obj) = delete;
    };
}