#ifndef EVALUATOR_H
#define EVALUATOR_H

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

#include "../Compiler/Data_Types/base_configuration/system_fundamentals.h"
#include "../text_reader.h"
#include "../parser.h"
#include "../file_writer.h"
#include "../Compiler/compiler.h"

namespace Olly {
    namespace eval {

        /********************************************************************************************/
        //
        //                                The Oliver Interpreter Runtime
        //
        //          The Oliver Interpreter runtime is defined below.  It is design to run
        //			as a stack inpterpreter evaluation of the code expression passed to it.
        // 
        //
        /********************************************************************************************/


        /********************************************************************************************/
        //
        //                              'evaluator' Class Definition
        //
        /********************************************************************************************/

        class evaluator {

            typedef     std::map<str_type, let> map_type;
            typedef     std::deque<let>	        deque_type;
            typedef     std::vector<map_type>	closure_type;

            closure_type                _variables;
            deque_type                      _deque;
            deque_type                       _code;
            size_type              _max_deque_size;
            stream_type                    _stream;

        public:
            static const size_type DEFAULT_STACK_LIMIT;

            evaluator();
            evaluator(evaluator& env) = delete;

            let eval(let exp);


        private:

            // let eval(let exp, closure_type& vars);

            void throw_error_message(const str_type& ermsg);

            void define_enclosure(let& lam);
            void define_enclosure();
            void delete_enclosure();

            // void define_lambda_enclosure(lambda& lam, let name);
            // void define_return_enclosure(let& exp);

            void   set_expression_on_code(let exp);
            void  set_expression_on_deque(let exp);

            let get_result_deque() const;
            let get_eval_que() const;

            let  get_symbol(let& var) const;
            void set_symbol(let& var, let& val);

            void rotate_deque(let& var);

            void find_at_deque_location(let& var);

            let get_expression_from_deque();
            let get_expression_from_code();
            let get_forward_expression();

            void print_current_closure(const map_type& i, str_type& shift);
            void print_all_closures();

            void eval();

            void fundamental_operators(OP_CODE& opr);
            void    sequence_operators(OP_CODE& opr);
            void associative_operators(OP_CODE& opr);
            void      binary_operators(OP_CODE& opr);
        };

        /********************************************************************************************/
        //
        //                                'evaluator' Class Implimentation
        //
        /********************************************************************************************/

        const size_type evaluator::DEFAULT_STACK_LIMIT = 2048;

        evaluator::evaluator() : _variables(), _deque(), _code(), _max_deque_size(DEFAULT_STACK_LIMIT), _stream() {
            _stream << std::boolalpha;
        }

        inline let evaluator::eval(let exp) {

            if (exp.type() != "expression") {
                return nothing();
            }

            exp = unwrap_expresion(exp);

            if (exp.is()) {

                _code.emplace_back(exp);

                define_enclosure();

                eval();

                return get_result_deque();
            }

            return nothing();
        }

        inline void evaluator::throw_error_message(const str_type& ermsg) {
            set_expression_on_code(op_call(OP_CODE::endl_op));
            set_expression_on_code(op_call(OP_CODE::emit_op));
            set_expression_on_code(error(ermsg));
        }

        inline void evaluator::define_enclosure(let& lam) {
            // Define a net new enclosure from a labda expression.
            const lambda* l = lam.cast<lambda>();
            _variables.emplace_back(l->variables());
        }

        inline void evaluator::define_enclosure() {
            // Define the global enclosure.
            _variables.emplace_back(map_type());
        }

        inline void evaluator::delete_enclosure() {
            // Delete the current enclosure, and return the stack.
            // print_all_closures();
            if (!_variables.empty()) {
                _variables.pop_back();
            }
            // print_all_closures();
        }

        //inline void evaluator::define_lambda_enclosure(lambda& lam, let name) {

        //    if (_variables.size() > 1) {
        //        /*
        //            Define the function's internal closure.

        //            Ignore global scope for the purpose of setting the values of
        //            the enclosure.  Get the current scope and bind it to the defined
        //            function.
        //        */

        //        lam.bind_scope(_variables.back());
        //    }

        //    lam.bind_variable(name, lam);  // Provide a self reference for recursion.  

        //    lam.bind_variable(string("self"), name);  // Identify the self refrence.  
        //}

        //inline void evaluator::define_return_enclosure(let& exp) {

        //    if (exp.type() == "expression") {

        //        let buffer = expression();

        //        while (exp.is()) {

        //            let temp = pop_lead(exp);

        //            if (temp.type() == "lambda") {

        //                lambda lam = temp.copy<lambda>();

        //                define_lambda_enclosure(lam, nothing());

        //                buffer = buffer.join(lam);
        //            }
        //            else {
        //                buffer = buffer.join(temp);
        //            }
        //        }

        //        exp = buffer.reverse();
        //    }
        //}

        inline void evaluator::set_expression_on_code(let exp) {

            if (_code.empty()) {
                _code.emplace_back(expression());
            }

            _code.back() = _code.back().join(exp);
        }

        inline void evaluator::set_expression_on_deque(let exp) {

            if (_deque.size() < _max_deque_size) {

                if (exp.is_something()) {
                    _deque.emplace_back(exp);
                }
                else {
                    throw_error_message("Nothing passed to deque!");
                }
                return;
            }

            throw_error_message("Deque overflow!");
        }

