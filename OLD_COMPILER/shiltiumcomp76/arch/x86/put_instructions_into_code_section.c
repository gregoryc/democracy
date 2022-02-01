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

#include "../../global_headers/headers.h"
#include "instructions/x86_instructions.h"
#include "../../global_headers/globals.h"

/* Note that this structure is SubroutineForwardReference, not SubroutineForwardReferences (with an s) */
typedef struct {
	char* address_of_mov_instruction_in_memory;
	u address_of_the_subroutine_that_the_mov_instruction_is_putting_in_a_register;
	small_num_type number_of_bytes_in_encoding;
} SubroutineForwardReference;

static void store_float_variable_on_stack(DataAndCode* cr dac, Variable* cr variable) {
	u variable_stack_offset;
	
	assert_comparison(dac, !=, NULL, Variable*, void*);
	assert_comparison(variable, !=, NULL, Variable*, void*);
	
	variable_stack_offset = ARROW(variable, stack_offset);
	
	#if DEBUG
		output_nullt_string("\n" TAB_STRING TAB_STRING "; store st0 on the stack\n");
	#endif
	
	#if SIXTY_FOUR_BIT
		if (variable_stack_offset) {
			if (variable_stack_offset <= 127) {
				dac_append_fstp_deref_register_as_qword_plus_signed_byte_offset(dac, REGISTER_RSP, variable_stack_offset);
			} else {
				dac_append_mov_reg64_lit64(dac, REGISTER_R11, variable_stack_offset);
				dac_append_add_reg64_reg64(dac, REGISTER_R11, REGISTER_RSP);
				dac_append_fstp_deref_register_as_qword(dac, REGISTER_R11);
			}
		} else {
			dac_append_fstp_deref_register_as_qword(dac, REGISTER_RSP);
		}
	#else
		if (variable_stack_offset) {
			dac_append_mov_reg32_lit32(dac, REGISTER_EDI, variable_stack_offset);
			dac_append_add_reg32_reg32(dac, REGISTER_EDI, REGISTER_ESP);
			dac_append_fstp_deref_register_as_qword(dac, REGISTER_EDI);
		} else {
			dac_append_fstp_deref_register_as_qword(dac, REGISTER_ESP);
		}
	#endif
	
	#if DEBUG
		output_newline();
	#endif
}

void store_u_variable_on_stack(DataAndCode* cr dac, Variable* cr variable) {
	u variable_stack_offset;
	
	assert_comparison(dac, !=, NULL, Variable*, void*);
	assert_comparison(variable, !=, NULL, Variable*, void*);
	
	variable_stack_offset = ARROW(variable, stack_offset);
	
	#if DEBUG
		#if SIXTY_FOUR_BIT
			output_nullt_string(TAB_STRING TAB_STRING "; store rax on the stack\n");
		#else
			output_nullt_string(TAB_STRING TAB_STRING "; store eax on the stack\n");
		#endif
	#endif
	
	#if SIXTY_FOUR_BIT
		if (variable_stack_offset) {
			dac_append_mov_reg64_lit64(dac, REGISTER_R11, variable_stack_offset);
			dac_append_add_reg64_reg64(dac, REGISTER_R11, REGISTER_RSP);
			dac_append_mov_deref_register_as_qword_reg64(dac, REGISTER_R11, REGISTER_RAX);
		} else {
			dac_append_mov_deref_register_as_qword_reg64(dac, REGISTER_RSP, REGISTER_RAX);
		}
	#else
		if (variable_stack_offset) {
			dac_append_mov_reg32_lit32(dac, REGISTER_EDI, variable_stack_offset);
			dac_append_add_reg32_reg32(dac, REGISTER_EDI, REGISTER_ESP);
			dac_append_mov_deref_register_as_dword_reg32(dac, REGISTER_EDI, REGISTER_EAX);
		} else {
			dac_append_mov_deref_register_as_dword_reg32(dac, REGISTER_ESP, REGISTER_EAX);
		}
	#endif
	
	#if DEBUG
		output_newline();
	#endif
}

#define mid_level_instruction_function(name) \
	static void name( \
		DataAndCode* cr dac, \
		MidLevelInstruction* cr instruction, \
		MidLevelInstructions* cr mli, \
		u* cr instruction_offset, \
		u* cr main_subroutine_offset, \
		bool* cr main_subroutine_offset_set, \
		LinkedListHead* cr forward_references, \
		u* cr number_of_nops_that_would_have_been_generated \
	) {

#define end_mid_level_instruction_function \
		(void)dac; \
		(void)instruction; \
		(void)mli; \
		(void)instruction_offset; \
		(void)main_subroutine_offset; \
		(void)main_subroutine_offset_set; \
		(void)forward_references; \
		(void)number_of_nops_that_would_have_been_generated; \
	}

mid_level_instruction_function(mov_uvar_uval)
	Variable* variable;
	OperandNode* operand_node;
	
	variable = (Variable*)ARRAY_INDEX(instruction->arguments, 0);
	operand_node = (OperandNode*)ARRAY_INDEX(instruction->arguments, 1);

	#if SIXTY_FOUR_BIT
		dac_append_mov_reg64_lit64(dac, REGISTER_RAX, ARROW(operand_node, value).uvalue);
	#else
		dac_append_mov_reg32_lit32(dac, REGISTER_EAX, ARROW(operand_node, value).uvalue);
	#endif
	
	if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
		store_u_variable_on_stack(dac, variable);
	}
end_mid_level_instruction_function

mid_level_instruction_function(mov_fvar_fval)
	Variable* variable = (Variable*)ARRAY_INDEX(instruction->arguments, 0);
	OperandNode* operand_node = (OperandNode*)ARRAY_INDEX(instruction->arguments, 1);
	u variable_stack_offset;
	
	#if DEBUG
		output_nullt_string(TAB_STRING TAB_STRING "; ");
		output_variable(variable);
		output_nullt_string(" = ");
		putf(ARROW(operand_node, value).fvalue);
		output_newline();
	#endif
	
	variable_stack_offset = ARROW(variable, stack_offset);
	
	if (ARROW(operand_node, value).fvalue == 0.0) {
		dac_append_fldz(dac);
	} else if (ARROW(operand_node, value).fvalue == 1.0) {
		dac_append_fld1(dac);
	} else {
		u absolute_address_in_memory;
		s rip_relative_offset;
		u rip_offset;
		
		/* Assert that the value stored at ARROW(operand_node, address_in_memory) is the proper value. */
		assert_comparison(DEREF((f*)DEREF(ARROW(operand_node, address_in_memory))), ==, ARROW(operand_node, value).fvalue, f, f);
		
		absolute_address_in_memory = DEREF((u*)ARROW(operand_node, address_in_memory));
		
		rip_offset = (u)(ARROW(dac, start_of_code) + ARROW(dac, current_instruction_offset) + dac_get_length_of_fld_qword_address());
		
		rip_relative_offset = absolute_address_in_memory - rip_offset;
		
		#if SIXTY_FOUR_BIT
			if (rip_relative_offset && rip_relative_offset == (s32)rip_relative_offset) {
				dac_append_fld_qword_address(dac, rip_relative_offset);
				
				DEREF_ASSIGN(number_of_nops_that_would_have_been_generated) +=
					dac_get_length_of_mov_reg64_lit64(REGISTER_R11, absolute_address_in_memory) +
					dac_get_length_of_fld_deref_register_as_qword(REGISTER_R11) -
					dac_get_length_of_fld_qword_address();
			} else {
				dac_append_mov_reg64_lit64(dac, REGISTER_R11, absolute_address_in_memory);
				dac_append_fld_deref_register_as_qword(dac, REGISTER_R11);
			}
		#else
			dac_append_mov_reg32_lit32(dac, REGISTER_EDI, DEREF((u*)ARROW(operand_node, address_in_memory)));
			dac_append_fld_deref_register_as_qword(dac, REGISTER_EDI);
		#endif
	}
	
	if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
		store_float_variable_on_stack(dac, variable);
	}
	
	m_free(operand_node);
end_mid_level_instruction_function

