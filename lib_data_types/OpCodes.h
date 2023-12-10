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

    const enum class op_code {
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

        BIND_op, APPLY_op,

        INFIX_OPERATORS_STOP,

        /**************************** Runtime Operators *****************************/

        shebang_op, no_except_op,
        deque_op, emit_op, endl_op, enter_op,      
        assign_op, let_op,
        is_def_op, size_op, type_op,
        l_eq_op, L_IMP_op, l_imp_op,
        if_op, elif_op, else_op,
        func_op, def_op, end_scope_op,
        FUNDAMENTAL_OPERATORS,

        lead_op, join_op, drop_op,
        deque_lead_op, deque_join_op, deque_drop_op,
        deque_last_op, deque_push_op, deque_shift_op,
        SEQUENTIAL_OPERATORS,

        add_op, sub_op, mul_op, div_op, mod_op, fdiv_op, rem_op, exp_op,
        eq_op, ne_op, lt_op, le_op, gt_op, ge_op,
        l_and_op, l_or_op, l_xor_op,
        BINARY_OPERATORS,

        bool_alpha_op, bool_numeric_op,
        IO_OPERATORS,



        apply_op, has_op, get_op, set_op, del_op, bind_op, 
        FUNCTIONAL_OPERATORS,

        map_op, clear_op,

        result_op,

        END_OPERATORS
    };

    /********************************************************************************************/
    //
    //                                 Interpreter Operator Map
    //
    /********************************************************************************************/

    //static const std::map<std::string, op_code> operators = {
    static const boost::container::flat_map<std::string, op_code> operators = {

        // Fundamental Operators
        { "#!",              op_code::shebang_op },    { "NO_EXCEPT",     op_code::no_except_op },
        { "none",            op_code::nothing_op },    { "nothing",         op_code::nothing_op },
        { "idnt",               op_code::idnt_op },    { "deque",             op_code::deque_op },
        { "<<",                 op_code::emit_op },    { ">>",                op_code::enter_op },
        { "assign",           op_code::assign_op },    { "let",                 op_code::let_op },
        { "if",                   op_code::if_op },    { "is",               op_code::is_def_op },
        { "elif",               op_code::elif_op },    { "else",               op_code::else_op },
        { "func",               op_code::func_op },    { "def",                 op_code::def_op },
        { "neg",                 op_code::neg_op },    { "endl",               op_code::endl_op },

        // Fundamental Sequential Operators
        { "lead",               op_code::lead_op },    { "<--",                op_code::LEAD_op },
        { "join",               op_code::join_op },    { "<->",                op_code::JOIN_op },
        { "drop",               op_code::drop_op },    { "-->",                op_code::DROP_op },
        { "lead_",        op_code::deque_lead_op },    { "_last",        op_code::deque_last_op },
        { "join_",        op_code::deque_join_op },    { "_join",        op_code::deque_push_op },
        { "drop_",        op_code::deque_drop_op },    { "_drop",       op_code::deque_shift_op },

        // Binary Postfix Mathmatical Operators
        { "add",                 op_code::add_op },    { "sub",                  op_code::sub_op },
        { "mul",                 op_code::mul_op },    { "div",                  op_code::div_op },
        { "mod",                 op_code::mod_op },    { "fdiv",                op_code::fdiv_op },
        { "rem",                 op_code::rem_op },    { "exp",                  op_code::exp_op },

        // Binary Infix Mathmatical Operators
        { "+",                   op_code::ADD_op },    { "-",                  op_code::SUB_op },
        { "*",                   op_code::MUL_op },    { "/",                  op_code::DIV_op },
        { "%",                   op_code::MOD_op },    { "//",                op_code::FDIV_op },
        { "%%",                  op_code::REM_op },    { "**",                 op_code::EXP_op },

        // Binary Postfix Relational Operators
        { "eq",                   op_code::eq_op },    { "le",                  op_code::le_op },
        { "ne",                   op_code::ne_op },    { "gt",                  op_code::gt_op },
        { "lt",                   op_code::lt_op },    { "ge",                  op_code::ge_op },

        // Binary Infix Relational Operators
        { "=",                    op_code::EQ_op },    { "-=",                  op_code::NE_op },
        { "<=",                   op_code::LE_op },    { ">=",                  op_code::GE_op },
        { "<",                    op_code::LT_op },    { ">",                   op_code::GT_op },

        // Binary Infix and Postfix Logical Operators
        { "&",                 op_code::L_AND_op },    { "and",              op_code::l_and_op },
        { "|",                  op_code::L_OR_op },    { "or",                op_code::l_or_op },
        { "^",                 op_code::L_XOR_op },    { "xor",              op_code::l_xor_op },
        { "then",              op_code::L_IMP_op },    { "imply",            op_code::l_imp_op },

        // Binary Infix and Postfix Logical Operators
        { "bool_alpha",        op_code::bool_alpha_op },    { "bool_numeric",    op_code::bool_numeric_op },



        // TODO: sort below operations

        { "size",               op_code::size_op },    { "size",               op_code::size_op },


        // print, str, repr, ...

        //  Operators to sort.


        { "get",                 op_code::get_op },    { "has",                 op_code::has_op },
        { "set",                 op_code::set_op },    { "del",                 op_code::del_op },

        /****************************************************************************/





        //{ "loop",         op_code::loop_op },
        //{ "imply",       op_code::imply_op },   { "else",        op_code::else_op },


        //{ "cond",         op_code::cond_op },
        //{ "const?",   op_code::is_const_op },
        //{ "var?",       op_code::is_var_op },



    };

}
