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

attr_nonnull void dac_append_add_or_sub_reg32_lit32(
	DataAndCode* cr dac,
	const Register32 register_,
	const s32 amount,
	const small_num_type register_code_base,
	const small_num_type special_eax_code
) {
	assert_comparison(dac, !=, NULL, DataAndCode*, void*);
	assert_comparison(register_, <=, REGISTER_EDI, Register32, Register32);
	
	if ((schar)amount == amount) {
		dac->start_of_code[dac->current_instruction_offset] = (uchar)131;
		dac->start_of_code[dac->current_instruction_offset + 2] = (schar)amount;
	} else {
		dac->start_of_code[dac->current_instruction_offset + 1 + (register_ != REGISTER_EAX)] = ((const uchar*)&amount)[0];
		dac->start_of_code[dac->current_instruction_offset + 2 + (register_ != REGISTER_EAX)] = ((const uchar*)&amount)[1];
		dac->start_of_code[dac->current_instruction_offset + 3 + (register_ != REGISTER_EAX)] = ((const uchar*)&amount)[2];
		dac->start_of_code[dac->current_instruction_offset + 4 + (register_ != REGISTER_EAX)] = ((const uchar*)&amount)[3];
		
		if (register_ == REGISTER_EAX) {
			dac->start_of_code[dac->current_instruction_offset] = special_eax_code;
			dac->current_instruction_offset += 5;
			return;
		} else {
			dac->start_of_code[dac->current_instruction_offset] = (uchar)129;
		}
	}
	
	dac->start_of_code[dac->current_instruction_offset + 1] = register_code_base + register_;
	dac->current_instruction_offset += 3 + ((schar)amount != amount) * 3;
}
