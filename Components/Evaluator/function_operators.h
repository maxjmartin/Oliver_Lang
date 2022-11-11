#ifndef FUNCTION_OPERATORS_H
#define FUNCTION_OPERATORS_H

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

namespace Olly {
    namespace eval {

        inline void evaluator::function_operators(OP_CODE& opr) {

            switch (opr) {







                else if (oper.op_code() == OP_CODE::APPLY_op) {
                    /*
                        The apply operator functions the same as
                        a normal let call, but applies the result
                        of a function instead of defining a new
                        function instance.  
                    */

                    // We can reuse the apply_op as all other instances
                    // of the operator were compiled out of the code.
                    let oper = op_call(OP_CODE::apply_op);

                    set_expression_on_code(vars);
                    set_expression_on_code(oper);
                    set_expression_on_code(vals);
                }
            }	break;

            case OP_CODE::apply_op: {
                /*
                    Part two of the applications process.  The result
                    of the function is gotten from the stack.  Then a
                    let assignment is written to the code to execute.
                */

                let vals = get_expression_from_deque();
                let vars = get_expression_from_code();

                set_expression_on_code(op_call(OP_CODE::EQ_op));
                set_expression_on_code(vals);
                set_expression_on_code(vars);
                set_expression_on_code(op_call(OP_CODE::let_op));
            } break;





            default:
                break;
            }
        }

        inline let evaluator::get_result_queue() {

            let args = get_expression_from_code();

            if (args.type() != "expression") {
                args = expression(args);
            }

            let queue = expression();

            while (args.is()) {

                let a = pop_lead(args);

                while (a.type() == "symbol") {
                    a = get_symbol(a);
                }

                queue = queue.join(a);
            }

            let end = op_call(OP_CODE::end_scope_op);
            let itr = get_expression_from_code();

            while (end != itr) {
                itr = get_expression_from_code();
            }
            
            return queue;
        }

    }  // end eval
}
#endif // FUNCTION_OPERATORS_H