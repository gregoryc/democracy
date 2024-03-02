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

#include "../global_headers/headers.h"

attr_noreturn
static void invalid_character(char char_) {
	char buffer[4] = {'0', '0', '\n', 0};
	
	buffer[1] = '0' + (char_ % 16);
	char_ /= 16;
	
	if (char_) {
		buffer[0] = '0' + (char_ % 16);
	}
	
	output_nullt_string("Invalid UTF-8 character U+");
	output_nullt_string(buffer);
	
	exit_program();
}

attr_nonnull void get_next_char(ParserState* cr state, char utf8_char[7]) {
	uchar char_;
	u length;
	
	assert_comparison(state, !=, NULL, ParserState* cr, void*);
	
	char_ = get_char_from_file(ARROW(state, file));
	
	ARRAY_INDEX_ASSIGN(utf8_char, 0) = char_;
	
	if (unlikely(is_eof(ARROW(state, file)))) {
		return;
	}
	
	if (char_ <= 127) {
		ARRAY_INDEX_ASSIGN(utf8_char, 6) = length = 1;
		return;
	}
	
	if ((char_ >> 5) == 6 /* 6 = 0b110 */) {
		ARRAY_INDEX_ASSIGN(utf8_char, 6) = length = 2;
		ARRAY_INDEX_ASSIGN(utf8_char, 1) = get_char_from_file(ARROW(state, file));
		
		if (unlikely(is_eof(ARROW(state, file)))) {
			output_nullt_string("Unexpected EOF in two-character UTF-8 sequence when trying to read character 2.\n");
			exit_program();
		}
		
		if (unlikely(*(u16*)utf8_char == 0xD800)) {
			output_nullt_string("Invalid UTF-8 sequence U+D800\n");
			exit_program();
		}
		
		if (unlikely(*(u16*)utf8_char == 0xDFFF)) {
			output_nullt_string("Invalid UTF-8 sequence U+DFFF\n");
			exit_program();
		}
		
		if (unlikely(*(u16*)utf8_char == 0xFFFE)) {
			output_nullt_string("Invalid UTF-8 sequence U+FFFE\n");
			exit_program();
		}
		
		if (unlikely(*(u16*)utf8_char == 0xFFFF)) {
			output_nullt_string("Invalid UTF-8 sequence U+FFFF\n");
			exit_program();
		}
		
		goto check_bytes;
	}
	
	if ((char_ >> 4) == 14 /* 14 = 0b1110 */) {
		ARRAY_INDEX_ASSIGN(utf8_char, 6) = length = 3;
		ARRAY_INDEX_ASSIGN(utf8_char, 1) = get_char_from_file(ARROW(state, file));
		
		if (unlikely(is_eof(ARROW(state, file)))) {
			output_nullt_string("Unexpected EOF in two-character UTF-8 sequence when trying to read character 2.\n");
			exit_program();
		}
		
		ARRAY_INDEX_ASSIGN(utf8_char, 2) = get_char_from_file(ARROW(state, file));
		
		if (unlikely(is_eof(ARROW(state, file)))) {
			output_nullt_string("Unexpected EOF in two-character UTF-8 sequence when trying to read character 3.\n");
			exit_program();
		}
		
		goto check_bytes;
	}
	
	if ((char_ >> 3) == 30 /* 30 = 0b11110 */) {
		ARRAY_INDEX_ASSIGN(utf8_char, 6) = length = 4;
		ARRAY_INDEX_ASSIGN(utf8_char, 1) = get_char_from_file(ARROW(state, file));
		
		if (unlikely(is_eof(ARROW(state, file)))) {
			output_nullt_string("Unexpected EOF in two-character UTF-8 sequence when trying to read character 2.\n");
			exit_program();
		}
		
		ARRAY_INDEX_ASSIGN(utf8_char, 2) = get_char_from_file(ARROW(state, file));
		
		if (unlikely(is_eof(ARROW(state, file)))) {
			output_nullt_string("Unexpected EOF in two-character UTF-8 sequence when trying to read character 3.\n");
			exit_program();
		}
		
		ARRAY_INDEX_ASSIGN(utf8_char, 3) = get_char_from_file(ARROW(state, file));
		
		if (unlikely(is_eof(ARROW(state, file)))) {
			output_nullt_string("Unexpected EOF in two-character UTF-8 sequence when trying to read character 4.\n");
			exit_program();
		}
		
		goto check_bytes;
	}
	
	if ((char_ >> 2) == 62 /* 62 = 0b111110 */) {
		ARRAY_INDEX_ASSIGN(utf8_char, 6) = length = 5;
		ARRAY_INDEX_ASSIGN(utf8_char, 1) = get_char_from_file(ARROW(state, file));
		
		if (unlikely(is_eof(ARROW(state, file)))) {
			output_nullt_string("Unexpected EOF in two-character UTF-8 sequence when trying to read character 2.\n");
			exit_program();
		}
		
		ARRAY_INDEX_ASSIGN(utf8_char, 2) = get_char_from_file(ARROW(state, file));
		
		if (unlikely(is_eof(ARROW(state, file)))) {
			output_nullt_string("Unexpected EOF in two-character UTF-8 sequence when trying to read character 3.\n");
			exit_program();
		}
		
		ARRAY_INDEX_ASSIGN(utf8_char, 3) = get_char_from_file(ARROW(state, file));
		
		if (unlikely(is_eof(ARROW(state, file)))) {
			output_nullt_string("Unexpected EOF in two-character UTF-8 sequence when trying to read character 4.\n");
			exit_program();
		}
		
		ARRAY_INDEX_ASSIGN(utf8_char, 4) = get_char_from_file(ARROW(state, file));
		
		if (unlikely(is_eof(ARROW(state, file)))) {
			output_nullt_string("Unexpected EOF in two-character UTF-8 sequence when trying to read character 5.\n");
			exit_program();
		}
		
		goto check_bytes;
	}
	
	if ((char_ >> 1) == 126 /* 126 = 0b1111110 */) {
		ARRAY_INDEX_ASSIGN(utf8_char, 6) = length = 6;
		ARRAY_INDEX_ASSIGN(utf8_char, 1) = get_char_from_file(ARROW(state, file));
		
		if (unlikely(is_eof(ARROW(state, file)))) {
			output_nullt_string("Unexpected EOF in two-character UTF-8 sequence when trying to read character 2.\n");
			exit_program();
		}
		
		ARRAY_INDEX_ASSIGN(utf8_char, 2) = get_char_from_file(ARROW(state, file));
		
		if (unlikely(is_eof(ARROW(state, file)))) {
			output_nullt_string("Unexpected EOF in two-character UTF-8 sequence when trying to read character 3.\n");
			exit_program();
		}
		
		ARRAY_INDEX_ASSIGN(utf8_char, 3) = get_char_from_file(ARROW(state, file));
		
		if (unlikely(is_eof(ARROW(state, file)))) {
			output_nullt_string("Unexpected EOF in two-character UTF-8 sequence when trying to read character 4.\n");
			exit_program();
		}
		
		ARRAY_INDEX_ASSIGN(utf8_char, 4) = get_char_from_file(ARROW(state, file));
		
		if (unlikely(is_eof(ARROW(state, file)))) {
			output_nullt_string("Unexpected EOF in two-character UTF-8 sequence when trying to read character 5.\n");
			exit_program();
		}
		
		ARRAY_INDEX_ASSIGN(utf8_char, 4) = get_char_from_file(ARROW(state, file));
		
		if (unlikely(is_eof(ARROW(state, file)))) {
			output_nullt_string("Unexpected EOF in two-character UTF-8 sequence when trying to read character 6.\n");
			exit_program();
		}
		
		goto check_bytes;
	}
	
	invalid_character(char_);
	
	check_bytes: {
		u i;
		
		if (unlikely((char_ << (length + 1)) == 0)) {
			output_nullt_string("The character that was read was detected to be not in the shortest form.\n");
			exit_program();
		}
		
		/* i = 1 to ignore the first character. */
		i = 1;
		
		do {
			char_ = ARRAY_INDEX(utf8_char, i);
			
			if (unlikely((char_ << 2) == 0)) {
				output_nullt_string("The character that was read was detected to be not in the shortest form.\n");
				exit_program();
			}
			
			if (unlikely((char_ >> 6) != 2 /* 0b10 */)) {
				invalid_character(char_);
			}
			
			++i;
		} while (i < length);
	}
}
