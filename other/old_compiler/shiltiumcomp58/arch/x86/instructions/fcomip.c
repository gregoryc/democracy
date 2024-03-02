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

u dac_get_length_of_fcomip(void) {
	return 2;
}

void dac_append_fcomip(DataAndCode* cr dac, const FPU_Register register_) {
	assert_comparison(dac, !=, NULL, DataAndCode*, void*);
	assert_comparison(register_, <=, REGISTER_ST7, FPU_Register, FPU_Register);
	
	#if DEBUG
		output_nullt_string(TAB_STRING TAB_STRING "fcomip st");
		
		/*
		This is a minor optimization.
		putu() can be used instead, but putu_without_commas() is slightly faster.
		*/
		putu_without_commas(register_);
		
		output_newline();
	#endif
	
	ARRAY_INDEX_ASSIGN(ARROW(dac, start_of_code), ARROW(dac, current_instruction_offset)) = (uchar)223;
	ARRAY_INDEX_ASSIGN(ARROW(dac, start_of_code), ARROW(dac, current_instruction_offset) + 1) = (uchar)240 + register_;
	ARROW_ASSIGN(dac, current_instruction_offset) += 2;
}
