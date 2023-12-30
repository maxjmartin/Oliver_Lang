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

#include <deque>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "compiler.h"
#include "text_support.h"
#include "data_types.h"

namespace Oliver {

    /********************************************************************************************/
    //    
    //                                The Oliver Interpreter Runtime
    //    
    //          The Oliver Interpreter runtime is defined below.  It is design to run
    //          as a stack interpreter evaluation of the code expression passed to it.
    //     
    //    
    /********************************************************************************************/

    class evaluator {

        using map_type     = Oliver::function::map_type;
        using deque_type   = std::deque<var>;
        using closure_type = std::vector<map_type>;

        closure_type    _variables;
        deque_type      _deque;
        deque_type      _code;

        // Shebang Variables
        bool   NO_EXCEPTIONS   = true;
        size_t RECURSION_LIMIT = 512ull;
        size_t STACK_LIMIT     = 512ull;

    public:

        evaluator();
        evaluator(const evaluator& env) = delete;
        evaluator(evaluator&& env)      = delete;

        var eval(var exp);


    private:

        void throw_error_message(const std::string& ermsg);

        void define_enclosure();
        void delete_enclosure();

        void  set_expression_on_code(var exp);
        void set_expression_on_deque(var exp);

        var get_result_deque() const;
        var get_eval_gue() const;

        var  get_symbol(var& var);
        void set_symbol(var& name, var& value);

        var get_expression_from_deque();
        var get_expression_from_code();

        void eval();

        void fundamental_operators(const op_code& opr);
        void    sequence_operators(const op_code& opr);
        void       index_operators(const op_code& opr);
        void      binary_operators(const op_code& opr);
        void   algorithm_operators(const op_code& opr);

        bool expression_is_empty(const var& expr) const;
    };
}

/********************************************************************************************/
//
//                                   Evaluator Implementation
//
/********************************************************************************************/
namespace Oliver {

    evaluator::evaluator() : _variables(), _deque(), _code() {
    }

    inline var evaluator::eval(var exp) {

        if (exp.type() != "expression") {
            return var();
        }

        exp = unwrap_expresion(exp);

        _code.push_back(exp);

        define_enclosure();

        eval();

        return get_result_deque();
    }

    inline void evaluator::throw_error_message(const std::string& ermsg) {
        if (!evaluator::NO_EXCEPTIONS) {
            set_expression_on_code(op_call(op_code::endl_op));
            set_expression_on_code(op_call(op_code::emit_op));
            set_expression_on_code(error(ermsg));
        }
    }

    inline void evaluator::define_enclosure() {
        // Define the global enclosure.
        _variables.push_back(map_type());
    }

    inline void evaluator::delete_enclosure() {
        if (!_variables.empty()) {
            _variables.pop_back();
        }
    }

    inline void evaluator::set_expression_on_code(var exp) {

        if (_code.empty()) {
            _code.emplace_back(expression());
        }

        _code.back() = _code.back().push(std::move(exp));
    }

    inline void evaluator::set_expression_on_deque(var exp) {

        if (_deque.size() < evaluator::STACK_LIMIT) {

            if (exp.is_something()) {
                _deque.push_back(std::move(exp));
                return;
            }
        }

        throw_error_message("Deque overflow!");
    }

    inline var evaluator::get_result_deque() const {

        if (!_deque.empty()) {

            var result = list();

            for (auto i = _deque.crbegin(); i != _deque.crend(); ++i) {

                result = result.push(*i);
            }

            return result;
        }

        return list();
    }

    inline var evaluator::get_eval_gue() const {

        if (!_code.empty()) {

            var result = expression();

            for (auto i = _code.crbegin(); i != _code.crend(); ++i) {

                result = result.push(*i);
            }

            return result;
        }

        return expression();
    }

    inline var evaluator::get_symbol(var& var) {

        std::string symbol_name = fmt::format("{}", var);

        for (auto i = _variables.crbegin(); i != _variables.crend(); ++i) {

            auto v_itr = i->find(symbol_name);

            if (v_itr != i->end()) {
                return v_itr->second;
            }
        }

        throw_error_message(symbol_name + " = undef_var");

        return nothing();
    }

    inline void evaluator::set_symbol(var& name, var& value) {

        //while (value.type() == "symbol") {
        //    value = get_symbol(value);
        //}

        std::string symbol_name = fmt::format("{}", name);

        if (_variables.empty()) {
            _variables.push_back(map_type());
        }

        _variables.back()[symbol_name] = std::move(value);
    }

