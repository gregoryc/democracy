/*
A function to put instructions into the code section.

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

static void store_float_variable_on_stack(DataAndCode* cr dac, Variable* cr variable) {
	u variable_stack_offset;
	
	assert_comparison(dac, !=, NULL, Variable*, void*);
	assert_comparison(variable, !=, NULL, Variable*, void*);
	
	variable_stack_offset = ARROW(variable, stack_offset);
	
	#if DEBUG
		output_nullt_string(TAB_STRING TAB_STRING "; store st0 on the stack\n");
	#endif
	
	#if SIXTY_FOUR_BIT
		if (variable_stack_offset) {
			dac_append_mov_reg64_lit64(dac, REGISTER_R11, variable_stack_offset);
			dac_append_add_reg64_reg64(dac, REGISTER_R11, REGISTER_RSP);
			dac_append_fstp_deref_register_as_qword(dac, REGISTER_R11);
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

#define mid_level_instruction_function(name)  static void name(DataAndCode* cr dac, MidLevelInstruction* cr instruction)

mid_level_instruction_function(mov_uvar_uval) {
	Variable* variable;
	OperandNode* operand_node;
	
	variable = (Variable*)ARROW(instruction, argument1);
	operand_node = (OperandNode*)ARROW(instruction, argument2);

	#if SIXTY_FOUR_BIT
		dac_append_mov_reg64_lit64(dac, REGISTER_RAX, ARROW(operand_node, value).uvalue);
	#else
		dac_append_mov_reg32_lit32(dac, REGISTER_EAX, ARROW(operand_node, value).uvalue);
	#endif
	
	if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
		store_u_variable_on_stack(dac, variable);
	}
}

mid_level_instruction_function(mov_fvar_fval) {
	Variable* variable = (Variable*)ARROW(instruction, argument1);
	OperandNode* operand_node = (OperandNode*)ARROW(instruction, argument2);
	u variable_stack_offset;
	
	(void)variable;
	
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
		#if SIXTY_FOUR_BIT
			dac_append_mov_reg64_lit64(dac, REGISTER_R11, DEREF((u*)ARROW(operand_node, address_in_memory)));
			dac_append_fld_deref_register_as_qword(dac, REGISTER_R11);
		#else
			dac_append_mov_reg32_lit32(dac, REGISTER_EDI, DEREF((u*)ARROW(operand_node, address_in_memory)));
			dac_append_fld_deref_register_as_qword(dac, REGISTER_EDI);
		#endif
	}
	
	if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
		store_float_variable_on_stack(dac, variable);
	}
}

mid_level_instruction_function(mov_fvar_fvar) {
	Variable* variable1 = (Variable*)ARROW(instruction, argument1);
	Variable* variable2 = (Variable*)ARROW(instruction, argument2);
	u variable_stack_offset;
	
	#if DEBUG
		output_nullt_string(TAB_STRING TAB_STRING "; ");
		output_variable(variable1);
		output_nullt_string(" = ");
		output_variable(variable2);
		output_newline();
	#endif
	
	variable_stack_offset = ARROW(variable2, stack_offset);
	
	#if SIXTY_FOUR_BIT
		if (variable_stack_offset) {
			dac_append_mov_reg64_reg64(dac, REGISTER_R11, REGISTER_RSP);
			dac_append_mov_reg64_lit64(dac, REGISTER_R12, variable_stack_offset);
			dac_append_add_reg64_reg64(dac, REGISTER_R11, REGISTER_R12);
		}
		dac_append_fld_deref_register_as_qword(dac, REGISTER_R11);
	#else
		if (variable_stack_offset) {
			dac_append_mov_reg32_reg32(dac, REGISTER_EDI, REGISTER_ESP);
			dac_append_mov_reg32_lit32(dac, REGISTER_ESI, variable_stack_offset);
			dac_append_add_reg32_reg32(dac, REGISTER_EDI, REGISTER_ESI);
		}
		dac_append_fld_deref_register_as_qword(dac, REGISTER_EDI);
	#endif
	
	if (instruction == ARROW(variable1, last_instruction_that_uses_this_variable)) {
		store_float_variable_on_stack(dac, variable1);
	}
}

mid_level_instruction_function(mul_uvar_uval) {
	(void)dac;
	(void)instruction;
	
	assert_comparison(0, !=, 0, u, u);
}

mid_level_instruction_function(mul_fvar_fval) {
	Variable* variable = (Variable*)ARROW(instruction, argument1);
	OperandNode* operand_node = (OperandNode*)ARROW(instruction, argument2);
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
}

mid_level_instruction_function(div_uvar_uval) {
	(void)dac;
	(void)instruction;
	
	assert_comparison(0, !=, 0, u, u);
}

mid_level_instruction_function(div_fvar_fval) {
	(void)dac;
	(void)instruction;
	
	assert_comparison(0, !=, 0, u, u);
}

mid_level_instruction_function(add_uvar_uval) {
	OperandNode* operand_node;
	Variable* variable;
	
	variable = (Variable*)ARROW(instruction, argument1);
	operand_node = (OperandNode*)ARROW(instruction, argument2);
	
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
}

mid_level_instruction_function(add_fvar_fval) {
	Variable* variable = (Variable*)ARROW(instruction, argument1);
	OperandNode* operand_node = (OperandNode*)ARROW(instruction, argument2);
	u variable_stack_offset;
	
	#if DEBUG
		output_nullt_string(TAB_STRING TAB_STRING "; ");
		output_variable(variable);
		output_nullt_string(" += ");
		putf(ARROW(operand_node, value).fvalue);
		output_newline();
	#endif
	
	variable_stack_offset = ARROW(variable, stack_offset);
	
	if (ARROW(operand_node, value).fvalue == 1.0) {
		dac_append_fld1(dac);
		dac_append_faddp(dac);
	} else {
		#if SIXTY_FOUR_BIT
			dac_append_mov_reg64_lit64(dac, REGISTER_R11, DEREF((u*)ARROW(operand_node, address_in_memory)));
			dac_append_fadd_deref_register_as_qword(dac, REGISTER_R11);
		#else
			dac_append_mov_reg32_lit32(dac, REGISTER_EDI, DEREF((u*)ARROW(operand_node, address_in_memory)));
			dac_append_fadd_deref_register_as_qword(dac, REGISTER_EDI);
		#endif
	}
	
	if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
		store_float_variable_on_stack(dac, variable);
	}
}

mid_level_instruction_function(add_uvar_uvar) {
	(void)dac;
	(void)instruction;
	
	assert_comparison(0, !=, 0, u, u);
}

mid_level_instruction_function(add_fvar_fvar) {
	Variable* variable1 = (Variable*)ARROW(instruction, argument1);
	Variable* variable2 = (Variable*)ARROW(instruction, argument2);
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
}

mid_level_instruction_function(sub_uvar_uval) {
	(void)dac;
	(void)instruction;
	
	assert_comparison(0, !=, 0, u, u);
}

mid_level_instruction_function(sub_fvar_fval) {
	Variable* variable = (Variable*)ARROW(instruction, argument1);
	OperandNode* operand_node = (OperandNode*)ARROW(instruction, argument2);
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
}

mid_level_instruction_function(sub_uvar_uvar) {
	(void)dac;
	(void)instruction;
	
	assert_comparison(0, !=, 0, u, u);
}

mid_level_instruction_function(sub_fvar_fvar) {
	Variable* variable1 = (Variable*)ARROW(instruction, argument1);
	Variable* variable2 = (Variable*)ARROW(instruction, argument2);
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
}

mid_level_instruction_function(cmp_le_uvar_uval) {
	Variable* variable = (Variable*)ARROW(instruction, argument1);
	
	output_nullt_string("cmp_le uvar, uval\n");
	
	if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
		store_float_variable_on_stack(dac, variable);
	}
}

mid_level_instruction_function(cmp_lt_uvar_uval) {
	Variable* variable = (Variable*)ARROW(instruction, argument1);
	
	output_nullt_string("cmp_lt uvar, uval\n");
	
	if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
		store_float_variable_on_stack(dac, variable);
	}
}

mid_level_instruction_function(cmp_eq_uvar_uval) {
	Variable* variable = (Variable*)ARROW(instruction, argument1);
	
	output_nullt_string("cmp_eq uvar, uval\n");
	
	if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
		store_float_variable_on_stack(dac, variable);
	}
}

mid_level_instruction_function(cmp_ne_uvar_uval) {
	Variable* variable = (Variable*)ARROW(instruction, argument1);
	
	output_nullt_string("cmp_ne uvar, uval\n");
	
	if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
		store_float_variable_on_stack(dac, variable);
	}
}

mid_level_instruction_function(cmp_gt_uvar_uval) {
	Variable* variable = (Variable*)ARROW(instruction, argument1);

	output_nullt_string("cmp_gt uvar, uval\n");

	if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
		store_float_variable_on_stack(dac, variable);
	}
}

mid_level_instruction_function(cmp_ge_uvar_uval) {
	Variable* variable = (Variable*)ARROW(instruction, argument1);

	output_nullt_string("cmp_ge uvar, uval\n");

	if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
		store_float_variable_on_stack(dac, variable);
	}
}

mid_level_instruction_function(cmp_le_fvar_fval) {
	Variable* variable = (Variable*)ARROW(instruction, argument1);

	output_nullt_string("cmp_le fvar, fval\n");

	if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
		store_float_variable_on_stack(dac, variable);
	}
}

mid_level_instruction_function(cmp_lt_fvar_fval) {
	Variable* variable = (Variable*)ARROW(instruction, argument1);

	output_nullt_string("cmp_lt fvar, fval\n");

	if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
		store_float_variable_on_stack(dac, variable);
	}
}

mid_level_instruction_function(cmp_eq_fvar_fval) {
	Variable* variable = (Variable*)ARROW(instruction, argument1);

	output_nullt_string("cmp_eq fvar, fval\n");

	if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
		store_float_variable_on_stack(dac, variable);
	}
}

mid_level_instruction_function(cmp_ne_fvar_fval) {
	Variable* variable = (Variable*)ARROW(instruction, argument1);
	
	output_nullt_string("cmp_ne fvar, fval\n");
	
	if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
		store_float_variable_on_stack(dac, variable);
	}
}

mid_level_instruction_function(cmp_gt_fvar_fval) {
	Variable* variable = (Variable*)ARROW(instruction, argument1);
	
	output_nullt_string("cmp_gt fvar, fval\n");
	
	if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
		store_float_variable_on_stack(dac, variable);
	}
}

mid_level_instruction_function(cmp_ge_fvar_fval) {
	Variable* variable = (Variable*)ARROW(instruction, argument1);
	OperandNode* operand_node = (OperandNode*)ARROW(instruction, argument2);
	
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
	Compare st1 (operand 1) with st0 (operand 2).
	The comparison is backwards, so  3.0 >= 2.0  will become  2.0 >= 3.0
	*/
	dac_append_fcomip(dac, REGISTER_ST1);
	dac_append_ja_short(dac, dac_get_length_of_fld1() + dac_get_length_of_jmp_short());
	dac_append_fld1(dac);
	dac_append_jmp_short(dac, dac_get_length_of_fldz());
	dac_append_fldz(dac);
	dac_append_fxch(dac, REGISTER_ST1);
	dac_append_ffree(dac, REGISTER_ST0);
	dac_append_fincstp(dac);
	
	if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
		store_float_variable_on_stack(dac, variable);
	}
}

