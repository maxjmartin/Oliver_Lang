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

#include "Oliver.h"

Olly::tokens_type parse_input(const Olly::str_type& input) {

    Olly::tokens_type code_tokens;

    Olly::parser lex(input);
    code_tokens = lex.parse();

    Olly::file_writer f("parsed_code.olc");

    for (auto i : code_tokens) {
        f.write_line(i);
    }

    return code_tokens;
}

Olly::let compile_tokens(const Olly::tokens_type& tokens) {

    Olly::compiler comp(tokens);
    return comp.compile();

}

Olly::let eval_input(Olly::str_type input) {

    Olly::let code = compile_tokens(parse_input(input));

    // Olly::print("input   = " + str(code));

    Olly::eval::evaluator olly;
    code = olly.eval(code);

    return code;
}

Olly::let eval(const Olly::str_type text) {
    return eval_input(text);
}

int main(Olly::int_type argc, char** argv) {
	
    try {
        if (argc == 1) {

            Olly::str_type input = "";

            while (Olly::to_lower(input) != "exit") {

                std::cout << ">> ";
                std::getline(std::cin, input);

                Olly::let output = eval_input(input);
                std::cout << std::endl;
            }
            return 0;
        }

        if (argc == 2) {

            Olly::let code = eval_input(argv[1]);
        }
    }
    catch (std::exception& e) {
        std::cerr << "Error during runtime: " << e.what() << std::endl;
    }
    return 0;
}
