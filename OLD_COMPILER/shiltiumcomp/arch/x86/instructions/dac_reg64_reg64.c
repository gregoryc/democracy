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
	) {
		assert_comparison(dac, !=, NULL, DataAndCode* cr, void*);
		assert_comparison(register1, <=, REGISTER_R15, Register64, Register64);
		assert_comparison(register2, <=, REGISTER_R15, Register64, Register64);
		
		#if DEBUG
			if (instruction_string1) {
				output_nullt_string(TAB_STRING TAB_STRING);
				output_nullt_string(instruction_string1);
				output_nullt_string(sixty_four_bit_register_numbers_to_strings[register1]);
				if (DEREF(instruction_string2)) {
					output_nullt_string(instruction_string2);
				}
				output_nullt_string(", ");
				if (DEREF(instruction_string3)) {
					output_nullt_string(instruction_string3);
				}
				output_nullt_string(sixty_four_bit_register_numbers_to_strings[register2]);
				if (DEREF(instruction_string4)) {
					output_nullt_string(instruction_string4);
				}
				output_newline();
			}
		#endif
		
		if (register1 <= REGISTER_RDI) {
			if (register2 <= REGISTER_RDI) {
				dac->start_of_code[dac->current_instruction_offset] = 72;
			} else {
				dac->start_of_code[dac->current_instruction_offset] = 76;
			}
		} else {
			if (register2 <= REGISTER_RDI) {
				dac->start_of_code[dac->current_instruction_offset] = 73;
			} else {
				dac->start_of_code[dac->current_instruction_offset] = 77;
			}
		}
		
		dac->start_of_code[dac->current_instruction_offset + 1] = instruction_number;
		dac->start_of_code[dac->current_instruction_offset + 2] = base + ((register1 % 8) + ((register2 % 8) * 8));
		dac->current_instruction_offset += 3;
	}
#endif
