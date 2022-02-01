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
#include "x86_instructions_globals.h"

#if SIXTY_FOUR_BIT
	/* The length of the ten byte encoding is 10, obviously. */
	attr_const u dac_get_length_of_mov_reg64_lit64_ten_byte_encoding(void) {
		return 10;
	}
	
	attr_nonnull void dac_append_mov_reg64_lit64_ten_byte_encoding(DataAndCode* cr dac, const Register64 register_, const s amount) {
		assert_comparison(dac, !=, NULL, DataAndCode*, void*);
		assert_comparison(register_, <=, REGISTER_R15, Register64, Register64);
		
		#if DEBUG
			output_nullt_string(TAB_STRING TAB_STRING "mov ");
			output_nullt_string(sixty_four_bit_register_numbers_to_strings[register_]);
			output_nullt_string(", ");
			putu_without_separators(amount);
			output_nullt_string("  ; 10 byte encoding\n");
		#endif
		
		ARROW_ASSIGN(dac, start_of_code)[dac->current_instruction_offset] = 72 + (register_ >= REGISTER_R8);
		ARROW_ASSIGN(dac, start_of_code)[dac->current_instruction_offset + 1] = 184 + (register_ % 8);
		
		DEREF_ASSIGN((u*)(dac->start_of_code + ARROW(dac, current_instruction_offset) + 2)) = amount;
		dac->current_instruction_offset += 10;
	}
#endif
