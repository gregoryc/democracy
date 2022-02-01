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
	attr_const u dac_get_length_of_mov_deref_register_as_qword_lit32(const Register64 register_) {
		return 7 + (register_ == REGISTER_RSP || register_ == REGISTER_RBP || register_ == REGISTER_R12 || register_ == REGISTER_R13);
	}
	
	attr_nonnull void dac_append_mov_deref_register_as_qword_lit32(DataAndCode* cr dac, const Register64 register_, const u number) {
		assert_comparison(dac, !=, NULL, DataAndCode*, void*);
		assert_comparison(register_, <=, REGISTER_R15, Register64, Register64);
		
		#if DEBUG
			output_nullt_string(TAB_STRING TAB_STRING);
			output_nullt_string("mov qword[");
			output_nullt_string(sixty_four_bit_register_numbers_to_strings[register_]);
			output_nullt_string("], ");
			putu_without_separators(number);
			output_newline();
		#endif
		
		ARRAY_INDEX_ASSIGN(ARROW(dac, start_of_code), ARROW(dac, current_instruction_offset)) = 72 + (register_ >= REGISTER_R8);
		ARRAY_INDEX_ASSIGN(ARROW(dac, start_of_code), ARROW(dac, current_instruction_offset) + 1) = (uchar)199;
		ARROW_ASSIGN(dac, current_instruction_offset) += 2;
		
		switch (register_) {
			case REGISTER_RSP:
			case REGISTER_R12:
				ARRAY_INDEX_ASSIGN(ARROW(dac, start_of_code), dac->current_instruction_offset) = 4;
				ARRAY_INDEX_ASSIGN(ARROW(dac, start_of_code), dac->current_instruction_offset + 1) = 36;
				ARROW_ASSIGN(dac, current_instruction_offset) += 2;
				break;
			case REGISTER_RBP:
			case REGISTER_R13:
				ARRAY_INDEX_ASSIGN(ARROW(dac, start_of_code), dac->current_instruction_offset) = 69;
				ARRAY_INDEX_ASSIGN(ARROW(dac, start_of_code), dac->current_instruction_offset + 1) = 0;
				ARROW_ASSIGN(dac, current_instruction_offset) += 2;
				break;
			default:
				ARRAY_INDEX_ASSIGN(ARROW(dac, start_of_code), dac->current_instruction_offset++) = register_;
		}
		
		ARRAY_INDEX_ASSIGN(ARROW(dac, start_of_code), ARROW(dac, current_instruction_offset)) = ((const uchar*)&number)[0];
		ARRAY_INDEX_ASSIGN(ARROW(dac, start_of_code), ARROW(dac, current_instruction_offset) + 1) = ((const uchar*)&number)[1];
		ARRAY_INDEX_ASSIGN(ARROW(dac, start_of_code), ARROW(dac, current_instruction_offset) + 2) = ((const uchar*)&number)[2];
		ARRAY_INDEX_ASSIGN(ARROW(dac, start_of_code), ARROW(dac, current_instruction_offset) + 3) = ((const uchar*)&number)[3];
		ARROW_ASSIGN(dac, current_instruction_offset) += 4;
	}
#endif
