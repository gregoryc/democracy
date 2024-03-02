#include "headers.h"

/* Function prototypes */

static void get_length_of_data_section(
	DataAndCode* const restrict dac,
	MidLevelInstructions* const restrict mli,
	HashTable* const restrict string_and_float_literals_hash_table
) attr_nonnull;

static void allocate_memory_for_data_section(
	DataAndCode* const restrict dac,
	HashTable* const restrict string_and_float_literals_hash_table
) attr_nonnull;

static void set_memory_addresses_for_literals(
	DataAndCode* const restrict dac,
	HashTable* const restrict string_and_float_literals_hash_table
) attr_nonnull;

static void get_length_of_code_section(
	DataAndCode* const restrict dac,
	MidLevelInstructions* const restrict mli
) attr_nonnull;

static
#if DEBUG
	u64
#else
	void
#endif
allocate_memory_for_code_section(DataAndCode* const restrict dac) attr_nonnull;

static void put_instructions_into_code_section(
	DataAndCode* const restrict dac,
	MidLevelInstructions* const restrict mli
) attr_nonnull;

static u64 get_length_of_instructions_generated_by_store_variable_on_stack(const Variable* const restrict variable);
static void store_variable_on_stack(DataAndCode* const restrict dac, Variable* const restrict variable) attr_nonnull;


/* Function definitions */


#if DEBUG
	static void output_variable(Variable* const restrict variable) {
		assert_comparison(variable, !=, NULL);
		
		if (ARROW(variable, name).length) {
			output_string(ARROW(variable, name).string, ARROW(variable, name).length);
		} else {
			output_nullt_string("(internal compiler variable [stack offset: ");
			output_u64(ARROW(variable, stack_offset));
			output_nullt_string("])");
		}
	}
	
	static void output_mid_level_instructions(MidLevelInstructions* const restrict mli) {
		u64 i;
		
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
			"sub_fvar_fvar"
		};
		
		assert_comparison(mli, !=, NULL);
		
		output_nullt_string("\nMid-level instructions:\n");
		
		for (i = 0; i < ARROW(mli, used_length); ++i) {
			MidLevelInstruction* const instruction = ARRAY_INDEX(ARROW(mli, mid_level_instructions), i);
			
			assert_comparison(ARROW(instruction, opcode), <=, LAST_MLI_OPCODE);
			
			output_nullt_string(TAB_STRING);
			output_nullt_string(opcodes_to_strings[ARROW(instruction, opcode)]);
			
			switch (ARROW(instruction, opcode)) {
				case MLI_OPCODE_MOV_UVAR_UVAL:
				case MLI_OPCODE_MOV_FVAR_FVAL:
				case MLI_OPCODE_MOV_FVAR_FVAR:
				case MLI_OPCODE_MUL_UVAR_UVAL:
				case MLI_OPCODE_MUL_FVAR_FVAL:
				case MLI_OPCODE_DIV_UVAR_UVAL:
				case MLI_OPCODE_DIV_FVAR_FVAL:
				case MLI_OPCODE_ADD_UVAR_UVAL:
				case MLI_OPCODE_ADD_FVAR_FVAL:
				case MLI_OPCODE_ADD_UVAR_UVAR:
				case MLI_OPCODE_ADD_FVAR_FVAR:
				case MLI_OPCODE_SUB_UVAR_UVAL:
				case MLI_OPCODE_SUB_FVAR_FVAL:
				case MLI_OPCODE_SUB_UVAR_UVAR:
				case MLI_OPCODE_SUB_FVAR_FVAR: {
					Variable* variable = (Variable*)ARROW(instruction, argument1);
					output_char(' ');
					output_variable(variable);
				}
				default: {
				}
			}
			
			switch (ARROW(instruction, opcode)) {
				case MLI_OPCODE_MOV_UVAR_UVAL:
				case MLI_OPCODE_MUL_UVAR_UVAL:
				case MLI_OPCODE_DIV_UVAR_UVAL:
				case MLI_OPCODE_ADD_UVAR_UVAL:
				case MLI_OPCODE_SUB_UVAR_UVAL:
					output_nullt_string(", ");
					output_u64(ARROW((OperandNode*)ARROW(instruction, argument2), value).u64_value);
					break;
				
				case MLI_OPCODE_MOV_FVAR_FVAL:
				case MLI_OPCODE_MUL_FVAR_FVAL:
				case MLI_OPCODE_ADD_FVAR_FVAL:
				case MLI_OPCODE_DIV_FVAR_FVAL:
				case MLI_OPCODE_SUB_FVAR_FVAL:
					output_nullt_string(", ");
					output_f64(ARROW((OperandNode*)ARROW(instruction, argument2), value).f64_value);
					break;
				
				case MLI_OPCODE_MOV_FVAR_FVAR:
				case MLI_OPCODE_ADD_UVAR_UVAR:
				case MLI_OPCODE_ADD_FVAR_FVAR:
				case MLI_OPCODE_SUB_UVAR_UVAR:
				case MLI_OPCODE_SUB_FVAR_FVAR: {
					Variable* variable = (Variable*)ARROW(instruction, argument2);
					output_nullt_string(", ");
					output_variable(variable);
					break;
				}
				
				default: {
				}
			}
			
			output_newline();
		}
		
		output_newline();
	}