mid_level_instruction_function(cmp_le_uvar_uvar) {
	Variable* variable = (Variable*)ARROW(instruction, argument1);

	output_nullt_string("cmp_le uvar, uvar\n");

	if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
		store_float_variable_on_stack(dac, variable);
	}
}

mid_level_instruction_function(cmp_lt_uvar_uvar) {
	Variable* variable = (Variable*)ARROW(instruction, argument1);

	output_nullt_string("cmp_lt uvar, uvar\n");

	if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
		store_float_variable_on_stack(dac, variable);
	}
}

mid_level_instruction_function(cmp_eq_uvar_uvar) {
	Variable* variable = (Variable*)ARROW(instruction, argument1);

	output_nullt_string("cmp_eq uvar, uvar\n");

	if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
		store_float_variable_on_stack(dac, variable);
	}
}

mid_level_instruction_function(cmp_ne_uvar_uvar) {
	Variable* variable = (Variable*)ARROW(instruction, argument1);
	
	output_nullt_string("cmp_ne uvar, uvar\n");
	
	if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
		store_float_variable_on_stack(dac, variable);
	}
}

mid_level_instruction_function(cmp_gt_uvar_uvar) {
	Variable* variable = (Variable*)ARROW(instruction, argument1);
	
	output_nullt_string("cmp_gt uvar, uvar\n");
	
	if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
		store_float_variable_on_stack(dac, variable);
	}
}

