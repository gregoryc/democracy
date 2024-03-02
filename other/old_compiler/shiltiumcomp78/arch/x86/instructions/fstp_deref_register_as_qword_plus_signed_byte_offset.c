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

#if SIXTY_FOUR_BIT
	#include "x86_instructions.h"
	#include "x86_instructions_globals.h"
	
	attr_const u dac_get_length_of_fstp_deref_register_as_qword_plus_signed_byte_offset(const Register64 register_) {
		return (register_ >= REGISTER_R8) + 3 + (register_ == REGISTER_RSP || register_ == REGISTER_R12);
	}
	
	attr_nonnull
	void dac_append_fstp_deref_register_as_qword_plus_signed_byte_offset(
		DataAndCode* cr dac,
		const Register64 register_,
		const char offset
	) {
		assert_comparison(dac, !=, NULL, DataAndCode* cr, void*);
		
		#if DEBUG
			output_nullt_string(TAB_STRING TAB_STRING "fstp qword[");
			output_nullt_string(ARRAY_INDEX(sixty_four_bit_register_numbers_to_strings, register_));
			output_nullt_string(" + ");
			puts_without_separators(offset);
			output_nullt_string("]\n");
		#endif
		
		if (register_ >= REGISTER_R8) {
			ARRAY_INDEX_ASSIGN(ARROW(dac, start_of_code), ARROW(dac, current_instruction_offset)) = 65;
			ARROW_ASSIGN(dac, current_instruction_offset)++;
		}
		
		ARRAY_INDEX_ASSIGN(ARROW(dac, start_of_code), ARROW(dac, current_instruction_offset)) = (uchar)221;
		ARRAY_INDEX_ASSIGN(ARROW(dac, start_of_code), ARROW(dac, current_instruction_offset) + 1) = (uchar)88 + (register_ % 8);
		ARROW_ASSIGN(dac, current_instruction_offset) += 2;
		
		if (register_ == REGISTER_RSP || register_ == REGISTER_R12) {
			ARRAY_INDEX_ASSIGN(ARROW(dac, start_of_code), ARROW(dac, current_instruction_offset)) = 36;
			ARROW_ASSIGN(dac, current_instruction_offset)++;
		}
		
		ARRAY_INDEX_ASSIGN(ARROW(dac, start_of_code), ARROW(dac, current_instruction_offset)) = offset;
		ARROW_ASSIGN(dac, current_instruction_offset)++;
	}
#endif