#endif

/* Function definitions */

void translate_mid_level_instructions_to_cpu_instructions(DataAndCode* const restrict dac, MidLevelInstructions* const restrict mli) {
	/* {string or float literal => addresses of the literal} */
	HashTable string_and_float_literals_hash_table;
	#if DEBUG
		/* The old code length, that is, the length of the code section before it gets rounded up to the next multiple of PAGE_SIZE. */
		u64 old_code_length;
		
		output_mid_level_instructions(mli);
	#endif
	
	hash_table_new(&string_and_float_literals_hash_table, 30, 8, 2.5);
	
	get_length_of_data_section(dac, mli, &string_and_float_literals_hash_table);
	allocate_memory_for_data_section(dac, &string_and_float_literals_hash_table);
	get_length_of_code_section(dac, mli);
	
	/* Yes, this might seem like an odd (or inelegant) use of the preproccessor, but it works ;) */
	#if DEBUG
		old_code_length =
	#endif
	allocate_memory_for_code_section(dac);
	
	put_instructions_into_code_section(dac, mli);
	
	#if DEBUG
		assert_comparison(ARROW(dac, current_instruction_offset), ==, old_code_length);
	#endif
	
	#if DEBUG
		/*
		Ensure that the literal (key) and the dereferenced address of the literal
		that the generated program will use (value) are equal.
		*/
		{
			HashIterator iterator;
			
			hash_iterator_init(&string_and_float_literals_hash_table, &iterator);
			
			while (hash_iterator_next(&iterator)) {
				assert_comparison(iterator.key.f64_value, ==, (*(f64*)(*iterator.value)));
			}
		}
	#endif
	
	#if DEBUG
	{
		HashIterator iterator;
		
		output_nullt_string("String and float literals hash table:\n");
		
		hash_iterator_init(&string_and_float_literals_hash_table, &iterator);
		
		output_nullt_string("{\n");
		
		while (hash_iterator_next(&iterator)) {
			output_nullt_string("   ");
			output_f64(iterator.key.f64_value);
			output_nullt_string(" => ");
			output_u64((u64)(*iterator.value));
			output_nullt_string("\n");
		}
		
		output_nullt_string("}\n");
	}
	#endif
	
	hash_table_del(&string_and_float_literals_hash_table);
}

/* This function gets the length of the data section, which is composed of floating point and string literals. */
static void get_length_of_data_section(
	DataAndCode* const restrict dac,
	MidLevelInstructions* const restrict mli,
	HashTable* const restrict string_and_float_literals_hash_table
) {
	u64 i;
	
	assert_comparison(dac, !=, NULL);
	assert_comparison(mli, !=, NULL);
	assert_comparison(string_and_float_literals_hash_table, !=, NULL);
	
	ARROW_ASSIGN(dac, data_length) = 0;
	
	for (i = 0; i < ARROW(mli, used_length); ++i) {
		MidLevelInstruction* const instruction = ARRAY_INDEX(ARROW(mli, mid_level_instructions), i);
		
		switch (ARROW(instruction, opcode)) {
			case MLI_OPCODE_MOV_FVAR_FVAL: {
				OperandNode* operand_node = (OperandNode*)ARROW(instruction, argument2);
				assert_comparison(operand_node, !=, NULL);
				
				if (ARROW(operand_node, value).f64_value == 0.0 || ARROW(operand_node, value).f64_value == 1.0) break;
				
				/* Fall through */
			}
			case MLI_OPCODE_MUL_FVAR_FVAL:
			case MLI_OPCODE_DIV_FVAR_FVAL:
			case MLI_OPCODE_ADD_FVAR_FVAL:
			case MLI_OPCODE_SUB_FVAR_FVAL: {
				OperandNode* operand_node = (OperandNode*)ARROW(instruction, argument2);
				assert_comparison(operand_node, !=, NULL);
				
				if (hash_table_insert_uint(string_and_float_literals_hash_table, ARROW(operand_node, value).u64_value, (void*)1, false)) {
					ARROW_ASSIGN(dac, data_length) += 8;
				}
				ARROW_ASSIGN(operand_node, address_in_memory) = hash_table_get_value_with_uint_key(string_and_float_literals_hash_table, ARROW(operand_node, value).u64_value);
				break;
			}
			default: {
			}
		}
	}
}

