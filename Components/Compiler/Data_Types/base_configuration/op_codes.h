#ifndef OP_CODES_H
#define OP_CODES_H

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

#include "system_fundamentals.h"

namespace Olly {

    /********************************************************************************************/
    //
    //                                 Interpreter Operator ENUM
    //
    /********************************************************************************************/

    const enum class OP_CODE {
        nothing_op = 0,

        /************************** Compile Time Operators ***************************/

        PREFIX_OPERATORS_START,

            idnt_op, neg_op,

        PREFIX_OPERATORS_STOP,

        INFIX_OPERATORS_START,

            AND_op, OR_op, XOR_op,
            ADD_op, SUB_op, MUL_op, DIV_op, MOD_op, FDIV_op, REM_op, POW_op,
            EQ_op, NE_op, LT_op, LE_op, GT_op, GE_op,
            L_AND_op, L_OR_op, L_XOR_op,
            LEAD_op, JOIN_op, DROP_op,

            BIND_op, APPLY_op,

        INFIX_OPERATORS_STOP,

        /**************************** Runtime Operators *****************************/

            
            deque_op, emit_op, endl_op, enter_op,
            assign_op, let_op,
            size_op, type_op, 
            l_eq_op, L_CON_op, l_imp_op,
            if_op, elif_op, else_op, 
            lambda_op, def_op, end_scope_op,
        FUNDAMENTAL_OPERATORS,

            lead_op, join_op, drop_op, 
            deque_lead_op, deque_join_op, deque_drop_op,
        SEQUENTIAL_OPERATORS,       

            add_op, sub_op, mul_op, div_op, mod_op, fdiv_op, rem_op, pow_op,
            eq_op, ne_op, lt_op, le_op, gt_op, ge_op,
            l_and_op, l_or_op, l_xor_op, 
        BINARY_OPERATORS,
            

            

            apply_op, has_op, get_op, set_op, del_op, bind_op,
        //FUNCTIONAL_OPERATORS,

            
            




            
              
            
            
            map_op, clear_op,
            

            
              result_op,





            
        END_OPERATORS
    };

    /********************************************************************************************/
    //
    //                                 Interpreter Operator Map
    //
    /********************************************************************************************/

    static const std::map<str_type, OP_CODE> OPERATORS = {

        // Fundamental Operators
        { "none",            OP_CODE::nothing_op },    { "nothing",         OP_CODE::nothing_op },
        { "idnt",               OP_CODE::idnt_op },    { "deque",             OP_CODE::deque_op },
        { "<<",                 OP_CODE::emit_op },    { ">>",                OP_CODE::enter_op },
        { "assign",           OP_CODE::assign_op },    { "let",                 OP_CODE::let_op },
        { "if",                   OP_CODE::if_op },    { "elif",               OP_CODE::elif_op },
        { "elif",               OP_CODE::elif_op },    { "else",               OP_CODE::else_op },
        { "lambda",           OP_CODE::lambda_op },    { "def",                 OP_CODE::def_op },
        { "neg",                 OP_CODE::neg_op },    { "endl",               OP_CODE::endl_op },

        // Fundamental Sequential Operators
        { "lead",               OP_CODE::lead_op },    { "<--",                OP_CODE::LEAD_op },
        { "join",               OP_CODE::join_op },    { "<->",                OP_CODE::JOIN_op },
        { "drop",               OP_CODE::drop_op },    { "-->",                OP_CODE::DROP_op },
        { "LEAD",         OP_CODE::deque_lead_op },
        { "JOIN",         OP_CODE::deque_join_op },
        { "DROP",         OP_CODE::deque_drop_op },
        
        // Binary Postfix Mathmatical Operators
        { "_+",                 OP_CODE::add_op },    { "_-",                  OP_CODE::sub_op },
        { "_*",                 OP_CODE::mul_op },    { "_/",                  OP_CODE::div_op },
        { "_%",                 OP_CODE::mod_op },    { "_//",                OP_CODE::fdiv_op },
        { "_%%",                OP_CODE::rem_op },    { "_**",                 OP_CODE::pow_op },

        // Binary Infix Mathmatical Operators
        { "+",                   OP_CODE::ADD_op },    { "-",                  OP_CODE::SUB_op },
        { "*",                   OP_CODE::MUL_op },    { "/",                  OP_CODE::DIV_op },
        { "%",                   OP_CODE::MOD_op },    { "//",                OP_CODE::FDIV_op },
        { "%%",                  OP_CODE::REM_op },    { "**",                 OP_CODE::POW_op },

        // Binary Postfix Relational Operators
        { "_eq",                  OP_CODE::eq_op },    { "_le",                 OP_CODE::le_op },
        { "_ne",                  OP_CODE::ne_op },    { "_gt",                 OP_CODE::gt_op },
        { "_lt",                  OP_CODE::lt_op },    { "_ge",                 OP_CODE::ge_op },

        // Binary Infix Relational Operators
        { "=",                    OP_CODE::EQ_op },    { "-=",                  OP_CODE::NE_op },
        { "<=",                   OP_CODE::LE_op },    { ">=",                  OP_CODE::GE_op },
        { "<",                    OP_CODE::LT_op },    { ">",                   OP_CODE::GT_op },

        // Binary Infix and Postfix Logical Operators
        { "&",                 OP_CODE::L_AND_op },    { "_and",             OP_CODE::l_and_op },
        { "|",                  OP_CODE::L_OR_op },    { "_or",               OP_CODE::l_or_op },
        { "^",                 OP_CODE::L_XOR_op },    { "_xor",             OP_CODE::l_xor_op },
        { "?",                 OP_CODE::L_CON_op },    { "_imp",             OP_CODE::l_imp_op },


                { "size",               OP_CODE::size_op },    { "size",               OP_CODE::size_op },

        
        // print, str, repr, ...

        //  Operators to sort.


        { "get",                 OP_CODE::get_op },    { "has",                 OP_CODE::has_op },
        { "set",                 OP_CODE::set_op },    { "del",                 OP_CODE::del_op },

        /****************************************************************************/





        //{ "loop",         OP_CODE::loop_op },
        //{ "imply",       OP_CODE::imply_op },   { "else",        OP_CODE::else_op },


        //{ "cond",         OP_CODE::cond_op },
        //{ "const?",   OP_CODE::is_const_op },
        //{ "var?",       OP_CODE::is_var_op },



    };

}
#endif // OP_CODES_H
