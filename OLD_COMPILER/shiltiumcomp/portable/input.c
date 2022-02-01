/*
Shiltiumcomp -- the first Shiltolang compiler

Copyright (C) 2010 Gregory Cohen <gregorycohen2@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <fcntl.h>
#include <unistd.h>

#include "../global_headers/headers.h"

attr_nonnull static void read_some_bytes(File* cr file) {
	assert_comparison(file, !=, NULL, File* cr, void*);
	
	ARROW_ASSIGN(file, buffer_offset) = 0;
	
	ARROW_ASSIGN(file, number_of_bytes_read) = read(ARROW(file, fd), file->buffer, sizeof(ARROW(file, buffer)));
	
	if (unlikely(ARROW(file, number_of_bytes_read) == -1)) {
		output_nullt_string("read() failed: ");
		output_errno_string();
		exit_program();
	}
	
	if (!ARROW(file, number_of_bytes_read)) {
		ARROW_ASSIGN(file, eof) = true;
	}
}

attr_nonnull void open_file_for_reading(File* cr file, const char* cr filename) {
	assert_comparison(file, !=, NULL, File* cr, void*);
	assert_comparison(filename, !=, NULL, const char* cr, void*);
	
	ARROW_ASSIGN(file, fd) = open(filename, O_RDONLY);
	if (unlikely(ARROW(file, fd) == -1)) {
		output_nullt_string("open() failed: ");
		output_errno_string();
		exit_program();
	}
	
	ARROW_ASSIGN(file, eof) = false;
	
	read_some_bytes(file);
}

attr_nonnull char get_char_from_file(File* cr file) {
	assert_comparison(file, !=, NULL, File* cr, void*);
	
	if (ARROW(file, buffer_offset) == ARROW(file, number_of_bytes_read)) {
		read_some_bytes(file);
	}
	
	if (ARROW(file, eof)) {
		return '\0';
	}
	
	return file->buffer[file->buffer_offset++];
}

attr_nonnull void rewind_file(File* cr file, const off_t number_of_bytes) {
	assert_comparison(file, !=, NULL, File* cr, void*);
	
	if (!number_of_bytes) return;
	
	if (number_of_bytes <= ARROW(file, buffer_offset)) {
		assert_comparison(ARROW(file, eof), ==, false, const bool, const bool);
		
		ARROW_ASSIGN(file, buffer_offset) -= number_of_bytes;
		return;
	}
	
	if (
		unlikely(
			lseek(
				ARROW(file, fd),
				-number_of_bytes - (ARROW(file, number_of_bytes_read) - ARROW(file, buffer_offset)),
				SEEK_CUR
			) == -1
		)
	) {
		output_nullt_string("lseek() failed: ");
		output_errno_string();
		output_newline();
		exit_program();
	}
	
	/*
	 *  This is done to make get_char_from_file() call read_some_bytes() the
	 *  next time it's called. That function will then set buffer_offset to 0.
	 */
	file->buffer_offset = file->number_of_bytes_read;
	
	file->eof = false;
}

attr_nonnull bool is_eof(File* cr file) {
	assert_comparison(file, !=, NULL, File* cr, void*);
	
	return ARROW(file, eof);
}

attr_nonnull void close_file(File* cr file) {
	assert_comparison(file, !=, NULL, File* cr, void*);
	
	if (unlikely(close(file->fd) == -1)) {
		output_nullt_string("close() failed: ");
		output_errno_string();
		exit_program();
	}
}
