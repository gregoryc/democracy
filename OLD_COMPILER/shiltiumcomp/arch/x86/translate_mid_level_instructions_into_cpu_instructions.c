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

/*
This file contains functions to translate mid-level instructions
(i.e., intermediary compiler instructions) into native CPU instructions.
*/

#include "../../global_headers/headers.h"
#include "instructions/x86_instructions.h"

#if DEBUG
	void output_mid_level_instructions(MidLevelInstructions* cr mli) {
		u i;
		
		static const char* const opcodes_to_strings[] = {
			/* Assignment */
			"mov_uvar_uval",
			"mov_fvar_fval",
			"mov_fvar_fvar",
			
			/* Multiplication */
			"mul_uvar_uval",
			"mul_fvar_fval",
			
			/* Division */
			"div_uvar_uval",
			"div_fvar_fval",
			
			/* Addition */
			"add_uvar_uval",
			"add_fvar_fval",
			"add_uvar_uvar",
			"add_fvar_fvar",
			
			/* Subtraction */
			"sub_uvar_uval",
			"sub_fvar_fval",
			"sub_uvar_uvar",
			"sub_fvar_fvar",
			
			/* Comparison */
			"cmp_le_uvar_uval",
			"cmp_lt_uvar_uval",
			"cmp_eq_uvar_uval",
			"cmp_ne_uvar_uval",
			"cmp_gt_uvar_uval",
			"cmp_ge_uvar_uval",
			
			"cmp_le_fvar_fval",
			"cmp_lt_fvar_fval",
			"cmp_eq_fvar_fval",
			"cmp_ne_fvar_fval",
			"cmp_gt_fvar_fval",
			"cmp_ge_fvar_fval",
			
			"cmp_le_uvar_uvar",
			"cmp_lt_uvar_uvar",
			"cmp_eq_uvar_uvar",
			"cmp_ne_uvar_uvar",
			"cmp_gt_uvar_uvar",
			"cmp_ge_uvar_uvar",
			
			"cmp_le_fvar_fvar",
			"cmp_lt_fvar_fvar",
			"cmp_eq_fvar_fvar",
			"cmp_ne_fvar_fvar",
			"cmp_gt_fvar_fvar",
			"cmp_ge_fvar_fvar",
			
			/* Miscellaneous */
			
			"change_sign",
			
			"test_and_jmp_if_zero_fvar_memaddr",
			
			"send_hello_world_message_to_ui",
			"send_output_uint_message_to_ui",
			"send_output_float_message_to_ui",
			
			"add_main_subroutine_starting_instructions",
			"add_main_subroutine_ending_instructions",
			
			"add_subroutine_starting_instructions",
			"add_subroutine_ending_instructions",
			
			"push_argument",
			"call_subroutine"
		};
		
		assert_comparison(mli, !=, NULL, MidLevelInstructions*, void*);
		
		output_nullt_string("\nMid-level instructions:\n");
		
		for (i = 0; i < ARROW(mli, used_length); ++i) {
			MidLevelInstruction* const instruction = ARRAY_INDEX(ARROW(mli, mid_level_instructions), i);
			MidLevelInstructionOpcode opcode = ARROW(instruction, opcode);
			
			output_nullt_string(TAB_STRING TAB_STRING);
			output_nullt_string(opcodes_to_strings[ARROW(instruction, opcode)]);
			
			switch (opcode) {
				case MLI_OPCODE_SEND_HELLO_WORLD_MESSAGE_TO_UI:
				case MLI_OPCODE_SEND_OUTPUT_UINT_MESSAGE_TO_UI:
				case MLI_OPCODE_SEND_OUTPUT_FLOAT_MESSAGE_TO_UI:
				case MLI_OPCODE_ADD_SUBROUTINE_ENDING_INSTRUCTIONS:
					break;
				
				case MLI_OPCODE_ADD_MAIN_SUBROUTINE_STARTING_INSTRUCTIONS:
				case MLI_OPCODE_ADD_MAIN_SUBROUTINE_ENDING_INSTRUCTIONS:
				case MLI_OPCODE_ADD_SUBROUTINE_STARTING_INSTRUCTIONS:
					output_char(' ');
					putu((u)ARRAY_INDEX(instruction->arguments, 0));
					output_nullt_string(" (stack offset)");
					break;
				
				case MLI_OPCODE_CALL_SUBROUTINE:
					output_char(' ');
					putu((u)ARRAY_INDEX(instruction->arguments, 0));
					break;
				
				default: {
					Variable* variable = (Variable*)ARRAY_INDEX(instruction->arguments, 0);
					output_char(' ');
					output_variable(variable);
				}
			}
			
			switch (opcode) {
				case MLI_OPCODE_MOV_UVAR_UVAL:
				case MLI_OPCODE_MUL_UVAR_UVAL:
				case MLI_OPCODE_DIV_UVAR_UVAL:
				case MLI_OPCODE_ADD_UVAR_UVAL:
				case MLI_OPCODE_SUB_UVAR_UVAL:
				case MLI_OPCODE_CMP_LE_UVAR_UVAL:
				case MLI_OPCODE_CMP_LT_UVAR_UVAL:
				case MLI_OPCODE_CMP_EQ_UVAR_UVAL:
				case MLI_OPCODE_CMP_NE_UVAR_UVAL:
				case MLI_OPCODE_CMP_GT_UVAR_UVAL:
				case MLI_OPCODE_CMP_GE_UVAR_UVAL:
					output_nullt_string(", ");
					putu(ARROW((OperandNode*)ARRAY_INDEX(instruction->arguments, 1), value).uvalue);
					break;
				
				case MLI_OPCODE_MOV_FVAR_FVAL:
				case MLI_OPCODE_MUL_FVAR_FVAL:
				case MLI_OPCODE_ADD_FVAR_FVAL:
				case MLI_OPCODE_DIV_FVAR_FVAL:
				case MLI_OPCODE_SUB_FVAR_FVAL:
				case MLI_OPCODE_CMP_LE_FVAR_FVAL:
				case MLI_OPCODE_CMP_LT_FVAR_FVAL:
				case MLI_OPCODE_CMP_EQ_FVAR_FVAL:
				case MLI_OPCODE_CMP_NE_FVAR_FVAL:
				case MLI_OPCODE_CMP_GT_FVAR_FVAL:
				case MLI_OPCODE_CMP_GE_FVAR_FVAL:
					output_nullt_string(", ");
					putf(ARROW((OperandNode*)ARRAY_INDEX(instruction->arguments, 1), value).fvalue);
					break;
				
				case MLI_OPCODE_MOV_FVAR_FVAR: {
					Variable* variable = ARROW((OperandNode*)ARRAY_INDEX(instruction->arguments, 1), variable);
					output_nullt_string(", ");
					output_variable(variable);
					break;
				}
				
				case MLI_OPCODE_ADD_UVAR_UVAR:
				case MLI_OPCODE_ADD_FVAR_FVAR:
				case MLI_OPCODE_SUB_UVAR_UVAR:
				case MLI_OPCODE_SUB_FVAR_FVAR:
				
				case MLI_OPCODE_CMP_LE_UVAR_UVAR:
				case MLI_OPCODE_CMP_LT_UVAR_UVAR:
				case MLI_OPCODE_CMP_EQ_UVAR_UVAR:
				case MLI_OPCODE_CMP_NE_UVAR_UVAR:
				case MLI_OPCODE_CMP_GT_UVAR_UVAR:
				case MLI_OPCODE_CMP_GE_UVAR_UVAR:
				
				case MLI_OPCODE_CMP_LE_FVAR_FVAR:
				case MLI_OPCODE_CMP_LT_FVAR_FVAR:
				case MLI_OPCODE_CMP_EQ_FVAR_FVAR:
				case MLI_OPCODE_CMP_NE_FVAR_FVAR:
				case MLI_OPCODE_CMP_GT_FVAR_FVAR:
				case MLI_OPCODE_CMP_GE_FVAR_FVAR: {
					Variable* variable = (Variable*)ARRAY_INDEX(instruction->arguments, 1);
					output_nullt_string(", ");
					output_variable(variable);
					break;
				}
				
				case MLI_OPCODE_TEST_AND_JMP_IF_ZERO_FVAR_MEMADDR:
					output_nullt_string(", ");
					putu((u)ARRAY_INDEX(instruction->arguments, 1));
					output_nullt_string(" (bytes after this instruction)\n");
					break;
				
				case MLI_OPCODE_CHANGE_SIGN:
				case MLI_OPCODE_SEND_HELLO_WORLD_MESSAGE_TO_UI:
				case MLI_OPCODE_ADD_MAIN_SUBROUTINE_STARTING_INSTRUCTIONS:
				case MLI_OPCODE_ADD_MAIN_SUBROUTINE_ENDING_INSTRUCTIONS:
				case MLI_OPCODE_ADD_SUBROUTINE_STARTING_INSTRUCTIONS:
				case MLI_OPCODE_PUSH_ARGUMENT:
				case MLI_OPCODE_CALL_SUBROUTINE:
					break;
				
				case MLI_OPCODE_ADD_SUBROUTINE_ENDING_INSTRUCTIONS:
					output_char(' ');
					putu((u)ARRAY_INDEX(instruction->arguments, 0));
					output_nullt_string(" (the number of arguments)");
					
					break;
				
				case MLI_OPCODE_SEND_OUTPUT_UINT_MESSAGE_TO_UI:
				case MLI_OPCODE_SEND_OUTPUT_FLOAT_MESSAGE_TO_UI:
					output_space();
					putu((u)ARRAY_INDEX(instruction->arguments, 0));
					output_nullt_string(" (stack offset), ");
					output_variable((Variable*)ARRAY_INDEX(instruction->arguments, 1));
					break;
			}
			
			output_newline();
		}
		
		output_newline();
	}
#endif

attrs_nonnull_and_warn_unused_result
u /* the main subroutine offset */
translate_mid_level_instructions_to_cpu_instructions(
	DataAndCode* cr dac,
	MidLevelInstructions* cr mli,
	const u code_length
) {
	u main_subroutine_offset;
	
	#if DEBUG
		output_mid_level_instructions(mli);
	#endif
	
	assert_comparison(dac, !=, NULL, DataAndCode* cr, void*);
	assert_comparison(mli, !=, NULL, MidLevelInstructions* cr, void*);
	
	if (likely(code_length)) {
		ARROW_ASSIGN(dac, start_of_code) = (char*)m_allocate_code_section(code_length);
	}
	
	main_subroutine_offset = put_instructions_into_code_section(dac, mli);
	
	assert_comparison(ARROW(dac, current_instruction_offset), <=, code_length, u, u);
	
	#if DEBUG
		if (code_length - ARROW(dac, current_instruction_offset)) {
			putu(code_length - ARROW(dac, current_instruction_offset));
			output_nullt_string(" wasted bytes.\n");
		}
	#endif
	
	return main_subroutine_offset;
}
