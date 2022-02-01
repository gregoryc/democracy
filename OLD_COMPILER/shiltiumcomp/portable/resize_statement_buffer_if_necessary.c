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
#include "utf8.h"

attr_nonnull void resize_statement_buffer_if_necessary(ParserState* cr state) {
	assert_comparison(state, !=, NULL, ParserState* cr, void*);
	
	if (
		ARROW(state, statement_buffer_offset) + get_length_of_utf8_char(state->utf8_char)
		>=
		ARROW(state, statement_buffer_allocated_length)
	) {
		char* old_buffer_pointer = ARROW(state, statement_buffer);
		
		ARROW_ASSIGN(state, statement_buffer_allocated_length) =
			(ARROW(state, statement_buffer_offset) + get_length_of_utf8_char(state->utf8_char)) * 2;
		
		ARROW_ASSIGN(state, statement_buffer) =
			m_realloc(ARROW(state, statement_buffer), ARROW(state, statement_buffer_allocated_length));
		
		if (unlikely(!ARROW(state, statement_buffer))) {
			m_free(old_buffer_pointer);
			output_memory_error(state, "Unable to allocate memory to parse block");
		}
	}
}
