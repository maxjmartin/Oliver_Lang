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

#include <deque>
#include <type_traits>

#include "tools.h"

//#ifdef _MSC_VER
//    #include "Windows.h"
//    #define _CRTDBG_MAP_ALLOC
//    #include <stdlib.h>  
//    #include <crtdbg.h>
//namespace Oliver {
//
//    _CrtMemState take_mem_snapshot() {
//        _CrtMemState mem_state;
//        _CrtMemCheckpoint(&mem_state);
//        return mem_state;
//    }
//
//    void detect_mem_leak(_CrtMemState start) {
//        _CrtMemState end = take_mem_snapshot();
//        _CrtMemState dif;
//        _CrtMemCheckpoint(&start);
//        if (_CrtMemDifference(&dif, &end, &start))
//        {
//            OutputDebugString("-----------_CrtMemDumpStatistics ---------");
//            _CrtMemDumpStatistics(&dif);
//            OutputDebugString("-----------_CrtMemDumpAllObjectsSince ---------");
//            _CrtMemDumpAllObjectsSince(&end);
//            OutputDebugString("-----------_CrtDumpMemoryLeaks ---------");
//            _CrtDumpMemoryLeaks();
//        }
//    }
//}
//#endif

namespace Oliver {

    template<typename A, typename B, typename R = std::common_type_t<A, B> >
    R max_val(A a, B b) {
        return a < b ? b : a;
    }

    template<typename A, typename B, typename R = std::common_type_t<A, B> >
    R min_val(A a, B b) {
        return a < b ? a : b;
    }

    template <typename Object >
    concept HasSizeMethod = requires(Object) {
        { std::is_member_function_pointer<decltype(&Object::size)>::value };
    };

    //template <typename Object >
    //concept HasSubScriptOp = requires(Object) {
    //    { std::is_member_function_pointer<decltype(&Object::operator[])>::value };
    //};

    //template <typename Value, typename Implementation>
    //concept IsDeque = requires(Implementation) {
    //    { std::is_same<Implementation, std::deque<Value>>::value };
    //};

    //template <typename Object>
    //concept HasSizeMethod = requires(Object obj) {
    //    { obj.size() } -> std::convertible_to<std::size_t>;
    //};

    template <class F>
    struct Y {
        F f; // the lambda will be stored here

        // a forwarding operator():
        template <class... Args>
        decltype(auto) operator()(Args&&... args) const {
            // we pass ourselves to f, then the arguments.
            return f(*this, std::forward<Args>(args)...);
        }
    };

    template<typename... Args>
    constexpr std::size_t size_of_arg_pack(Args...) {
        return sizeof...(Args);
    }

    template <typename Object >
    concept HasReserveMethod = requires(Object) {
        { std::is_member_function_pointer<decltype(&Object::reserve)>::value };
    };
}
