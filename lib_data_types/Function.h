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

#include "Expression.h"

namespace Oliver {

    /********************************************************************************************/
    //
    //                              'function' Class Definition
    //
    //          The function class is an ananymous function in Oliver. It consists of
    //          arguments, a body, and a scope.  Individual variables can be bound to
    //          the scope of a function after its definition.  
    //
    /********************************************************************************************/

    class function {

    public:
        //using map_type = std::map <std::string, var>;
        using map_type = boost::container::flat_map <std::string, var>;

        function();
        function(var exp);
        function(var args, var body);

        friend std::string   _type_(const function& self);
        friend bool            _is_(const function& self);
        friend order         _comp_(const function& self, const var& other);
        friend std::string    _str_(const function& self, const Format_Args& fmt);

        friend var            _abs_(function& self);
        friend var           _args_(function& self);
        friend var           _body_(function& self);
        friend var           _encl_(function& self);

        void bind_scope(const map_type& scope);
        void bind_variable(var name, var value);

        map_type variables() const;

        var args() const;
        var body() const;

        void print_enclosure() const;

        bool is_bound(const std::string& name);

    private:
        var         _args;
        var         _body;
        map_type    _scope;
    };

    /********************************************************************************************/
    //
    //                               'function' Class Implementation
    //
    /********************************************************************************************/

    function::function() : _args(expression()), _body(expression()), _scope() {
    }

    function::function(var exp) : _args(), _body(), _scope() {

        const function* l = exp.cast<function>();

        if (l) {
            _args  = var(l->_args);
            _body  = var(l->_body);
            _scope =     l->_scope;
        }
    }

    function::function(var args, var body) : _args(args), _body(body), _scope() {
    }

    std::string _type_(const function& self) {
        return "function"s;
    }

    bool _is_(const function& self) {

        if (self._args || self._body) {
            return true;
        }

        return false;
    }

    order _comp_(const function& self, const var& other) {

        const function* e = other.cast<function>();

        if (e) {

            if ((self._args == e->_args) && (self._body == e->_body)) {
                return order::equivalent;
            }
        }

        return order::unordered;
    }

    std::string _str_(const function& self, const Format_Args& fmt) {

        if (!_is_(self)) {
            return "func():;"s;
        }

        std::string result = "func"s;

        result = fmt::format("{}{}", result, self._args);

        std::string body = fmt::format("{}", self._body);

        body[0]               = ':';
        body[body.size() - 1] = ';';

        result = fmt::format("{}{}", result, body);

        return result;
    }

    var _abs_(function& self) {
        return number(self._args.abs());
    }

    var _args_(function& self) {

        if (!self._args) {
            return var();
        }

        return self._args;
    }

    var _body_(function& self) {

        if (!self._body) {
            return var();
        }

        return self._body;
    }

    var _encl_(const function& self) {
        return self.variables();
    }

    inline void function::bind_scope(const map_type& scope) {

        auto name_loc = scope.find("self");

        auto name = (name_loc == scope.cend()) ? "" : fmt::format("{}", name_loc->second);

        for (auto &itr : scope) {

            if (itr.first != name && _scope.find(itr.first) == _scope.end()) {

                _scope[itr.first] = var(itr.second);
            }
        }
    }

    inline void function::bind_variable(var name, var value) {
        _scope[fmt::format("{}", name)] = value;
    }

    inline function::map_type function::variables() const {
        return _scope;
    }
    
    inline var function::args() const {
        return _args;
    }

    inline var function::body() const  {
        return _body;
    }

    inline void function::print_enclosure() const {

        std::string result = ""s;

        for (auto& itr : _scope) {
            result = fmt::format("{} = {}\n", itr.first, fmt::format("{}", itr.second));
        }
    }

    inline bool function::is_bound(const std::string& name) {
        
        var args = _args;

        while (args) {

            if (name == fmt::format("{}", args.lead())) {
                return true;
            }
            args = args.next();
        }
        return false;
    }

}