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

#include <map>
#include "boost/container/flat_map.hpp"

#include "Var.h"
#include "Boolean.h"
#include "Number.h"
#include "Text.h"

namespace Oliver {

    /********************************************************************************************/
    //
    //                               'object' Class Definition
    //
    //          The object class manages evaluatable term(s).  It is simply a
    //          wrapper around a std::vector<var>.  The key diffrence is that the
    //          order of sequence is reversed from the order of a vector.
    //
    /********************************************************************************************/

    class object {

        std::map<std::string, var> _map;
        std::string                                  _type;

    public:

        object();
        object(var terms);

        friend std::string          _type_(const object& self);
        friend std::size_t     _size_type_(const object& self);
        friend bool                   _is_(const object& self);
        friend auto                 _comp_(const object& self, const var& other);

        friend std::string           _str_(const object& self, const Format_Args& fmt);

        friend var                  _set_(object& self, var& index, var& other);
        friend var                  _del_(object& self, var& index);
        friend var                  _get_(object& self, var& index);
        friend var                  _has_(object& self, var& index);

        friend var                  _sub_(object& self, var& index);
        friend var                  _mod_(object& self, var& index);
    };

    /********************************************************************************************/
    //
    //                                 'object' Class Implementation
    //
    /********************************************************************************************/

    object::object() : _map{}, _type{ "object" } {
    }

    object::object(var terms) : _map{}, _type{ "object" } {

        while (terms) {
            var val = terms.lead();
            var key = terms.lead();

            if (key.is_something()) {
                if (auto str = key.str(Format_Args{}); str == "type") {
                    _type = val.str(Format_Args{});
                }
                else {
                    _map[str] = val;
                }
            }
        }
    }

    std::string _type_(const object& self) {
        return self._type;
    }

    std::size_t _size_type_(const object& self) {
        return self._map.size();
    }

    bool _is_(const object& self) {
        return _size_type_(self);
    }

    auto _comp_(const object& self, const var& other) {

        const object* ptr = other.cast<object>();

        if (ptr) {

            if (self._type == ptr->_type && self._map == ptr->_map) {
                return order::equivalent;
            }
        }

        return order::unordered;
    }

    std::string _str_(const object& self, const Format_Args& fmt) {

        if (!_is_(self)) {
            return "{}"s;
        }

        std::string result = "{"s;

        for (auto i = self._map.begin(); i != self._map.end(); ++i) {
            result = fmt::format("{}{} = {}, ", result, i->first, i->second.str(fmt));
        }
        result.pop_back();
        result.pop_back();
        result += "}"s;

        return result;
    }

    var _set_(object& self, var& index, var& other) {

        if (index.is_nothing() || other.is_nothing()) {
            return value_of(self);
        }

        if (index.type() == "list") {
            switch (index.size_type()) {

            case 1:
                index = index.lead();
                self._map[index.str(Format_Args{})] = other;
                return value_of(self);

            }
        }
        return error(fmt::format("Invalid index - {} - provided!", index));
    }

    var _del_(object& self, var& index) {

        if (index.is_nothing()) {
            return value_of(self);
        }

        self._map.erase(index.lead().str(Format_Args{}));

        return value_of(self);
    }

    var _get_(object& self, var& index) {

        if (index.is_nothing()) {
            return value_of(self);
        }

        if (index.type() == "list") {
            switch (index.size_type()) {

                case 1:
                    index = index.lead();
                    return self._map[index.str(Format_Args{})];

            }
        }
        return error(fmt::format("Invalid index - {} - provided!", index));
    }

    var _has_(object& self, var& index) {

        if (index.is_nothing()) {
            return value_of(self);
        }

        if (self._map.contains(index.lead().str(Format_Args{}))) {
            return boolean(true);
        }

        return boolean(false);
    }

    var _sub_(object& self, var& index) {

        return _del_(self, index);
    }

    var _mod_(object& self, var& index) {

        return _has_(self, index);
    }
}