mid_level_instruction_function(mov_fvar_fvar)
	Variable* variable1 = (Variable*)ARRAY_INDEX(instruction->arguments, 0);
	OperandNode* operand_node = (OperandNode*)ARRAY_INDEX(instruction->arguments, 1);
	Variable* variable2 = ARROW(operand_node, variable);
	u variable_stack_offset;
	
	#if DEBUG
		output_nullt_string("\n" TAB_STRING TAB_STRING "; ");
		output_variable(variable1);
		output_nullt_string(" = ");
		output_variable(variable2);
		output_newline();
	#endif
	
	variable_stack_offset = ARROW(variable2, stack_offset);
	
	#if SIXTY_FOUR_BIT
		if (ARROW(variable2, is_argument)) {
			dac_append_mov_reg64_reg64(dac, REGISTER_R11, REGISTER_RBP);
			dac_append_mov_reg64_lit64(dac, REGISTER_R12, variable_stack_offset);
			dac_append_add_reg64_reg64(dac, REGISTER_R11, REGISTER_R12);
		} else {
			dac_append_mov_reg64_reg64(dac, REGISTER_R11, REGISTER_RSP);
			dac_append_mov_reg64_lit64(dac, REGISTER_R12, variable_stack_offset);
			dac_append_add_reg64_reg64(dac, REGISTER_R11, REGISTER_R12);
		}
		dac_append_fld_deref_register_as_qword(dac, REGISTER_R11);
	#else
		if (ARROW(variable2, is_argument)) {
			dac_append_mov_reg32_reg32(dac, REGISTER_EDI, REGISTER_RBP);
			dac_append_mov_reg32_lit32(dac, REGISTER_ESI, variable_stack_offset);
			dac_append_add_reg32_reg32(dac, REGISTER_EDI, REGISTER_ESI);
		} else {
			dac_append_mov_reg32_reg32(dac, REGISTER_EDI, REGISTER_ESP);
			dac_append_mov_reg32_lit32(dac, REGISTER_ESI, variable_stack_offset);
			dac_append_add_reg32_reg32(dac, REGISTER_EDI, REGISTER_ESI);
		}
		
		dac_append_fld_deref_register_as_qword(dac, REGISTER_EDI);
	#endif
	
	if (instruction == ARROW(variable1, last_instruction_that_uses_this_variable)) {
		store_float_variable_on_stack(dac, variable1);
	}
	
	m_free(operand_node);
end_mid_level_instruction_function

mid_level_instruction_function(mul_uvar_uval)
	assert_comparison(0, !=, 0, u, u);
end_mid_level_instruction_function

mid_level_instruction_function(mul_fvar_fval)
	Variable* variable = (Variable*)ARRAY_INDEX(instruction->arguments, 0);
	OperandNode* operand_node = (OperandNode*)ARRAY_INDEX(instruction->arguments, 1);
	u variable_stack_offset;

	#if DEBUG
		output_nullt_string(TAB_STRING TAB_STRING "; ");
		output_variable(variable);
		output_nullt_string(" *= ");
		putf(ARROW(operand_node, value).fvalue);
		output_newline();
	#endif

	variable_stack_offset = ARROW(variable, stack_offset);

	if (ARROW(operand_node, value).fvalue == 0.0) {
		#if SIXTY_FOUR_BIT
			dac_append_fstp_deref_register_as_qword(dac, REGISTER_RAX);
		#else
			dac_append_fstp_deref_register_as_qword(dac, REGISTER_EAX);
		#endif
		
		dac_append_fldz(dac);
	} else {
		#if SIXTY_FOUR_BIT
			dac_append_mov_reg64_lit64(dac, REGISTER_R11, DEREF((u*)ARROW(operand_node, address_in_memory)));
			dac_append_fmul_deref_register_as_qword(dac, REGISTER_R11);
		#else
			dac_append_mov_reg32_lit32(dac, REGISTER_EDI, DEREF((u*)ARROW(operand_node, address_in_memory)));
			dac_append_fmul_deref_register_as_qword(dac, REGISTER_EDI);
		#endif
	}

	if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
		store_float_variable_on_stack(dac, variable);
	}
	
	m_free(operand_node);
end_mid_level_instruction_function

mid_level_instruction_function(div_uvar_uval)
	assert_comparison(0, !=, 0, u, u);
end_mid_level_instruction_function

mid_level_instruction_function(div_fvar_fval)
	assert_comparison(0, !=, 0, u, u);
end_mid_level_instruction_function

mid_level_instruction_function(add_uvar_uval)
	OperandNode* operand_node;
	Variable* variable;
	
	variable = (Variable*)ARRAY_INDEX(instruction->arguments, 0);
	operand_node = (OperandNode*)ARRAY_INDEX(instruction->arguments, 1);
	
	#if SIXTY_FOUR_BIT
		dac_append_mov_reg64_lit64(dac, REGISTER_RBX, ARROW(operand_node, value).uvalue);
		dac_append_add_reg64_reg64(dac, REGISTER_RAX, REGISTER_RBX);
	#else
		dac_append_mov_reg32_lit32(dac, REGISTER_EBX, ARROW(operand_node, value).uvalue);
		dac_append_add_reg32_reg32(dac, REGISTER_EAX, REGISTER_EBX);
	#endif
	
	if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
		store_u_variable_on_stack(dac, variable);
	}
end_mid_level_instruction_function

mid_level_instruction_function(add_fvar_fval)
	Variable* variable = (Variable*)ARRAY_INDEX(instruction->arguments, 0);
	OperandNode* operand_node = (OperandNode*)ARRAY_INDEX(instruction->arguments, 1);
	u variable_stack_offset;
	
	#if DEBUG
		output_nullt_string(TAB_STRING TAB_STRING "; ");
		output_variable(variable);
		output_nullt_string(" += ");
		putf(ARROW(operand_node, value).fvalue);
		output_newline();
	#endif
	
	variable_stack_offset = ARROW(variable, stack_offset);
	
	if (ARROW(operand_node, value).fvalue == 0.0) {
		/* Don't add 0.0 to any number. */
	} else if (ARROW(operand_node, value).fvalue == 1.0) {
		dac_append_fld1(dac);
		dac_append_faddp(dac);
	} else {
		u absolute_address_in_memory;
		s rip_relative_offset;
		u rip_offset;
		
		absolute_address_in_memory = DEREF((u*)ARROW(operand_node, address_in_memory));
		
		rip_offset = (u)(ARROW(dac, start_of_code) + ARROW(dac, current_instruction_offset) + dac_get_length_of_fadd_qword_address());
		
		rip_relative_offset = absolute_address_in_memory - rip_offset;
		
		#if SIXTY_FOUR_BIT
			if (rip_relative_offset && rip_relative_offset == (s32)rip_relative_offset) {
				dac_append_fadd_qword_address(dac, rip_relative_offset);
				
				DEREF_ASSIGN(number_of_nops_that_would_have_been_generated) +=
					dac_get_length_of_mov_reg64_lit64(REGISTER_R11, absolute_address_in_memory) +
					dac_get_length_of_fadd_deref_register_as_qword(REGISTER_R11) -
					dac_get_length_of_fadd_qword_address();
			} else {
				dac_append_mov_reg64_lit64(dac, REGISTER_R11, absolute_address_in_memory);
				dac_append_fadd_deref_register_as_qword(dac, REGISTER_R11);
			}
		#else
			dac_append_mov_reg32_lit32(dac, REGISTER_EDI, absolute_address_in_memory);
			dac_append_fadd_deref_register_as_qword(dac, REGISTER_EDI);
		#endif
	}
	
	if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
		store_float_variable_on_stack(dac, variable);
	}
	
	m_free(operand_node);
end_mid_level_instruction_function

mid_level_instruction_function(add_uvar_uvar)
	assert_comparison(0, !=, 0, u, u);
end_mid_level_instruction_function

