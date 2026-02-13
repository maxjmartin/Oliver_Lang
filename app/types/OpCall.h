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

#include <map>
#include <string> 

#include "Var.h"
#include "support_types/OpCodes.h"

namespace Oliver {

    class op_call {

        static const std::map<std::string, op_code>& get_operators() {
            static const std::map<std::string, op_code>& operators = *new std::map<std::string, op_code>{

                // Shebang Operators
                { "#!",              op_code::shebang_op },    { "NO_EXCEPT",     op_code::no_except_op },
                { "Recur_Limit",   op_code::rec_limit_op },    { "STACK_LIMIT", op_code::stack_limit_op },

                // Fundamental Operators
                { "none",            op_code::nothing_op },    { "nothing",         op_code::nothing_op },
                { "idnt",               op_code::idnt_op },    { "deque",             op_code::deque_op },
                { "<<",                 op_code::emit_op },    { ">>",                op_code::input_op },
                { "assign",           op_code::assign_op },    { "let",                 op_code::let_op },
                { "if",                   op_code::if_op },    { "??",                 op_code::bool_op },
                { "elif",               op_code::elif_op },    { "else",               op_code::else_op },
                { "func",               op_code::func_op },    { "def",                 op_code::def_op },
                { "neg",                 op_code::neg_op },    { "endl",               op_code::endl_op },
                { "type",               op_code::type_op },    { "size",               op_code::size_op },

                // Sequential Operators
                { "lead",               op_code::lead_op },    { "<--",                op_code::LEAD_op },
                { "join",               op_code::join_op },    { "<->",                op_code::JOIN_op },
                { "drop",               op_code::drop_op },    { "-->",                op_code::DROP_op },
                { "next",               op_code::next_op },
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

                // Mapping Operators
                { "get",                 op_code::get_op },    { "has",                 op_code::has_op },
                { "set",                 op_code::set_op },    { "del",                 op_code::del_op },
                { ".",                   op_code::GET_op },

                // Binary Infix and Postfix Logical Operators
                { "bool_alpha",        op_code::bool_alpha_op },    { "bool_numeric",    op_code::bool_numeric_op },



                // TODO: sort below operations

                { "size",               op_code::size_op },    { "size",               op_code::size_op },


                // print, str, repr, ...

                //  Operators to sort.


                { "rev",                 op_code::rev_op },

                /****************************************************************************/





                //{ "loop",         op_code::loop_op },
                //{ "imply",       op_code::imply_op },   { "else",        op_code::else_op },


                //{ "cond",         op_code::cond_op },
                //{ "const?",   op_code::is_const_op },
                //{ "var?",       op_code::is_var_op },



            };
            return operators;
        }

        op_code _value;

    public:

        op_call(op_code val);
        op_call(std::string str);

        friend bool                  _is_(const op_call& self);
        friend std::string         _type_(const op_call& self);
        friend std::size_t    _size_type_(const op_call& self);
        friend order               _comp_(const op_call& self, const var& other);
        friend std::string          _str_(const op_call& self, const Format_Args& fmt);
        friend op_code          _op_call_(const op_call& self);
    };

    op_call::op_call(op_code val) : _value(val) {
    }

    op_call::op_call(std::string str) : _value(op_code::nothing_op) {

        auto it = get_operators().find(str);

        if (it != get_operators().end()) {

            _value = it->second;
        }
    }

    bool _is_(const op_call& self) {
        return self._value != op_code::nothing_op;
    }

    std::string _type_(const op_call& self) {
        (void)self;
        return "op_call"s;
    }

    std::size_t _size_type_(const op_call& self){
        return static_cast<std::size_t>(self._value);
    }

    order _comp_(const op_call& self, const var& other) {

        const op_call* s = other.cast<op_call>();

        if (s) {
            if (self._value > s->_value) {
                return order::greater;
            }
            if (self._value < s->_value) {
                return order::less;
            }
            return order::equivalent;
        }

        return order::unordered;
    }

    std::string _str_(const op_call& self, const Format_Args& fmt) {

        (void)fmt;
        for (auto it = op_call::get_operators().cbegin(); it != op_call::get_operators().cend(); ++it) {

            if (it->second == self._value) {
                return it->first;
            }
        }
        return "unknown_operator"s;
    }

    op_code _op_call_(const op_call& self) {

        return self._value;
    }
}