    inline var evaluator::get_expression_from_deque() {

        if (_deque.empty()) {
            throw_error_message("\nDeque_Underflow");
            return var();
        }

        var val = std::move(_deque.back());
        _deque.pop_back();

        return val;
    }

    inline var evaluator::get_expression_from_code() {

        if (_code.empty()) {
            return var();
        }

        var a = _code.back().lead();

        while (!_code.empty() && expression_is_empty(_code.back())) {
            _code.pop_back();
        }

        return a;
    }

    inline bool evaluator::expression_is_empty(const var& expr) const {
        return !expr;
    }
}

/********************************************************************************************/
//
//                                   Evaluation Loop
//
/********************************************************************************************/
namespace Oliver {
    inline void evaluator::eval() {

        do {

            var exp = get_expression_from_code();  // Get an element from the code expression.

            while (exp.type() == "symbol") {  // Get the value of an abstraction.
                exp = get_symbol(exp);
            }

            if (exp.type() == "expression") {
                /*
                    Any elements which are expressions are
                    placed back on to the code to have their
                    individual elements evaluated.
                */

                exp = unwrap_expresion(exp);

                if (!expression_is_empty(exp)) {
                    _code.push_back(std::move(exp));
                }
            }

            else if (exp.type() == "function") {

                auto func = exp.move<function>();

                map_type enc = func->variables();

                var args = func->args();
                var body = func->body();

                while (args) {

                    var name = args.lead();
                    var value = get_expression_from_code();

                    if (value.op_call() == op_code::deque_op) {
                        value = get_result_deque();
                    }

                    while (value.type() == "symbol") {
                        value = get_symbol(value);
                    }

                    if (name.type() == "symbol") {
                        enc[fmt::format("{}", name)] = value;
                    }
                    else {
                        throw_error_message(fmt::format("Invalid function symbol defined: {}", name));
                    }
                }

                _variables.push_back(enc);
                set_expression_on_code(op_call(op_code::end_scope_op));
                set_expression_on_code(std::move(body));
            }

            else if (exp.type() != "op_call") {
                set_expression_on_deque(exp);
            }

            else {

                op_code opr = exp.op_call();

                if (opr > op_code::nothing_op && opr < op_code::END_OPERATORS) {

                    if (opr < op_code::FUNDAMENTAL_OPERATORS) {
                        fundamental_operators(opr);
                    }

                    else if (opr < op_code::SEQUENTIAL_OPERATORS) {
                        sequence_operators(opr);
                    }

                    else if (opr < op_code::INDEX_OPERATORS) {
                        index_operators(opr);
                    }

                    else if (opr < op_code::BINARY_OPERATORS) {
                        binary_operators(opr);
                    }

                    else if (opr < op_code::ALGORITHM_OPERATORS) {
                        algorithm_operators(opr);
                    }
                }
            }

            while (_code.size() && !_code.back()) {
                _code.pop_back();
            }
        } while (!_code.empty());
    }
}

/********************************************************************************************/
//
//                                   Fundamental Operators
//
/********************************************************************************************/
namespace Oliver {

