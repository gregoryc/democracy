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

void ensure_type_is_valid_otherwise_give_type_error(String* cr string, const u line_number) {
	if (
		ARROW(string, length) != 1 || (
		DEREF(ARROW(string, string)) != 'u' &&
		DEREF(ARROW(string, string)) != 's' &&
		DEREF(ARROW(string, string)) != 'f' &&
		DEREF(ARROW(string, string)) != 'b')
	) {
		output_nullt_string("Type error on line ");
		putu(line_number);
		output_nullt_string(": invalid type " OPENING_QUOTE);
		output_string(string);
		
		output_nullt_string(
			CLOSING_QUOTE "\n\nThe valid types are:\n"
			"    u (unsigned 32-bit integer)\n       Range: 0 to 4294967295\n\n"
			"    s (signed 32-bit integer)\n       Range: 2147483648 to 2147483647\n\n"
			"    f (double precision floating-point number)\n        Range: about 10^-308 to about 10^308\n\n"
			"    b (boolean)\n        Possible values: true or false"
			"\n\n"
		);
		
		exit_program();
	}
}