mid_level_instruction_function(add_fvar_fvar)
	Variable* variable1 = (Variable*)ARRAY_INDEX(instruction->arguments, 0);
	Variable* variable2 = (Variable*)ARRAY_INDEX(instruction->arguments, 1);
	u variable_stack_offset;
	
	#if DEBUG
		output_nullt_string(TAB_STRING TAB_STRING "; ");
		output_variable(variable1);
		output_nullt_string(" += ");
		output_variable(variable2);
		output_newline();
	#endif
	
	variable_stack_offset = ARROW(variable2, stack_offset);
	
	#if SIXTY_FOUR_BIT
		dac_append_mov_reg64_reg64(dac, REGISTER_R11, REGISTER_RSP);
		dac_append_mov_reg64_lit64(dac, REGISTER_R12, variable_stack_offset);
		dac_append_add_reg64_reg64(dac, REGISTER_R11, REGISTER_R12);
		dac_append_fadd_deref_register_as_qword(dac, REGISTER_R11);
	#else
		dac_append_mov_reg32_reg32(dac, REGISTER_EDI, REGISTER_ESP);
		dac_append_mov_reg32_lit32(dac, REGISTER_ESI, variable_stack_offset);
		dac_append_add_reg32_reg32(dac, REGISTER_EDI, REGISTER_ESI);
		dac_append_fadd_deref_register_as_qword(dac, REGISTER_EDI);
	#endif
	
	if (instruction == ARROW(variable1, last_instruction_that_uses_this_variable)) {
		store_float_variable_on_stack(dac, variable1);
	}
end_mid_level_instruction_function

mid_level_instruction_function(sub_uvar_uval)
	assert_comparison(0, !=, 0, u, u);
end_mid_level_instruction_function

mid_level_instruction_function(sub_fvar_fval)
	Variable* variable = (Variable*)ARRAY_INDEX(instruction->arguments, 0);
	OperandNode* operand_node = (OperandNode*)ARRAY_INDEX(instruction->arguments, 1);
	u variable_stack_offset;

	#if DEBUG
		output_nullt_string(TAB_STRING TAB_STRING "; ");
		output_variable(variable);
		output_nullt_string(" -= ");
		putf(ARROW(operand_node, value).fvalue);
		output_newline();
	#endif

	variable_stack_offset = ARROW(variable, stack_offset);

	if (ARROW(operand_node, value).fvalue == 1.0) {
		dac_append_fld1(dac);
		dac_append_fsubp(dac);
	} else {
		#if SIXTY_FOUR_BIT
			dac_append_mov_reg64_lit64(dac, REGISTER_R11, DEREF((u*)ARROW(operand_node, address_in_memory)));
			dac_append_fsub_deref_register_as_qword(dac, REGISTER_R11);
		#else
			dac_append_mov_reg32_lit32(dac, REGISTER_EDI, DEREF((u*)ARROW(operand_node, address_in_memory)));
			dac_append_fsub_deref_register_as_qword(dac, REGISTER_EDI);
		#endif
	}

	if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
		store_float_variable_on_stack(dac, variable);
	}
	
	m_free(operand_node);
end_mid_level_instruction_function

mid_level_instruction_function(sub_uvar_uvar)
	assert_comparison(0, !=, 0, u, u);
end_mid_level_instruction_function

mid_level_instruction_function(sub_fvar_fvar)
	Variable* variable1 = (Variable*)ARRAY_INDEX(instruction->arguments, 0);
	Variable* variable2 = (Variable*)ARRAY_INDEX(instruction->arguments, 1);
	u variable_stack_offset;
	
	#if DEBUG
		output_nullt_string(TAB_STRING TAB_STRING "; ");
		output_variable(variable1);
		output_nullt_string(" -= ");
		output_variable(variable2);
		output_newline();
	#endif
	
	variable_stack_offset = ARROW(variable2, stack_offset);
	
	#if SIXTY_FOUR_BIT
		dac_append_mov_reg64_reg64(dac, REGISTER_R11, REGISTER_RSP);
		dac_append_mov_reg64_lit64(dac, REGISTER_R12, variable_stack_offset);
		dac_append_add_reg64_reg64(dac, REGISTER_R11, REGISTER_R12);
		dac_append_fsub_deref_register_as_qword(dac, REGISTER_R11);
	#else
		dac_append_mov_reg32_reg32(dac, REGISTER_EDI, REGISTER_ESP);
		dac_append_mov_reg32_lit32(dac, REGISTER_ESI, variable_stack_offset);
		dac_append_add_reg32_reg32(dac, REGISTER_EDI, REGISTER_ESI);
		dac_append_fsub_deref_register_as_qword(dac, REGISTER_EDI);
	#endif
	
	if (instruction == ARROW(variable1, last_instruction_that_uses_this_variable)) {
		store_float_variable_on_stack(dac, variable1);
	}
end_mid_level_instruction_function

mid_level_instruction_function(cmp_le_uvar_uval)
	Variable* variable = (Variable*)ARRAY_INDEX(instruction->arguments, 0);
	
	output_nullt_string("cmp_le uvar, uval\n");
	
	if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
		store_float_variable_on_stack(dac, variable);
	}
end_mid_level_instruction_function

mid_level_instruction_function(cmp_lt_uvar_uval)
	Variable* variable = (Variable*)ARRAY_INDEX(instruction->arguments, 0);
	OperandNode* operand_node = (OperandNode*)ARRAY_INDEX(instruction->arguments, 1);
	
	output_nullt_string("cmp_lt uvar, uval\n");
	
	if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
		store_float_variable_on_stack(dac, variable);
	}
	
	m_free(operand_node);
end_mid_level_instruction_function

mid_level_instruction_function(cmp_eq_uvar_uval)
	Variable* variable = (Variable*)ARRAY_INDEX(instruction->arguments, 0);
	
	output_nullt_string("cmp_eq uvar, uval\n");
	
	if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
		store_float_variable_on_stack(dac, variable);
	}
end_mid_level_instruction_function

mid_level_instruction_function(cmp_ne_uvar_uval)
	Variable* variable = (Variable*)ARRAY_INDEX(instruction->arguments, 0);
	
	output_nullt_string("cmp_ne uvar, uval\n");
	
	if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
		store_float_variable_on_stack(dac, variable);
	}
end_mid_level_instruction_function

mid_level_instruction_function(cmp_gt_uvar_uval)
	Variable* variable = (Variable*)ARRAY_INDEX(instruction->arguments, 0);

	output_nullt_string("cmp_gt uvar, uval\n");

	if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
		store_float_variable_on_stack(dac, variable);
	}
end_mid_level_instruction_function

mid_level_instruction_function(cmp_ge_uvar_uval)
	Variable* variable = (Variable*)ARRAY_INDEX(instruction->arguments, 0);

	output_nullt_string("cmp_ge uvar, uval\n");

	if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
		store_float_variable_on_stack(dac, variable);
	}
end_mid_level_instruction_function

mid_level_instruction_function(cmp_le_fvar_fval)
	Variable* variable = (Variable*)ARRAY_INDEX(instruction->arguments, 0);
	OperandNode* operand_node = (OperandNode*)ARRAY_INDEX(instruction->arguments, 1);
	
	output_nullt_string("cmp_le fvar, fval\n");

	if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
		store_float_variable_on_stack(dac, variable);
	}
	
	m_free(operand_node);
end_mid_level_instruction_function

mid_level_instruction_function(cmp_lt_fvar_fval)
	Variable* variable = (Variable*)ARRAY_INDEX(instruction->arguments, 0);
	OperandNode* operand_node = (OperandNode*)ARRAY_INDEX(instruction->arguments, 1);
	
	#if DEBUG
		output_nullt_string(TAB_STRING TAB_STRING "; ");
		output_variable(variable);
		output_nullt_string(" = (float)(");
		output_variable(variable);
		output_nullt_string(" < ");
		putf(ARROW(operand_node, value).fvalue);
		output_nullt_string(")\n");
	#endif
	
	if (ARROW(operand_node, value).fvalue == 0.0) {
		dac_append_fldz(dac);
	} else if (ARROW(operand_node, value).fvalue == 1.0) {
		dac_append_fld1(dac);
	} else {
		#if SIXTY_FOUR_BIT
			dac_append_mov_reg64_lit64(dac, REGISTER_R11, DEREF((u*)ARROW(operand_node, address_in_memory)));
			dac_append_fld_deref_register_as_qword(dac, REGISTER_R11);
		#else
			dac_append_mov_reg32_lit32(dac, REGISTER_EDI, DEREF((u*)ARROW(operand_node, address_in_memory)));
			dac_append_fld_deref_register_as_qword(dac, REGISTER_EDI);
		#endif
	}
	
	/*
	Compare st0 (operand 2) with st1 (operand 1).
	The comparison is backwards, so  3.0 < 2.0  will become  2.0 < 3.0
	*/
	dac_append_fcomip(dac, REGISTER_ST1);
	dac_append_fstp_fpu_register(dac, REGISTER_ST0);
	
	dac_append_jbe(dac,
		dac_get_length_of_fld1() +
		dac_get_length_of_jmp_short()
	);
	
	dac_append_fld1(dac);
	dac_append_jmp_short(dac, dac_get_length_of_fldz());
	dac_append_fldz(dac);
	
	if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
		store_float_variable_on_stack(dac, variable);
	}
	
	m_free(operand_node);