mid_level_instruction_function(cmp_ge_uvar_uvar) {
	Variable* variable = (Variable*)ARROW(instruction, argument1);
	
	output_nullt_string("cmp_ge uvar, uvar\n");
	
	if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
		store_float_variable_on_stack(dac, variable);
	}
}

mid_level_instruction_function(cmp_le_fvar_fvar) {
	Variable* variable = (Variable*)ARROW(instruction, argument1);
	
	output_nullt_string("cmp_le fvar, fvar\n");
	
	if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
		store_float_variable_on_stack(dac, variable);
	}
}

mid_level_instruction_function(cmp_lt_fvar_fvar) {
	Variable* variable = (Variable*)ARROW(instruction, argument1);
	
	output_nullt_string("cmp_lt fvar, fvar\n");
	
	if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
		store_float_variable_on_stack(dac, variable);
	}
}

mid_level_instruction_function(cmp_eq_fvar_fvar) {
	Variable* variable = (Variable*)ARROW(instruction, argument1);
	
	output_nullt_string("cmp_eq fvar, fvar\n");
	
	if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
		store_float_variable_on_stack(dac, variable);
	}
}

mid_level_instruction_function(cmp_ne_fvar_fvar) {
	Variable* variable = (Variable*)ARROW(instruction, argument1);
	
	output_nullt_string("cmp_ne fvar, fvar\n");
	
	if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
		store_float_variable_on_stack(dac, variable);
	}
}

