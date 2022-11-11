#ifndef ASSOCIATIVE_OPERATORS_H
#define ASSOCIATIVE_OPERATORS_H

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

        inline void evaluator::associative_operators(OP_CODE& opr) {

            switch (opr) {



            



            



            case OP_CODE::has_op: {

                let y = get_expression_from_deque();
                let x = get_expression_from_deque();

                x = boolean(x.has(y));

                set_expression_on_deque(x);

            }   break;



            case OP_CODE::get_op: {

                let y = get_expression_from_deque();
                let x = get_expression_from_deque();

                x = x.get(y);

                set_expression_on_deque(x);

            }   break;



            case OP_CODE::set_op: {

                let z = get_expression_from_deque();
                let y = get_expression_from_deque();
                let x = get_expression_from_deque();

                while (x.type() == "symbol") {
                    x = get_symbol(x);
                }

                x = x.set(y, z);

                set_expression_on_deque(x);

            }   break;



            case OP_CODE::del_op: {

                let y = get_expression_from_deque();
                let x = get_expression_from_deque();

                x = x.del(y);

                set_expression_on_deque(x);

            }   break;



            default:
                break;
            }
        }

    }  // end eval
}
#endif // ASSOCIATIVE_OPERATORS_H