static void allocate_memory_for_data_section(
	DataAndCode* const restrict dac,
	HashTable* const restrict string_and_float_literals_hash_table
) {
	assert_comparison(dac, !=, NULL);
	assert_comparison(string_and_float_literals_hash_table, !=, NULL);
	
	if (ARROW(dac, data_length)) {
		ARROW_ASSIGN(dac, start_of_data) = m_alloc(ARROW(dac, data_length));
		
		assert_comparison((u64)ARROW(dac, start_of_data) % 8, ==, 0);
		
		if (unlikely(!ARROW(dac, start_of_data))) {
			output_nullt_string("Unable to allocate memory for floating point and string literals.\n");
			exit(1);
		}
		
		set_memory_addresses_for_literals(dac, string_and_float_literals_hash_table);
	}
}

static void set_memory_addresses_for_literals(
	DataAndCode* const restrict dac,
	HashTable* const restrict string_and_float_literals_hash_table
) {
	u64 data_section_offset;
	HashIterator iterator;
	
	assert_comparison(dac, !=, NULL);
	assert_comparison(string_and_float_literals_hash_table, !=, NULL);
	
	data_section_offset = 0;
	
	hash_iterator_init(string_and_float_literals_hash_table, &iterator);
	
	while (hash_iterator_next(&iterator)) {
		if (iterator.type_of_key == HASH_TYPE_UINT) {
			*iterator.value = ((char*)ARROW(dac, start_of_data)) + data_section_offset;
			*(f64*)(*iterator.value) = iterator.key.f64_value;
			data_section_offset += 8;
		} else {
			assert_comparison(iterator.type_of_key, !=, iterator.type_of_key);
		}
	}
	
	assert_comparison(data_section_offset, ==, ARROW(dac, data_length));
}

