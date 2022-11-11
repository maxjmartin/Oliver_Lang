#ifndef BINARY_OPERATORS_H
#define BINARY_OPERATORS_H

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
        
        inline void evaluator::binary_operators(OP_CODE& opr) {

            let y = get_expression_from_deque();
            let x = get_expression_from_deque();

            switch (opr) {

            case OP_CODE::l_and_op:
                x = logical_term(x.is() && y.is());
                break;

            case OP_CODE::l_or_op:
                x = logical_term(x.is() || y.is());
                break;

            case OP_CODE::l_xor_op:
                x = logical_term(static_cast<int_type>(x.is()) ^ static_cast<int_type>(y.is()));
                break;

            case OP_CODE::eq_op:
                x = logical_term(x == y);
                break;

            case OP_CODE::ne_op:
                x = logical_term(x != y);
                break;

            case OP_CODE::gt_op:
                x = logical_term(x > y);
                break;

            case OP_CODE::ge_op:
                x = logical_term(x >= y);
                break;

            case OP_CODE::lt_op:
                x = logical_term(x < y);
                break;

                case OP_CODE::le_op:
                x = logical_term(x <= y);
                break;

            case OP_CODE::add_op:
                x = x + y;
                break;

            case OP_CODE::sub_op:
                x = x - y;
                break;

            case OP_CODE::mul_op:
                x = x * y;
                break;

            case OP_CODE::div_op:
                x = x / y;
                break;

            case OP_CODE::mod_op:
                x = x % y;
                break;

            case OP_CODE::fdiv_op:
                x = x.f_div(y);
                break;

            case OP_CODE::rem_op:
                x = x.rem(y);
                break;

            case OP_CODE::pow_op:
                x = x.pow(y);
                break;

            default:
                break;
            }

            set_expression_on_deque(x);
        }

    }  // end eval
}
#endif // BINARY_OPERATORS_H