end_mid_level_instruction_function

mid_level_instruction_function(cmp_eq_fvar_fval)
	Variable* variable = (Variable*)ARRAY_INDEX(instruction->arguments, 0);

	output_nullt_string("cmp_eq fvar, fval\n");

	if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
		store_float_variable_on_stack(dac, variable);
	}
end_mid_level_instruction_function

mid_level_instruction_function(cmp_ne_fvar_fval)
	Variable* variable = (Variable*)ARRAY_INDEX(instruction->arguments, 0);
	
	output_nullt_string("cmp_ne fvar, fval\n");
	
	if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
		store_float_variable_on_stack(dac, variable);
	}
end_mid_level_instruction_function

mid_level_instruction_function(cmp_gt_fvar_fval)
	Variable* variable = (Variable*)ARRAY_INDEX(instruction->arguments, 0);
	
	output_nullt_string("cmp_gt fvar, fval\n");
	
	if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
		store_float_variable_on_stack(dac, variable);
	}
end_mid_level_instruction_function

mid_level_instruction_function(cmp_ge_fvar_fval)
	Variable* variable = (Variable*)ARRAY_INDEX(instruction->arguments, 0);
	OperandNode* operand_node = (OperandNode*)ARRAY_INDEX(instruction->arguments, 1);
	
	#if DEBUG
		output_nullt_string(TAB_STRING TAB_STRING "; ");
		output_variable(variable);
		output_nullt_string(" = (float)(");
		output_variable(variable);
		output_nullt_string(" >= ");
		putf(ARROW(operand_node, value).fvalue);
		output_nullt_string(")\n");
	#endif
	
	if (ARROW(operand_node, value).fvalue == 0.0) {
		dac_append_fldz(dac);
	} else if (ARROW(operand_node, value).fvalue == 1.0) {
		dac_append_fld1(dac);
	} else {
		#if SIXTY_FOUR_BIT
			dac_append_mov_reg64_lit64(dac, REGISTER_R11, DEREF((u*)ARROW(operand_node, address_in_memory)));
			dac_append_fld_deref_register_as_qword(dac, REGISTER_R11);
		#else
			dac_append_mov_reg32_lit32(dac, REGISTER_EDI, DEREF((u*)ARROW(operand_node, address_in_memory)));
			dac_append_fld_deref_register_as_qword(dac, REGISTER_EDI);
		#endif
	}
	
	/*
	Compare st0 (operand 2) with st1 (operand 1).
	The comparison is backwards, so  3.0 >= 2.0  will become  2.0 >= 3.0
	*/
	dac_append_fcomip(dac, REGISTER_ST1);
	dac_append_fstp_fpu_register(dac, REGISTER_ST0);
	
	/* It jumps if less than because the comparison is backwards. */
	dac_append_jb_short(dac,
		dac_get_length_of_fld1() +
		dac_get_length_of_jmp_short()
	);
	
	dac_append_fld1(dac);
	dac_append_jmp_short(dac, dac_get_length_of_fldz());
	dac_append_fldz(dac);
	
	if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
		store_float_variable_on_stack(dac, variable);
	}
	
	m_free(operand_node);
end_mid_level_instruction_function

mid_level_instruction_function(cmp_le_uvar_uvar)
	Variable* variable = (Variable*)ARRAY_INDEX(instruction->arguments, 0);

	output_nullt_string("cmp_le uvar, uvar\n");

	if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
		store_float_variable_on_stack(dac, variable);
	}
end_mid_level_instruction_function

mid_level_instruction_function(cmp_lt_uvar_uvar)
	Variable* variable = (Variable*)ARRAY_INDEX(instruction->arguments, 0);

	output_nullt_string("cmp_lt uvar, uvar\n");

	if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
		store_float_variable_on_stack(dac, variable);
	}
end_mid_level_instruction_function

mid_level_instruction_function(cmp_eq_uvar_uvar)
	Variable* variable = (Variable*)ARRAY_INDEX(instruction->arguments, 0);

	output_nullt_string("cmp_eq uvar, uvar\n");

	if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
		store_float_variable_on_stack(dac, variable);
	}
end_mid_level_instruction_function

mid_level_instruction_function(cmp_ne_uvar_uvar)
	Variable* variable = (Variable*)ARRAY_INDEX(instruction->arguments, 0);
	
	output_nullt_string("cmp_ne uvar, uvar\n");
	
	if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
		store_float_variable_on_stack(dac, variable);
	}
end_mid_level_instruction_function

mid_level_instruction_function(cmp_gt_uvar_uvar)
	Variable* variable = (Variable*)ARRAY_INDEX(instruction->arguments, 0);
	
	output_nullt_string("cmp_gt uvar, uvar\n");
	
	if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
		store_float_variable_on_stack(dac, variable);
	}
end_mid_level_instruction_function

mid_level_instruction_function(cmp_ge_uvar_uvar)
	Variable* variable = (Variable*)ARRAY_INDEX(instruction->arguments, 0);
	
	output_nullt_string("cmp_ge uvar, uvar\n");
	
	if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
		store_float_variable_on_stack(dac, variable);
	}
end_mid_level_instruction_function

mid_level_instruction_function(cmp_le_fvar_fvar)
	Variable* variable = (Variable*)ARRAY_INDEX(instruction->arguments, 0);
	
	output_nullt_string("cmp_le fvar, fvar\n");
	
	if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
		store_float_variable_on_stack(dac, variable);
	}
end_mid_level_instruction_function

mid_level_instruction_function(cmp_lt_fvar_fvar)
	Variable* variable = (Variable*)ARRAY_INDEX(instruction->arguments, 0);
	
	output_nullt_string("cmp_lt fvar, fvar\n");
	
	if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
		store_float_variable_on_stack(dac, variable);
	}
end_mid_level_instruction_function

mid_level_instruction_function(cmp_eq_fvar_fvar)
	Variable* variable = (Variable*)ARRAY_INDEX(instruction->arguments, 0);
	
	output_nullt_string("cmp_eq fvar, fvar\n");
	
	if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
		store_float_variable_on_stack(dac, variable);
	}
end_mid_level_instruction_function

mid_level_instruction_function(cmp_ne_fvar_fvar)
	Variable* variable = (Variable*)ARRAY_INDEX(instruction->arguments, 0);
	
	output_nullt_string("cmp_ne fvar, fvar\n");
	
	if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
		store_float_variable_on_stack(dac, variable);
	}
end_mid_level_instruction_function

mid_level_instruction_function(cmp_gt_fvar_fvar)
	Variable* variable = (Variable*)ARRAY_INDEX(instruction->arguments, 0);
	
	output_nullt_string("cmp_gt fvar, fvar\n");
	
	if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
		store_float_variable_on_stack(dac, variable);
	}
end_mid_level_instruction_function

mid_level_instruction_function(cmp_ge_fvar_fvar)
	Variable* variable = (Variable*)ARRAY_INDEX(instruction->arguments, 0);
	
	output_nullt_string("cmp_ge fvar, fvar\n");
	
	if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
		store_float_variable_on_stack(dac, variable);
	}
end_mid_level_instruction_function

mid_level_instruction_function(change_sign)
	Variable* variable = (Variable*)ARRAY_INDEX(instruction->arguments, 0);
	
	dac_append_fchs(dac);
	
	if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
		store_float_variable_on_stack(dac, variable);
	}
end_mid_level_instruction_function

