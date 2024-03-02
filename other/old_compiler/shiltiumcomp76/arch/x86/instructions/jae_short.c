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

#include "../../../global_headers/headers.h"
#include "x86_instructions.h"

u dac_get_length_of_jae_short(void) {
	return 2;
}

void dac_append_jae_short(DataAndCode* cr dac, const schar displacement) {
	assert_comparison(dac, !=, NULL, DataAndCode*, void*);
	
	#if DEBUG
		output_nullt_string("jae short $ + ");
		puts_without_separators(displacement);
		output_newline();
	#endif
	
	ARRAY_INDEX_ASSIGN(ARROW(dac, start_of_code), ARROW(dac, current_instruction_offset)) = (uchar)115;
	ARRAY_INDEX_ASSIGN(ARROW(dac, start_of_code), ARROW(dac, current_instruction_offset + 1)) = displacement;
	ARROW_ASSIGN(dac, current_instruction_offset) += 2;
}
