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

u dac_get_length_of_int(void) {
	return 2;
}

void dac_append_int(DataAndCode* cr dac, const uchar interrupt_number) {
	assert_comparison(dac, !=, NULL, DataAndCode*, void*);
	
	#if DEBUG
		output_nullt_string(TAB_STRING TAB_STRING "int ");
		
		/*
		This is a minor optimization.
		putu() can be used instead, but putu_without_separators() is slightly faster.
		*/
		putu_without_separators(interrupt_number);
		
		output_newline();
	#endif
	
	ARRAY_INDEX_ASSIGN(ARROW(dac, start_of_code), ARROW(dac, current_instruction_offset)) = (uchar)205;
	ARRAY_INDEX_ASSIGN(ARROW(dac, start_of_code), ARROW(dac, current_instruction_offset) + 1) = interrupt_number;
	ARROW_ASSIGN(dac, current_instruction_offset) += 2;
}
