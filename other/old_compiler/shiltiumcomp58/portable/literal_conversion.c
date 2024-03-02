/*
Functions for conversion of Shiltolang literals into C types.

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

#define NUMBER_SEPARATOR '_'
#define VALID_CHARACTERS_STRING  "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"

/* This function gets the 'e6' part in 3FAe16:16, for example. */
attr_nonnull static int get_exponent_and_remove_it_from_literal(const char* cr string, u* cr string_length, const u line_number) {
	u i = 0;
	
	assert_comparison(string, !=, NULL, const void*, void*);
	assert_comparison(string_length, !=, NULL, void*, void*);
	
	while (i < *string_length) {
		if (string[i++] == 'e') {
			goto use_exponent_index;
		}
	}
	
	return 0;
	
use_exponent_index: {
	u length_of_exponent_in_string, end, orig_i = i;
	small_num_type number;
	bool neg = false;
	
	while (i < *string_length && string[i] != ':') {
		++i;
	}
	length_of_exponent_in_string = i - orig_i;
	
	if (!length_of_exponent_in_string) {
		output_nullt_string("Literal error on line ");
		putu(line_number);
		output_nullt_string(": empty exponent\n");
		exit_program();
	}
	
	end = i;
	i = orig_i;
	
	if (string[i] == '+') {
		++i;
	} else if (string[i] == '-') {
		neg = true;
		++i;
	}
	
	number = 0;
	
	for (; i < end; ++i) {
		if (!(string[i] >= '0' && string[i] <= '9')) {
			output_nullt_string("Literal error on line ");
			putu(line_number);
			output_nullt_string(": invalid exponent " OPENING_QUOTE);
			output_string(string + i, length_of_exponent_in_string);
			output_nullt_string(CLOSING_QUOTE);
			
			/*
			Easter egg #1
			By the way, I don't have a monkey obsession ;)
			*/
			if (unlikely(strnequal(string + i, "MONKEYS", length_of_exponent_in_string, 7))) {
				output_nullt_string("; however, monkeys are still nice animals.\n\nMONKEYS ARE NICE ANIMALS!!!\nMONKEYS! MONKEYS! MONKEYS!");
			}
			output_newline();
			exit_program();
		}
		
		number = number * 10 + (string[i] - '0');
		
		if (number > 325) break;
	}
	
	if (neg) number = -number;
	*string_length -= length_of_exponent_in_string + 1;
	
	return number;
}
}

/* This function gets the ':16' part in 3FAe16:16 or F3A, for example. */
attr_nonnull static small_num_type get_base_and_remove_it_from_literal(const char* string, u* string_length, const u line_number) {
	u i = 0;
	
	assert_comparison(string, !=, NULL, const void*, void*);
	
	while (i < *string_length) {
		if (string[i++] == ':') {
			goto use_base_index;
		}
	}
	
	/* no colon found, so return 10, as that is the default base */
	return 10;
	
use_base_index: {
	u length_of_base_in_string = *string_length - i;
	small_num_type number;
	
	if (!(
		(length_of_base_in_string == 1 || length_of_base_in_string == 2) && (string[i] >= '0' && string[i] <= '9')
	)) {
		output_nullt_string("Literal error on line ");
		putu(line_number);
		output_nullt_string(": invalid base " OPENING_QUOTE);
		output_string(string + i, length_of_base_in_string);
		output_nullt_string(CLOSING_QUOTE "\n");
		exit_program();
	}
	
	DEREF_ASSIGN(string_length) -= length_of_base_in_string + 1;
	
	number = string[i] - '0';
	if (length_of_base_in_string == 2) {
		number = number * 10 + string[i + 1] - '0';
	}
	return number;
}
}

attr_nonnull static void invalid_character_in_base(u line_number, u character_offset, u base) {
	u first_valid_base;
	
	assert_comparison(character_offset, <, 36, u, u);
	
	output_nullt_string("Literal error on line ");
	putu(line_number);
	output_nullt_string(": " OPENING_QUOTE);
	output_char(VALID_CHARACTERS_STRING[character_offset]);
	output_nullt_string(CLOSING_QUOTE " is not a valid character in base ");
	putu(base);
	output_nullt_string(" but it is valid in base");
	first_valid_base = character_offset + 1;
	
	if (first_valid_base < 35) {
		output_char('s');
	}
	
	output_char(' ');
	
	putu(first_valid_base);
	
	if (first_valid_base == 35) {
		output_nullt_string(" or 36");
	} else if (first_valid_base < 36) {
		output_nullt_string("‒36");
	}
	
	output_nullt_string(".\n");
	
	exit_program();
}

