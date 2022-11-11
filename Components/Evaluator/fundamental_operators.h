#ifndef FUNDAMENTAL_OPERATORS_H
#define FUNDAMENTAL_OPERATORS_H

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

#include "evaluator.h"

#include "../../Oliver.h"

namespace Olly {
    namespace eval {

        inline void evaluator::fundamental_operators(OP_CODE& opr) {

            switch (opr) {

                case OP_CODE::idnt_op: {   // Place on to the stack an expression without evaluation.

                    let exp = get_expression_from_code();
                    set_expression_on_deque(exp);
                }	break;


                case OP_CODE::neg_op: {    // Get the negation of an object.

                    let a = get_expression_from_code();

                    while (a.type() == "symbol") {
                        a = get_symbol(a);
                    }

                    if (a.type() == "lambda" && _variables.size()) {

                        lambda l = a.copy<lambda>();

                        l.bind_scope(_variables.back());

                        a = l;
                    }

                    set_expression_on_deque(a.neg());

                }   break;



                case OP_CODE::deque_op: {   // Print a string representation of the stack.

                    let deque = get_result_deque();

                    set_expression_on_deque(deque);
                }	break;



                case OP_CODE::emit_op: {

                    let val = get_expression_from_deque();

                    val.str(_stream);

                    std::cout << _stream.str();

                    _stream.str(str_type());
                    _stream.clear();

                }   break;


                case OP_CODE::enter_op: {

                    Olly::let result;

                
                    str_type input = "";

                    std::cin >> input;

                    tokens_type code_tokens;

                    parser lex(input);
                    code_tokens = lex.parse();

                    compiler comp(code_tokens);
                    result = comp.compile();                                   

                    set_expression_on_code(result);

                }   break;



                case OP_CODE::endl_op: {

                    _stream << std::endl;

                }   break;



                case OP_CODE::assign_op: {

                    let val = get_expression_from_deque();
                    let var = get_expression_from_deque();

                    while (val.type() == "symbol") {  // Get the value of an abstraction.
                        val = get_symbol(val);
                    }

                    if (var.type() == "symbol") {  // First assign the value to a variable.
                        set_symbol(var, val);
                    }

                    else if (var.op_code() == OP_CODE::deque_op) {  // set the elements of a list to the deque.

                        _deque.clear();

                        if (val.type() == "list") {

                            while (val.is()) {

                                _deque.emplace_back(pop_lead(val));
                            }
                        }
                        else {
                            _deque.emplace_back(val);
                        }
                    }
                    else {
                        throw_error_message("Miss handled assignment: " + str(val) + " = " + str(var));
                    }

                }   break;



                case OP_CODE::let_op: {  // Assign or apply a value to a variable.

                    let vars = get_expression_from_code();
                    let vals = get_expression_from_code();
                    let oper = get_expression_from_code();

                    if (oper.op_code() == OP_CODE::eq_op) {
                        /*
                            Simple assignment of one or more variables.
                            Functions are not evaluated before assignment.
                            Instead they are applied as a 'def' operator
                            has been called.
                        */

                        if (vars.type() != "expression") {

                            vars = expression(vars);
                            vals = expression(vals);
                        }

                        while (vars.is()) {

                            let var = pop_lead(vars);
                            let val = pop_lead(vals);

                            if (val.op_code() == OP_CODE::idnt_op) {
                                val = vals.is() ? expression(pop_lead(vals)) : expression(get_expression_from_code());
                                val = val.join(op_call(OP_CODE::idnt_op));
                            }
                            else if (val.op_code() == OP_CODE::neg_op) {
                                val = vals.is() ? expression(pop_lead(vals)) : expression(get_expression_from_code());
                                val = val.join(op_call(OP_CODE::neg_op));
                            }

                            if (val.type() == "lambda") {
                                const lambda* l = val.cast<lambda>();

                                set_expression_on_code(l->body());
                                set_expression_on_code(l->args());
                                set_expression_on_code(var);
                                set_expression_on_code(op_call(OP_CODE::def_op));
                            }

                            else {
                                let exp = expression();
                                exp = exp.join(op_call(OP_CODE::assign_op));
                                exp = exp.join(val);
                                // exp = exp.join(op_call(OP_CODE::idnt_op));
                                exp = exp.join(var);
                                exp = exp.join(op_call(OP_CODE::idnt_op));
                            
                                set_expression_on_code(exp);
                            }
                        }
                    }
                }	break;


                    // TODO MOVE TO NEW LOCATION
                case OP_CODE::size_op: {   // Get the size of an object.
                
                    let val = get_expression_from_deque();

                    val = number(val.size());

                    set_expression_on_deque(val);

                }	break;


                    // TODO MOVE TO NEW LOCATION
                case OP_CODE::type_op: {   // Get the type description of an object.

                    let val = get_expression_from_deque();

                    val = string(val.type());

                    set_expression_on_deque(val);

                }	break;




                case OP_CODE::L_CON_op: {
                    /*
                        This is the basic infix p -> q else -q.

                        It simply rearanges the expression to be
                        a postfix implimentation.  

                        example:
                            let x = '42'

                            (x > '10') ? :
                                ("x is greater than 10" << endl)
                                ("x is not greater than 10" << endl)
                            ;
                    */

                    let x = get_expression_from_deque();
                    let y = get_expression_from_code();

                    let exp = expression();
                    exp = exp.join(op_call(OP_CODE::l_imp_op));
                    exp = exp.join(y);
                    exp = exp.join(op_call(OP_CODE::idnt_op));
                    exp = exp.join(x);

                    set_expression_on_code(exp);
                }   break;



                case OP_CODE::l_imp_op: {
                    /*
                        This is the basic postfix p -> q else -q.  

                        example:
                            let x = '42'

                            (x > '10')+:
                                ("x is greater than 10" << endl)
                                ("x is not greater than 10" << endl)
                            ;l_imp
                    */

                    let q = get_expression_from_deque();
                    let p = get_expression_from_deque();

                    p = p.is() ? first(q) : second(q);

                    set_expression_on_code(p);
                }   break;



                case OP_CODE::if_op: {
                    /*
                        This is the basic prefix p -> q else -q.

                        It simply rearanges the expressions ahead
                        of the operator into a postfix implimentation
                        of the implications.  

                        example:
                            let x = '42'

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

                    let oper;
                    let code = expression();

                    do {  // Loop through the conditions and consequents.
                          // Generate an expression list of each implication.

                        let p = get_expression_from_code();
                        let q = get_expression_from_code();

                        if (oper.op_code() == OP_CODE::else_op) {

                            q = p;
                            p = logical_term(true);
                        }

                        code = code.join(q);
                        code = code.join(p);

                        oper = get_expression_from_code();

                        if (!(oper.op_code() == OP_CODE::elif_op || oper.op_code() == OP_CODE::else_op)) {
                            set_expression_on_code(oper);
                        }

                    } while (oper.op_code() == OP_CODE::elif_op || oper.op_code() == OP_CODE::else_op);

                    // Now loop through the implication list.
                    // Generating the required, postfix implication.  

                    let elif_statement;

                    while (code.is()) {

                        let p = pop_lead(code);
                        let q = pop_lead(code);

                        let e = expression();

                        e = e.join(op_call(OP_CODE::l_imp_op));
                        e = e.join(make_pair(q, elif_statement));
                        e = e.join(op_call(OP_CODE::idnt_op));
                        e = e.join(p);

                        elif_statement = e;
                    }

                    // print("statement = " + str(elif_statement));

                    set_expression_on_code(elif_statement);
                } break;



                case OP_CODE::def_op: {  // Presupose the definition of a function.  

                    let var  = get_expression_from_code();
                    let args = get_expression_from_code();
                    let body = get_expression_from_code();

                    lambda lam(args, body);

                    if (_variables.size() > 1) {
                        /*
                            Define the function's internal closure.

                            Ignore global scope for the purpose of setting the values of
                            the enclosure.  Get the current scope and bind it to the defined
                            function.
                        */

                        // print_all_closures();

                        lam.bind_scope(_variables.back());
                    }

                    lam.bind_variable(var, lam);  // Provide a self reference for recursion.  

                    lam.bind_variable(string("self"), var);  // Identify the self refrence.  

                    set_expression_on_deque(var);
                    set_expression_on_deque(lam);
                    set_expression_on_code(op_call(OP_CODE::assign_op));

                }	break;



                case OP_CODE::end_scope_op: {   // Delete the current enclosure scope.
                    delete_enclosure();
                }	break;
            }
        }

    }  // end eval
}
#endif // FUNDAMENTAL_OPERATORS_H