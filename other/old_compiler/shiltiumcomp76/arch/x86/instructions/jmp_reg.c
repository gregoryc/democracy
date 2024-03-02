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
	u dac_get_length_of_jmp_reg(Register64 register_) {
		return 2 + (register_ >= REGISTER_R8);
	}
	
	void dac_append_jmp_reg(DataAndCode* cr dac, const Register64 register_) {
		assert_comparison(dac, !=, NULL, DataAndCode* cr, void*);
		assert_comparison(register_, <=, REGISTER_R15, Register64, Register64);
		
		#if DEBUG
			output_nullt_string(TAB_STRING TAB_STRING "jmp ");
			output_nullt_string(sixty_four_bit_register_numbers_to_strings[register_]);
			output_newline();
		#endif
		
		if (register_ >= REGISTER_R8) {
			ARRAY_INDEX_ASSIGN(ARROW(dac, start_of_code), ARROW(dac, current_instruction_offset)) = 65;
			ARROW_ASSIGN(dac, current_instruction_offset)++;
		}
		
		ARRAY_INDEX_ASSIGN(ARROW(dac, start_of_code), ARROW(dac, current_instruction_offset)) = (uchar)255;
		ARRAY_INDEX_ASSIGN(ARROW(dac, start_of_code), ARROW(dac, current_instruction_offset) + 1) = (uchar)224 + register_;
		ARROW_ASSIGN(dac, current_instruction_offset) += 2;
	}
#else
	u dac_get_length_of_jmp_reg(void) {
		return 2;
	}
	
	void dac_append_jmp_reg(DataAndCode* cr dac, const Register32 register_) {
		assert_comparison(dac, !=, NULL, DataAndCode* cr, void*);
		assert_comparison(register_, <=, REGISTER_EDI, Register32, Register32);
		
		#if DEBUG
			output_nullt_string(TAB_STRING TAB_STRING "jmp ");
			output_nullt_string(thirty_two_bit_register_numbers_to_strings[register_]);
			output_newline();
		#endif
		
		ARRAY_INDEX_ASSIGN(ARROW(dac, start_of_code), ARROW(dac, current_instruction_offset)) = (uchar)255;
		ARRAY_INDEX_ASSIGN(ARROW(dac, start_of_code), ARROW(dac, current_instruction_offset) + 1) = (uchar)224 + register_;
		ARROW_ASSIGN(dac, current_instruction_offset) += 2;
	}
#endif