    inline void evaluator::fundamental_operators(const op_code& opr) {

        switch (opr) {

            case op_code::idnt_op: {   // Place on to the stack an expression without evaluation.

                var exp = get_expression_from_code();
                set_expression_on_deque(exp);
            }   break;

            case op_code::neg_op: {    // Get the negation of an object.

                var a = get_expression_from_code();

                while (a.type() == "symbol") {
                    a = get_symbol(a);
                }

                if (a.type() == "function" && _variables.size()) {
                    /*
                        For function types we bind the current scope.
                        That way it is specialized, negating the 
                        variable abstractions.  
                        Then it will be left on the deque.
                    */

                    auto l = a.move<function>();

                    l->bind_scope(_variables.back());

                    a = *l;
                }

                set_expression_on_deque(-a);

            }   break;

            case op_code::deque_op: {  // Place copy of deque on back of deque.
                var deque = get_result_deque();
                set_expression_on_deque(deque);
            }   break;

            case op_code::emit_op: {

                var val = get_expression_from_deque();

                if (val.type() == "error") {
                    if (!evaluator::NO_EXCEPTIONS) {

                        fmt::print(fmt::emphasis::bold | fg(fmt::color::red), "{}!\nContinue runtime? ", val);

                        std::string continue_runtime;
                        std::getline(std::cin, continue_runtime);
                        continue_runtime = to_lower(continue_runtime);
                        continue_runtime = trim(continue_runtime);

                        if (continue_runtime != "y" && continue_runtime != "yes") {
                            _code.clear();
                        }
                    }
                }
                else {
                    fmt::print("{}", val);
                }

            }   break;

            case op_code::input_op: {

                var val = get_expression_from_code();

                if (val.type() == "symbol") {
                    std::string user_input;
                    std::getline(std::cin, user_input);

                    set_expression_on_code(compiler{ TextParser{ user_input }.parse() }.compile());
                }
                else {
                    throw_error_message("Invalid symbol assignment from user unput!");
                }

            }   break;

            case op_code::endl_op: {
                fmt::println("");
            }   break;

            case op_code::assign_op: {

                var val = get_expression_from_deque();
                var var = get_expression_from_deque();

                if (var.type() == "symbol") {  // First assign the value to a variable.
                    set_symbol(var, val);
                }

                else if (var.op_call() == op_code::deque_op) {  // set the elements of a list to the deque.

                    _deque.clear();

                    if (val.type() == "expression") {

                        while (val) {
                            _deque.push_back(val.lead());
                        }
                    }
                    else {
                        _deque.push_back(val);
                    }
                }

                else {
                    throw_error_message("Miss handled assignment: " + fmt::format("{}", val) + " = " + fmt::format("{}", var));
                }

            }   break;

            case op_code::let_op: {  // Assign or apply a value to a variable.

                var name  = get_expression_from_code();
                var value = get_expression_from_code();
                var oper  = get_expression_from_code();

                if (oper.op_call() != op_code::eq_op) {
                    var index = value;
                    
                    value = oper;
                    oper = get_expression_from_code();

                    var exp = expression(op_call(op_code::set_op));
                    exp = exp.push(value);
                    exp = exp.push(index);
                    exp = exp.push(name);

                    value = exp;
                }

                if (oper.op_call() == op_code::eq_op) {
                    /*
                        Simple assignment of one or more variables.
                        Functions are not evaluated before assignment.
                        Instead they are applied as a 'def' operator
                        has been called.
                    */

                    if (value.op_call() == op_code::idnt_op) {
                        value = expression(get_expression_from_code());
                        value = value.push(op_call(op_code::idnt_op));
                    }
                    else if (value.op_call() == op_code::neg_op) {
                        value = expression(get_expression_from_code());
                        value = value.push(op_call(op_code::neg_op));
                    }

                    if (value.type() == "function") {
                        const function* l = value.cast<function>();

                        set_expression_on_code(l->body());
                        set_expression_on_code(l->args());
                        set_expression_on_code(name);
                        set_expression_on_code(op_call(op_code::def_op));
                    }

                    else {
                        var exp = expression();
                        exp = exp.push(op_call(op_code::assign_op));
                        exp = exp.push(value);
                        exp = exp.push(name);
                        exp = exp.push(op_call(op_code::idnt_op));

                        set_expression_on_code(exp);
                    }
                }
            }   break;

            case op_code::size_op: {   // Get the size of an object.

                var val = get_expression_from_deque();

                val = number(val.size_type());

                set_expression_on_deque(val);

            }   break;

            case op_code::type_op: {   // Get the type description of an object.

                var val = get_expression_from_deque();

                val = text(val.type());

                set_expression_on_deque(val);

            }   break;

            case op_code::bool_op: {   // Determine if an object is defined.

                var val = get_expression_from_deque();

                val = boolean(val.op_call() == op_code::deque_op ? !_deque.empty() : bool(val));

                set_expression_on_deque(val);

            }   break;

            case op_code::L_IMP_op: {
                /*
                    This is the basic infix p -> q else -q.

                    It simply rearranges the expression to be
                    a postfix implementation.

                    example:
                        var x = '42'

                        (x > '10') then :
                            ("x is greater than 10" << endl)
                            ("x is not greater than 10" << endl)
                        ;
                */

                var p = get_expression_from_deque();
                var q = get_expression_from_code();

                if (p) {
                    p = q.lead();
                }
                else {
                    p = q.drop().lead();
                }

                set_expression_on_code(p);
            }   break;

            case op_code::l_imp_op: {
                /*
                    This is the basic postfix p -> q else -q.

                    example:
                        var x = '42'

                        (x > '10')+:
                            ("x is greater than 10" << endl)
                            ("x is not greater than 10" << endl)
                        ; imply
                */

                var q = get_expression_from_deque();
                var p = get_expression_from_deque();

                if (bool(p)) {
                    p = q.lead();
                }
                else {
                    p = q.drop().lead();
                }

                set_expression_on_code(p);
            }   break;

            case op_code::if_op: {
                /*
                    This is the basic prefix p -> q else -q.

                    It simply rearranges the expressions ahead
                    of the operator into a postfix implementation
                    of the implications.

                    example:
                        var x = '42'

                        if (x > '10'):
                            "x is greater than 10" << endl
                        ;
                        elif (x < '10'):
                            "x is less than 10" << endl
                        ;
                        else:
                            "x is equal to 10" << endl
                        ;
                */

                var oper;
                var code = expression();

                do {  // Loop through the conditions and consequents.
                      // Generate an expression list of each implication.

                    var p = get_expression_from_code();
                    var q = get_expression_from_code();

                    if (oper.op_call() == op_code::else_op) {
                        set_expression_on_code(q);
                        q = p;
                        p = boolean(true);
                    }

                    code = code.push(q);
                    code = code.push(p);

                    oper = get_expression_from_code();

                    if (!(oper.op_call() == op_code::elif_op || oper.op_call() == op_code::else_op)) {
                        set_expression_on_code(oper);
                    }

                } while (oper.op_call() == op_code::elif_op || oper.op_call() == op_code::else_op);

                // Now loop through the implication list.
                // Generating the required, postfix implication.  

                var elif_statement = expression();

                while (code) {

                    var p = code.lead();
                    var q = code.lead();

                    var e = expression();

                    e = e.push(op_call(op_code::l_imp_op));
                    e = e.push(make_pair(q, elif_statement));
                    e = e.push(op_call(op_code::idnt_op));
                    e = e.push(p);

                    elif_statement = e;
                }

                // fmt::println("statement = {}", elif_statement);

                set_expression_on_code(elif_statement);
            } break;

            case op_code::def_op: {  // Presuppose the definition of a function.  

                var name = get_expression_from_code();
                var args = get_expression_from_code();
                var body = get_expression_from_code();

                function lam(args, body);

                if (_variables.size() > 1) {
                    /*
                        Define the function's internal closure.

                        Ignore global scope for the purpose of setting the values of
                        the enclosure.  Get the current scope and bind it to the defined
                        function.
                    */
                    lam.bind_scope(_variables.back());
                }

                lam.bind_variable(name, lam);           // Provide a self reference for recursion.  
                lam.bind_variable(text("self"), name);  // Identify the self reference.  

                set_expression_on_deque(name);
                set_expression_on_deque(lam);
                set_expression_on_code(op_call(op_code::assign_op));

            }   break;

            case op_code::end_scope_op: {   // Delete the current enclosure scope.
                delete_enclosure();
            }   break;

            case op_code::shebang_op: {    // Process a shebang assignment.

                var tag = get_expression_from_code();
                var val = get_expression_from_code();
                var eql = get_expression_from_code();

                if (eql.op_call() == op_code::eq_op) {

                    switch (tag.op_call()) {

                    case op_code::no_except_op:
                        NO_EXCEPTIONS = val.type() == "boolean" && val ? true : false;
                        break;
                    }
                }
            }   break;
        }
    }
}

