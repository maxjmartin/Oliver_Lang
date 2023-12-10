
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

#include "text_parser.h"

namespace Oliver {

    const std::vector<std::string> TextParser::EnclosureChars = {
        "(", ")", "'", "'", "\"", "\"", "[", "]", "{", "}", "`", "`"
    };

    TextParser::TextParser(std::string_view input) : _input(input), _code(), _skip(false), _c('\0') {
    }

    TextParser::~TextParser() {
    }

    std::vector<std::string> TextParser::parse() {
        /*
            Ensure that the file has content to parse.
        */

        if (!_input.is()) {
            return _code;
        }

        handle_leading_whitespace();

        std::string  word = "("s;

        process_word(word);

        while (_input.is()) {
            /*
                Grab each character and split each group
                of characters on whitespace or on a few
                special characters which have a specific
                semantic meaning.
            */

            _c = _input.next();

            if (!_skip) {

                if (whitespace_char(_c) || _c == ',') {

                    process_word(word);
                }

                else if (_c == '#') {
                    handle_comment_operator(word);
                }

                else if (word == "" && _c == '-') {
                    handle_unary_negation_operator(word);
                }

                else if (word == "" && _c == '+') {
                    handle_unary_addition_operator(word);
                }

                else if (word != "" && _c == '?') {
                    handle_logical_equivalence_operator(word);
                }

                else if (_c == '\'') {
                    handle_numeric_identifier(word);
                }

                else if (_c == '"') {
                    handle_string_identifier(word);
                }

                else if (_c == '\\') {
                    handle_regex_identifier(word);
                }

                else if (_c == '`') {
                    handle_io_format_identifier(word);
                }

                else if (_c == '(' || _c == ')') {
                    handle_paren_expression_identifier(word);
                }

                else if (_c == ':' || _c == ';') {
                    handle_colon_expression_identifier(word);
                }

                else if (_c == '[' || _c == ']') {
                    handle_list_identifier(word);
                }

                else if (_c == '{' || _c == '}') {
                    handle_map_identifier(word);
                }

                else {
                    word += _c;
                }
            }
            else if (_c == '#') {
                handle_comment_operator(word);
            }
        }

        process_word(word);

        word = ")"s;

        process_word(word);

        return _code;
    }

    int TextParser::is_regex_escape_char(const char& c) {
        /*
            Determine if an encountered character
            could be a part of an escaped char
            like '\t' or '\"' for examples.
        */

        for (char o : "\\") {  // -[]/{}()*+?.\\^$|
            // TODO - flesh this out.  

            if (c == o) {
                return true;
            }
        }

        return false;
    }

    int TextParser::is_string_escape_char(const char& c) {
        /*
            Determine if an encountered character
            could be a part of an escaped char
            like '\t' or '\"' for examples.
        */

        for (char o : "\'\"\\abfnrtv") {

            if (c == o) {
                return true;
            }
        }

        return false;
    }

    void TextParser::process_word(std::string& word) {
        /*
            Check that we are not within a comment block.
            Else ensure we have a word to handle and place
            it on the back of the str_t queue.
        */

        if (word != "") {
            _code.push_back(word);
            word.clear();
        }
    }

    void TextParser::handle_leading_whitespace() {

        while (_input.is()) {
            /*
                Loop through any whitespace at the start
                of the file.
            */

            if (!whitespace_char(_input.peek())) {
                break;
            }


            _input.next();
        }
    }

    void TextParser::handle_comment_operator(std::string& word) {
        /*
            TODO:  Change this over to the typical C++ comment str_t.
            A comment has probably been encountered.
            '#'  comment to the end of the line.
            '##' comments everything up to a second '##'.
            '#!' Reserved to implement a preprocessor type system?
            '!#' Or maybe this should be the reserved preprocessor symbol?
        */

        process_word(word);

        if (_input.peek() == '#') {
            /*
                Comment blocks are handled by setting a
                boolean value to identify when a comment
                is found.  Else a function is called to
                handle the comment up to the next line.
            */

            _skip = !_skip;

            skip_comment_line();
        }

        else if (_input.peek() == '!') {
            /*
                Comment blocks are handled by setting a
                boolean value to identify when a comment
                is found.  Else a function is called to
                handle the comment up to the next line.
            */

            _code.push_back("#!");
            _input.next();
            _input.next();
        }

        else {
            skip_comment_line();
        }
    }

