/*
Functions for conversion of Shilto Language literals into C types.
*/

#include "headers.h"
#include "valid_characters_string.h"

#define _LITERAL_CONVERSION_NUMBER_SEPARATOR '_'

/* This function gets the 'e6' part in 3FAe16:16, for example. */
attr_nonnull static int get_exponent_and_remove_it_from_literal(
	const char* string, u64* string_length, u64 line_number, ErrorNumber* error_number
) {
	u64 i = 0;
	
	assert_comparison(string, !=, NULL);
	assert_comparison(string_length, !=, NULL);
	assert_comparison(error_number, !=, NULL);
	
	while (i < *string_length) {
		if (string[i++] == 'e') {
			goto use_exponent_index;
		}
	}
	
	return 0;
	
use_exponent_index: {
	u64 length_of_exponent_in_string, end, orig_i = i;
	small_num_type number;
	bool neg = false;
	
	while (i < *string_length && string[i] != ':') {
		++i;
	}
	length_of_exponent_in_string = i - orig_i;
	
	if (!length_of_exponent_in_string) {
		output_nullt_string("Literal error on line ");
		output_u64(line_number);
		output_nullt_string(": empty exponent\n");
		*error_number = ERROR_NUMBER__LITERAL_ERROR__EMPTY_EXPONENT;
		return 0.0;
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
			output_u64(line_number);
			output_nullt_string(": invalid exponent ‘");
			output_string(string + i, length_of_exponent_in_string);
			output_nullt_string("’");
			
			/*
			Easter egg ;)
			This is the only easter egg in all of Shiltium, and it's pretty well hidden, but only in the
			interface and not the code since you can simply grep for 'Easter egg' and you will find this.
			P.S., I do not have a monkey obsession ;)
			*/
			if (unlikely(strnequal(string + i, "MONKEYS", length_of_exponent_in_string, 7))) {
				output_nullt_string("; however, monkeys are still nice animals.\n\nMONKEYS ARE NICE ANIMALS!!!\nMONKEYS! MONKEYS! MONKEYS!");
			}
			output_char('\n');
			
			*error_number = ERROR_NUMBER__LITERAL_ERROR__INVALID_EXPONENT;
			return 0.0;
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
__attribute__((nonnull))
static small_num_type get_base_and_remove_it_from_literal(
	const char* string, u64* string_length, u64 line_number, ErrorNumber* error_number
) {
	u64 i = 0;
	
	assert_comparison(string, !=, NULL);
	assert_comparison(error_number, !=, NULL);
	
	while (i < *string_length) {
		if (string[i++] == ':') {
			goto use_base_index;
		}
	}
	
	/* no colon found, so return 10, as that is the default base */
	return 10;
	
use_base_index: {
	u64 length_of_base_in_string = *string_length - i;
	small_num_type number;
	
	if (!(
		(length_of_base_in_string == 1 || length_of_base_in_string == 2) && (string[i] >= '0' && string[i] <= '9')
	)) {
		output_nullt_string("Literal error on line ");
		output_u64(line_number);
		output_nullt_string(": invalid base ‘");
		output_string(string + i, length_of_base_in_string);
		output_nullt_string("’\n");
		
		*error_number = ERROR_NUMBER__LITERAL_ERROR__INVALID_BASE;
		return 0.0;
	}
	
	*string_length -= length_of_base_in_string + 1;
	
	number = string[i] - '0';
	if (length_of_base_in_string == 2) {
		number = number * 10 + string[i + 1] - '0';
	}
	return number;
}
}

__attribute__((nonnull)) static void invalid_character_in_base(u64 line_number, u64 character_offset, u64 base) {
	u64 first_valid_base;
	
	assert_comparison(character_offset, <, 36);
	
	output_nullt_string("Literal error on line ");
	output_u64(line_number);
	output_nullt_string(": ‘");
	output_char(_CONVERSION__VALID_CHARACTERS_STRING[character_offset]);
	output_nullt_string("’ is not a valid character in base ");
	output_u64(base);
	output_nullt_string(" but it is valid in base");
	first_valid_base = character_offset + 1;
	
	if (first_valid_base < 35) {
		output_char('s');
	}
	
	output_char(' ');
	
	output_u64(first_valid_base);
	
	if (first_valid_base == 35) {
		output_nullt_string(" or 36");
	} else if (first_valid_base < 36) {
		output_nullt_string("‒36");
	}
	
	output_nullt_string(".\n");
}

/* Get the offset of character c and return true if found, and false otherwise. */
static bool get_offset(const char* restrict str, char c, u64* offset) {
	*offset = 0;
	
	while (str[*offset]) {
		if (c == str[*offset]) return true;
		++*offset;
	}
	return false;
}

u64 convert_literal_to_uint(const char* string, u64 string_length, u64 line_number, ErrorNumber* error_number) {
	char buffer[27];
	u64 result = 0;
	small_num_type i, base, number_of_commas;
	
	assert_comparison(string, !=, NULL);
	assert_comparison(error_number, !=, NULL);
	assert_comparison(line_number, >, 0);
	
	*error_number = ERROR_NUMBER_NO_ERROR;
	
	base = get_base_and_remove_it_from_literal(string, &string_length, line_number, error_number);
	if (*error_number != ERROR_NUMBER_NO_ERROR) return 0;
	
	for (i = 0; i < string_length; ++i) {
		u64 number_this_char_represents;
		
		if (string[i] == _LITERAL_CONVERSION_NUMBER_SEPARATOR) continue;
		
		if (get_offset(_CONVERSION__VALID_CHARACTERS_STRING, string[i], &number_this_char_represents)) {
			if (number_this_char_represents >= base) {
				invalid_character_in_base(line_number, number_this_char_represents, base);
				*error_number = ERROR_NUMBER__LITERAL_ERROR__INVALID_CHARACTER_IN_BASE;
				return 0;
			}
			result = result * base + number_this_char_represents;
		} else {
			output_nullt_string("Literal error on line ");
			output_u64(line_number);
			output_nullt_string(": ‘");
			output_string(string, string_length);
			output_nullt_string("’ is an invalid unsigned integer because it contains the character ‘");
			output_char(string[i]);
			output_nullt_string("’\n");
			*error_number = ERROR_NUMBER__LITERAL_ERROR__INVALID_UINT;
			return 0;
		}
	}
	
	u64_to_string(result, buffer, base);
	
	for (i = 0, number_of_commas = 0; buffer[i]; ++i) {
		if (string[i] == _LITERAL_CONVERSION_NUMBER_SEPARATOR) ++number_of_commas;
		if (buffer[i] != string[i + number_of_commas]) goto too_large;
	}
	
	return result;

too_large:
	output_nullt_string("Literal error on line ");
	output_u64(line_number);
	output_nullt_string(": ‘");
	output_string(string, string_length);
	output_nullt_string(
		"’ is too large to be an unsigned integer (the range of an unsigned integer is from 0 to 18446744073709551615 [2^64-1] inclusive)\n");
	*error_number = ERROR_NUMBER__LITERAL_ERROR__UINT_TOO_LARGE;
	return 0;
}

f64 convert_literal_to_float(const char* string, u64 string_length, u64 line_number, ErrorNumber* error_number) {
	u64 i;
	s32 exponent, real_exponent;
	f64 number;
	bool after_point;
	small_num_type base;
	
	assert_comparison(string, !=, NULL);
	assert_comparison(error_number, !=, NULL);
	assert_comparison(line_number, >, 0);
	
	*error_number = ERROR_NUMBER_NO_ERROR;
	
	base = get_base_and_remove_it_from_literal(string, &string_length, line_number, error_number);
	if (*error_number != ERROR_NUMBER_NO_ERROR) return 0.0;
	exponent = get_exponent_and_remove_it_from_literal(string, &string_length, line_number, error_number);
	if (*error_number != ERROR_NUMBER_NO_ERROR) return 0.0;
	
	number = 0.0;
	real_exponent = 0;
	
	after_point = false;
	
	if (*string == '-') {
		number = -number;
		/* Skip the first character. */
		i = 1;
	} else if (*string == '+') {
		/* Skip the first character. */
		i = 1;
	} else {
		i = 0;
	}
	
	for (; i < string_length; ++i) {
		u64 number_this_char_represents;
		
		if (string[i] == _LITERAL_CONVERSION_NUMBER_SEPARATOR) continue;
		
		if (string[i] == '.') {
			if (after_point) {
				output_nullt_string("Literal error on line ");
				output_u64(line_number);
				output_nullt_string(": more than one ‘.’ in floating point literal");
				*error_number = ERROR_NUMBER__LITERAL_ERROR__MORE_THAN_ONE_POINT_IN_FLOAT_LITERAL;
				return 0.0;
			}
			after_point = true;
			continue;
		} else if (get_offset(_CONVERSION__VALID_CHARACTERS_STRING, string[i], &number_this_char_represents)) {
			if (number_this_char_represents >= base) {
				invalid_character_in_base(line_number, number_this_char_represents, base);
				
				*error_number = ERROR_NUMBER__LITERAL_ERROR__INVALID_CHARACTER_IN_BASE;
				return 0.0;
			}
			number = number * base + number_this_char_represents;
		} else {
			output_nullt_string("Literal error on line ");
			output_u64(line_number);
			output_nullt_string(": ‘");
			output_string(string, string_length);
			output_nullt_string("’ is an invalid floating point number because it contains the character ‘");
			output_char(string[i]);
			output_nullt_string("’\n");
			*error_number = ERROR_NUMBER__LITERAL_ERROR__INVALID_FLOAT;
			return 0.0;
		}
		
		if (after_point) {
			++real_exponent;
		}
	}
	
	real_exponent += exponent;
	
	/* 0 is ignored. */
	if (real_exponent > 0) {
		while (real_exponent--) {
			number /= 10.0;
		}
	} else if (real_exponent < 0) {
		real_exponent = -real_exponent;
		while (real_exponent--) {
			number *= 10.0;
		}
	}
	
	return number;
	#undef _CONVERSION__VALID_CHARACTERS_STRING___IN_FUNCTION_COVERT_STRING_TO_FLOAT
}

#undef _LITERAL_CONVERSION_NUMBER_SEPARATOR