        inline let evaluator::get_result_deque() const {

            if (!_deque.empty()) {

                let result = list();

                for (auto i = _deque.crbegin(); i != _deque.crend(); ++i) {

                    result = result.join(*i);
                }

                return result;
            }

            return list();
        }

        inline let evaluator::get_eval_que() const {

            if (!_code.empty()) {

                let result = expression();

                for (auto i = _code.crbegin(); i != _code.crend(); ++i) {

                    result = result.join(*i);
                }

                return result;
            }

            return expression();
        }

        inline let evaluator::get_symbol(let& var) const {

            str_type symbol_name = str(var);

            for (auto i = _variables.crbegin(); i != _variables.crend(); ++i) {

                auto v_itr = i->find(symbol_name);

                if (v_itr != i->end()) {
                    return v_itr->second;
                }
            }

            return error("undef_var");
        }

        inline void evaluator::set_symbol(let& var, let& val) {

            while (val.type() == "symbol") {
                val = get_symbol(val);
            }

            str_type symbol_name = repr(var);

            if (_variables.empty()) {
                _variables.emplace_back(map_type());
            }

            _variables.back()[symbol_name] = val;
        }

        inline void evaluator::rotate_deque(let& var) {

            int_type i = var.get_integer();

            if (i > 0) {
                std::rotate(_deque.begin(), _deque.begin() + i, _deque.end());
            }

            else if (i < 0) {
                std::rotate(_deque.rbegin(), _deque.rbegin() + -i, _deque.rend());
            }
        }

        inline void evaluator::find_at_deque_location(let& var) {

            int_type i = var.get_integer();

            if (i > 0) {
                
                if (i <= _deque.size()) {

                    _deque.emplace_back(_deque[static_cast<size_type>(i - 1)]);
                }
            }

            else if (i < 0) {
                i += 1;

                if (i <= _deque.size()) {

                    _deque.emplace_back(_deque[static_cast<size_type>(_deque.size() + i)]);
                }
            }

        }

        inline let evaluator::get_expression_from_deque() {

            if (_deque.empty()) {
                return error("Deque_Underflow");
            }

            let val = _deque.back();
            _deque.pop_back();

            return val;
        }

        inline let evaluator::get_expression_from_code() {

            if (_code.empty()) {
                return error("Code_Underflow");
            }

            let a = pop_lead(_code.back());

            if (expression_is_empty(_code.back())) {
                _code.pop_back();
            }

            return a;
        }

        inline let evaluator::get_forward_expression() {
            
            let exp = get_expression_from_code();  // Get an element from the code expression.

            while (exp.type() == "symbol") {  // Get the value of an abstraction.
                exp = get_symbol(exp);
            }

            return exp;
        }

        inline void evaluator::print_current_closure(const map_type& map, str_type& shift) {

            for (auto i = map.begin(); i != map.end(); ++i) {

                std::cout << shift << i->first << " = " << str(i->second) << std::endl;
            }
        }

        inline void evaluator::print_all_closures() {

            str_type shift = "  ";

            std::cout << "{" << std::endl;

            for (auto i = _variables.begin(); i != _variables.end(); ++i) {

                print_current_closure(*i, shift);

                shift += "  ";
            }
            std::cout << "}" << std::endl;
        }

        inline void evaluator::eval() {

            do {

                let exp = get_expression_from_code();  // Get an element from the code expression.

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
                        _code.emplace_back(exp);
                    }
                }

                else if (exp.type() == "lambda") {
                    
                    const lambda* l = exp.cast<lambda>();

                    map_type enc = l->variables();

                    let args = l->args();
                    let body = l->body();
                                        
                    while (args.is()) {

                        let var = pop_lead(args);
                        let val = get_expression_from_code();

                        if (val.op_code() == OP_CODE::deque_op) {
                            val = get_result_deque();
                        }

                        while (val.type() == "symbol") {
                            val = get_symbol(val);
                        }

                        //print("var = " + str(var));
                        //print("val = " + str(val));

                        if (var.type() == "symbol") {
                            enc.emplace(str(var), val);
                        }
                    }

                    _variables.emplace_back(enc);
                    
                    set_expression_on_code(op_call(OP_CODE::end_scope_op));
                    set_expression_on_code(body);
                }

                else if (exp.type() != "op_call") {
                    set_expression_on_deque(exp);
                }

                else {

                    OP_CODE opr = exp.op_code();

                    if (opr > OP_CODE::nothing_op && opr < OP_CODE::END_OPERATORS) {

                        if (opr < OP_CODE::FUNDAMENTAL_OPERATORS) {
                            fundamental_operators(opr);
                        }

                        else if (opr < OP_CODE::SEQUENTIAL_OPERATORS) {
                            sequence_operators(opr);
                        }

                        else if (opr < OP_CODE::BINARY_OPERATORS) {
                            binary_operators(opr);
                        }
                    }
                }

            } while (!_code.empty());
        }
    }  // end eval
}
#endif // EVALUATOR_H

#include "fundamental_operators.h"
#include    "sequence_operators.h"
#include "associative_operators.h"
#include      "binary_operators.h"