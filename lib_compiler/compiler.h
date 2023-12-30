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

#include <vector>

#include "data_types.h"
#include "text_parser.h"

namespace Oliver {

    /********************************************************************************************/
    //
    //                                'compiler' class definition
    //
    //        The compiler class accepts either a file string, or a text vector, that is
    //        compiled to Oliver expressions. 
    //
    /********************************************************************************************/

    class compiler {

        using text_tokens = std::vector<std::string>;

        text_tokens _tokens;
        var         _code;

    public:

        compiler(std::string text);
        compiler(std::vector<std::string> text);

        var compile();

    private:

        compiler()                    = delete;
        compiler(const compiler& obj) = delete;

        bool is_prefix_unary_operator(op_code opr) const;
        bool is_infix_binary_operator(op_code opr) const;

        var get_infix_operator(op_code opr) const;

        void place_term(var t);

        std::string collect_string(text_tokens::const_iterator& word, const std::string& stop);
    };


    /********************************************************************************************/
    //
    //                             'compiler' class implementation
    //
    /********************************************************************************************/


    compiler::compiler(std::string text) : _tokens(), _code(expression()) {

        TextReader text_file(text);

        while (text_file.is()) {
            _tokens.push_back(text_file.get_line());
        }
    }

    compiler::compiler(std::vector<std::string> text) : _tokens(text), _code(expression()) {
    }

    var compiler::compile() {

        _code = _code.push(expression());

        auto word = _tokens.begin();

        while (word != _tokens.cend()) {

            if (*word == "(" || *word == "[") {
                _code = _code.push(expression());
                *word = ""s;
            }

            if (*word == "{") {
                _code = _code.push(expression());
                place_term(op_call(op_code::map_op));
            }

            else if (*word == "\'") {
                std::string str = collect_string(word, "\'");

                place_term(number(str));
            }

            else if (*word == "\"") {
                std::string str = collect_string(word, "\"");

                place_term(text(str));
            }

            else if (*word == ")" || *word == ";" || *word == "]") {

                var terms = _code.lead();

                var exp = expression();

                if (*word == "]") {
                    exp = list();
                }

                while (terms) {

                    var term = terms.lead();

                    if (term.op_call() == op_code::func_op) {
                        // Define an anonymous function.

                        var a = exp.lead();
                        var b = exp.lead();

                        exp = exp.push(function(a, b));
                    }

                    else if (is_prefix_unary_operator(term.op_call())) {
                        // Convert prefix unary operators to postfix unary.

                        var a = exp.lead();
                        var b = term;

                        var p = expression();

                        p = p.push(a);
                        p = p.push(b);

                        exp = exp.push(p);
                    }

                    else if (is_infix_binary_operator(term.op_call())) {
                        // Convert infix operators to postfix operators.

                        var a = exp.lead();
                        var b = get_infix_operator(term.op_call());

                        exp = exp.push(b);
                        exp = exp.push(a);
                    }

                    else {  // Place the term on the expression, list, or map.
                        exp = exp.push(term);
                    }
                }

                place_term(exp);   // TODO: get maps to work.
            }

            else if (*word == "}") {
                var terms = _code.lead();

                object obj{ terms };

                place_term(obj);
            }

            else if (*word != "") {

                auto it = operators.find(*word);

                if (it != operators.end()) {

                    var opr = op_call(it->second);

                    place_term(opr);
                }
                else {

                    std::string lower_case = to_lower(*word);

                    if (lower_case == "true" || lower_case == "false" ||
                        lower_case == "1" || lower_case == "0" ||
                        lower_case == "undef" || lower_case == "undefined") {

                        place_term(boolean(lower_case));
                    }

                    else if (lower_case != "nothing" && lower_case != "none") {

                        place_term(symbol(*word));
                    }
                }
            }

            ++word;
        }

        return var(_code.lead()).reverse();
    }

    bool compiler::is_prefix_unary_operator(op_code opr) const {

        if (opr > op_code::PREFIX_OPERATORS_START && opr < op_code::PREFIX_OPERATORS_STOP) {
            return true;
        }
        return false;
    }

    bool compiler::is_infix_binary_operator(op_code opr) const {

        if (opr > op_code::INFIX_OPERATORS_START && opr < op_code::INFIX_OPERATORS_STOP) {
            return true;
        }
        return false;
    }

    var compiler::get_infix_operator(op_code opr) const {

        var op;
        // TODO Must review if all these operators are in use.  

        switch (opr) {

        case op_code::AND_op:
            op = op_call(op_code::l_and_op);
            break;

        case op_code::OR_op:
            op = op_call(op_code::l_or_op);
            break;

        case op_code::XOR_op:
            op = op_call(op_code::l_xor_op);
            break;

        case op_code::EQ_op:
            op = op_call(op_code::eq_op);
            break;

        case op_code::NE_op:
            op = op_call(op_code::ne_op);
            break;

        case op_code::GE_op:
            op = op_call(op_code::ge_op);
            break;

        case op_code::GT_op:
            op = op_call(op_code::gt_op);
            break;

        case op_code::LE_op:
            op = op_call(op_code::le_op);
            break;

        case op_code::LT_op:
            op = op_call(op_code::lt_op);
            break;

        case op_code::ADD_op:
            op = op_call(op_code::add_op);
            break;

        case op_code::SUB_op:
            op = op_call(op_code::sub_op);
            break;

        case op_code::MUL_op:
            op = op_call(op_code::mul_op);
            break;

        case op_code::DIV_op:
            op = op_call(op_code::div_op);
            break;

        case op_code::MOD_op:
            op = op_call(op_code::mod_op);
            break;

        case op_code::FDIV_op:
            op = op_call(op_code::fdiv_op);
            break;

        case op_code::REM_op:
            op = op_call(op_code::rem_op);
            break;

        case op_code::EXP_op:
            op = op_call(op_code::exp_op);
            break;

        case op_code::L_AND_op:
            op = op_call(op_code::l_and_op);
            break;

        case op_code::L_OR_op:
            op = op_call(op_code::l_or_op);
            break;

        case op_code::L_XOR_op:
            op = op_call(op_code::l_xor_op);
            break;


        case op_code::LEAD_op:
            op = op_call(op_code::lead_op);
            break;

        case op_code::JOIN_op:
            op = op_call(op_code::join_op);
            break;

        case op_code::DROP_op:
            op = op_call(op_code::drop_op);
            break;

        case op_code::GET_op:
            op = op_call(op_code::get_op);
            break;
        }

        return op;
    }

    void compiler::place_term(var t) {

        var terms = _code.lead();
            terms = terms.push(t);
            _code = _code.push(terms);
    }

    std::string compiler::collect_string(text_tokens::const_iterator& word, const std::string& stop) {

        std::string text = ""s;

        ++word;

        while (*word != stop && word != _tokens.cend()) {

            text += *word;

            ++word;
        }

        return text;
    }
}