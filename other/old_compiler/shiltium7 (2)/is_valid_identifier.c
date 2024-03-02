#include "headers.h"

bool is_valid_identifier(const char* const restrict string, const u64 string_length) {
	u64 i;
	
	assert_comparison(string, !=, NULL);
	assert_comparison(string_length, !=, 0);
	
	if (!(
		(*string >= 'a' && *string <= 'z') ||
		(*string >= 'A' && *string <= 'Z') ||
		*string == '$' || *string == '_' || *string == '@'
	)) return false;
	
	for (i = 1; i < string_length; ++i) {
		if (!(
			(string[i] >= 'a' && string[i] <= 'z') ||
			(string[i] >= 'A' && string[i] <= 'Z') ||
			(string[i] >= '0' && string[i] <= '9') ||
			string[i] == '$' || string[i] == '_' || string[i] == '@'
		)) return false;
	}
	return true;
}