mid_level_instruction_function(cmp_gt_fvar_fvar) {
	Variable* variable = (Variable*)ARROW(instruction, argument1);
	
	output_nullt_string("cmp_gt fvar, fvar\n");
	
	if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
		store_float_variable_on_stack(dac, variable);
	}
}

mid_level_instruction_function(cmp_ge_fvar_fvar) {
	Variable* variable = (Variable*)ARROW(instruction, argument1);
	
	output_nullt_string("cmp_ge fvar, fvar\n");
	
	if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
		store_float_variable_on_stack(dac, variable);
	}
}

mid_level_instruction_function(change_sign) {
	Variable* variable = (Variable*)ARROW(instruction, argument1);
	
	dac_append_fchs(dac);
	
	if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
		store_float_variable_on_stack(dac, variable);
	}
}

mid_level_instruction_function(test_and_jmp_if_zero_fvar_memaddr) {
	Variable* variable;
	u relative_instruction_offset;
	u variable_stack_offset;

	variable = (Variable*)ARROW(instruction, argument1);
	assert_comparison(variable, !=, NULL, Variable*, void*);

	relative_instruction_offset = (u)ARROW(instruction, argument2);
	assert_comparison(relative_instruction_offset, !=, 0, u, u);

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

	if (relative_instruction_offset <= 127) {
		dac_append_je_short(dac, relative_instruction_offset);
	} else {
		output_nullt_string("FIXME: Non-short jump.\n");
		exit_program();
	}

	if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
		store_float_variable_on_stack(dac, variable);
	}
}