mid_level_instruction_function(test_and_jmp_if_zero_fvar_memaddr)
	Variable* variable;
	u relative_instruction_offset;
	u variable_stack_offset;

	variable = (Variable*)ARRAY_INDEX(instruction->arguments, 0);
	assert_comparison(variable, !=, NULL, Variable*, void*);

	relative_instruction_offset = (u)ARRAY_INDEX(instruction->arguments, 1);
	
	if (likely(relative_instruction_offset)) {
		variable_stack_offset = ARROW(variable, stack_offset);
		
		#if DEBUG
			output_nullt_string(TAB_STRING TAB_STRING "; if (!");
			output_variable(variable);
			output_nullt_string(") goto after_if;\n");
		#endif
		
		#if SIXTY_FOUR_BIT
			dac_append_mov_reg64_reg64(dac, REGISTER_R11, REGISTER_RSP);
			dac_append_mov_reg64_lit64(dac, REGISTER_R12, variable_stack_offset);
			dac_append_add_reg64_reg64(dac, REGISTER_R11, REGISTER_R12);
			dac_append_fld_deref_register_as_qword(dac, REGISTER_R11);
		#else
			dac_append_mov_reg32_reg32(dac, REGISTER_EDI, REGISTER_ESP);
			dac_append_mov_reg32_lit32(dac, REGISTER_ESI, variable_stack_offset);
			dac_append_add_reg32_reg32(dac, REGISTER_EDI, REGISTER_ESI);
			dac_append_fld_deref_register_as_qword(dac, REGISTER_EDI);
		#endif
		
		dac_append_fldz(dac);
		dac_append_fcomip(dac, REGISTER_ST1);
		dac_append_fstp_fpu_register(dac, REGISTER_ST0);
		dac_append_je(dac, relative_instruction_offset);
	}
	
	if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
		store_float_variable_on_stack(dac, variable);
	}
end_mid_level_instruction_function

mid_level_instruction_function(send_hello_world_message_to_ui)
	u variable_stack_offset;
	
	#if DEBUG
		output_nullt_string("\n" TAB_STRING TAB_STRING "; send message " OPENING_QUOTE "hello_world" CLOSING_QUOTE " (message code 0) to the ui");
		output_newline();
	#endif
	
	variable_stack_offset = (s)ARRAY_INDEX(instruction->arguments, 0);
	
	#if LINUX
		#if SIXTY_FOUR_BIT
			dac_append_mov_reg32_lit32(dac, REGISTER_EAX, 1 /* the system call number */);
			dac_append_mov_reg32_lit32(dac, REGISTER_EDI, 1);
		
			if (!variable_stack_offset) {
				dac_append_mov_deref_register_as_qword_lit32(dac, REGISTER_RSP, 0 /* the message number */);
			
				dac_append_mov_reg64_reg64(dac, REGISTER_RSI, REGISTER_RSP);
			} else if ((s)variable_stack_offset == (schar)variable_stack_offset) {
				dac_append_mov_deref_register_as_qword_plus_signed_byte_offset_lit32(
					dac,
					REGISTER_RSP,
					(schar)variable_stack_offset,
					0 /* the message number */
				);
			
				dac_append_mov_reg64_reg64(dac, REGISTER_RSI, REGISTER_RSP);
				dac_append_add_reg64_lit32(dac, REGISTER_RSI, (s)ARRAY_INDEX(instruction->arguments, 0));
			} else {
				dac_append_mov_reg64_reg64(dac, REGISTER_RSI, REGISTER_RSP);
				dac_append_mov_reg64_lit64(dac, REGISTER_R11, (s)ARRAY_INDEX(instruction->arguments, 0) /* stack offset */);
				dac_append_add_reg64_reg64(dac, REGISTER_RSI, REGISTER_R11);
				dac_append_mov_deref_register_as_qword_lit32(dac, REGISTER_RSI, 0 /* the message number */);
			}
		
			dac_append_mov_reg32_lit32(dac, REGISTER_EDX, 2);
			
			#if DEBUG
				output_newline();
			#endif
			
			dac_append_syscall(dac);
		#else
			dac_append_mov_reg32_lit32(dac, REGISTER_EAX, 4 /* the system call number */);
			dac_append_mov_reg32_lit32(dac, REGISTER_EBX, 1);
		
			if (!variable_stack_offset) {
				dac_append_mov_deref_register_as_dword_lit32(dac, REGISTER_ESP, 0 /* the message number */);
			
				dac_append_mov_reg32_reg32(dac, REGISTER_ECX, REGISTER_ESP);
			} else if ((s)variable_stack_offset == (schar)variable_stack_offset) {
				dac_append_mov_deref_register_as_dword_plus_signed_byte_offset_lit32(
					dac,
					REGISTER_ESP,
					(schar)variable_stack_offset,
					0 /* the message number */
				);
			
				dac_append_mov_reg32_reg32(dac, REGISTER_ECX, REGISTER_ESP);
				dac_append_add_reg32_lit32(dac, REGISTER_ECX, (s)ARRAY_INDEX(instruction->arguments, 0));
			} else {
				dac_append_mov_reg32_lit32(dac, REGISTER_EDI, 0 /* the message number */);
				dac_append_mov_reg32_reg32(dac, REGISTER_ECX, REGISTER_ESP);
				dac_append_mov_reg32_lit32(dac, REGISTER_ESI, (s)ARRAY_INDEX(instruction->arguments, 0) /* stack offset */);
				dac_append_add_reg32_reg32(dac, REGISTER_ECX, REGISTER_ESI);
				dac_append_mov_deref_register_as_dword_reg32(dac, REGISTER_ECX, REGISTER_EDI);
			}
			dac_append_mov_reg32_lit32(dac, REGISTER_EDX, 2);
			dac_append_int(dac, 128);
		#endif
	#endif
end_mid_level_instruction_function

mid_level_instruction_function(send_output_uint_message_to_ui)
	u variable_stack_offset = (u)ARRAY_INDEX(instruction->arguments, 0);
	Variable* variable = (Variable*)ARRAY_INDEX(instruction->arguments, 1);
	
	assert_comparison(variable, !=, NULL, Variable* cr, void*);
	
	#if SIXTY_FOUR_BIT
		#if DEBUG
			output_nullt_string(
				"\n"
				TAB_STRING TAB_STRING "; send message " OPENING_QUOTE "output_uint" CLOSING_QUOTE " (message code 2) to the ui\n\n"
				TAB_STRING TAB_STRING "; setup 3 of the 4 Linux system call registers\n"
			);
		#endif
	
		dac_append_mov_reg32_lit32(dac, REGISTER_EAX, 1);
		#if DEBUG
			rewind_output_pointer(1);
			output_nullt_string("    ; syscall #1 is write()\n");
		#endif
		dac_append_mov_reg64_lit64(dac, REGISTER_RDI, 1);
		#if DEBUG
			rewind_output_pointer(1);
			output_nullt_string("    ; 1 = STDOUT_FILENO\n");
		#endif
		dac_append_mov_reg64_lit64(dac, REGISTER_RDX, 10);
		#if DEBUG
			rewind_output_pointer(1);
			output_nullt_string("   ; 2 (length of opcode) + sizeof(f)\n");
		#endif
	
		#if DEBUG
			output_nullt_string("\n" TAB_STRING TAB_STRING "; rsi = rsp + ");
			putu(variable_stack_offset);
			output_nullt_string(" (the stack offset)\n");
		#endif
		dac_append_mov_reg64_reg64(dac, REGISTER_RSI, REGISTER_RSP);
		dac_append_mov_reg64_lit64(dac, REGISTER_R11, variable_stack_offset /* stack offset */);
		dac_append_add_reg64_reg64(dac, REGISTER_RSI, REGISTER_R11);
	
		#if DEBUG
			output_nullt_string("\n" TAB_STRING TAB_STRING "; *rsi = 1 (the UI message number)\n");
		#endif
		dac_append_mov_reg64_lit64(dac, REGISTER_R12, 1 /* the message number */);
		dac_append_mov_deref_register_as_qword_reg64(dac, REGISTER_RSI, REGISTER_R12);
		
		#if DEBUG
			output_nullt_string("\n" TAB_STRING TAB_STRING "; rsi += 2\n");
		#endif
		dac_append_add_reg64_lit32(dac, REGISTER_RSI, 2);
	
		#if DEBUG
			output_nullt_string("\n" TAB_STRING TAB_STRING "; *rsi = *((char*)");
			if (ARROW(variable, is_argument)) {
				output_nullt_string("rbp");
			} else {
				output_nullt_string("rsp");
			}
			output_nullt_string(" + variable_stack_offset)\n");
		#endif
		
		dac_append_mov_reg64_lit64(dac, REGISTER_R12, (s)ARROW(variable, stack_offset));
		#if DEBUG
			rewind_output_pointer(1);
			output_nullt_string("    ; the variable stack offset\n");
		#endif
	
		dac_append_add_reg64_reg64(dac, REGISTER_R12, REGISTER_RSP);
		dac_append_mov_reg64_deref_register_as_qword(dac, REGISTER_R12, REGISTER_R12);
		dac_append_mov_deref_register_as_qword_reg64(dac, REGISTER_RSI, REGISTER_R12);
	
		#if DEBUG
			output_nullt_string("\n" TAB_STRING TAB_STRING "; rsi -= 2\n");
		#endif
		dac_append_sub_reg64_lit32(dac, REGISTER_RSI, 2);
		
		#if DEBUG
			output_newline();
		#endif
		
		dac_append_syscall(dac);
	#endif
