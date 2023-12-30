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
#include "Number.h"

namespace Oliver {

    /********************************************************************************************/
    //
    //                               'list' Class Definition
    //
    //          The list class is implemented as a wrapper around a std::vector<var>.  
    //          The order of sequence for a list is reversed from that of a vector.   
    //
    /********************************************************************************************/

    class list {

        std::vector<var> _list;

    public:

        list();
        list(var x);

        friend std::string          _type_(const list& self);
        friend std::size_t     _size_type_(const list& self);
        friend bool                   _is_(const list& self);
        friend auto                 _comp_(const list& self, const var& other);

        friend std::string           _str_(const list& self, const Format_Args& fmt);

        friend var                  _lead_(list& self);
        friend var                  _push_(list& self, var& other);
        friend var                  _drop_(list& self);
        friend var                 _shift_(list& self);
        friend var               _reverse_(list& self);

        friend var                   _add_(list& self, var& other);
    };

    /********************************************************************************************/
    //
    //                                 'list' Class Implementation
    //
    /********************************************************************************************/

    list::list() : _list() {
    }

    list::list(var x) : _list() {
        _list.push_back(x);
    }

    std::string _type_(const list& self) {
        return "list"s;
    }

    std::size_t _size_type_(const list& self) {
        return self._list.size();
    }

    bool _is_(const list& self) {
        return self._list.size();
    }

    auto _comp_(const list& self, const var& other) {

        const list* ptr = other.cast<list>();

        if (ptr) {

            if (self._list == ptr->_list) {
                return order::equivalent;
            }
        }

        return order::unordered;
    }

    std::string _str_(const list& self, const Format_Args& fmt) {

        if (!_is_(self)) {
            return "[]"s;
        }

        std::string result = fmt::format("{}", self._list.back().str(fmt));

        for (auto i = self._list.crbegin() + 1; i != self._list.crend(); ++i) {
            result = fmt::format("{}, {}", result, i->str(fmt));
        }

        return fmt::format("[{}]", result);
    }

    var _lead_(list& self) {

        if (self._list.empty()) {
            return var();
        }
        return self._list.back();
    }

    var _push_(list& self, var& other) {

        if (other.is_nothing()) {
            return value_of(self);
        }

        self._list.push_back(other);

        return value_of(self);
    }

    var _drop_(list& self) {

        if (!self._list.empty()) {
            self._list.pop_back();
        }

        return value_of(self);
    }

    var _shift_(list& self) {

        if (!self._list.empty()) {
            var a = std::move(self._list.back());
            self._list.pop_back();
            return make_pair(a, self);
        }

        return value_of(self);
    }

    var _reverse_(list& self) {

        if (self._list.empty()) {
            return value_of(self);
        }

        std::reverse(self._list.begin(), self._list.end());

        return value_of(self);
    }

    var _add_(list& self, var& other) {

        if (other.type() == "list") {
            auto ptr = other.move<list>();

            ptr->_list.insert(
                ptr->_list.end(),
                std::make_move_iterator(self._list.begin()),
                std::make_move_iterator(self._list.end())
            );

            self._list = std::move(ptr->_list);

            return value_of(self);
        }

        return var();
    }
}