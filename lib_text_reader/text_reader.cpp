
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

#include "text_reader.h"

namespace Oliver {
    
    TextReader::TextReader(const std::string_view input_code) : _c('\0'), _input(), _is_file(std::filesystem::exists(input_code.data())), _stream(), _mutex() {
        /*
            Open a file 'file_name' and retrieve its first character
            Setting the value of the character to '_c'.  So long as
            the file is not empty.
        */
        std::lock_guard<std::recursive_mutex> lock(_mutex);

        if (_is_file) {

            _input = std::fstream(input_code.data(), std::fstream::in);

            _c = _input.get();
        }
        else {
            /*
                Not a file, so send the text to a stream to be
                treated just like a file.
            */
            _is_file = false;
            _stream << input_code;

            _c = _stream.get();
        }
    }

    TextReader::~TextReader() {
        _input.close();
    }

    char TextReader::next() {
        /*
            As long as the file is not at the end of the file,
            return the current value of '_c' and set the next
            value of '_c'.  Return a null character if the
            file is eof.
        */

        if (_is_file) {
            std::lock_guard<std::recursive_mutex> lock(_mutex);

            char t = _c;

            if (is()) {
                _input >> std::noskipws >> _c;
            }

            return t;
        }


        if (is()) {

            char t = _c;

            _stream >> std::noskipws >> _c;

            return t;
        }

        return '\0';
    }

    char TextReader::peek() const {
        /*
            Return the current value of '_c'.
        */
        return _c;
    }

    bool TextReader::is() {
        /*
            Return true if the file is not eof
            and in good condition.
        */

        if (_is_file) {
            return (_input.is_open() && _input.good() && !_input.eof());
        }
        return (!_stream.eof() && _stream.good());
    }

    bool TextReader::is_file() const {
        /*
            Return true if the file is not eof
            and in good condition.
        */

        return _is_file;
    }

    std::string TextReader::get_line() {

        if (_is_file) {
            std::lock_guard<std::recursive_mutex> lock(_mutex);

            if (is()) {

                std::string line;

                std::getline(_input, line);

                lrtrim(line);

                if (line.empty()) {
                    return ""s;
                }

                return line;
            }
        }

        return ""s;
    }
}