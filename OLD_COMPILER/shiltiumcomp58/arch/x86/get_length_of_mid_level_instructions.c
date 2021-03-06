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

/* Get the length of the instructions generated by store_float_variable_on_stack() */
static u get_length_of_instructions_generated_by_store_float_variable_on_stack(const Variable* cr variable) {
	u variable_stack_offset;
	
	assert_comparison(variable, !=, NULL, const Variable*, void*);
	
	variable_stack_offset = ARROW(variable, stack_offset);
	
	#if SIXTY_FOUR_BIT
		if (variable_stack_offset) {
			return
				dac_get_length_of_mov_reg64_lit64(REGISTER_R11, variable_stack_offset) +
				dac_get_length_of_add_reg64_reg64() +
				dac_get_length_of_fstp_deref_register_as_qword(REGISTER_R11);
		} else {
			return dac_get_length_of_fstp_deref_register_as_qword(REGISTER_RSP);
		}
	#else
		if (!variable_stack_offset) {
			return dac_get_length_of_fstp_deref_register_as_qword(REGISTER_ESP);
		}
		
		return
			dac_get_length_of_mov_reg32_lit32(variable_stack_offset) +
			dac_get_length_of_add_reg32_reg32() +
			dac_get_length_of_fstp_deref_register_as_qword(REGISTER_EDI);
	#endif
}

/* Get the length of the instructions generated by store_u_variable_on_stack() */
static u get_length_of_instructions_generated_by_store_u_variable_on_stack(Variable* cr variable) {
	u variable_stack_offset;
	assert_comparison(variable, !=, NULL, Variable*, void*);
	
	variable_stack_offset = ARROW(variable, stack_offset);
	
	#if SIXTY_FOUR_BIT
		if (variable_stack_offset) {
			return
				dac_get_length_of_mov_reg64_lit64(REGISTER_R11, variable_stack_offset) +
				dac_get_length_of_add_reg64_reg64() +
				dac_get_length_of_mov_deref_register_as_qword_reg64(REGISTER_R11);
		} else {
			return dac_get_length_of_mov_deref_register_as_qword_reg64(REGISTER_RSP);
		}
	#else
		if (variable_stack_offset) {
			return
				dac_get_length_of_mov_reg32_lit32(variable_stack_offset) +
				dac_get_length_of_add_reg32_reg32() +
				dac_get_length_of_mov_deref_register_as_dword_reg32(REGISTER_EDI);
		} else {
			return dac_get_length_of_mov_deref_register_as_dword_reg32(REGISTER_ESP);
		}
	#endif
}