static void get_length_of_code_section(
	DataAndCode* const restrict dac,
	MidLevelInstructions* const restrict mli
) {
	u64 i;
	
	assert_comparison(dac, !=, NULL);
	assert_comparison(mli, !=, NULL);
	
	ARROW_ASSIGN(dac, code_length) = dac_get_length_of_starting_instructions();
	
	for (i = 0; i < ARROW(mli, used_length); ++i) {
		MidLevelInstruction* const instruction = ARRAY_INDEX(ARROW(mli, mid_level_instructions), i);
		
		switch (ARROW(instruction, opcode)) {
			case MLI_OPCODE_MOV_UVAR_UVAL: {
				/* FIXME */
				assert_comparison(0, !=, 0);
				break;
			}
			case MLI_OPCODE_MOV_FVAR_FVAL: {
				Variable* variable;
				OperandNode* operand_node;
				
				variable = (Variable*)ARROW(instruction, argument1);
				operand_node = (OperandNode*)ARROW(instruction, argument2);
				
				assert_comparison(variable, !=, NULL);
				assert_comparison(operand_node, !=, NULL);
				
				if (ARROW(operand_node, value).f64_value == 0.0) {
					ARROW_ASSIGN(dac, code_length) += dac_get_length_of_fldz();
				} else if (ARROW(operand_node, value).f64_value == 1.0) {
					ARROW_ASSIGN(dac, code_length) += dac_get_length_of_fld1();
				} else {
					ARROW_ASSIGN(dac, code_length) += dac_get_length_of_mov_reg_lit64(*(u64*)ARROW(operand_node, address_in_memory));
					ARROW_ASSIGN(dac, code_length) += dac_get_length_of_fld_deref_register_as_qword(REG_R11);
					
					if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
						ARROW_ASSIGN(dac, code_length) += get_length_of_instructions_generated_by_store_variable_on_stack(variable);
					}
				}
				
				break;
			}
			case MLI_OPCODE_MUL_FVAR_FVAL: {
				Variable* variable;
				OperandNode* operand_node;
				
				variable = (Variable*)ARROW(instruction, argument1);
				operand_node = (OperandNode*)ARROW(instruction, argument2);
				
				assert_comparison(variable, !=, NULL);
				assert_comparison(operand_node, !=, NULL);
				
				ARROW_ASSIGN(dac, code_length) += dac_get_length_of_mov_reg_lit64(*(u64*)ARROW(operand_node, address_in_memory));
				ARROW_ASSIGN(dac, code_length) += dac_get_length_of_fmul_deref_register_as_qword(REG_R11);
				
				if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
					ARROW_ASSIGN(dac, code_length) += get_length_of_instructions_generated_by_store_variable_on_stack(variable);
				}
				
				break;
			}
			case MLI_OPCODE_MOV_FVAR_FVAR: {
				Variable* variable1;
				Variable* variable2;
				
				variable1 = (Variable*)ARROW(instruction, argument1);
				variable2 = (Variable*)ARROW(instruction, argument2);
				
				assert_comparison(variable1, !=, NULL);
				assert_comparison(variable2, !=, NULL);
				
				{
					const u64 stack_offset = ARROW(variable2, stack_offset);
					
					assert_comparison(stack_offset, !=, 0);
					
					ARROW_ASSIGN(dac, code_length) += dac_get_length_of_mov_reg_reg();
					ARROW_ASSIGN(dac, code_length) += dac_get_length_of_mov_reg_lit64(stack_offset);
					ARROW_ASSIGN(dac, code_length) += dac_get_length_of_sub_reg_reg();
					ARROW_ASSIGN(dac, code_length) += dac_get_length_of_fld_deref_register_as_qword(REG_R11);
				}
				
				if (instruction == ARROW(variable1, last_instruction_that_uses_this_variable)) {
					ARROW_ASSIGN(dac, code_length) += get_length_of_instructions_generated_by_store_variable_on_stack(variable1);
				}
				
				break;
			}
			case MLI_OPCODE_ADD_FVAR_FVAL: {
				Variable* variable;
				OperandNode* operand_node;
				
				variable = (Variable*)ARROW(instruction, argument1);
				operand_node = (OperandNode*)ARROW(instruction, argument2);
				
				assert_comparison(variable, !=, NULL);
				assert_comparison(operand_node, !=, NULL);
				
				ARROW_ASSIGN(dac, code_length) += dac_get_length_of_mov_reg_lit64(*(u64*)ARROW(operand_node, address_in_memory));
				ARROW_ASSIGN(dac, code_length) += dac_get_length_of_fadd_deref_register_as_qword(REG_R11);
				
				if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
					ARROW_ASSIGN(dac, code_length) += get_length_of_instructions_generated_by_store_variable_on_stack(variable);
				}
				
				break;
			}
			case MLI_OPCODE_ADD_FVAR_FVAR: {
				Variable* variable1;
				Variable* variable2;
				
				variable1 = (Variable*)ARROW(instruction, argument1);
				variable2 = (Variable*)ARROW(instruction, argument2);
				
				assert_comparison(variable1, !=, NULL);
				assert_comparison(variable2, !=, NULL);
				
				{
					const u64 stack_offset = ARROW(variable2, stack_offset);
					
					assert_comparison(stack_offset, !=, 0);
					
					ARROW_ASSIGN(dac, code_length) += dac_get_length_of_mov_reg_reg();
					ARROW_ASSIGN(dac, code_length) += dac_get_length_of_mov_reg_lit64(stack_offset);
					ARROW_ASSIGN(dac, code_length) += dac_get_length_of_sub_reg_reg();
					ARROW_ASSIGN(dac, code_length) += dac_get_length_of_fadd_deref_register_as_qword(REG_R11);
				}
				
				if (instruction == ARROW(variable1, last_instruction_that_uses_this_variable)) {
					ARROW_ASSIGN(dac, code_length) += get_length_of_instructions_generated_by_store_variable_on_stack(variable1);
				}
				
				break;
			}
			case MLI_OPCODE_SUB_FVAR_FVAL: {
				Variable* variable;
				OperandNode* operand_node;
				
				variable = (Variable*)ARROW(instruction, argument1);
				operand_node = (OperandNode*)ARROW(instruction, argument2);
				
				assert_comparison(variable, !=, NULL);
				assert_comparison(operand_node, !=, NULL);
				
				ARROW_ASSIGN(dac, code_length) += dac_get_length_of_mov_reg_lit64(*(u64*)ARROW(operand_node, address_in_memory));
				ARROW_ASSIGN(dac, code_length) += dac_get_length_of_fsub_deref_register_as_qword(REG_R11);
				
				if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
					ARROW_ASSIGN(dac, code_length) += get_length_of_instructions_generated_by_store_variable_on_stack(variable);
				}
				
				break;
			}
			case MLI_OPCODE_SUB_FVAR_FVAR: {
				Variable* variable1;
				Variable* variable2;
				
				variable1 = (Variable*)ARROW(instruction, argument1);
				variable2 = (Variable*)ARROW(instruction, argument2);
				
				assert_comparison(variable1, !=, NULL);
				assert_comparison(variable2, !=, NULL);
				
				{
					const u64 stack_offset = ARROW(variable2, stack_offset);
					
					assert_comparison(stack_offset, !=, 0);
					
					ARROW_ASSIGN(dac, code_length) += dac_get_length_of_mov_reg_reg();
					ARROW_ASSIGN(dac, code_length) += dac_get_length_of_mov_reg_lit64(stack_offset);
					ARROW_ASSIGN(dac, code_length) += dac_get_length_of_sub_reg_reg();
					ARROW_ASSIGN(dac, code_length) += dac_get_length_of_fsub_deref_register_as_qword(REG_R11);
				}
				
				if (instruction == ARROW(variable1, last_instruction_that_uses_this_variable)) {
					ARROW_ASSIGN(dac, code_length) += get_length_of_instructions_generated_by_store_variable_on_stack(variable1);
				}
				
				break;
			}
			case MLI_OPCODE_DIV_FVAR_FVAL:
				/* FIXME */
				
				assert_comparison(0, !=, 0);
				break;
			default:
				assert_comparison(ARROW(instruction, opcode), !=, ARROW(instruction, opcode));
		}
	}
	
	ARROW_ASSIGN(dac, code_length) += dac_get_length_of_ending_instructions();
}

