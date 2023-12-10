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

#include <algorithm>
#include <format>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "text_reader.h"
#include "text_support.h"

namespace Oliver {

    /********************************************************************************************/
    //
    //                                'TextParser' class definition
    //
    //        The TextParser class accepts individual characters from an instance of
    //        the TextReader.  The TextParser then appends each character together
    //        until a specific combination of characters or an individual character
    //        is recognized invoking a specific behavior.  Else all words are 
    //        defined by being split at whitespace, or on comas.  
    //
    /********************************************************************************************/

    class TextParser {

    public:
        using tokens = std::vector<std::string>;

        TextParser(std::string_view input);
        ~TextParser();

        std::vector<std::string> parse();

    private:

        TextReader      _input;    // The lexical code to parse.
        tokens          _code;     // The parsed code read from the input.
        bool            _skip;     // Identifies if a token exists within the bounds of a comment block.
        char            _c;        // The current character being handled.

        TextParser()                            = delete;
        TextParser(const TextParser& obj)       = delete;
        TextParser(TextParser&& other) noexcept = delete;

        TextParser& operator=(const TextParser& other)     = delete;
        TextParser& operator=(TextParser&& other) noexcept = delete;

        int is_regex_escape_char(const char& c);
        int is_string_escape_char(const char& c);

        void process_word(std::string& word);

        void handle_leading_whitespace();
        void handle_comment_operator(std::string& word);
        void handle_unary_negation_operator(std::string& word);
        void handle_unary_addition_operator(std::string& word);
        void handle_logical_equivalence_operator(std::string& word);
        void handle_numeric_identifier(std::string& word);
        void handle_string_identifier(std::string& word);
        void handle_regex_identifier(std::string& word);
        void handle_io_format_identifier(std::string& word);
        void handle_paren_expression_identifier(std::string& word);
        void handle_colon_expression_identifier(std::string& word);
        void handle_list_identifier(std::string& word);
        void handle_map_identifier(std::string& word);

        std::string read_string();
        std::string read_number();
        std::string read_format();
        std::string read_regex();
        std::string list_op(const char& c);
        std::string map_op(const char& c);
        std::string expression_op(const char& c);

        void skip_comment_line();

        bool whitespace_char(char c);
            
        std::string pop_code();

        static const std::vector<std::string> EnclosureChars;
    };
}