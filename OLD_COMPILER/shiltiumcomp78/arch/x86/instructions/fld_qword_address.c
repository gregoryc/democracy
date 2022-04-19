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

#if SIXTY_FOUR_BIT
	attr_const u dac_get_length_of_fld_qword_address(void) {
		return 6;
	}
	
	attr_nonnull void dac_append_fld_qword_address(DataAndCode* cr dac, const s rip_relative_address) {
		assert_comparison(dac, !=, NULL, DataAndCode*, void*);
		
		#if DEBUG
			output_nullt_string(TAB_STRING TAB_STRING "fld qword[rip ");
			
			if (rip_relative_address < 0) {
				output_nullt_string("- ");
				puts_without_separators(-rip_relative_address);
			} else {
				output_nullt_string("+ ");
				puts_without_separators(rip_relative_address);
			}
			output_nullt_string("]\n");
		#endif
		
		ARRAY_INDEX_ASSIGN(ARROW(dac, start_of_code), ARROW(dac, current_instruction_offset)) = (uchar)221;
		ARRAY_INDEX_ASSIGN(ARROW(dac, start_of_code), ARROW(dac, current_instruction_offset) + 1) = 5;
		DEREF_ASSIGN((s32*)(ARROW(dac, start_of_code) + ARROW(dac, current_instruction_offset) + 2)) = rip_relative_address;
		ARROW_ASSIGN(dac, current_instruction_offset) += 6;
	}
#endif
