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

attr_const u dac_get_length_of_je_short(void) {
	return 2;
}

attr_nonnull void dac_append_je_short(DataAndCode* cr dac, const schar displacement) {
	assert_comparison(dac, !=, NULL, DataAndCode*, void*);
	
	#if DEBUG
		output_nullt_string(TAB_STRING TAB_STRING "je short $ + ");
		puts_without_separators(displacement);
		output_newline();
	#endif
	
	ARRAY_INDEX_ASSIGN(ARROW(dac, start_of_code), ARROW(dac, current_instruction_offset)) = 116;
	ARRAY_INDEX_ASSIGN(ARROW(dac, start_of_code), ARROW(dac, current_instruction_offset + 1)) = displacement;
	ARROW_ASSIGN(dac, current_instruction_offset) += 2;
}

attr_const u dac_get_length_of_je_near(void) {
	return 6;
}

attr_nonnull void dac_append_je_near(DataAndCode* cr dac, const s32 displacement) {
	assert_comparison(dac, !=, NULL, DataAndCode*, void*);
	
	#if DEBUG
		output_nullt_string(TAB_STRING TAB_STRING "je near $ + ");
		puts_without_separators(displacement);
		output_newline();
	#endif
	
	ARRAY_INDEX_ASSIGN(ARROW(dac, start_of_code), ARROW(dac, current_instruction_offset)) = 15;
	ARRAY_INDEX_ASSIGN(ARROW(dac, start_of_code), ARROW(dac, current_instruction_offset) + 1) = (uchar)132;
	DEREF_ASSIGN((s32*)(ARROW(dac, start_of_code) + ARROW(dac, current_instruction_offset) + 2)) = displacement;
	ARROW_ASSIGN(dac, current_instruction_offset) += 6;
}

attr_const u dac_get_length_of_je(const s displacement) {
	if (displacement == (schar)displacement) {
		return dac_get_length_of_je_short();
	}
	
	if (displacement == (s32)displacement) {
		return dac_get_length_of_je_near();
	}
	
	output_nullt_string("Very large jump. FIXME\n");
	exit_program();
}

attr_nonnull void dac_append_je(DataAndCode* cr dac, const s displacement) {
	if (displacement == (schar)displacement) {
		dac_append_je_short(dac, displacement);
	} else if (displacement == (s32)displacement) {
		dac_append_je_near(dac, displacement);
	} else {
		output_nullt_string("Very large jump. FIXME\n");
		exit_program();
	}
}
