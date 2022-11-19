#ifndef COMPILER_H
#define COMPILER_H

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

#include <string>

#include "../text_reader.h"
#include "../token_reader.h"
#include "Data_Types/let.h"
#include "Data_Types/fundamental_types/expression.h"
#include "Data_Types/fundamental_types/error.h"
#include "Data_Types/fundamental_types/format.h"
#include "Data_Types/fundamental_types/lambda.h"
#include "Data_Types/fundamental_types/list.h"
#include "Data_Types/fundamental_types/logical_term.h"
#include "Data_Types/fundamental_types/number.h"
#include "Data_Types/fundamental_types/op_call.h"
#include "Data_Types/fundamental_types/map.h"
#include "Data_Types/fundamental_types/string.h"
#include "Data_Types/fundamental_types/symbol.h"

namespace Olly {

        /********************************************************************************************/
        //
        //                                'compiler' class definition
        //
        //        The comiler class accepts either a file string, or a text vector, that is
        //        compiled to Oliver expressions. 
        //
        /********************************************************************************************/

        class compiler {

            std::vector<std::string>    _tokens;
            let                         _code;

        public:

            compiler(std::string text);
            compiler(std::vector<std::string> text);
            virtual ~compiler();

            let compile();

        private:

            compiler() = delete;
            compiler(const compiler& obj) = delete;

            bool_type is_prefix_unary_operator(OP_CODE opr) const;
            bool_type is_infix_binary_operator(OP_CODE opr) const;

            let get_infix_operator(OP_CODE opr) const;

            void place_term(let t);

            str_type collect_string(tokens_type::const_iterator& word, const str_type& stop);
        };


        /********************************************************************************************/
        //
        //                             'compiler' class implimentation
        //
        /********************************************************************************************/


        compiler::compiler(std::string text) : _tokens(), _code(expression()) {

            text_reader text_file(text);

            // _code = _code.join(expression());
            while (text_file.is()) {
                _tokens.push_back(text_file.get_line());
            }
        }

        compiler::compiler(std::vector<std::string> text) : _tokens(text), _code(expression()) {
        }

        compiler::~compiler() {
        }

        inline let compiler::compile() {
            _code = _code.join(expression());

            auto word = _tokens.begin();

            while (word != _tokens.cend()) {

                if (*word == "(" || *word == "[") {
                    _code = _code.join(expression());
                    *word = "";
                }

                if (*word == "{") {
                    _code = _code.join(expression());
                    place_term(op_call(OP_CODE::map_op));
                }

                else if (*word == "\'") {
                    str_type str = collect_string(word, "\'");

                    place_term(number(str));
                }

                else if (*word == "\"") {
                    str_type str = collect_string(word, "\"");

                    place_term(string(str));
                }

                else if (*word == "`") {
                    str_type str = collect_string(word, "`");

                    place_term(format(str));
                }

                //else if (*word == "`") {
                //    str_type str = collect_string(word, "`");

                //    tokens_type code_tokens;

                //    parser lex(str);
                //    code_tokens = lex.parse();

                //    compiler c(code_tokens);

                //    // place_term(format(c.compile()));
                //}

                else if (*word == ")" || *word == ";" || *word == "]" || *word == "}") {

                    let terms = pop_lead(_code);

                    let exp;
                        
                    if (*word == ")") {
                        exp = expression();
                    }
                    if (*word == ";") {
                        exp = expression();
                    }
                    else if (*word == "]") {
                        exp = list();
                    }
                    else if (*word == "}") {
                        exp = expression();
                    }

                    while (terms.is()) {

                        let term = pop_lead(terms);

                        if (term.op_code() == OP_CODE::lambda_op) {
                            // Define an anonymous function.

                            let a = pop_lead(exp);
                            let b = pop_lead(exp);

                            //if (b.op_code() == OP_CODE::start_scope_op) {
                            //    
                            //    b = pop_lead(exp).join(b);
                            //}

                            exp = exp.join(lambda(a, b));
                        }

                        else if (is_prefix_unary_operator(term.op_code())) {
                            // Convert prefix unary operators to postix unary.

                            let a = pop_lead(exp);
                            let b = term;

                            let p = expression();

                            p = p.join(a);
                            p = p.join(b);

                            exp = exp.join(p);
                        }

                        else if (is_infix_binary_operator(term.op_code())) {
                            // Convert infix operators to postfix operators.

                            let a = pop_lead(exp);
                            let b = get_infix_operator(term.op_code());

                            exp = exp.join(b);
                            exp = exp.join(a);
                        }

                        else {  // Place the term on the expression, list, or map.
                            exp = exp.join(term);
                        }
                    }

                    if (exp.lead().op_code() == OP_CODE::map_op) {

                        let args = exp.drop();

                        place_term(map(args));
                    }

                    else {
                        place_term(exp);
                    }
                }

                else if (*word == "LEAD" || *word == "JOIN" || *word == "DROP") {

                    auto it = OPERATORS.find(*word);

                    if (it != OPERATORS.end()) {

                        let opr = op_call(it->second);

                        place_term(opr);
                    }
                }

                else if (*word != "") {

                    str_type lower_case = to_lower(*word);

                    auto it = OPERATORS.find(lower_case);

                    if (it != OPERATORS.end()) {

                        let opr = op_call(it->second);

                        place_term(opr);
                    }
                    else {

                        if (lower_case == "true"  || lower_case == "false"     ||
                            lower_case == "1"     || lower_case == "0"         ||
                            lower_case == "undef" || lower_case == "undefined") {

                            place_term(logical_term(lower_case));
                        }

                        else if (lower_case != "nothing" && lower_case != "node") {

                            place_term(symbol(*word));
                        }
                    }
                }

                ++word;
            } 

            return _code.lead().reverse();
        }