attr_nonnull
static
#if DEBUG
	u64
#else
	void
#endif
allocate_memory_for_code_section(DataAndCode* const restrict dac) {
	u64 old_code_length;
	
	assert_comparison(dac, !=, NULL);
	
	old_code_length = ARROW(dac, code_length);
	
	ARROW_ASSIGN(dac, start_of_code) = (char*)m_allocate_writable_and_executable_memory(dac, ARROW(dac, code_length));
	if (unlikely(ARROW(dac, start_of_code) == (void*)-1)) {
		exit(1);
	}
	
	#if DEBUG
		return old_code_length;
	#endif
}

static void put_instructions_into_code_section(
	DataAndCode* const restrict dac,
	MidLevelInstructions* const restrict mli
) {
	u64 i;
	
	dac_append_starting_instructions(dac);
	
	for (i = 0; i < ARROW(mli, used_length); ++i) {
		MidLevelInstruction* const instruction = ARRAY_INDEX(ARROW(mli, mid_level_instructions), i);
		
		switch (ARROW(instruction, opcode)) {
			case MLI_OPCODE_MOV_UVAR_UVAL: {
				Variable* variable = (Variable*)ARROW(instruction, argument1);
				OperandNode* operand_node = (OperandNode*)ARROW(instruction, argument2);
				
				(void)variable;
				(void)operand_node;
				
				#if DEBUG
					output_nullt_string("mov uresult, ");
					output_u64(ARROW(operand_node, value).u64_value);
					output_newline();
				#endif
				break;
			}
			case MLI_OPCODE_MOV_FVAR_FVAL: {
				Variable* variable = (Variable*)ARROW(instruction, argument1);
				OperandNode* operand_node = (OperandNode*)ARROW(instruction, argument2);
				
				(void)variable;
				
				#if DEBUG
					output_nullt_string(TAB_STRING "; ");
					output_variable(variable);
					output_nullt_string(" = ");
					output_f64(ARROW(operand_node, value).f64_value);
					output_newline();
				#endif
				
				if (ARROW(operand_node, value).f64_value == 0.0) {
					dac_append_fldz(dac);
				} else if (ARROW(operand_node, value).f64_value == 1.0) {
					dac_append_fld1(dac);
				} else {
					dac_append_mov_reg_lit64(dac, REG_R11, *(u64*)ARROW(operand_node, address_in_memory));
					dac_append_fld_deref_register_as_qword(dac, REG_R11);
				}
				
				if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
					store_variable_on_stack(dac, variable);
				}
				
				break;
			}
			case MLI_OPCODE_MOV_FVAR_FVAR: {
				Variable* variable1 = (Variable*)ARROW(instruction, argument1);
				Variable* variable2 = (Variable*)ARROW(instruction, argument2);
				
				#if DEBUG
					output_nullt_string(TAB_STRING "; ");
					output_variable(variable1);
					output_nullt_string(" = ");
					output_variable(variable2);
					output_newline();
				#endif
				
				{
					const u64 stack_offset = ARROW(variable2, stack_offset);
					
					assert_comparison(stack_offset, !=, 0);
					
					dac_append_mov_reg_reg(dac, REG_R11, REG_RSP);
					dac_append_mov_reg_lit64(dac, REG_R12, stack_offset);
					dac_append_sub_reg_reg(dac, REG_R11, REG_R12);
					dac_append_fld_deref_register_as_qword(dac, REG_R11);
				}
				
				if (instruction == ARROW(variable1, last_instruction_that_uses_this_variable)) {
					store_variable_on_stack(dac, variable1);
				}
				break;
			}
			case MLI_OPCODE_MUL_UVAR_UVAL: {
				/* FIXME */
				assert_comparison(0, !=, 0);
				
				break;
			}
			case MLI_OPCODE_MUL_FVAR_FVAL: {
				Variable* variable = (Variable*)ARROW(instruction, argument1);
				OperandNode* operand_node = (OperandNode*)ARROW(instruction, argument2);
				
				#if DEBUG
					output_nullt_string(TAB_STRING "; ");
					output_variable(variable);
					output_nullt_string(" *= ");
					output_f64(ARROW(operand_node, value).f64_value);
					output_newline();
				#endif
				
				dac_append_mov_reg_lit64(dac, REG_R11, DEREF((u64*)ARROW(operand_node, address_in_memory)));
				dac_append_fmul_deref_register_as_qword(dac, REG_R11);
				if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
					store_variable_on_stack(dac, variable);
				}
				break;
			}
			case MLI_OPCODE_DIV_UVAR_UVAL: {
				assert_comparison(0, !=, 0);
				break;
			}
			case MLI_OPCODE_DIV_FVAR_FVAL: {
				assert_comparison(0, !=, 0);
				break;
			}
			case MLI_OPCODE_ADD_UVAR_UVAL: {
				assert_comparison(0, !=, 0);
				break;
			}
			case MLI_OPCODE_ADD_FVAR_FVAL: {
				Variable* variable = (Variable*)ARROW(instruction, argument1);
				OperandNode* operand_node = (OperandNode*)ARROW(instruction, argument2);
				
				#if DEBUG
					output_nullt_string(TAB_STRING "; ");
					output_variable(variable);
					output_nullt_string(" += ");
					output_f64(ARROW(operand_node, value).f64_value);
					output_newline();
				#endif
				
				dac_append_mov_reg_lit64(dac, REG_R11, DEREF((u64*)ARROW(operand_node, address_in_memory)));
				dac_append_fadd_deref_register_as_qword(dac, REG_R11);
				if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
					store_variable_on_stack(dac, variable);
				}
				
				break;
			}
			case MLI_OPCODE_ADD_UVAR_UVAR: {
				assert_comparison(0, !=, 0);
				break;
			}
			case MLI_OPCODE_ADD_FVAR_FVAR: {
				Variable* variable1 = (Variable*)ARROW(instruction, argument1);
				Variable* variable2 = (Variable*)ARROW(instruction, argument2);
				
				#if DEBUG
					output_nullt_string(TAB_STRING "; ");
					output_variable(variable1);
					output_nullt_string(" += ");
					output_variable(variable2);
					output_newline();
				#endif
				
				{
					const u64 stack_offset = ARROW(variable2, stack_offset);
					
					assert_comparison(stack_offset, !=, 0);
					
					dac_append_mov_reg_reg(dac, REG_R11, REG_RSP);
					dac_append_mov_reg_lit64(dac, REG_R12, stack_offset);
					dac_append_sub_reg_reg(dac, REG_R11, REG_R12);
					dac_append_fadd_deref_register_as_qword(dac, REG_R11);
				}
				
				if (instruction == ARROW(variable1, last_instruction_that_uses_this_variable)) {
					store_variable_on_stack(dac, variable1);
				}
				
				break;
			}
			case MLI_OPCODE_SUB_UVAR_UVAL: {
				assert_comparison(0, !=, 0);
				break;
			}
			case MLI_OPCODE_SUB_FVAR_FVAL: {
				Variable* variable = (Variable*)ARROW(instruction, argument1);
				OperandNode* operand_node = (OperandNode*)ARROW(instruction, argument2);
				
				#if DEBUG
					output_nullt_string(TAB_STRING "; ");
					output_variable(variable);
					output_nullt_string(" -= ");
					output_f64(ARROW(operand_node, value).f64_value);
					output_newline();
				#endif
				
				dac_append_mov_reg_lit64(dac, REG_R11, DEREF((u64*)ARROW(operand_node, address_in_memory)));
				dac_append_fsub_deref_register_as_qword(dac, REG_R11);
				if (instruction == ARROW(variable, last_instruction_that_uses_this_variable)) {
					store_variable_on_stack(dac, variable);
				}
				
				break;
			}
			case MLI_OPCODE_SUB_UVAR_UVAR: {
				assert_comparison(0, !=, 0);
				break;
			}
			case MLI_OPCODE_SUB_FVAR_FVAR: {
				Variable* variable1 = (Variable*)ARROW(instruction, argument1);
				Variable* variable2 = (Variable*)ARROW(instruction, argument2);
				
				#if DEBUG
					output_nullt_string(TAB_STRING "; ");
					output_variable(variable1);
					output_nullt_string(" -= ");
					output_variable(variable2);
					output_newline();
				#endif
				
				{
					const u64 stack_offset = ARROW(variable2, stack_offset);
					
					assert_comparison(stack_offset, !=, 0);
					
					dac_append_mov_reg_reg(dac, REG_R11, REG_RSP);
					dac_append_mov_reg_lit64(dac, REG_R12, stack_offset);
					dac_append_sub_reg_reg(dac, REG_R11, REG_R12);
					dac_append_fsub_deref_register_as_qword(dac, REG_R11);
				}
				
				if (instruction == ARROW(variable1, last_instruction_that_uses_this_variable)) {
					store_variable_on_stack(dac, variable1);
				}
				
				break;
			}
			default:
				assert_comparison(ARROW(instruction, opcode), !=, ARROW(instruction, opcode));
		}
	}
	
	dac_append_ending_instructions(dac);
}

