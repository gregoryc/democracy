/*
Mid-level instruction functions.

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

#include "../global_headers/headers.h"

static inline void unable_to_allocate_memory_if_null(MidLevelInstruction** instructions) {
	if (unlikely(!instructions)) {
		output_nullt_string("Memory error: unable to allocate memory for intermediary compiler instructions.\n");
		exit_program();
	}
}

void mid_level_instructions_init(MidLevelInstructions* cr mli) {
	assert_comparison(mli, !=, NULL, MidLevelInstructions*, void*);
	
	ARROW_ASSIGN(mli, mid_level_instructions) = (MidLevelInstruction**)m_alloc(sizeof(MidLevelInstruction*) * 50);
	
	unable_to_allocate_memory_if_null(ARROW(mli, mid_level_instructions));
	
	mli->used_length = 0;
	mli->allocated_length = 10;
}

static void create_data_for_instruction_if_necessary(MidLevelInstruction* cr instruction, DataAndCode* cr dac) {
	assert_comparison(instruction, !=, NULL, MidLevelInstruction* cr, void*);
	assert_comparison(dac, !=, NULL, DataAndCode* cr, void*);
	
	switch (ARROW(instruction, opcode)) {
		case MLI_OPCODE_MOV_FVAR_FVAL:
		case MLI_OPCODE_MUL_FVAR_FVAL:
		case MLI_OPCODE_DIV_FVAR_FVAL:
		case MLI_OPCODE_ADD_FVAR_FVAL:
		case MLI_OPCODE_SUB_FVAR_FVAL:
		case MLI_OPCODE_CMP_LE_FVAR_FVAL:
		case MLI_OPCODE_CMP_LT_FVAR_FVAL:
		case MLI_OPCODE_CMP_EQ_FVAR_FVAL:
		case MLI_OPCODE_CMP_NE_FVAR_FVAL:
		case MLI_OPCODE_CMP_GT_FVAR_FVAL:
		case MLI_OPCODE_CMP_GE_FVAR_FVAL: {
			OperandNode* operand_node = (OperandNode*)ARRAY_INDEX(instruction->arguments, 1);
			f value;
			
			assert_comparison(operand_node, !=, NULL, OperandNode*, void*);
			
			value = ARROW(operand_node, value).fvalue;
			
			if (value != 0.0 && value != 1.0) {
				void** new_value;
				
				if (hash_table_insert_with_f_key(&dac->float_literals, value, NULL, false /* replace? */, &new_value)) {
					f* floating_point_number = (f*)m_alloc(sizeof(f));
					
					linked_list_append(&dac->literals, floating_point_number);
					
					DEREF_ASSIGN(floating_point_number) = value;
					DEREF_ASSIGN(new_value) = floating_point_number;
				}
				
				ARROW_ASSIGN(operand_node, address_in_memory) = new_value;
			}
		}
		
		default: {
		}
	}
}

void mid_level_instructions_append(
	MidLevelInstructions* cr mli,
	MidLevelInstruction* cr instruction,
	DataAndCode* cr dac
) {
	assert_comparison(mli, !=, NULL, MidLevelInstructions*, void*);
	assert_comparison(instruction, !=, NULL, MidLevelInstruction*, void*);
	assert_comparison(dac, !=, NULL, DataAndCode*, void*);
	
	if (ARROW(mli, used_length) == ARROW(mli, allocated_length)) {
		ARROW_ASSIGN(mli, allocated_length) *= 2;
		
		ARROW_ASSIGN(mli, mid_level_instructions) =
			(MidLevelInstruction**)m_realloc(
				ARROW(mli, mid_level_instructions),
				sizeof(MidLevelInstruction*) * ARROW(mli, allocated_length)
			);
		
		unable_to_allocate_memory_if_null(ARROW(mli, mid_level_instructions));
	}
	
	ARRAY_INDEX_ASSIGN(ARROW(mli, mid_level_instructions), mli->used_length++) = instruction;
	
	create_data_for_instruction_if_necessary(instruction, dac);
}

MidLevelInstruction* mid_level_instructions_get_last_instruction(MidLevelInstructions* cr mli) {
	assert_comparison(mli, !=, NULL, MidLevelInstructions*, void*);
	assert_comparison(ARROW(mli, used_length), !=, 0, u, u);
	
	return ARRAY_INDEX(ARROW(mli, mid_level_instructions), ARROW(mli, used_length) - 1);
}

void mid_level_instructions_del(MidLevelInstructions* cr mli) {
	u i;
	
	assert_comparison(mli, !=, NULL, MidLevelInstructions*, void*);
	
	i = ARROW(mli, used_length);
	
	while (i--) {
		m_free(ARRAY_INDEX(ARROW(mli, mid_level_instructions), i));
	}
	
	m_free(ARROW(mli, mid_level_instructions));
}
