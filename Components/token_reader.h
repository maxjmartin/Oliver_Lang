#ifndef TOKEN_READER_H
#define TOKEN_READER_H

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
#include <vector>
#include "text_reader.h"

namespace Olly {

		/********************************************************************************************/
		//
		//                              'token_reader' class definition
		//
		//        The token_reader class simply manages iteration over a series of text tokens.
		//
		/********************************************************************************************/

		class token_reader {

		public:

			token_reader(std::vector<std::string>& input_code);
			virtual ~token_reader();

			std::string next();
			std::string peek();

			bool is() const;

			bool check_if_bounded(std::string left, std::string right);

		private:

			std::vector<std::string>&	_code;
			std::string					_word;
			int							_i;

			token_reader() = delete;
			token_reader(const token_reader& obj) = delete;
		};


		token_reader::token_reader(std::vector<std::string>& input_code) : _code(input_code), _i(0), _word("") {

			if (is()) {
				_word = _code[_i];
			}
		}

		token_reader::~token_reader() {
		}

		std::string token_reader::next() {

			if (is()) {

				std::string t = _word;

				_i += 1;

				_word = is() ? _code[_i] : "";

				return t;
			}
			else {
				_i = static_cast<int>(_code.size());
			}

			return "";
		}

		std::string token_reader::peek() {
			/*
				Return the current value of '_c'.
			*/
			return _word;
		}

		bool token_reader::is() const {
			/*
				Return true if the file is not eof
				and in good condition.
			*/
			return (_i < _code.size());
		}

		bool token_reader::check_if_bounded(std::string left, std::string right) {

			if (is()) {

				if (_code.back() == right) {
					_code.pop_back();

					if (_code.back() == right && _word == left) {
						return true;
					}
					else {
						_code.push_back(right);
					}
				}
			}
			return false;
		}

}
#endif // TOKEN_READER_H