/* Get the length of the instructions generated by store_variable_on_stack() */
static u64 get_length_of_instructions_generated_by_store_variable_on_stack(
	const Variable* const restrict variable
) {
	assert_comparison(variable, !=, NULL);
	
	{
		const u64 stack_offset = ARROW(variable, stack_offset);
		assert_comparison(stack_offset, !=, 0);
		
		return
			dac_get_length_of_mov_reg_reg() +
			dac_get_length_of_mov_reg_lit64(stack_offset) +
			dac_get_length_of_sub_reg_reg() +
			dac_get_length_of_fstp_deref_register_as_qword(REG_R11);
	}
}

static void store_variable_on_stack(DataAndCode* const restrict dac, Variable* const restrict variable) {
	assert_comparison(dac, !=, NULL);
	assert_comparison(variable, !=, NULL);
	
	{
		const u64 stack_offset = ARROW(variable, stack_offset);
		assert_comparison(stack_offset, !=, 0);
		
		output_nullt_string(TAB_STRING "; store st0 on the stack\n");
		
		dac_append_mov_reg_reg(dac, REG_R11, REG_RSP);
		dac_append_mov_reg_lit64(dac, REG_R12, stack_offset);
		dac_append_sub_reg_reg(dac, REG_R11, REG_R12);
		dac_append_fstp_deref_register_as_qword(dac, REG_R11);
		output_newline();
	}
}
