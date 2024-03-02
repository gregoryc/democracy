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

attr_nonnull
void insert_variable_into_scope(
	ParserState* cr state,
	char* const name,
	const u length,
	Variable* cr variable
) {
	void** new_value;
	
	assert_comparison(state, !=, NULL, ParserState* cr, void*);
	assert_comparison(name, !=, NULL, const String* cr, void*);
	assert_comparison(variable, !=, NULL, Variable* cr, void*);
	
	if (!hash_table_insert_with_string_key(
		&state->scope.variables_in_scope,
		name,
		length,
		variable,
		false,
		&new_value
	)) {
		output_nullt_string("Identifier error: variable " OPENING_QUOTE);
		output_string(name, length);
		output_nullt_string(CLOSING_QUOTE " already declared on line ");
		putu(ARROW((Variable*)DEREF(new_value), line_declared_on));
		output_newline();
	}
}
