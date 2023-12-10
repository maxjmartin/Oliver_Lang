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

#include "Var.h"

namespace Oliver {

    /********************************************************************************************/
    //
    //                               'error' Class Definition
    //
    //        The error class provides a wrapper around an error message to pass at
    //        runtime.  
    //
    /********************************************************************************************/


    class error {

        std::string _value;

    public:

        error(std::string val);

        friend std::string   _type_(const error& self);
        friend bool          _is_(const error& self);
        friend std::string   _str_(const error& self, const Format_Args& fmt);
    };

    error::error(std::string val) : _value(val) {
    }

    std::string _type_(const error& self) {
        return "error"s;
    }

    bool _is_(const error& self) {
        return !self._value.empty();
    }

    std::string _str_(const error& self, const Format_Args& fmt) {
        return self._value;
    }
}