        inline bool_type compiler::is_prefix_unary_operator(OP_CODE opr) const {

            if (opr > OP_CODE::PREFIX_OPERATORS_START && opr < OP_CODE::PREFIX_OPERATORS_STOP) {
                return true;
            }
            return false;
        }

        inline bool_type compiler::is_infix_binary_operator(OP_CODE opr) const {

            if (opr > OP_CODE::INFIX_OPERATORS_START && opr < OP_CODE::INFIX_OPERATORS_STOP) {
                return true;
            }
            return false;
        }

        inline let compiler::get_infix_operator(OP_CODE opr) const {

            let op;
            // TODO Must review if all these operators are in use.  

            switch (opr) {
                
                case OP_CODE::BIND_op:
                    op = op_call(OP_CODE::bind_op);
                    break;

                case OP_CODE::APPLY_op:
                    op = op_call(OP_CODE::apply_op);
                    break;

                case OP_CODE::AND_op:
                    op = op_call(OP_CODE::l_and_op);
                    break;

                case OP_CODE::OR_op:
                    op = op_call(OP_CODE::l_or_op);
                    break;

                case OP_CODE::XOR_op:
                    op = op_call(OP_CODE::l_xor_op);
                    break;

                case OP_CODE::EQ_op:
                    op = op_call(OP_CODE::eq_op);
                    break;

                case OP_CODE::NE_op:
                    op = op_call(OP_CODE::ne_op);
                    break;

                case OP_CODE::GE_op:
                    op = op_call(OP_CODE::ge_op);
                    break;

                case OP_CODE::GT_op:
                    op = op_call(OP_CODE::gt_op);
                    break;

                case OP_CODE::LE_op:
                    op = op_call(OP_CODE::le_op);
                    break;

                case OP_CODE::LT_op:
                    op = op_call(OP_CODE::lt_op);
                    break;

                case OP_CODE::ADD_op:
                    op = op_call(OP_CODE::add_op);
                    break;

                case OP_CODE::SUB_op:
                    op = op_call(OP_CODE::sub_op);
                    break;

                case OP_CODE::MUL_op:
                    op = op_call(OP_CODE::mul_op);
                    break;

                case OP_CODE::DIV_op:
                    op = op_call(OP_CODE::div_op);
                    break;

                case OP_CODE::MOD_op:
                    op = op_call(OP_CODE::mod_op);
                    break;

                case OP_CODE::FDIV_op:
                    op = op_call(OP_CODE::fdiv_op);
                    break;

                case OP_CODE::REM_op:
                    op = op_call(OP_CODE::rem_op);
                    break;

                case OP_CODE::POW_op:
                    op = op_call(OP_CODE::pow_op);
                    break;

                case OP_CODE::L_AND_op:
                    op = op_call(OP_CODE::l_and_op);
                    break;

                case OP_CODE::L_OR_op:
                    op = op_call(OP_CODE::l_or_op);
                    break;

                case OP_CODE::L_XOR_op:
                    op = op_call(OP_CODE::l_xor_op);
                    break;


                case OP_CODE::LEAD_op:
                    op = op_call(OP_CODE::lead_op);
                    break;

                case OP_CODE::JOIN_op:
                    op = op_call(OP_CODE::join_op);
                    break;

                case OP_CODE::DROP_op:
                    op = op_call(OP_CODE::drop_op);
                    break;
            }

            return op;
        }

        inline void compiler::place_term(let t) {

            let terms = pop_lead(_code);

            terms = terms.join(t);

            _code = _code.join(terms);
        }

        inline str_type compiler::collect_string(tokens_type::const_iterator& word, const str_type& stop) {

            str_type text = "";

            ++word;

            while (*word != stop && word != _tokens.cend()) {

                text += *word;

                ++word;
            }

            return text;
        }
}
#endif // COMPILER_H