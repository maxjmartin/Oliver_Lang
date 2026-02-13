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
//        along with this program.If not, see <https://www.gnu.org/licenses/>.
//    
//        The author can be reached at: maxjmartin@gmail.com
//
/*****************************************************************************************/

namespace Oliver {

    /********************************************************************************************/
    //
    //                                 Interpreter Operator ENUM
    // 
    //          This file is required by the 'var' class.  But at this time is not used
    //          in any other data types.  This will be updated once the 'op_code' data 
    //          type is reimplemented.
    //
    /********************************************************************************************/

    enum class op_code {
        nothing_op = 0,

        /************************** Compile Time Operators ***************************/

        PREFIX_OPERATORS_START,

        idnt_op, neg_op,

        PREFIX_OPERATORS_STOP,

        INFIX_OPERATORS_START,

        AND_op, OR_op, XOR_op,
        ADD_op, SUB_op, MUL_op, DIV_op, MOD_op, FDIV_op, REM_op, EXP_op,
        EQ_op, NE_op, LT_op, LE_op, GT_op, GE_op,
        L_AND_op, L_OR_op, L_XOR_op,
        LEAD_op, JOIN_op, DROP_op,
        GET_op, SET_op, HAS_op, DEL_op,

        BIND_op, APPLY_op,

        INFIX_OPERATORS_STOP,

        /**************************** Runtime Operators *****************************/

        shebang_op, no_except_op, rec_limit_op, stack_limit_op,

        deque_op, emit_op, endl_op, input_op,
        assign_op, let_op,
        bool_op, size_op, type_op,
        L_IMP_op, l_imp_op,
        if_op, elif_op, else_op,
        func_op, def_op, end_scope_op,
        FUNDAMENTAL_OPERATORS,

        lead_op, join_op, drop_op, next_op,
        deque_lead_op, deque_join_op, deque_drop_op,
        deque_last_op, deque_push_op, deque_shift_op,
        SEQUENTIAL_OPERATORS,

        has_op, get_op, set_op, del_op,
        INDEX_OPERATORS,

        add_op, sub_op, mul_op, div_op, mod_op, fdiv_op, rem_op, exp_op,
        eq_op, ne_op, lt_op, le_op, gt_op, ge_op,
        l_and_op, l_or_op, l_xor_op,
        BINARY_OPERATORS,

        bool_alpha_op, bool_numeric_op,
        IO_OPERATORS,





        map_op, clear_op,

        result_op,

        rev_op,
        ALGORITHM_OPERATORS,

        END_OPERATORS
    };
}
