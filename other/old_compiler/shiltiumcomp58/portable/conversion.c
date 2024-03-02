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

#define VALID_CHARACTERS_STRING "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"

attr_nonnull static void reverse_string(char* cr string, const u length) {
	u i;
	
	assert_comparison(string, !=, NULL, void*, void*);
	
	for (i = 0; i < length / 2; ++i) {
		char temp = ARRAY_INDEX(string, length - i - 1);
		ARRAY_INDEX_ASSIGN(string, length - i - 1) = ARRAY_INDEX(string, i);
		ARRAY_INDEX_ASSIGN(string, i) = temp;
	}
}

static inline char get_number_separator(void) {
	return ',';
}

/*
Takes a u and returns a string of that number with a thousands separator in it.

The string parameter should be at least 27 bytes long because the length
of "18,446,744,073,709,551,615" plus 1 for the null byte equals 27.
*/
void add_separators_to_u(u number, char* cr string) {
	small_num_type character_offset, comma_counter;
	
	assert_comparison(string, !=, NULL, void*, void*);
	
	character_offset = 0;
	comma_counter = 0;
	
	do {
		if (comma_counter == 3 && character_offset) {
			ARRAY_INDEX_ASSIGN(string, character_offset) = get_number_separator();
			++character_offset;
			comma_counter = 0;
		}
		
		ARRAY_INDEX_ASSIGN(string, character_offset) = '0' + (number % 10);
		++character_offset;
		++comma_counter;
	} while (number /= 10);
	
	ARRAY_INDEX_ASSIGN(string, character_offset) = '\0';
	
	assert_comparison(character_offset, <, 27, small_num_type, small_num_type);
	
	reverse_string(string, character_offset);
}

/*
The string parameter should be at least 21 bytes long if the base is 10 because
the length of "18446744073709551615" plus 1 for the null byte equals 21.
*/
void u_to_string(u number, char* cr string, small_num_type base) {
	small_num_type character_offset;
	
	assert_comparison(string, !=, NULL, void*, void*);
	assert_comparison(base, <=, 36, small_num_type, small_num_type);
	
	character_offset = 0;
	
	do {
		ARRAY_INDEX_ASSIGN(string, character_offset) = ARRAY_INDEX(VALID_CHARACTERS_STRING, number % base);
		character_offset++;
	} while (number /= base);
	
	ARRAY_INDEX_ASSIGN(string, character_offset) = '\0';
	
	reverse_string(string, character_offset);
}
