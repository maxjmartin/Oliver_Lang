#ifndef TEXT_READER_H
#define TEXT_READER_H

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

#include <algorithm>
#include <fstream>
#include <mutex>
#include <sstream>
#include <string>

#include "string_support_functions.h"

namespace Olly {

	/********************************************************************************************/
	//
	//                              'text_reader' class definition
	//
	//        The text_reader class opens a file and then passes each individual character
	//        to the text_reader for evaluation.  If a valid file is not opened then the string
	//        itself is iterated over for reading.  
	//
	/********************************************************************************************/

	class text_reader {

		char					_c;
		std::fstream			_input;
		bool   					_is_file;
		std::stringstream		_stream;
		std::recursive_mutex	_mutex;

	public:

		text_reader(const std::string& inp);
		virtual ~text_reader();

		char next();
		char peek();

		std::string get_line();

		bool is();

		bool is_file();

	private:
		text_reader();
		text_reader(const text_reader& obj) = delete;
	};

	/********************************************************************************************/
	//
	//   Text reader implimentation.
	//
	/********************************************************************************************/


	text_reader::text_reader() : _c('\0'), _input(), _is_file(false), _stream(), _mutex() {
    }

    text_reader::text_reader(const std::string& input_code) : _c('\0'), _input(), _is_file(true), _stream(), _mutex() {
        /*
            Open a file 'file_name' and retrieve its first character
            Setting the value of the character to '_c'.  So long as
            the file is not empty.
        */
        std::lock_guard<std::recursive_mutex> lock(_mutex);

        _input = std::fstream(input_code, std::fstream::in);

        if (_input.is_open() && _input.good() && !_input.eof()) {

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

    text_reader::~text_reader() {
        _input.close();
    }

    char text_reader::next() {
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

    char text_reader::peek() {
        /*
            Return the current value of '_c'.
        */
        return _c;
    }

    bool text_reader::is() {
        /*
            Return true if the file is not eof
            and in good condition.
        */

        if (_is_file) {
            return (_input.is_open() && _input.good() && !_input.eof());
        }
        return (!_stream.eof() && _stream.good());
    }

    bool text_reader::is_file() {
        /*
            Return true if the file is not eof
            and in good condition.
        */

        return _is_file;
    }

    std::string text_reader::get_line() {

        if (_is_file) {
            std::lock_guard<std::recursive_mutex> lock(_mutex);

            if (is()) {

                std::string line;

                std::getline(_input, line);

                lrtrim(line);

                if (line.empty()) {
                    return "";
                }

                return line;
            }
        }

        return "";
    }

}
#endif // TEXT_READER_H