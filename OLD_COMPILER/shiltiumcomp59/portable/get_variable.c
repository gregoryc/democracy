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

attr_nonnull Variable* get_variable(ParserState* cr state, const String* cr name) {
	Scope* scope;
	
	assert_comparison(state, !=, NULL, ParserState* cr, void*);
	assert_comparison(name, !=, NULL, const String* cr, void*);
	
	scope = &state->scope;
	
	while (scope) {
		bool in_hash_table;
		Variable* variable;
		
		variable = (Variable*)DEREF(
			hash_table_get_value_from_string_key(
				&scope->variables_in_scope,
				ARROW(name, string),
				ARROW(name, length),
				&in_hash_table
			)
		);
		
		if (in_hash_table) {
			return variable;
		}
		
		scope = ARROW(scope, parent_scope);
	}
	
	return NULL;
}
