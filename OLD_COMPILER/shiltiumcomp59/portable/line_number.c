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

attrs_const_and_nonnull u get_line_number(ParserState* cr state) {
	return DEREF(ARROW(state, line_number));
}

attr_nonnull void increment_line_number(ParserState* cr state) {
	DEREF_ASSIGN(ARROW(state, line_number))++;
}

attr_nonnull void decrement_line_number(ParserState* cr state) {
	DEREF_ASSIGN(ARROW(state, line_number))--;
}