    void TextParser::handle_unary_negation_operator(std::string& word) {

        if (_input.peek() == '-') {

            auto c = _input.next();

            if (_input.peek() == '>'/* || _input.peek() == '<'*/) {

                word = ""s;
                word.push_back(_c);
                word.push_back(c);
                word.push_back(_input.next());

                process_word(word);
                _c = ' ';
            }
            else {
                word = "neg"s;
                process_word(word);

                word = "neg"s;
                process_word(word);
            }
        }
        else if (!std::isdigit(_input.peek())) {

            if (_input.peek() != ' ') {
                word = "neg"s;
                process_word(word);
            }

            else {
                word = "-"s;
                process_word(word);
            }
        }
        else {
            word += _c;
        }
    }

    void TextParser::handle_unary_addition_operator(std::string& word) {

        if (!std::isdigit(_input.peek())) {

            if (_input.peek() != ' ') {
                word = "idnt"s;
                process_word(word);
            }
            else {
                word = "+"s;
                process_word(word);
            }
        }
        else {
            word += _c;
        }
    }

    inline void TextParser::handle_logical_equivalence_operator(std::string& word) {

        process_word(word);

        while (_input.peek() == '?') {
            _input.next();
        }

        word = "??"s;
        process_word(word);
    }

    void TextParser::handle_numeric_identifier(std::string& word) {
        /*
            A number was encountered.
        */

        process_word(word);

        _code.push_back("\'");
        _code.push_back(read_number());
        _code.push_back("\'");
    }

    void TextParser::handle_string_identifier(std::string& word) {
        /*
            A string identification was encountered.
            Process the string, and add it to _code
            so long as we are not in a comment block.
        */

        process_word(word);

        _code.push_back("\"");
        _code.push_back(read_string());
        _code.push_back("\"");
    }

    void TextParser::handle_regex_identifier(std::string& word) {
        /*
            A regex identification was encountered.
            Process the regex, and add it to _code
            so long as we are not in a comment block.
        */

        process_word(word);

        _code.push_back("\\");
        _code.push_back(read_regex());
        _code.push_back("\\");
    }

    void TextParser::handle_io_format_identifier(std::string& word) {
        /*
            A format identification was encountered.
            Process the format, and add it to _code
            so long as we are not in a comment block.
        */

        process_word(word);

        _code.push_back("`");
        _code.push_back(read_format());
        _code.push_back("`");
    }

    void TextParser::handle_paren_expression_identifier(std::string& word) {
        /*
            An expression operator was encountered.
            Process the expression, and add it to _code
            so long as we are not in a comment block.
        */

        process_word(word);

        _code.push_back(expression_op(_c));
    }

    void TextParser::handle_colon_expression_identifier(std::string& word) {
        /*
            An expression operator was encountered.
            Process the expression, and add it to _code
            so long as we are not in a comment block.
        */

        process_word(word);

        _code.push_back(expression_op(_c));
        _c = ' ';
    }

    void TextParser::handle_list_identifier(std::string& word) {
        /*
            A list operator was encountered.
            Process the list, and add it to _code
            so long as we are not in a comment block
        */

        process_word(word);

        _code.push_back(list_op(_c));

        _c = ' ';
    }

    void TextParser::handle_map_identifier(std::string& word) {
        /*
            An map operator was encountered.
            Process the map, and add it to _code
            so long as we are not in a comment block
        */

        process_word(word);

        if (_c == '{') {
            word = "{"s;
        }

        else {
            word = "}"s;
        }

        process_word(word);
        _c = ' ';
    }