mid_level_instruction_function(send_hello_world_message_to_ui) {
	u variable_stack_offset;
	
	#if DEBUG
		output_nullt_string(TAB_STRING TAB_STRING "; send message " OPENING_QUOTE "hello_world" CLOSING_QUOTE " (message code 0) to the ui");
		output_newline();
	#endif
	
	variable_stack_offset = (s)ARROW(instruction, argument1);
	
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
				dac_append_add_reg64_lit32(dac, REGISTER_RSI, (s)ARROW(instruction, argument1));
			} else {
				dac_append_mov_reg64_reg64(dac, REGISTER_RSI, REGISTER_RSP);
				dac_append_mov_reg64_lit64(dac, REGISTER_R11, (s)ARROW(instruction, argument1) /* stack offset */);
				dac_append_add_reg64_reg64(dac, REGISTER_RSI, REGISTER_R11);
				dac_append_mov_deref_register_as_qword_lit32(dac, REGISTER_RSI, 0 /* the message number */);
			}
		
			dac_append_mov_reg32_lit32(dac, REGISTER_EDX, 2);
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
				dac_append_add_reg32_lit32(dac, REGISTER_ECX, (s)ARROW(instruction, argument1));
			} else {
				dac_append_mov_reg32_lit32(dac, REGISTER_EDI, 0 /* the message number */);
				dac_append_mov_reg32_reg32(dac, REGISTER_ECX, REGISTER_ESP);
				dac_append_mov_reg32_lit32(dac, REGISTER_ESI, (s)ARROW(instruction, argument1) /* stack offset */);
				dac_append_add_reg32_reg32(dac, REGISTER_ECX, REGISTER_ESI);
				dac_append_mov_deref_register_as_dword_reg32(dac, REGISTER_ECX, REGISTER_EDI);
			}
			dac_append_mov_reg32_lit32(dac, REGISTER_EDX, 2);
			dac_append_int(dac, 128);
		#endif
	#else
		(void)dac;
	#endif
}

mid_level_instruction_function(send_output_uint_message_to_ui) {
	u variable_stack_offset = (u)ARROW(instruction, argument1);
	Variable* variable = (Variable*)ARROW(instruction, argument2);
	
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
			output_nullt_string("\n" TAB_STRING TAB_STRING "; *rsi = *((char*)rsp + variable_stack_offset)\n");
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
	
		dac_append_syscall(dac);
	
		#if DEBUG
			output_newline();
		#endif
	#else
		(void)variable_stack_offset;
		(void)dac;
	#endif
}

mid_level_instruction_function(send_output_float_message_to_ui) {
	u variable_stack_offset = (u)ARROW(instruction, argument1);
	Variable* variable = (Variable*)ARROW(instruction, argument2);
	
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
			output_nullt_string("\n" TAB_STRING TAB_STRING "; *rsi = *((char*)rsp + variable_stack_offset)\n");
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
	
		dac_append_syscall(dac);
	
		#if DEBUG
			output_newline();
		#endif
	#else
		(void)variable_stack_offset;
		(void)dac;
	#endif
}

mid_level_instruction_function(add_main_subroutine_starting_instructions) {
	const u stack_offset = (u)ARROW(instruction, argument1);

	#if DEBUG
		output_nullt_string(TAB_STRING "main:\n");
	
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

	#if SIXTY_FOUR_BIT
		dac_append_mov_reg64_lit64_ten_byte_encoding(dac, REGISTER_RBP, stack_offset);
		dac_append_sub_reg64_reg64(dac, REGISTER_RSP, REGISTER_RBP);
	#else
		dac_append_mov_reg32_lit32(dac, REGISTER_EBP, stack_offset);
		dac_append_sub_reg32_reg32(dac, REGISTER_ESP, REGISTER_EBP);
	#endif

	#if DEBUG
		output_newline();
	#endif
}

mid_level_instruction_function(add_main_subroutine_ending_instructions) {
	const u stack_offset = (u)ARROW(instruction, argument1);
	
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
}

mid_level_instruction_function(add_subroutine_starting_instructions) {
	const u stack_offset = (u)ARROW(instruction, argument1);
	
	#if DEBUG
		output_nullt_string("\n" TAB_STRING "some_subroutine:\n");
	#endif
	
	if (likely(stack_offset)) {
		#if SIXTY_FOUR_BIT
			dac_append_mov_reg64_lit64_ten_byte_encoding(dac, REGISTER_RBP, stack_offset);
			dac_append_sub_reg64_reg64(dac, REGISTER_RSP, REGISTER_RBP);
		#else
			dac_append_mov_reg32_lit32(dac, REGISTER_EBP, stack_offset);
			dac_append_sub_reg32_reg32(dac, REGISTER_ESP, REGISTER_EBP);
		#endif
	}
}