end_mid_level_instruction_function

mid_level_instruction_function(send_output_float_message_to_ui)
	u variable_stack_offset = (u)ARRAY_INDEX(instruction->arguments, 0);
	Variable* variable = (Variable*)ARRAY_INDEX(instruction->arguments, 1);
	
	assert_comparison(variable, !=, NULL, Variable* cr, void*);
	
	#if SIXTY_FOUR_BIT
		#if DEBUG
			output_nullt_string(
				"\n"
				TAB_STRING TAB_STRING "; send message " OPENING_QUOTE "output_float" CLOSING_QUOTE " (message code 1) to the ui\n\n"
				TAB_STRING TAB_STRING "; setup 3 of the 4 Linux system call registers\n"
			);
		#endif
		
		dac_append_mov_reg32_lit32(dac, REGISTER_EAX, 1);
		#if DEBUG
			rewind_output_pointer(1);
			output_nullt_string("    ; syscall #1 is write()\n");
		#endif
		dac_append_mov_reg64_lit64(dac, REGISTER_RDI, 1);
		#if DEBUG
			rewind_output_pointer(1);
			output_nullt_string("    ; 1 = STDOUT_FILENO\n");
		#endif
		dac_append_mov_reg64_lit64(dac, REGISTER_RDX, 10);
		#if DEBUG
			rewind_output_pointer(1);
			output_nullt_string("   ; 2 (length of opcode) + sizeof(f)\n");
		#endif
		
		#if DEBUG
			output_nullt_string("\n" TAB_STRING TAB_STRING "; rsi = rsp + ");
			putu(variable_stack_offset);
			output_nullt_string(" (the stack offset)\n");
		#endif
		dac_append_mov_reg64_reg64(dac, REGISTER_RSI, REGISTER_RSP);
		dac_append_mov_reg64_lit64(dac, REGISTER_R11, variable_stack_offset /* stack offset */);
		dac_append_add_reg64_reg64(dac, REGISTER_RSI, REGISTER_R11);
		
		#if DEBUG
			output_nullt_string("\n" TAB_STRING TAB_STRING "; *rsi = 2 (the UI message number)\n");
		#endif
		dac_append_mov_reg64_lit64(dac, REGISTER_R12, 2 /* the message number */);
		dac_append_mov_deref_register_as_qword_reg64(dac, REGISTER_RSI, REGISTER_R12);
		
		#if DEBUG
			output_nullt_string("\n" TAB_STRING TAB_STRING "; rsi += 2\n");
		#endif
		dac_append_add_reg64_lit32(dac, REGISTER_RSI, 2);
		
		#if DEBUG
			output_nullt_string("\n" TAB_STRING TAB_STRING "; *rsi = *((char*)");
			if (ARROW(variable, is_argument)) {
				output_nullt_string("rbp");
			} else {
				output_nullt_string("rsp");
			}
			output_nullt_string(" + variable_stack_offset)\n");
		#endif
		
		if (ARROW(variable, is_argument)) {
			dac_append_mov_reg64_lit64(dac, REGISTER_R12, (s)ARROW(variable, stack_offset));
			#if DEBUG
				rewind_output_pointer(1);
				output_nullt_string("    ; the variable stack offset\n");
			#endif
			
			dac_append_add_reg64_reg64(dac, REGISTER_R12, REGISTER_RBP);
			dac_append_mov_reg64_deref_register_as_qword(dac, REGISTER_R12, REGISTER_R12);
			dac_append_mov_deref_register_as_qword_reg64(dac, REGISTER_RSI, REGISTER_R12);
		} else {
			dac_append_mov_reg64_lit64(dac, REGISTER_R12, (s)ARROW(variable, stack_offset));
			#if DEBUG
				rewind_output_pointer(1);
				output_nullt_string("    ; the variable stack offset\n");
			#endif
			
			dac_append_add_reg64_reg64(dac, REGISTER_R12, REGISTER_RSP);
			dac_append_mov_reg64_deref_register_as_qword(dac, REGISTER_R12, REGISTER_R12);
			dac_append_mov_deref_register_as_qword_reg64(dac, REGISTER_RSI, REGISTER_R12);
		}
		
		#if DEBUG
			output_nullt_string("\n" TAB_STRING TAB_STRING "; rsi -= 2\n");
		#endif
		dac_append_sub_reg64_lit32(dac, REGISTER_RSI, 2);
		
		dac_append_syscall(dac);
	#else
		(void)variable_stack_offset;
	#endif
end_mid_level_instruction_function

attr_nonnull
static void decrement_mov_instructions_that_are_right_before_call_instructions(
	LinkedListHead* cr forward_references,
	u* cr number_of_nops_that_would_have_been_generated
) {
	LinkedListHead node;
	
	assert_comparison(forward_references, !=, NULL, LinkedListHead* cr, void*);
	
	node = DEREF(forward_references);
	
	while (node) {
		SubroutineForwardReference* subroutine_forward_reference = (SubroutineForwardReference*)&node->value;
		
		switch (ARROW(subroutine_forward_reference, number_of_bytes_in_encoding)) {
			case 5:
				#if DEBUG
					output_nullt_string(TAB_STRING TAB_STRING "; mov reg, ");
					putu_without_separators(DEREF((u32*)(ARROW(subroutine_forward_reference, address_of_mov_instruction_in_memory) + 1)));
				#endif
				
				DEREF_ASSIGN((u32*)(ARROW(subroutine_forward_reference, address_of_mov_instruction_in_memory) + 1)) -=
					DEREF(number_of_nops_that_would_have_been_generated);
				
				ARROW_ASSIGN(subroutine_forward_reference, address_of_the_subroutine_that_the_mov_instruction_is_putting_in_a_register) =
					DEREF((u32*)(ARROW(subroutine_forward_reference, address_of_mov_instruction_in_memory) + 1));
				
				#if DEBUG
					output_nullt_string(" -> mov reg, ");
					putu_without_separators(DEREF((u32*)(ARROW(subroutine_forward_reference, address_of_mov_instruction_in_memory) + 1)));
					output_newline();
				#endif
				
				break;
			case 6:
				#if DEBUG
					output_nullt_string(TAB_STRING TAB_STRING "; mov reg, ");
					putu_without_separators(DEREF((u32*)(ARROW(subroutine_forward_reference, address_of_mov_instruction_in_memory) + 2)));
				#endif
				
				DEREF_ASSIGN((u32*)(ARROW(subroutine_forward_reference, address_of_mov_instruction_in_memory) + 2)) -=
					DEREF(number_of_nops_that_would_have_been_generated);
				
				ARROW_ASSIGN(subroutine_forward_reference, address_of_the_subroutine_that_the_mov_instruction_is_putting_in_a_register) =
					DEREF((u32*)(ARROW(subroutine_forward_reference, address_of_mov_instruction_in_memory) + 2));
				
				#if DEBUG
					output_nullt_string(" -> mov reg, ");
					putu_without_separators(DEREF((u32*)(ARROW(subroutine_forward_reference, address_of_mov_instruction_in_memory) + 2)));
					output_newline();
				#endif
				
				break;
			case 10:
				#if DEBUG
					output_nullt_string(TAB_STRING TAB_STRING "; mov reg, ");
					putu_without_separators(DEREF((u*)(ARROW(subroutine_forward_reference, address_of_mov_instruction_in_memory) + 2)));
				#endif
				
				DEREF_ASSIGN((u*)(ARROW(subroutine_forward_reference, address_of_mov_instruction_in_memory) + 2)) -=
					DEREF(number_of_nops_that_would_have_been_generated);
				
				ARROW_ASSIGN(subroutine_forward_reference, address_of_the_subroutine_that_the_mov_instruction_is_putting_in_a_register) =
					DEREF((u*)(ARROW(subroutine_forward_reference, address_of_mov_instruction_in_memory) + 2));
				
				#if DEBUG
					output_nullt_string(" -> mov reg, ");
					putu_without_separators(DEREF((u*)(ARROW(subroutine_forward_reference, address_of_mov_instruction_in_memory) + 2)));
					output_newline();
				#endif
				
				break;
			
			#if DEBUG
				default:
					assert_comparison(0, !=, 0, u, u);
			#endif
		}
		
		node = ARROW(node, next_node);
	}
}