u get_length_of_mid_level_instructions(MidLevelInstructions* cr mli) {
	u i;
	u length;
	
	assert_comparison(mli, !=, NULL, MidLevelInstructions*, void*);
	
	length = 0;
	
	for (i = 0; i < ARROW(mli, used_length); ++i) {
		MidLevelInstruction* const instruction = ARRAY_INDEX(ARROW(mli, mid_level_instructions), i);
		
		switch (ARROW(instruction, opcode)) {
			case MLI_OPCODE_MOV_UVAR_UVAL: {
				Variable* variable;
				OperandNode* operand_node;
				
				variable = (Variable*)ARROW(instruction, argument1);
				operand_node = (OperandNode*)ARROW(instruction, argument2);
				
				#if SIXTY_FOUR_BIT
					length += dac_get_length_of_mov_reg64_lit64(REGISTER_RAX, ARROW(operand_node, value).uvalue);
				#else
					length += dac_get_length_of_mov_reg32_lit32(ARROW(operand_node, value).uvalue);
				#endif
				
				if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
					length += get_length_of_instructions_generated_by_store_u_variable_on_stack(variable);
				}
				
				break;
			}
			case MLI_OPCODE_MOV_FVAR_FVAL: {
				Variable* variable;
				OperandNode* operand_node;
				
				variable = (Variable*)ARROW(instruction, argument1);
				operand_node = (OperandNode*)ARROW(instruction, argument2);
				
				assert_comparison(variable, !=, NULL, Variable*, void*);
				assert_comparison(operand_node, !=, NULL, OperandNode*, void*);
				
				if (ARROW(operand_node, value).fvalue == 0.0) {
					length += dac_get_length_of_fldz();
				} else if (ARROW(operand_node, value).fvalue == 1.0) {
					length += dac_get_length_of_fld1();
				} else {
					#if SIXTY_FOUR_BIT
						length += dac_get_length_of_mov_reg64_lit64(REGISTER_R11, DEREF((u*)ARROW(operand_node, address_in_memory)));
						length += dac_get_length_of_fld_deref_register_as_qword(REGISTER_R11);
					#else
						length += dac_get_length_of_mov_reg32_lit32(DEREF((u*)ARROW(operand_node, address_in_memory)));
						length += dac_get_length_of_fld_deref_register_as_qword(REGISTER_EDI);
					#endif
				}
				
				if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
					length += get_length_of_instructions_generated_by_store_float_variable_on_stack(variable);
				}
				
				break;
			}
			case MLI_OPCODE_MUL_UVAR_UVAL:
				output_nullt_string("mul_uvar_uval\n");
				break;
			case MLI_OPCODE_MUL_FVAR_FVAL: {
				Variable* variable;
				OperandNode* operand_node;
				
				variable = (Variable*)ARROW(instruction, argument1);
				operand_node = (OperandNode*)ARROW(instruction, argument2);
				
				assert_comparison(variable, !=, NULL, Variable*, void*);
				assert_comparison(operand_node, !=, NULL, Variable*, void*);
				
				if (ARROW(operand_node, value).fvalue == 0.0) {
					#if SIXTY_FOUR_BIT
						length += dac_get_length_of_fstp_deref_register_as_qword(REGISTER_RAX);
					#else
						length += dac_get_length_of_fstp_deref_register_as_qword(REGISTER_EAX);
					#endif
					
					length += dac_get_length_of_fldz();
				} else {
					#if SIXTY_FOUR_BIT
						length += dac_get_length_of_mov_reg64_lit64(REGISTER_R11, DEREF((u*)ARROW(operand_node, address_in_memory)));
						length += dac_get_length_of_fmul_deref_register_as_qword(REGISTER_R11);
					#else
						length += dac_get_length_of_mov_reg32_lit32(DEREF((u*)ARROW(operand_node, address_in_memory)));
						length += dac_get_length_of_fmul_deref_register_as_qword(REGISTER_EDI);
					#endif
				}
				
				if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
					length += get_length_of_instructions_generated_by_store_float_variable_on_stack(variable);
				}
				
				break;
			}
			case MLI_OPCODE_MOV_FVAR_FVAR: {
				Variable* variable1;
				Variable* variable2;
				u variable_stack_offset;
				
				variable1 = (Variable*)ARROW(instruction, argument1);
				variable2 = (Variable*)ARROW(instruction, argument2);
				
				assert_comparison(variable1, !=, NULL, Variable*, void*);
				assert_comparison(variable2, !=, NULL, Variable*, void*);
				
				variable_stack_offset = ARROW(variable2, stack_offset);
				
				#if SIXTY_FOUR_BIT
					if (variable_stack_offset) {
						length += dac_get_length_of_mov_reg64_reg64();
						length += dac_get_length_of_mov_reg64_lit64(REGISTER_R11, variable_stack_offset);
						length += dac_get_length_of_add_reg64_reg64();
					}
					length += dac_get_length_of_fld_deref_register_as_qword(REGISTER_R11);
				#else
					if (variable_stack_offset) {
						length += dac_get_length_of_mov_reg32_reg32();
						length += dac_get_length_of_mov_reg32_lit32(variable_stack_offset);
						length += dac_get_length_of_add_reg32_reg32();
					}
					length += dac_get_length_of_fld_deref_register_as_qword(REGISTER_EDI);
				#endif
				
				if (instruction == ARROW(variable1, last_instruction_that_uses_this_variable)) {
					length += get_length_of_instructions_generated_by_store_float_variable_on_stack(variable1);
				}
				
				break;
			}
			case MLI_OPCODE_ADD_UVAR_UVAL: {
				OperandNode* operand_node;
				Variable* variable;
				
				variable = (Variable*)ARROW(instruction, argument1);
				operand_node = (OperandNode*)ARROW(instruction, argument2);
				
				#if SIXTY_FOUR_BIT
					length += dac_get_length_of_mov_reg64_lit64(REGISTER_RBX, ARROW(operand_node, value).uvalue);
					length += dac_get_length_of_add_reg64_reg64();
				#else
					length += dac_get_length_of_mov_reg32_lit32(ARROW(operand_node, value).uvalue);
					length += dac_get_length_of_add_reg32_reg32();
				#endif
				
				if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
					length += get_length_of_instructions_generated_by_store_u_variable_on_stack(variable);
				}
				break;
			}
			case MLI_OPCODE_ADD_FVAR_FVAL: {
				Variable* variable;
				OperandNode* operand_node;
				
				variable = (Variable*)ARROW(instruction, argument1);
				operand_node = (OperandNode*)ARROW(instruction, argument2);
				
				assert_comparison(variable, !=, NULL, Variable*, void*);
				assert_comparison(operand_node, !=, NULL, OperandNode*, void*);
				
				if (ARROW(operand_node, value).fvalue == 1.0) {
					length += dac_get_length_of_fld1();
					length += dac_get_length_of_faddp();
				} else {
					#if SIXTY_FOUR_BIT
						length += dac_get_length_of_mov_reg64_lit64(REGISTER_R11, DEREF((u*)ARROW(operand_node, address_in_memory)));
						length += dac_get_length_of_fadd_deref_register_as_qword(REGISTER_R11);
					#else
						length += dac_get_length_of_mov_reg32_lit32(DEREF((u*)ARROW(operand_node, address_in_memory)));
						length += dac_get_length_of_fadd_deref_register_as_qword(REGISTER_EDI);
					#endif
				}
				
				if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
					length += get_length_of_instructions_generated_by_store_float_variable_on_stack(variable);
				}
				
				break;
			}
			case MLI_OPCODE_ADD_UVAR_UVAR:
				output_nullt_string("add_uvar_uvar\n");
				break;
			case MLI_OPCODE_ADD_FVAR_FVAR: {
				Variable* variable1;
				Variable* variable2;
				u variable_stack_offset;
				
				variable1 = (Variable*)ARROW(instruction, argument1);
				variable2 = (Variable*)ARROW(instruction, argument2);
				
				assert_comparison(variable1, !=, NULL, Variable*, void*);
				assert_comparison(variable2, !=, NULL, Variable*, void*);
				
				variable_stack_offset = ARROW(variable2, stack_offset);
				
				#if SIXTY_FOUR_BIT
					length += dac_get_length_of_mov_reg64_reg64();
					length += dac_get_length_of_mov_reg64_lit64(REGISTER_R11, variable_stack_offset);
					length += dac_get_length_of_add_reg64_reg64();
					length += dac_get_length_of_fadd_deref_register_as_qword(REGISTER_R11);
				#else
					length += dac_get_length_of_mov_reg32_reg32();
					length += dac_get_length_of_mov_reg32_lit32(variable_stack_offset);
					length += dac_get_length_of_add_reg32_reg32();
					length += dac_get_length_of_fadd_deref_register_as_qword(REGISTER_EDI);
				#endif
				
				if (instruction == ARROW(variable1, last_instruction_that_uses_this_variable)) {
					length += get_length_of_instructions_generated_by_store_float_variable_on_stack(variable1);
				}
				
				break;
			}
			case MLI_OPCODE_SUB_UVAR_UVAL:
				output_nullt_string("sub_uvar_uval\n");
				break;
			case MLI_OPCODE_SUB_FVAR_FVAL: {
				Variable* variable;
				OperandNode* operand_node;
				
				variable = (Variable*)ARROW(instruction, argument1);
				operand_node = (OperandNode*)ARROW(instruction, argument2);
				
				assert_comparison(variable, !=, NULL, Variable*, void*);
				assert_comparison(operand_node, !=, NULL, OperandNode*, void*);
				
				if (ARROW(operand_node, value).fvalue == 1.0) {
					length += dac_get_length_of_fld1();
					length += dac_get_length_of_fsubp();
				} else {
					#if SIXTY_FOUR_BIT
						length += dac_get_length_of_mov_reg64_lit64(REGISTER_R11, DEREF((u*)ARROW(operand_node, address_in_memory)));
						length += dac_get_length_of_fsub_deref_register_as_qword(REGISTER_R11);
					#else
						length += dac_get_length_of_mov_reg32_lit32(DEREF((u*)ARROW(operand_node, address_in_memory)));
						length += dac_get_length_of_fsub_deref_register_as_qword(REGISTER_EDI);
					#endif
				}
				
				if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
					length += get_length_of_instructions_generated_by_store_float_variable_on_stack(variable);
				}
				
				break;
			}
			case MLI_OPCODE_SUB_UVAR_UVAR:
				output_nullt_string("sub_uvar_uvar\n");
				break;
			case MLI_OPCODE_SUB_FVAR_FVAR: {
				Variable* variable1;
				Variable* variable2;
				u variable_stack_offset;
				
				variable1 = (Variable*)ARROW(instruction, argument1);
				variable2 = (Variable*)ARROW(instruction, argument2);
				
				assert_comparison(variable1, !=, NULL, Variable*, void*);
				assert_comparison(variable2, !=, NULL, Variable*, void*);
				
				variable_stack_offset = ARROW(variable2, stack_offset);
				
				#if SIXTY_FOUR_BIT
					length += dac_get_length_of_mov_reg64_reg64();
					length += dac_get_length_of_mov_reg64_lit64(REGISTER_R11, variable_stack_offset);
					length += dac_get_length_of_add_reg64_reg64();
					length += dac_get_length_of_fsub_deref_register_as_qword(REGISTER_R11);
				#else
					length += dac_get_length_of_mov_reg32_reg32();
					length += dac_get_length_of_mov_reg32_lit32(variable_stack_offset);
					length += dac_get_length_of_add_reg32_reg32();
					length += dac_get_length_of_fsub_deref_register_as_qword(REGISTER_EDI);
				#endif
				
				if (instruction == ARROW(variable1, last_instruction_that_uses_this_variable)) {
					length += get_length_of_instructions_generated_by_store_float_variable_on_stack(variable1);
				}
				
				break;
			}
			case MLI_OPCODE_DIV_UVAR_UVAL:
				output_nullt_string("div_uvar_uval\n");
				break;
			case MLI_OPCODE_DIV_FVAR_FVAL:
				/* FIXME */
				
				assert_comparison(0, !=, 0, u, u);
				break;
			case MLI_OPCODE_CMP_LE_UVAR_UVAL: {
				Variable* variable = (Variable*)ARROW(instruction, argument1);
				
				if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
					length += get_length_of_instructions_generated_by_store_float_variable_on_stack(variable);
				}
				
				break;
			}
			case MLI_OPCODE_CMP_LT_UVAR_UVAL: {
				Variable* variable = (Variable*)ARROW(instruction, argument1);
				
				if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
					length += get_length_of_instructions_generated_by_store_float_variable_on_stack(variable);
				}
				
				break;
			}
			case MLI_OPCODE_CMP_EQ_UVAR_UVAL: {
				Variable* variable = (Variable*)ARROW(instruction, argument1);
				
				if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
					length += get_length_of_instructions_generated_by_store_float_variable_on_stack(variable);
				}
				
				break;
			}
			case MLI_OPCODE_CMP_NE_UVAR_UVAL: {
				Variable* variable = (Variable*)ARROW(instruction, argument1);
				
				if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
					length += get_length_of_instructions_generated_by_store_float_variable_on_stack(variable);
				}
				
				break;
			}
			case MLI_OPCODE_CMP_GT_UVAR_UVAL: {
				Variable* variable = (Variable*)ARROW(instruction, argument1);
				
				if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
					length += get_length_of_instructions_generated_by_store_float_variable_on_stack(variable);
				}
				
				break;
			}
			case MLI_OPCODE_CMP_GE_UVAR_UVAL: {
				Variable* variable = (Variable*)ARROW(instruction, argument1);
				
				if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
					length += get_length_of_instructions_generated_by_store_float_variable_on_stack(variable);
				}
				
				break;
			}
			
			case MLI_OPCODE_CMP_LE_FVAR_FVAL: {
				Variable* variable = (Variable*)ARROW(instruction, argument1);
				
				if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
					length += get_length_of_instructions_generated_by_store_float_variable_on_stack(variable);
				}
				
				break;
			}
			case MLI_OPCODE_CMP_LT_FVAR_FVAL: {
				Variable* variable = (Variable*)ARROW(instruction, argument1);
				
				if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
					length += get_length_of_instructions_generated_by_store_float_variable_on_stack(variable);
				}
				
				break;
			}
			case MLI_OPCODE_CMP_EQ_FVAR_FVAL: {
				Variable* variable = (Variable*)ARROW(instruction, argument1);
				
				if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
					length += get_length_of_instructions_generated_by_store_float_variable_on_stack(variable);
				}
				
				break;
			}
			case MLI_OPCODE_CMP_NE_FVAR_FVAL: {
				Variable* variable = (Variable*)ARROW(instruction, argument1);
				
				if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
					length += get_length_of_instructions_generated_by_store_float_variable_on_stack(variable);
				}
				
				break;
			}
			case MLI_OPCODE_CMP_GT_FVAR_FVAL: {
				Variable* variable = (Variable*)ARROW(instruction, argument1);
				
				if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
					length += get_length_of_instructions_generated_by_store_float_variable_on_stack(variable);
				}
				
				break;
			}
			case MLI_OPCODE_CMP_GE_FVAR_FVAL: {
				Variable* variable;
				OperandNode* operand_node;
				
				variable = (Variable*)ARROW(instruction, argument1);
				operand_node = (OperandNode*)ARROW(instruction, argument2);
				
				assert_comparison(variable, !=, NULL, Variable*, void*);
				assert_comparison(operand_node, !=, NULL, OperandNode*, void*);
				
				if (ARROW(operand_node, value).fvalue == 0.0) {
					length += dac_get_length_of_fldz();
				} else if (ARROW(operand_node, value).fvalue == 1.0) {
					length += dac_get_length_of_fld1();
				} else {
					#if SIXTY_FOUR_BIT
						length += dac_get_length_of_mov_reg64_lit64(REGISTER_R11, DEREF((u*)ARROW(operand_node, address_in_memory)));
						length += dac_get_length_of_fld_deref_register_as_qword(REGISTER_R11);
					#else
						length += dac_get_length_of_mov_reg32_lit32(DEREF((u*)ARROW(operand_node, address_in_memory)));
						length += dac_get_length_of_fld_deref_register_as_qword(REGISTER_EDI);
					#endif
				}
				
				length += dac_get_length_of_fcomip();
				length += dac_get_length_of_ja_short();
				length += dac_get_length_of_fld1();
				length += dac_get_length_of_jmp_short();
				length += dac_get_length_of_fldz();
				length += dac_get_length_of_fxch();
				length += dac_get_length_of_ffree();
				length += dac_get_length_of_fincstp();
				
				if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
					length += get_length_of_instructions_generated_by_store_float_variable_on_stack(variable);
				}
				
				break;
			}
			
			case MLI_OPCODE_CMP_LE_UVAR_UVAR: {
				Variable* variable = (Variable*)ARROW(instruction, argument1);
				
				if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
					length += get_length_of_instructions_generated_by_store_float_variable_on_stack(variable);
				}
				
				break;
			}
			case MLI_OPCODE_CMP_LT_UVAR_UVAR: {
				Variable* variable = (Variable*)ARROW(instruction, argument1);
				
				if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
					length += get_length_of_instructions_generated_by_store_float_variable_on_stack(variable);
				}
				
				break;
			}
			case MLI_OPCODE_CMP_EQ_UVAR_UVAR: {
				Variable* variable = (Variable*)ARROW(instruction, argument1);
				
				if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
					length += get_length_of_instructions_generated_by_store_float_variable_on_stack(variable);
				}
				
				break;
			}
			case MLI_OPCODE_CMP_NE_UVAR_UVAR: {
				Variable* variable = (Variable*)ARROW(instruction, argument1);
				
				if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
					length += get_length_of_instructions_generated_by_store_float_variable_on_stack(variable);
				}
				
				break;
			}
			case MLI_OPCODE_CMP_GT_UVAR_UVAR: {
				Variable* variable = (Variable*)ARROW(instruction, argument1);
				
				if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
					length += get_length_of_instructions_generated_by_store_float_variable_on_stack(variable);
				}
				
				break;
			}
			case MLI_OPCODE_CMP_GE_UVAR_UVAR: {
				Variable* variable = (Variable*)ARROW(instruction, argument1);
				
				if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
					length += get_length_of_instructions_generated_by_store_float_variable_on_stack(variable);
				}
				
				break;
			}
			
			case MLI_OPCODE_CMP_LE_FVAR_FVAR: {
				Variable* variable = (Variable*)ARROW(instruction, argument1);
				
				if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
					length += get_length_of_instructions_generated_by_store_float_variable_on_stack(variable);
				}
				
				break;
			}
			case MLI_OPCODE_CMP_LT_FVAR_FVAR: {
				Variable* variable = (Variable*)ARROW(instruction, argument1);
				
				if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
					length += get_length_of_instructions_generated_by_store_float_variable_on_stack(variable);
				}
				
				break;
			}
			case MLI_OPCODE_CMP_EQ_FVAR_FVAR: {
				Variable* variable = (Variable*)ARROW(instruction, argument1);
				
				if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
					length += get_length_of_instructions_generated_by_store_float_variable_on_stack(variable);
				}
				
				break;
			}
			case MLI_OPCODE_CMP_NE_FVAR_FVAR: {
				Variable* variable = (Variable*)ARROW(instruction, argument1);
				
				if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
					length += get_length_of_instructions_generated_by_store_float_variable_on_stack(variable);
				}
				
				break;
			}
			case MLI_OPCODE_CMP_GT_FVAR_FVAR: {
				Variable* variable = (Variable*)ARROW(instruction, argument1);
				
				if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
					length += get_length_of_instructions_generated_by_store_float_variable_on_stack(variable);
				}
				
				break;
			}
			case MLI_OPCODE_CMP_GE_FVAR_FVAR: {
				Variable* variable = (Variable*)ARROW(instruction, argument1);
				
				if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
					length += get_length_of_instructions_generated_by_store_float_variable_on_stack(variable);
				}
				
				break;
			}
			
			case MLI_OPCODE_CHANGE_SIGN: {
				Variable* variable = (Variable*)ARROW(instruction, argument1);
				
				length += dac_get_length_of_fchs();
				
				if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
					length += get_length_of_instructions_generated_by_store_float_variable_on_stack(variable);
				}
				
				break;
			}
			
			case MLI_OPCODE_TEST_AND_JMP_IF_ZERO_FVAR_MEMADDR: {
				Variable* variable;
				u relative_instruction_offset;
				u variable_stack_offset;
				
				variable = (Variable*)ARROW(instruction, argument1);
				assert_comparison(variable, !=, NULL, Variable*, void*);
				
				relative_instruction_offset = (u)ARROW(instruction, argument2);
				assert_comparison(relative_instruction_offset, !=, 0, u, u);
				
				variable_stack_offset = ARROW(variable, stack_offset);
				
				#if SIXTY_FOUR_BIT
					length += dac_get_length_of_mov_reg64_reg64();
					length += dac_get_length_of_mov_reg64_lit64(REGISTER_R12, variable_stack_offset);
					length += dac_get_length_of_add_reg64_reg64();
					length += dac_get_length_of_fld_deref_register_as_qword(REGISTER_R11);
				#else
					length += dac_get_length_of_mov_reg32_reg32();
					length += dac_get_length_of_mov_reg32_lit32(variable_stack_offset);
					length += dac_get_length_of_add_reg32_reg32();
					length += dac_get_length_of_fld_deref_register_as_qword(REGISTER_EDI);
				#endif
				
				length += dac_get_length_of_fldz();
				length += dac_get_length_of_fcomip();
				
				if (relative_instruction_offset <= 127) {
					length += dac_get_length_of_je_short();
				} else {
					output_nullt_string("FIXME: Non-short jump.\n");
					exit_program();
				}
				
				if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
					length += get_length_of_instructions_generated_by_store_float_variable_on_stack(variable);
				}
				
				break;
			}
			
			case MLI_OPCODE_SEND_HELLO_WORLD_MESSAGE_TO_UI: {
				const u variable_stack_offset = (u)ARROW(instruction, argument1);
				
				#if LINUX
					#if SIXTY_FOUR_BIT
						length += dac_get_length_of_mov_reg32_lit32(1 /* the system call number */);
						length += dac_get_length_of_mov_reg32_lit32(1);
						
						if (!variable_stack_offset) {
							length += dac_get_length_of_mov_deref_register_as_qword_lit32(REGISTER_RSP);
							length += dac_get_length_of_mov_reg64_reg64();
						} else if ((s)variable_stack_offset == (schar)variable_stack_offset) {
							length += dac_get_length_of_mov_deref_register_as_qword_plus_signed_byte_offset_lit32(REGISTER_RSP);
							length += dac_get_length_of_mov_reg64_reg64();
							length += dac_get_length_of_add_reg64_lit32(REGISTER_RSI, (s)ARROW(instruction, argument1));
						} else {
							length += dac_get_length_of_mov_reg64_reg64();
							length += dac_get_length_of_mov_reg64_lit64(REGISTER_R11, (s)ARROW(instruction, argument1) /* stack offset */);
							length += dac_get_length_of_add_reg64_reg64();
							length += dac_get_length_of_mov_deref_register_as_qword_lit32(REGISTER_RSI);
						}
						
						length += dac_get_length_of_mov_reg32_lit32(2);
						length += dac_get_length_of_syscall();
					#else
						length += dac_get_length_of_mov_reg32_lit32(4 /* the system call number */);
						length += dac_get_length_of_mov_reg32_lit32(1);
						
						if (!variable_stack_offset) {
							length += dac_get_length_of_mov_deref_register_as_dword_lit32(REGISTER_ESP);
							length += dac_get_length_of_mov_reg32_reg32();
						} else if ((s)variable_stack_offset == (schar)variable_stack_offset) {
							length += dac_get_length_of_mov_deref_register_as_dword_plus_signed_byte_offset_lit32(REGISTER_ESP);
							length += dac_get_length_of_mov_reg32_reg32();
							length += dac_get_length_of_add_reg32_lit32(REGISTER_ESI, (s)ARROW(instruction, argument1));
						} else {
							length += dac_get_length_of_mov_reg32_lit32(0 /* the message number */);
							length += dac_get_length_of_mov_reg32_reg32();
							length += dac_get_length_of_mov_reg32_lit32((s)ARROW(instruction, argument1) /* stack offset */);
							length += dac_get_length_of_add_reg32_reg32();
							length += dac_get_length_of_mov_deref_register_as_dword_reg32(REGISTER_ECX);
						}
						
						length += dac_get_length_of_mov_reg32_lit32(2);
						length += dac_get_length_of_int();
					#endif
				#else
					(void)variable_stack_offset;
				#endif
				
				break;
			}
			case MLI_OPCODE_SEND_OUTPUT_UINT_MESSAGE_TO_UI: {
				u variable_stack_offset = (u)ARROW(instruction, argument1);
				Variable* variable = (Variable*)ARROW(instruction, argument2);
				
				assert_comparison(variable, !=, NULL, Variable* cr, void*);
				
				#if SIXTY_FOUR_BIT
					length += dac_get_length_of_mov_reg32_lit32(1);
					length += dac_get_length_of_mov_reg64_lit64(REGISTER_RDI, 1);
					length += dac_get_length_of_mov_reg64_lit64(REGISTER_RDX, 10);
					length += dac_get_length_of_mov_reg64_reg64();
					length += dac_get_length_of_mov_reg64_lit64(REGISTER_R11, variable_stack_offset);
					length += dac_get_length_of_add_reg64_reg64();
					length += dac_get_length_of_mov_reg64_lit64(REGISTER_R12, 1);
					length += dac_get_length_of_mov_deref_register_as_qword_reg64(REGISTER_RSI);
					length += dac_get_length_of_add_reg64_lit32(REGISTER_RSI, 2);
					length += dac_get_length_of_mov_reg64_lit64(REGISTER_R12, (s)ARROW(variable, stack_offset));
					length += dac_get_length_of_add_reg64_reg64();
					length += dac_get_length_of_mov_reg64_deref_register_as_qword(REGISTER_R12);
					length += dac_get_length_of_mov_deref_register_as_qword_reg64(REGISTER_RSI);
					length += dac_get_length_of_sub_reg64_lit32(REGISTER_RSI, 2);
					length += dac_get_length_of_syscall();
				#else
					(void)variable_stack_offset;
				#endif
				
				break;
			}
			case MLI_OPCODE_SEND_OUTPUT_FLOAT_MESSAGE_TO_UI: {
				u variable_stack_offset = (u)ARROW(instruction, argument1);
				Variable* variable = (Variable*)ARROW(instruction, argument2);
				
				assert_comparison(variable, !=, NULL, Variable* cr, void*);
				
				#if SIXTY_FOUR_BIT
					length += dac_get_length_of_mov_reg32_lit32(1);
					length += dac_get_length_of_mov_reg64_lit64(REGISTER_RDI, 1);
					length += dac_get_length_of_mov_reg64_lit64(REGISTER_RDX, 10);
					length += dac_get_length_of_mov_reg64_reg64();
					length += dac_get_length_of_mov_reg64_lit64(REGISTER_R11, variable_stack_offset);
					length += dac_get_length_of_add_reg64_reg64();
					length += dac_get_length_of_mov_reg64_lit64(REGISTER_R12, 2);
					length += dac_get_length_of_mov_deref_register_as_qword_reg64(REGISTER_RSI);
					length += dac_get_length_of_add_reg64_lit32(REGISTER_RSI, 2);
					length += dac_get_length_of_mov_reg64_lit64(REGISTER_R12, (s)ARROW(variable, stack_offset));
					length += dac_get_length_of_add_reg64_reg64();
					length += dac_get_length_of_mov_reg64_deref_register_as_qword(REGISTER_R12);
					length += dac_get_length_of_mov_deref_register_as_qword_reg64(REGISTER_RSI);
					length += dac_get_length_of_sub_reg64_lit32(REGISTER_RSI, 2);
					length += dac_get_length_of_syscall();
				#else
					(void)variable_stack_offset;
				#endif
				
				break;
			}
			
			case MLI_OPCODE_ADD_MAIN_SUBROUTINE_STARTING_INSTRUCTIONS: {
				#if !SIXTY_FOUR_BIT
					const u stack_offset = (u)ARROW(instruction, argument1);
				#endif
				
				length += dac_get_length_of_pusha_except_stack_pointer();
				
				#if SIXTY_FOUR_BIT
					length += dac_get_length_of_mov_reg64_lit64_ten_byte_encoding();
					length += dac_get_length_of_sub_reg64_reg64();
				#else
					length += dac_get_length_of_mov_reg32_lit32(stack_offset);
					length += dac_get_length_of_sub_reg32_reg32();
				#endif
				
				break;
			}
			case MLI_OPCODE_ADD_MAIN_SUBROUTINE_ENDING_INSTRUCTIONS: {
				const u stack_offset = (u)ARROW(instruction, argument1);
				
				if (stack_offset) {
					#if SIXTY_FOUR_BIT
						length += dac_get_length_of_mov_reg64_lit64(REGISTER_RBP, stack_offset);
						length += dac_get_length_of_add_reg64_reg64();
					#else
						length += dac_get_length_of_mov_reg32_lit32(stack_offset);
						length += dac_get_length_of_add_reg32_reg32();
					#endif
				}
				
				length += dac_get_length_of_popa_except_stack_pointer();
				length += dac_get_length_of_ret();
				
				break;
			}
			
			case MLI_OPCODE_ADD_SUBROUTINE_STARTING_INSTRUCTIONS: {
				const u stack_offset = (u)ARROW(instruction, argument1);
				
				if (likely(stack_offset)) {
					#if SIXTY_FOUR_BIT
						length += dac_get_length_of_mov_reg64_lit64_ten_byte_encoding();
						length += dac_get_length_of_sub_reg64_reg64();
					#else
						length += dac_get_length_of_mov_reg32_lit32(stack_offset);
						length += dac_get_length_of_sub_reg32_reg32();
					#endif
				}
				
				break;
			}
			
			case MLI_OPCODE_ADD_SUBROUTINE_ENDING_INSTRUCTIONS: {
				const u stack_offset = (u)ARROW(instruction, argument1);
				const u number_of_arguments = (u)ARROW(instruction, argument2);
				
				#if SIXTY_FOUR_BIT
					length += dac_get_length_of_mov_reg64_lit64(REGISTER_RBP, stack_offset);
					length += dac_get_length_of_add_reg64_reg64();
					length += dac_get_length_of_mov_reg64_deref_register_as_qword(REGISTER_RBP);
					
					length += dac_get_length_of_mov_reg64_lit64(REGISTER_RBP, stack_offset + 8 /* for return address */ + number_of_arguments * 8);
					length += dac_get_length_of_add_reg64_reg64();
					
					length += dac_get_length_of_jmp_reg(REGISTER_RAX);
				#else
					length += dac_get_length_of_mov_reg32_lit32(stack_offset);
					length += dac_get_length_of_add_reg32_reg32();
					length += dac_get_length_of_mov_reg32_deref_register_as_dword(REGISTER_EBP);
					
					length += dac_get_length_of_mov_reg32_lit32(stack_offset + 4 /* for return address */ + number_of_arguments * 4);
					length += dac_get_length_of_add_reg32_reg32();
					
					length += dac_get_length_of_jmp_reg();
				#endif
				
				break;
			}
			
			case MLI_OPCODE_PUSH_ARGUMENT: {
				Variable* variable = (Variable*)ARROW(instruction, argument1);
				
				#if SIXTY_FOUR_BIT
					length += dac_get_length_of_mov_reg64_lit64(REGISTER_R11, ARROW(variable, stack_offset));
					length += dac_get_length_of_add_reg64_reg64();
					length += dac_get_length_of_mov_reg64_deref_register_as_qword(REGISTER_R11);
					length += dac_get_length_of_push(REGISTER_RAX);
				#else
					length += dac_get_length_of_mov_reg32_lit32(ARROW(variable, stack_offset));
					length += dac_get_length_of_add_reg32_reg32();
					length += dac_get_length_of_mov_reg32_deref_register_as_dword(REGISTER_EDI);
					length += dac_get_length_of_push();
				#endif
				
				break;
			}
			
			case MLI_OPCODE_CALL_SUBROUTINE:
				#if SIXTY_FOUR_BIT
					length += dac_get_length_of_mov_reg64_lit64_ten_byte_encoding();
					length += dac_get_length_of_call_reg64(REGISTER_RDI);
				#else
					length += 5;
					length += dac_get_length_of_call_reg32();
				#endif
				
				break;
		}
	}
	
	return length;
}
