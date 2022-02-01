/*
Contains a function that checks whether or not a string is a valid identifier.

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

bool is_valid_identifier(const char* cr string, u string_length) {
	bool only_digits;
	
	assert_comparison(string, !=, NULL, const void*, void*);
	assert_comparison(string_length, !=, 0, u, u);
	
	only_digits = true;
	
	while (string_length--) {
		if (string[string_length] >= '0' && string[string_length] <= '9') continue;
		
		if (!(
			(string[string_length] >= 'a' && string[string_length] <= 'z') ||
			(string[string_length] >= 'A' && string[string_length] <= 'Z') ||
			string[string_length] == '$' || string[string_length] == '_' || string[string_length] == '@'
		)) return false;
		
		only_digits = false;
	}
	
	if (only_digits) {
		return false;
	}
	
	return true;
}