__attribute__((nonnull (1)))
static void delete_forward_references(DataAndCode* cr dac, LinkedListHead* cr forward_references) {
	LinkedListHead node;
	LinkedListHead previous_node;
	
	assert_comparison(dac, !=, NULL, DataAndCode* cr, void*);
	assert_comparison(forward_references, !=, NULL, LinkedListHead* cr, void*);
	
	node = DEREF(forward_references);
	previous_node = NULL;
	
	while (node) {
		LinkedListHead next_node;
		SubroutineForwardReference* subroutine_forward_reference;
		
		next_node = ARROW(node, next_node);
		subroutine_forward_reference = (SubroutineForwardReference*)&node->value;
		
		if (
			ARROW(subroutine_forward_reference, address_of_the_subroutine_that_the_mov_instruction_is_putting_in_a_register) ==
			(u)(ARROW(dac, start_of_code) + ARROW(dac, current_instruction_offset))
		) {
			if (previous_node) {
				ARROW_ASSIGN(previous_node, next_node) = next_node;
			} else {
				DEREF_ASSIGN(forward_references) = next_node;
			}
			
			m_free(node);
		} else {
			previous_node = node;
		}
		
		node = next_node;
	}
}

mid_level_instruction_function(add_main_subroutine_starting_instructions)
	const u stack_offset = (u)ARRAY_INDEX(instruction->arguments, 0);
	
	decrement_mov_instructions_that_are_right_before_call_instructions(
		forward_references,
		number_of_nops_that_would_have_been_generated
	);
	
	delete_forward_references(dac, forward_references);
	
	/*
	This function should only be called once, and it should be impossible for
	the user to make it be called more than once as that is handled in a earlier
	and more portable part of the program -- the parser. Hence, assert that
	main_subroutine_offset_set is false, because this function -- and only this
	function -- sets it to true at a later point in this function, which means that
	if main_subroutine_offset is true, this function was previously executed.
	*/
	assert_comparison(DEREF(main_subroutine_offset_set), ==, false, bool, bool);
	
	DEREF_ASSIGN(main_subroutine_offset) = ARROW(dac, current_instruction_offset);
	DEREF_ASSIGN(main_subroutine_offset_set) = true;
	
	#if DEBUG
		output_nullt_string(TAB_STRING "main:   ; ");
		putu((u)(ARROW(dac, start_of_code) + ARROW(dac, current_instruction_offset)));
		output_newline();
	#endif
	
	/* Don't add the main subroutine starting instructions if the next instructions adds the main subroutine ending instructions. */
	if (
		unlikely(
			DEREF(instruction_offset) + 1 < ARROW(mli, used_length) &&
			ARROW(ARRAY_INDEX(ARROW(mli, mid_level_instructions), DEREF(instruction_offset) + 1), opcode) ==
				MLI_OPCODE_ADD_MAIN_SUBROUTINE_ENDING_INSTRUCTIONS
		)
	) {
		#if DEBUG
			output_nullt_string(TAB_STRING TAB_STRING "; This generated program doesn't do anything, so just return.\n");
		#endif
		dac_append_ret(dac);
		DEREF_ASSIGN(instruction_offset)++;
		return;
	}
	
	#if DEBUG
		#if SIXTY_FOUR_BIT
			output_nullt_string(TAB_STRING TAB_STRING "; Save all the GPR's except rsp.\n");
		#else
			output_nullt_string(TAB_STRING TAB_STRING "; Save all the GPR's except esp.\n");
		#endif
	#endif
	
	dac_append_pusha_except_stack_pointer(dac);
	
	#if DEBUG
		output_newline();
	#endif
	
	if (stack_offset) {
		#if SIXTY_FOUR_BIT
			DEREF_ASSIGN(number_of_nops_that_would_have_been_generated) +=
				dac_get_length_of_mov_reg64_lit64_ten_byte_encoding() -
				dac_get_length_of_mov_reg64_lit64(REGISTER_RBP, stack_offset);
			
			dac_append_mov_reg64_lit64(dac, REGISTER_RBP, stack_offset);
			dac_append_sub_reg64_reg64(dac, REGISTER_RSP, REGISTER_RBP);
		#else
			dac_append_mov_reg32_lit32(dac, REGISTER_EBP, stack_offset);
			dac_append_sub_reg32_reg32(dac, REGISTER_ESP, REGISTER_EBP);
		#endif
	}
	
	#if DEBUG
		output_newline();
	#endif
end_mid_level_instruction_function

mid_level_instruction_function(add_main_subroutine_ending_instructions)
	const u stack_offset = (u)ARRAY_INDEX(instruction->arguments, 0);
	
	#if DEBUG
		output_newline();
	#endif
	
	if (stack_offset) {
		#if SIXTY_FOUR_BIT
			dac_append_mov_reg64_lit64(dac, REGISTER_RBP, stack_offset);
			dac_append_add_reg64_reg64(dac, REGISTER_RSP, REGISTER_RBP);
		#else
			dac_append_mov_reg32_lit32(dac, REGISTER_EBP, stack_offset);
			dac_append_add_reg32_reg32(dac, REGISTER_ESP, REGISTER_EBP);
		#endif
	}
	
	#if DEBUG
		#if SIXTY_FOUR_BIT
			output_nullt_string("\n" TAB_STRING TAB_STRING "; Restore all the GPR's except rsp, which was never saved.\n");
		#else
			output_nullt_string("\n" TAB_STRING TAB_STRING "; Restore all the GPR's except esp, which was never saved.\n");
		#endif
	#endif
	
	dac_append_popa_except_stack_pointer(dac);
	
	#if DEBUG
		output_newline();
	#endif
	
	dac_append_ret(dac);
end_mid_level_instruction_function

mid_level_instruction_function(add_subroutine_starting_instructions)
	const u stack_offset = (u)ARRAY_INDEX(instruction->arguments, 0);
	
	decrement_mov_instructions_that_are_right_before_call_instructions(
		forward_references,
		number_of_nops_that_would_have_been_generated
	);
	
	delete_forward_references(dac, forward_references);
	
	#if DEBUG
		output_nullt_string("\n" TAB_STRING "some_subroutine:  ; ");
		putu((u)(ARROW(dac, start_of_code) + ARROW(dac, current_instruction_offset)));
		output_newline();
	#endif
	
	#if SIXTY_FOUR_BIT
		DEREF_ASSIGN(number_of_nops_that_would_have_been_generated) +=
			dac_get_length_of_mov_reg64_lit64_ten_byte_encoding() -
			dac_get_length_of_mov_reg64_lit64(REGISTER_RBP, stack_offset);
		
		dac_append_push(dac, REGISTER_RBP);
		dac_append_mov_reg64_lit64(dac, REGISTER_RBX, stack_offset);
		dac_append_mov_reg64_reg64(dac, REGISTER_RBP, REGISTER_RSP);
		dac_append_sub_reg64_reg64(dac, REGISTER_RSP, REGISTER_RBX);
	#else
		#error "FIXME"
	#endif
end_mid_level_instruction_function

