#ifndef SEQUENCE_OPERATORS_H
#define SEQUENCE_OPERATORS_H

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

        inline void evaluator::sequence_operators(OP_CODE& opr) {

            switch (opr) {

                case OP_CODE::lead_op: {

                    let x = get_expression_from_deque();

                    x = x.lead();
                    set_expression_on_deque(x);

                }   break;


                case OP_CODE::join_op: {

                    let y = get_expression_from_deque();
                    let x = get_expression_from_deque();

                    x = y.join(x);
                    set_expression_on_deque(x);

                }   break;


                case OP_CODE::drop_op: {

                    let x = get_expression_from_deque();

                    x = x.drop();
                    set_expression_on_deque(x);

                }   break;


                case OP_CODE::deque_lead_op: {

                    let x = get_expression_from_deque(); 

                    if (x.op_code() == OP_CODE::deque_op) {

                        x = _deque.size() ? _deque.front() : nothing();
                    }

                    set_expression_on_deque(x);

                }   break;


                case OP_CODE::deque_join_op: {

                    let x = get_expression_from_deque();
                    let y = get_expression_from_deque();

                    if (x.op_code() == OP_CODE::deque_op && y.is_something()) {

                        _deque.emplace_front(y);
                    }

                }   break;


                case OP_CODE::deque_drop_op: {

                    let x = get_expression_from_deque();

                    if (x.op_code() == OP_CODE::deque_op && _deque.size()) {

                        _deque.pop_front();
                    }

                }   break;
            }
        }

    }  // end eval
}
#endif // SEQUENCE_OPERATORS_H