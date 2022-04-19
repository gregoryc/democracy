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
	u dac_get_length_of_mov_deref_register_as_qword_reg64(const Register64 register1) {
		return 3 + (register1 == REGISTER_RSP || register1 == REGISTER_RBP || register1 == REGISTER_R12 || register1 == REGISTER_R13);
	}
	
	void dac_append_deref_register_as_qword_reg64(
		DataAndCode* cr dac,
		const Register64 register1,
		const Register64 register2,
		const uchar base
		#if DEBUG
			, const char* cr instruction_string1
		#endif
	) attr_nonnull;
	
	void dac_append_mov_deref_register_as_qword_reg64(DataAndCode* cr dac, const Register64 register1, const Register64 register2) {
		assert_comparison(dac, !=, NULL, DataAndCode*, void*);
		
		dac_append_deref_register_as_qword_reg64(dac, register1, register2, 137
		#if DEBUG
			, "mov qword["
		#endif
		);
	}
#endif
