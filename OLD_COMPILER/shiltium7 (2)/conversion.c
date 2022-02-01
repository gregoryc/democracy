/*
Various conversion functions.
This file is licensed under the WTFPL which is found in the LICENSE file.
*/

#include "headers.h"
#include "valid_characters_string.h"

__attribute__((nonnull)) static void reverse_string(char* const restrict string, const u64 length) {
	u64 i;
	
	assert_comparison(string, !=, NULL);
	
	for (i = 0; i < length / 2; ++i) {
		char temp = ARRAY_INDEX(string, length - i - 1);
		ARRAY_INDEX_ASSIGN(string, length - i - 1) = ARRAY_INDEX(string, i);
		ARRAY_INDEX_ASSIGN(string, i) = temp;
	}
}

/* Takes a u64 and returns a string of that number with commas in it. */
/* The string parameter should be at least 27 bytes long because the length of "18,446,744,073,709,551,615" plus 1 for the null byte equals 27. */
void add_commas_to_u64(u64 number, char* restrict const string) {
	small_num_type character_offset, comma_counter;
	
	assert_comparison(string, !=, NULL);
	
	character_offset = 0;
	comma_counter = 0;
	
	do {
		if (comma_counter == 3 && character_offset) {
			ARRAY_INDEX_ASSIGN(string, character_offset) = ',';
			++character_offset;
			comma_counter = 0;
		}
		ARRAY_INDEX_ASSIGN(string, character_offset) = '0' + (number % 10);
		++character_offset;
		++comma_counter;
	} while (number /= 10);
	
	ARRAY_INDEX_ASSIGN(string, character_offset) = '\0';
	
	assert_comparison(character_offset, <, 27);
	
	reverse_string(string, character_offset);
}

/*
The string parameter should be at least 21 bytes long if base is 10 because
the length of "18446744073709551615" plus 1 for the null byte equals 21.
*/
void u64_to_string(u64 number, char* restrict const string, small_num_type base) {
	small_num_type character_offset;
	
	assert_comparison(string, !=, NULL);
	assert_comparison(base, <=, 36);
	
	character_offset = 0;
	
	do {
		ARRAY_INDEX_ASSIGN(string, character_offset) = ARRAY_INDEX(_CONVERSION__VALID_CHARACTERS_STRING, number % base);
		character_offset++;
	} while (number /= base);
	
	ARRAY_INDEX_ASSIGN(string, character_offset) = '\0';
	
	reverse_string(string, character_offset);
}
