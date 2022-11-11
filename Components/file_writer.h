#ifndef FILE_WRITER_H
#define FILE_WRITER_H

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

#include <fstream>
#include <mutex>
#include <sstream>

namespace Olly {

	/********************************************************************************************/
	//
	//                              'file_writer' class definition
	//
	//        The file_writer class opens a file and then is used to write text to the
	//        file.
	//
	/********************************************************************************************/

	class file_writer {

		std::fstream			_output;
		std::stringstream   	_stream;
		std::recursive_mutex	_mutex;
		bool					_locked;

	public:

		file_writer(const std::string& inp);
		virtual ~file_writer();

		bool is();

		void write(const std::string& word);
		void write_line(const std::string& word);

	private:
		file_writer();
		file_writer(const file_writer& obj) = delete;
	};

	/********************************************************************************************/
	//
	//   file_writer implimentation.
	//
	/********************************************************************************************/

	file_writer::file_writer() : _output(""), _stream(), _mutex(), _locked(false) {
	}

	file_writer::file_writer(const std::string& output_code) : _output(output_code, std::fstream::out), _stream(), _mutex(), _locked(_mutex.try_lock()) {
	}

	file_writer::~file_writer() {
		_output.close();

		if (_locked) {
			_mutex.unlock();
		}
	}


	bool file_writer::is() {
		/*
			Return true if the file is not eof
			and in good condition.
		*/

		return _output.good();
	}



	void file_writer::write(const std::string& word) {

		if (_output.good()) {

			_output << word;
		}
	}

	void file_writer::write_line(const std::string& word) {

		if (_output.good()) {

			_output << word;
			_output << std::endl;
		}
	}
}
#endif // FILE_WRITER_H