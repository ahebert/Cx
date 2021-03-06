/*
The MIT License (MIT)

Copyright (c) 2015 Aaron Hebert <aaron.hebert@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#ifndef CX_DYNAMIC_API
#include <cstdio>
#include <cstring>
#include <cerrno>
#include <codecvt>
#include <fstream>
#include <ctime>
#endif

#include <iostream>
#include <string>
#include "buffer.h"
#include "symtab.h"

namespace cx{
	namespace buffer {
		// special end-of-file character
		const char EOF_CHAR = 0x7F;
		int current_line_number = 0;

		/* "virtual" position of the current char
		 *  In the input buffer (with tabs expanded) */
		int input_position = 0;
		const int max_printline_length = 80;
		bool list_flag = false;
		list_buffer list; // the list file buffer
	}
	
	/** Constructor     Construct a input text buffer by opening the
	 *                  input file.
	 *
	 * @param p_input_file_name : ptr to the name of the input file
	 * @param ac             : abort code to use if open failed
	 */
	text_in_buffer::text_in_buffer(std::wstring input_file_name,
		abort_code ac) : file_name_(input_file_name) {

		// Open the input file.  Abort if failed.
		file.open(file_name_.c_str(), std::ios::in);
		file.imbue(std::locale(file.getloc(), new std::codecvt_utf8<wchar_t, 0x10ffff, std::little_endian>));

		if (!file.good()) {
			std::wcout << file_name_ << ": " << std::strerror(errno) << std::endl;
			abort_translation(ac);
		}
	}

	/** get_char        Fetch and return the next__ character from the
	 *                 text buffer.  If at the end of the buffer,
	 *                 read the next__ source line.  If at the end of
	 *                 the file, return the end-of-file character.
	 *
	 * @return next__ character from the source file
	 *          or the end-of-file character.
	 */
	wchar_t text_in_buffer::get_char(void) {
		const int tab_size = 8; // size of tabs
		wchar_t ch; // character to return

		if (*p_char == buffer::EOF_CHAR) return buffer::EOF_CHAR; // end of file
		else if (*p_char == L'\0') ch = get_line(); // null
		else { // next__ char
			++p_char;
			++buffer::input_position;
			ch = *p_char;
		}

		// If tab character, increment input_position to the next__
		// multiple of tab_size.
		if (ch == L'\t') buffer::input_position += tab_size - buffer::input_position % tab_size;

		return ch;
	}

	/** put_back_char     put the current character back into the
	 *                  input buffer so that the next__ call to
	 *                  get_char will fetch this character. (Only
	 *                  called to put back a '.')
	 *
	 * @return the previous character
	 */
	wchar_t text_in_buffer::put_back_char(void) {
		--p_char;
		--buffer::input_position;

		return *p_char;
	}

	/** Constructor     Construct a source buffer by opening the
	 *                  source file.  Initialize the list file, and
	 *                  read the first line from the source file.
	 *
	 * @param p_source_file_name : ptr to name of source file
	 */
	source_buffer::source_buffer(const std::wstring source_file_name)
		: text_in_buffer(source_file_name, ABORT_SOURCE_FILE_OPEN_FAILED) {
		// Initialize the list file and read the first source line.
		if (buffer::list_flag) buffer::list.initialize(source_file_name);
		get_line();
	}

	/** get_line         Read the next__ line from the source file, and
	 *                  print it to the list file preceded by the
	 *                  line number and the current nesting level.
	 *
	 * @return first character of the source line, or the
	 *          end-of-file character if at the end of the file
	 */
	wchar_t source_buffer::get_line(void) {
		if (file.eof()) {
			p_char = (wchar_t *)&buffer::EOF_CHAR;
			file.close();
		}

		// Else read the next__ source line and print it to the list file.
		else {
			memset(text, L'\0', sizeof(text));

			while (!wcscmp(text, L"\0")) {
				file.getline(text, MAX_INPUT_BUFFER_SIZE);

				if (file.eof()) break;

				++buffer::current_line_number;
			}

			p_char = text; // point to first source line char

			//std::wcout << text << std::endl;

			// just buffer current line, we can display on error
			buffer::list.wbuffer(
				text,
				buffer::current_line_number,
				scoping::current_nesting_level
				);

			if (buffer::list_flag) buffer::list.put_line();
		}

		buffer::input_position = 0;
		return *p_char;
	}



	/** Initialize      Initialize the list buffer.  Set the date
	 *                  for the page header, and print the first
	 *                  header.
	 *
	 * @param p_file_name : ptr to source file name (for page header)
	 */
	void list_buffer::initialize(const std::wstring file_name) {

		if (file_name.empty()) return;

		memset(text, L'\0', sizeof(text));

		// Copy the input file name.
		source_file_name = file_name;
	}


	///  put_line         print a line of text to the list file.

	void list_buffer::put_line(void) {

		// Truncate the line if it's too long.
		text[buffer::max_printline_length] = '\0';

		// print the text line, and then blank out the text.
		std::wcout << text << std::endl;
		memset(text, '\0', sizeof(text));

		++line_count;
	}

	text_out_buffer::~text_out_buffer() {}
}