/* Get the offset of character c and return true if found, and false otherwise. */
static bool get_offset(const char* restrict str, char c, u* restrict offset) {
	*offset = 0;
	
	while (str[*offset]) {
		if (c == str[*offset]) return true;
		++*offset;
	}
	return false;
}

u convert_literal_to_uint(const char* cr string, u string_length, u line_number) {
	char buffer[27];
	u result = 0;
	small_num_type i, base, number_of_separators;
	
	assert_comparison(string, !=, NULL, const char*, void*);
	assert_comparison(line_number, >, 0, u, u);
	
	base = get_base_and_remove_it_from_literal(string, &string_length, line_number);
	
	for (i = 0; i < string_length; ++i) {
		u number_this_char_represents;
		
		if (string[i] == NUMBER_SEPARATOR) continue;
		
		if (get_offset(VALID_CHARACTERS_STRING, string[i], &number_this_char_represents)) {
			if (number_this_char_represents >= base) {
				invalid_character_in_base(line_number, number_this_char_represents, base);
			}
			result = result * base + number_this_char_represents;
		} else {
			output_nullt_string("Literal error on line ");
			putu(line_number);
			output_nullt_string(": " OPENING_QUOTE);
			output_string(string, string_length);
			output_nullt_string(CLOSING_QUOTE " is an invalid unsigned integer because it contains the character " OPENING_QUOTE);
			output_char(string[i]);
			output_nullt_string(CLOSING_QUOTE "\n");
			exit_program();
		}
	}
	
	u_to_string(result, buffer, base);
	
	for (i = 0, number_of_separators = 0; buffer[i]; ++i) {
		if (string[i] == NUMBER_SEPARATOR) {
			++number_of_separators;
		}
		
		if (unlikely(buffer[i] != string[i + number_of_separators])) {
			output_nullt_string("Literal error on line ");
			putu(line_number);
			output_nullt_string(": " OPENING_QUOTE);
			output_string(string, string_length);
			output_nullt_string(CLOSING_QUOTE " is too large to be an unsigned integer\n");
			exit_program();
		}
	}
	
	return result;
}

f convert_literal_to_float(const char* cr string, u string_length, const u line_number) {
	u i;
	s32 exponent, real_exponent = 0;
	f number = 0.0;
	bool after_point = false;
	bool negative = false;
	small_num_type base;
	
	assert_comparison(string, !=, NULL, const char*, void*);
	assert_comparison(line_number, >, 0, u, u);
	
	base = get_base_and_remove_it_from_literal(string, &string_length, line_number);
	exponent = get_exponent_and_remove_it_from_literal(string, &string_length, line_number);
	
	for (i = 0; i < string_length; ++i) {
		u number_this_char_represents;
		
		if (string[i] == NUMBER_SEPARATOR) continue;
		
		if (string[i] == '.') {
			if (after_point) {
				output_nullt_string("Literal error on line ");
				putu(line_number);
				output_nullt_string(": more than one " OPENING_QUOTE "." CLOSING_QUOTE " in floating point literal");
				exit_program();
			}
			after_point = true;
			continue;
		} else if (get_offset(VALID_CHARACTERS_STRING, string[i], &number_this_char_represents)) {
			if (number_this_char_represents >= base) {
				invalid_character_in_base(line_number, number_this_char_represents, base);
			}
			number = number * base + number_this_char_represents;
		} else {
			output_nullt_string("Literal error on line ");
			putu(line_number);
			output_nullt_string(": " OPENING_QUOTE);
			output_string(string, string_length);
			output_nullt_string(CLOSING_QUOTE " is an invalid floating point number because it contains the character " CLOSING_QUOTE);
			output_char(string[i]);
			output_nullt_string(CLOSING_QUOTE "\n");
			exit_program();
		}
		
		if (after_point) {
			--real_exponent;
		}
	}
	
	real_exponent += exponent;
	
	/* 0 is ignored. */
	if (real_exponent > 0) {
		while (real_exponent--) {
			number *= 10.0;
		}
	} else if (real_exponent < 0) {
		while (real_exponent < 0) {
			number /= 10.0;
			++real_exponent;
		}
	}
	
	if (negative) {
		number = -number;
	}
	
	return number;
}