/********************************************************************************************/
//
//                                    Sequence Operators
//
/********************************************************************************************/
namespace Oliver {

    inline void evaluator::sequence_operators(const op_code& opr) {

        switch (opr) {

            case op_code::lead_op: {

                var x = get_expression_from_deque();

                x = x.lead();
                set_expression_on_deque(x);

            }   break;


            case op_code::join_op: {

                var y = get_expression_from_deque();
                var x = get_expression_from_deque();

                x = y.push(x);
                set_expression_on_deque(x);

            }   break;


            case op_code::drop_op: {

                var x = get_expression_from_deque();

                x = x.drop();
                set_expression_on_deque(x);

            }   break;


            case op_code::next_op: {

                var x = get_expression_from_deque();

                x = x.drop();
                set_expression_on_code(x);

            }   break;


            case op_code::deque_lead_op: {

                var x = get_expression_from_deque();

                if (x.op_call() == op_code::deque_op) {

                    x = _deque.size() ? _deque.front() : nothing();
                    set_expression_on_deque(x);
                }
                
            }   break;


            case op_code::deque_last_op: {

                var x = get_expression_from_deque();

                if (x.op_call() == op_code::deque_op) {

                    x = _deque.size() ? _deque.back() : nothing();
                    set_expression_on_deque(x);
                }
                
            }   break;


            case op_code::deque_join_op: {

                var x = get_expression_from_deque();
                var y = get_expression_from_deque();

                if (x.op_call() == op_code::deque_op && y.is_something()) {
                    _deque.push_front(y);
                }
            }   break;


            case op_code::deque_push_op: {

                var x = get_expression_from_deque();
                var y = get_expression_from_deque();

                if (x.op_call() == op_code::deque_op && y.is_something()) {
                    _deque.push_back(y);
                }
            }   break;


            case op_code::deque_drop_op: {

                var x = get_expression_from_deque();

                if (x.op_call() == op_code::deque_op && _deque.size()) {
                    _deque.pop_front();
                }
            }   break;


            case op_code::deque_shift_op: {

                var x = get_expression_from_deque();

                if (x.op_call() == op_code::deque_op && _deque.size()) {
                    _deque.pop_back();                }

            }   break;
        }
    }

}

