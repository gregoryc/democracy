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
#include "get_length_of_add_or_sub_reg64_lit32.h"

#if SIXTY_FOUR_BIT
	void dac_append_add_or_sub_reg64_lit32(
		DataAndCode* cr dac,
		const Register64 register_,
		s32 amount,
		const small_num_type register_code_base,
		const small_num_type special_rax_code
	) attr_nonnull;
	
	u dac_get_length_of_add_reg64_lit32(const Register64 register_, const s32 amount) {
		return dac_get_length_of_add_or_sub_reg64_lit32(register_, amount);
	}
	
	void dac_append_add_reg64_lit32(DataAndCode* cr dac, const Register64 register_, const s32 amount) {
		assert_comparison(dac, !=, NULL, DataAndCode*, void*);
		assert_comparison(register_, <=, REGISTER_R15, Register64, Register64);
		
		#if DEBUG
			output_nullt_string(TAB_STRING TAB_STRING "add ");
			output_nullt_string(sixty_four_bit_register_numbers_to_strings[register_]);
			output_nullt_string(", ");
			putu_without_commas(amount);
			output_newline();
		#endif
		
		dac_append_add_or_sub_reg64_lit32(dac, register_, amount, 192, 5);
	}
#endif
