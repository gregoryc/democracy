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
	__attribute__((nonnull (1)))
	void dac_reg64_reg64(
		DataAndCode* cr dac,
		const Register64 register1,
		const Register64 register2,
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
	
	void dac_append_deref_register_as_qword_reg64(
		DataAndCode* cr dac,
		const Register64 register1,
		const Register64 register2,
		const uchar base
		#if DEBUG
			, const char* cr instruction_string1
		#endif
	) {
		assert_comparison(dac, !=, NULL, DataAndCode*, void*);
		assert_comparison(register1, <=, REGISTER_R15, Register64, Register64);
		assert_comparison(register2, <=, REGISTER_R15, Register64, Register64);
		assert_comparison(instruction_string1, !=, NULL, const char* cr, void*);
		
		if (register1 == REGISTER_RBP || register1 == REGISTER_R13) {
			/*
			REGISTER_RAX is being given as an argument to this function
			because REGISTER_RAX == 0, and it needs to be zero to generate
			the right instruction.
			*/
			dac_reg64_reg64(dac, REGISTER_RAX, register2, base, 69
			#if DEBUG
			, "mov qword[", "]", "", ""
			#endif
			);
			dac->start_of_code[dac->current_instruction_offset++] = 0;
			return;
		}
		
		dac_reg64_reg64(dac, register1, register2, base, 0
		#if DEBUG
		, "mov qword[", "]", "", ""
		#endif
		);
		
		if (register1 == REGISTER_RSP || register1 == REGISTER_R12) {
			dac->start_of_code[dac->current_instruction_offset++] = 36;
		}
	}
#endif
