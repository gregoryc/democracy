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

u dac_get_length_of_fxch(void) {
	return 2;
}

void dac_append_fxch(DataAndCode* cr dac, const FPU_Register register_) {
	#if DEBUG
		output_nullt_string(TAB_STRING TAB_STRING "fxch");
		
		if (register_ != REGISTER_ST1) {
			output_nullt_string(" st");
			putu(register_);
		}
		
		output_newline();
	#endif
	
	dac->start_of_code[dac->current_instruction_offset] = (uchar)217;
	dac->start_of_code[dac->current_instruction_offset + 1] = (uchar)200 + register_;
	dac->current_instruction_offset += 2;
}