mid_level_instruction_function(add_subroutine_ending_instructions)
	const u number_of_arguments = (u)ARRAY_INDEX(instruction->arguments, 0);
	
	#if DEBUG
		#if SIXTY_FOUR_BIT
			output_nullt_string("\n" TAB_STRING TAB_STRING "; put the return address in rax...\n");
		#else
			output_nullt_string("\n" TAB_STRING TAB_STRING "; put the return address in eax...\n");
		#endif
	#endif
	
	#if SIXTY_FOUR_BIT
		dac_append_mov_reg64_reg64(dac, REGISTER_RBX, REGISTER_RBP);
		dac_append_add_reg64_lit32(dac, REGISTER_RBX, 8);
		dac_append_mov_reg64_deref_register_as_qword(dac, REGISTER_RAX, REGISTER_RBX);
	#else
		#error "FIXME"
	#endif
	
	#if DEBUG
		output_nullt_string(
			"\n"
			TAB_STRING TAB_STRING "; increment the stack pointer so that it is right above\n"
			TAB_STRING TAB_STRING "; the parameters that were passed to this function...\n"
		);
	#endif
	
	#if SIXTY_FOUR_BIT
		dac_append_mov_reg64_reg64(dac, REGISTER_RSP, REGISTER_RBP);
		dac_append_pop(dac, REGISTER_RBP);
		dac_append_mov_reg64_lit64(dac, REGISTER_RBX, (number_of_arguments + 1) * 8);
		dac_append_add_reg64_reg64(dac, REGISTER_RSP, REGISTER_RBX);
	#else
		#error "FIXME"
	#endif
	
	#if DEBUG
		output_nullt_string("\n" TAB_STRING TAB_STRING "; and jump to the return address\n");
	#endif
	
	#if SIXTY_FOUR_BIT
		dac_append_jmp_reg(dac, REGISTER_RAX);
	#else
		dac_append_jmp_reg(dac, REGISTER_EAX);
	#endif
end_mid_level_instruction_function

mid_level_instruction_function(push_argument)
	Variable* variable;
	
	#if DEBUG
		output_nullt_string(TAB_STRING TAB_STRING "; push argument\n");
	#endif
	
	variable = (Variable*)ARRAY_INDEX(instruction->arguments, 0);
	
	#if SIXTY_FOUR_BIT
		dac_append_mov_reg64_lit64(dac, REGISTER_R11, ARROW(variable, stack_offset));
		dac_append_add_reg64_reg64(dac, REGISTER_R11, REGISTER_RSP);
		dac_append_mov_reg64_deref_register_as_qword(dac, REGISTER_RAX, REGISTER_R11);
		dac_append_push(dac, REGISTER_RAX);
	#else
		dac_append_mov_reg32_lit32(dac, REGISTER_EDI, ARROW(variable, stack_offset));
		dac_append_add_reg32_reg32(dac, REGISTER_EDI, REGISTER_ESP);
		dac_append_mov_reg32_deref_register_as_dword(dac, REGISTER_EAX, REGISTER_EDI);
		dac_append_push(dac, REGISTER_EAX);
	#endif
	
	#if DEBUG
		output_newline();
	#endif
	
	m_free(ARRAY_INDEX(instruction->arguments, 0));
end_mid_level_instruction_function

mid_level_instruction_function(call_subroutine)
	#if SIXTY_FOUR_BIT
		const u subroutine_address = (u)ARROW(dac, start_of_code) + (u)ARRAY_INDEX(instruction->arguments, 0);
		
		/* if the function is ahead, i.e., if the function is not generated yet */
		if (
			ARROW(dac, current_instruction_offset) - DEREF(number_of_nops_that_would_have_been_generated)
			<
			(u)ARRAY_INDEX(instruction->arguments, 0)
		) {
			SubroutineForwardReference subroutine_forward_reference;
			
			subroutine_forward_reference.address_of_mov_instruction_in_memory =
				ARROW(dac, start_of_code) + ARROW(dac, current_instruction_offset);
			
			subroutine_forward_reference.number_of_bytes_in_encoding = dac_get_length_of_mov_reg64_lit64(REGISTER_RDI, subroutine_address);
			
			linked_list_append_n_bytes(forward_references, &subroutine_forward_reference, sizeof(SubroutineForwardReference));
		}
		
		dac_append_mov_reg64_lit64(dac, REGISTER_RDI, subroutine_address);
		dac_append_call_reg64(dac, REGISTER_RDI);
		
		DEREF_ASSIGN(number_of_nops_that_would_have_been_generated) +=
			dac_get_length_of_mov_reg64_lit64_ten_byte_encoding() -
			dac_get_length_of_mov_reg64_lit64(REGISTER_RDI, subroutine_address);
	#else
		dac_append_mov_reg32_lit32(dac, REGISTER_EDI, (u)ARROW(dac, start_of_code) + (u)ARRAY_INDEX(instruction->arguments, 0));
		dac_append_call_reg32(dac, REGISTER_EDI);
	#endif
end_mid_level_instruction_function

static void (*mid_level_instruction_function_array[])(
	DataAndCode* cr,
	MidLevelInstruction* cr,
	MidLevelInstructions* cr,
	u* cr,
	u* cr,
	bool* cr,
	LinkedListHead* cr,
	u* cr
) = {
	mov_uvar_uval,
	mov_fvar_fval,
	mov_fvar_fvar,
	mul_uvar_uval,
	mul_fvar_fval,
	div_uvar_uval,
	div_fvar_fval,
	add_uvar_uval,
	add_fvar_fval,
	add_uvar_uvar,
	add_fvar_fvar,
	sub_uvar_uval,
	sub_fvar_fval,
	sub_uvar_uvar,
	sub_fvar_fvar,
	
	cmp_le_uvar_uval,
	cmp_lt_uvar_uval,
	cmp_eq_uvar_uval,
	cmp_ne_uvar_uval,
	cmp_gt_uvar_uval,
	cmp_ge_uvar_uval,
	
	cmp_le_fvar_fval,
	cmp_lt_fvar_fval,
	cmp_eq_fvar_fval,
	cmp_ne_fvar_fval,
	cmp_gt_fvar_fval,
	cmp_ge_fvar_fval,
	
	cmp_le_uvar_uvar,
	cmp_lt_uvar_uvar,
	cmp_eq_uvar_uvar,
	cmp_ne_uvar_uvar,
	cmp_gt_uvar_uvar,
	cmp_ge_uvar_uvar,
	
	cmp_le_fvar_fvar,
	cmp_lt_fvar_fvar,
	cmp_eq_fvar_fvar,
	cmp_ne_fvar_fvar,
	cmp_gt_fvar_fvar,
	cmp_ge_fvar_fvar,
	
	change_sign,
	test_and_jmp_if_zero_fvar_memaddr,
	
	send_hello_world_message_to_ui,
	send_output_uint_message_to_ui,
	send_output_float_message_to_ui,
	
	add_main_subroutine_starting_instructions,
	add_main_subroutine_ending_instructions,
	
	add_subroutine_starting_instructions,
	add_subroutine_ending_instructions,
	
	push_argument,
	call_subroutine
};

attrs_nonnull_and_warn_unused_result
u /* the main subroutine offset */
put_instructions_into_code_section(DataAndCode* cr dac, MidLevelInstructions* cr mli) {
	LinkedListHead forward_references = NULL;
	u number_of_nops_that_would_have_been_generated = 0;
	u main_subroutine_offset;
	bool main_subroutine_offset_set = false;
	u i;
	
	#if DEBUG
		output_nullt_string("\ngenerated_code:\n");
	#endif
	
	for (i = 0; i < ARROW(mli, used_length); ++i) {
		MidLevelInstruction* const instruction = ARRAY_INDEX(ARROW(mli, mid_level_instructions), i);
		
		ARRAY_INDEX(mid_level_instruction_function_array, ARROW(instruction, opcode))(
			dac,
			instruction,
			mli,
			&i,
			&main_subroutine_offset,
			&main_subroutine_offset_set,
			&forward_references,
			&number_of_nops_that_would_have_been_generated
		);
	}
	
	if (unlikely(!main_subroutine_offset_set)) {
		output_nullt_string("Subroutine error: no main subroutine defined\n");
		exit_program();
	}
	
	#if DEBUG
		output_nullt_string("Main subroutine offset: ");
		putu(main_subroutine_offset);
		output_newline();
	#endif
	
	return main_subroutine_offset;
}
