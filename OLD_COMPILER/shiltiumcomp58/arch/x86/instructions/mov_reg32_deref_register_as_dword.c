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

u dac_get_length_of_mov_reg32_deref_register_as_dword(const Register32 register2) {
	return 2 + (register2 == REGISTER_ESP || register2 == REGISTER_EBP);
}

void dac_reg32_reg32(
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
) __attribute__((nonnull (1)));

/*
This function, which generates the instruction

mov reg32, dword[reg32]

is basically equivalent to dac_append_mov_deref_register_as_dword_reg32(), which generates the instruction

mov dword[reg32], reg32

except the instruction number is 139 instead of 137 and all the registers are backwards.

So

foo(register1, register2)

in dac_append_mov_deref_register_as_dword_reg32() would be

foo(register2, register1)

in this function.
*/
void dac_append_mov_reg32_deref_register_as_dword(DataAndCode* cr dac, const Register32 register1, const Register32 register2) {
	assert_comparison(dac, !=, NULL, DataAndCode*, void*);
	assert_comparison(register1, <=, REGISTER_EDI, Register32, Register32);
	assert_comparison(register2, <=, REGISTER_EDI, Register32, Register32);
	
	#if DEBUG
		output_nullt_string(TAB_STRING TAB_STRING);
		output_nullt_string("mov ");
		output_nullt_string(thirty_two_bit_register_numbers_to_strings[register1]);
		output_nullt_string(", dword[");
		output_nullt_string(thirty_two_bit_register_numbers_to_strings[register2]);
		output_nullt_string("]\n");
	#endif
	
	if (register2 == REGISTER_EBP) {
		/*
		REGISTER_EAX is being given as an argument to this function
		because REGISTER_EAX == 0, and it needs to be zero to generate
		the right instruction.
		*/
		dac_reg32_reg32(dac, REGISTER_EAX, register1, 139, 69
		#if DEBUG
		, NULL, NULL, NULL, NULL
		#endif
		);
		
		ARRAY_INDEX_ASSIGN(ARROW(dac, start_of_code), ARROW(dac, current_instruction_offset)) = 0;
		ARROW_ASSIGN(dac, current_instruction_offset)++;
		return;
	}
	
	dac_reg32_reg32(dac, register2, register1, 139, 0
	#if DEBUG
		, NULL, NULL, NULL, NULL
	#endif
	);
	
	if (register2 == REGISTER_ESP) {
		ARRAY_INDEX_ASSIGN(ARROW(dac, start_of_code), ARROW(dac, current_instruction_offset)) = 36;
		ARROW_ASSIGN(dac, current_instruction_offset)++;
	}
}