mid_level_instruction_function(add_subroutine_ending_instructions) {
	const u stack_offset = (u)ARROW(instruction, argument1);
	const u number_of_arguments = (u)ARROW(instruction, argument2);
	
	#if DEBUG
		#if SIXTY_FOUR_BIT
			output_nullt_string("\n" TAB_STRING TAB_STRING "; put the return address in rax...\n");
		#else
			output_nullt_string("\n" TAB_STRING TAB_STRING "; put the return address in eax...\n");
		#endif
	#endif
	
	#if SIXTY_FOUR_BIT
		dac_append_mov_reg64_lit64(dac, REGISTER_RBP, stack_offset);
		dac_append_add_reg64_reg64(dac, REGISTER_RBP, REGISTER_RSP);
		dac_append_mov_reg64_deref_register_as_qword(dac, REGISTER_RAX, REGISTER_RBP);
	#else
		dac_append_mov_reg32_lit32(dac, REGISTER_EBP, stack_offset);
		dac_append_add_reg32_reg32(dac, REGISTER_EBP, REGISTER_ESP);
		dac_append_mov_reg32_deref_register_as_dword(dac, REGISTER_EAX, REGISTER_EBP);
	#endif
	
	#if DEBUG
		output_nullt_string(
			"\n"
			TAB_STRING TAB_STRING "; increment the stack pointer so that it's right above\n"
			TAB_STRING TAB_STRING "; the parameters that were passed to this function...\n"
		);
	#endif
	
	#if SIXTY_FOUR_BIT
		dac_append_mov_reg64_lit64(dac, REGISTER_RBP, stack_offset + 8 /* for return address */ + number_of_arguments * 8);
		dac_append_add_reg64_reg64(dac, REGISTER_RSP, REGISTER_RBP);
	#else
		dac_append_mov_reg32_lit32(dac, REGISTER_EBP, stack_offset + 8 /* for return address */ + number_of_arguments * 8);
		dac_append_add_reg32_reg32(dac, REGISTER_ESP, REGISTER_EBP);
	#endif
	
	#if DEBUG
		output_nullt_string("\n" TAB_STRING TAB_STRING "; and jump to the return address\n");
	#endif
	
	#if SIXTY_FOUR_BIT
		dac_append_jmp_reg(dac, REGISTER_RAX);
	#else
		dac_append_jmp_reg(dac, REGISTER_EAX);
	#endif
}

mid_level_instruction_function(push_argument) {
	Variable* variable;
	
	#if DEBUG
		output_nullt_string(TAB_STRING TAB_STRING "; push argument\n");
	#endif
	
	variable = (Variable*)ARROW(instruction, argument1);
	
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
	
	m_free(ARROW(instruction, argument1));
}

mid_level_instruction_function(call_subroutine) {
	#if SIXTY_FOUR_BIT
		dac_append_mov_reg64_lit64_ten_byte_encoding(dac, REGISTER_RDI, (u)ARROW(dac, start_of_code) + (u)ARROW(instruction, argument1));
		dac_append_call_reg64(dac, REGISTER_RDI);
	#else
		dac_append_mov_reg32_lit32(dac, REGISTER_EDI, (u)ARROW(dac, start_of_code) + (u)ARROW(instruction, argument1));
		dac_append_call_reg32(dac, REGISTER_EDI);
	#endif
}

static void (*mid_level_instruction_function_array[])(DataAndCode* cr dac, MidLevelInstruction* cr instruction) = {
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

void put_instructions_into_code_section(DataAndCode* cr dac, MidLevelInstructions* cr mli) {
	u i;
	
	#if DEBUG
		output_nullt_string("\ngenerated_code:\n");
	#endif
	
	for (i = 0; i < ARROW(mli, used_length); ++i) {
		MidLevelInstruction* const instruction = ARRAY_INDEX(ARROW(mli, mid_level_instructions), i);
		ARRAY_INDEX(mid_level_instruction_function_array, ARROW(instruction, opcode))(dac, instruction);
	}
}
