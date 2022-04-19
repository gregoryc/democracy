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

attr_nonnull void subroutine_value_deletion_function(const TypeUnion key, const u length_of_key, void* subroutine) {
	LinkedListHead argument_type_list_head;
	
	assert_comparison(key.string_value, !=, NULL, char*, void*);
	assert_comparison(length_of_key, !=, 0, u, u);
	assert_comparison(subroutine, !=, NULL, void*, void*);
	
	if (unlikely(!ARROW((Subroutine*)subroutine, used) && !strnequal(key.string_value, "main", length_of_key, 4))) {
		output_nullt_string("Subroutine error: unused subroutine " OPENING_QUOTE);
		output_string_with_length(key.string_value, length_of_key);
		output_nullt_string(CLOSING_QUOTE "\n");
		exit_program();
	}
	
	argument_type_list_head = ARROW((Subroutine*)subroutine, argument_type_list);
	
	while (argument_type_list_head) {
		LinkedListNode* next_node;
		
		next_node = ARROW(argument_type_list_head, next_node);
		m_free(argument_type_list_head);
		argument_type_list_head = next_node;
	}
	
	m_free(subroutine);
}
