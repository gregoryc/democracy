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
	attr_nonnull void dac_append_add_or_sub_reg64_lit32(
		DataAndCode* cr dac,
		const Register64 register_,
		s32 amount,
		const small_num_type register_code_base,
		const small_num_type special_rax_code
	) {
		assert_comparison(dac, !=, NULL, DataAndCode*, void*);
		assert_comparison(register_, <=, REGISTER_R15, Register64, Register64);
		
		dac->start_of_code[dac->current_instruction_offset] = 72 + (register_ >= REGISTER_R8);
		
		if ((schar)amount == amount) {
			dac->start_of_code[dac->current_instruction_offset + 1] = (uchar)131;
			dac->start_of_code[dac->current_instruction_offset + 3] = (schar)amount;
		} else {
			dac->start_of_code[dac->current_instruction_offset + 2 + (register_ != REGISTER_RAX)] = ((uchar*)&amount)[0];
			dac->start_of_code[dac->current_instruction_offset + 3 + (register_ != REGISTER_RAX)] = ((uchar*)&amount)[1];
			dac->start_of_code[dac->current_instruction_offset + 4 + (register_ != REGISTER_RAX)] = ((uchar*)&amount)[2];
			dac->start_of_code[dac->current_instruction_offset + 5 + (register_ != REGISTER_RAX)] = ((uchar*)&amount)[3];
			
			if (register_ == REGISTER_RAX) {
				dac->start_of_code[dac->current_instruction_offset + 1] = special_rax_code;
				dac->current_instruction_offset += 6;
				return;
			} else {
				dac->start_of_code[dac->current_instruction_offset + 1] = (uchar)129;
			}
		}
		dac->start_of_code[dac->current_instruction_offset + 2] = register_code_base + register_ - ((register_ >= REGISTER_R8) * 8);
		dac->current_instruction_offset += 4 + ((schar)amount != amount) * 3;
	}
#endif
