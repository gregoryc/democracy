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

bool strequal(const char* cr string1, const char* cr string2) {
	u i;
	
	assert_comparison(string1, !=, NULL, const char*, void*);
	assert_comparison(string2, !=, NULL, const char*, void*);
	
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

/* str-num-equal, not str-not-equal */
bool strnequal(const char* cr string1, const char* cr string2, const u length1, const u length2) {
	u i;
	
	assert_comparison(string1, !=, NULL, const char*, void*);
	assert_comparison(string2, !=, NULL, const char*, void*);
	
	if (length1 != length2) return false;
	
	i = length1;
	while (i--) {
		if (ARRAY_INDEX(string1, i) != ARRAY_INDEX(string2, i)) return false;
	}
	
	return true;
}