/********************************************************************************************/
//
//                                     Index Operators
//
/********************************************************************************************/
namespace Oliver {

    inline void evaluator::index_operators(const op_code& opr) {

        switch (opr) {

            case op_code::get_op: {
                var k = get_expression_from_deque();
                var m = get_expression_from_deque();
                m = m.get(k);
                set_expression_on_deque(m);
            }   break;

            case op_code::set_op: {
                var v = get_expression_from_deque();
                var k = get_expression_from_deque();
                var m = get_expression_from_deque();
;               m = m.set(k, v);
                set_expression_on_deque(m);
            }   break;

            case op_code::has_op: {
                var k = get_expression_from_deque();
                var m = get_expression_from_deque();
                m = m.has(k);
                set_expression_on_deque(m);
            }   break;

            case op_code::del_op: {
                var k = get_expression_from_deque();
                var m = get_expression_from_deque();
                m = m.del(k);
                set_expression_on_deque(m);
            }   break;
        }
    }
}

/********************************************************************************************/
//
//                                      Binary Operators
//
/********************************************************************************************/
namespace Oliver {

    inline void evaluator::binary_operators(const op_code& opr) {

        var y = get_expression_from_deque();
        var x = get_expression_from_deque();

        switch (opr) {

            case op_code::l_and_op:
                x = boolean(x && y);
                break;

            case op_code::l_or_op:
                x = boolean(x || y);
                break;

            case op_code::l_xor_op:
                x = boolean(bool(x) != bool(y));
                break;


            case op_code::eq_op:
                x = boolean(x == y);
                break;

            case op_code::ne_op:
                x = boolean(x != y);
                break;

            case op_code::gt_op:
                x = boolean(x > y);
                break;

            case op_code::ge_op:
                x = boolean(x >= y);
                break;

            case op_code::lt_op:
                x = boolean(x < y);
                break;

            case op_code::le_op:
                x = boolean(x <= y);
                break;

            case op_code::add_op:
                x = x + y;
                break;

            case op_code::sub_op:
                x = x - y;
                break;

            case op_code::mul_op:
                x = x * y;
                break;

            case op_code::div_op:
                x = x / y;
                break;

            case op_code::mod_op:
                x = x % y;
                break;

                //case op_code::fdiv_op:  TODO: Should be defined here instead of in var class
                //    x = x.f_div(y);
                //    break;

                //case op_code::rem_op:
                //    x = x.rem(y);
                //    break;

            case op_code::exp_op:
                x = x.pow(y);
                break;
        }

        set_expression_on_deque(x);
    }
}

/********************************************************************************************/
//
//                                    Algorithm Operators
//
/********************************************************************************************/
namespace Oliver {

    inline void evaluator::algorithm_operators(const op_code& opr) {

        var x = get_expression_from_deque();

        switch (opr) {
            case op_code::rev_op:
                x = x.reverse();
                break;
        }

        set_expression_on_deque(x);
    }
}