    std::string TextParser::read_format() {
        /*
            Read each character including
            whitespace and form a word to
            be used to define a format
            map.
        */

        std::string str;

        char c;

        while (_input.is()) {

            c = _input.next();

            if (c == '`') {
                break;
            }

            else {
                str += c;
            }
        }

        if (str.empty()) {
            return ""s;
        }

        return str;
    }

    std::string TextParser::read_string() {
        /*
            Read each character including
            whitespace and form a word to
            be used to define a string of
            characters.

            When a '\' character is found
            check to see if it is used to
            define an escaped character.
            If it is append the escaped
            character to the string.
        */

        bool escaped = false;

        std::string str;

        char c;

        while (_input.is()) {

            c = _input.next();

            if (escaped) {

                switch (c) {

                case '\\':
                    str += '\\';
                    break;

                case 'a':
                    str += '\a';
                    break;

                case 'b':
                    str += '\b';
                    break;

                case 'f':
                    str += '\f';
                    break;

                case 'n':
                    str += '\n';
                    break;

                case 'r':
                    str += '\r';
                    break;

                case 't':
                    str += '\t';
                    break;

                case 'v':
                    str += '\v';
                    break;

                default:
                    str += c;
                    break;
                }

                escaped = false;
            }
            else if ((c == '\\') && (is_string_escape_char(_input.peek()))) {

                escaped = true;
            }
            else if (c == '"') {
                break;
            }
            else {
                str += c;
            }
        }

        return str;
    }

    std::string TextParser::read_number() {
        /*
            Read each character including
            whitespace and form a word to
            be used to define a string of
            characters.

            When a '\' character is found
            check to see if it is used to
            define an escaped character.
            If it is append the escaped
            character to the string.
        */

        std::string str;

        char c;

        while (_input.is()) {

            c = _input.next();

            if (c == '\'') {
                break;
            }
            else {
                str += c;
            }
        }

        return str;
    }

    std::string TextParser::read_regex() {
        /*
            Read each character including
            whitespace and form a word to
            be used to define a regex of
            characters.

            When a '\' character is found
            check to see if it is used to
            define an escaped character.
            If it is append the escaped
            character to the regex.
        */

        bool escaped = false;

        std::string str;

        char c;

        while (_input.is()) {

            c = _input.next();

            if (escaped) {

                if (c == '\\') {
                    str += '\\';
                }
                else {
                    str += c;
                }

                escaped = false;
            }
            else if ((c == '\\') && (is_string_escape_char(_input.peek()))) {

                escaped = true;
            }
            else if (c == '\\' && !escaped) {
                break;
            }
            else {
                str += c;
            }
        }

        return str;
    }

    std::string TextParser::list_op(const char& c) {
        /*
            Validate which list operator is provided.
        */
        if (c == '[') {

            return "["s;
        }

        return "]"s;
    }

    std::string TextParser::map_op(const char& c) {
        /*
            Validate which map operator is provided.
        */
        if (c == '{') {

            return "{"s;
        }

        return "}"s;
    }

    std::string TextParser::expression_op(const char& c) {
        /*
            Validate which expression operator is provided.
        */

        if (c == '(' || c == ':') {

            return "("s;
        }

        return ")"s;
    }

    void TextParser::skip_comment_line() {
        /*
            Skip all other characters until a newline is encountered.
        */
        char c;

        while (_input.is()) {

            c = _input.next();

            if (c == '\n') {
                break;
            }
        }
    }

    bool TextParser::whitespace_char(char c) {

        if (c < 32) {
            return true;
        }

        if (::isspace(c)) {
            return true;
        }

        return false;
    }

    inline std::string TextParser::pop_code() {

        if (!_code.empty()) {

            std::string word = _code.back();
            _code.pop_back();

            return word;
        }

        return ""s;
    }
}