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

attr_nonnull void dac_reg32_reg32(
	DataAndCode* cr dac,
	const Register32 register1,
	const Register32 register2,
	const small_num_type instruction_number,
	const small_num_type base
	#if DEBUG
		,
		const char* cr instruction_string1,
		const char* cr instruction_string2,
		const char* cr instruction_string3,
		const char* cr instruction_string4
	#endif
);

u dac_get_length_of_mov_deref_register_as_dword_reg32(const Register32 register1) {
	return 2 + (register1 == REGISTER_ESP || register1 == REGISTER_EBP);
}

void dac_append_mov_deref_register_as_dword_reg32(DataAndCode* cr dac, const Register32 register1, const Register32 register2) {
	assert_comparison(dac, !=, NULL, DataAndCode*, void*);
	assert_comparison(register1, <=, REGISTER_EDI, Register32, Register32);
	assert_comparison(register2, <=, REGISTER_EDI, Register32, Register32);
	
	#if SIXTY_FOUR_BIT
		(void)dac;
		(void)register1;
		(void)register2;
		
		#if DEBUG
			output_nullt_string("dac_append_mov_deref_register_as_qword_reg32() called in 64-bit mode! This should be fixed.\n");
			exit_program();
		#endif
	#else
		if (register1 == REGISTER_EBP) {
			/*
			REGISTER_EAX is being given as an argument to this function
			because REGISTER_EAX == 0, and it needs to be zero to generate
			the right instruction.
			*/
			dac_reg32_reg32(dac, REGISTER_EAX, register2, 137, 69
			#if DEBUG
			, "mov dword[", "]", "", ""
			#endif
			);
			dac->start_of_code[dac->current_instruction_offset++] = 0;
			return;
		}
		
		dac_reg32_reg32(dac, register1, register2, 137, 0
		#if DEBUG
		, "mov dword[", "]", "", ""
		#endif
		);
		
		if (register1 == REGISTER_ESP) {
			dac->start_of_code[dac->current_instruction_offset++] = 36;
		}
	#endif
}
