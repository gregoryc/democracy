#include "headers.h"

bool strequal(const char* const restrict string1, const char* const restrict string2) {
	u64 i;
	
	for (i = 0 ;; ++i) {
		if (!ARRAY_INDEX(string1, i)) {
			return !ARRAY_INDEX(string2, i);
		}
		if (!ARRAY_INDEX(string2, i)) {
			return !ARRAY_INDEX(string1, i);
		}
		if (ARRAY_INDEX(string1, i) != ARRAY_INDEX(string2, i)) return false;
	}
}

bool strnequal(const char* const restrict string1, const char* const restrict string2, const u64 length1, const u64 length2) {
	u64 i;
	
	assert_comparison(string1, !=, NULL);
	assert_comparison(string2, !=, NULL);
	
	if (length1 != length2) return false;
	i = length1;
	while (i--) {
		if (ARRAY_INDEX(string1, i) != ARRAY_INDEX(string2, i)) return false;
	}
	return true;
}
