/*
Functions that translate an expression into mid-level instructions.

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
This file contains functions that translate an expression into a
mid-level instruction (i.e., an intermediary compiler instruction).
*/

#include "../global_headers/headers.h"

static inline void memory_allocation_error_if_null(const void* cr pointer) {
	if (unlikely(!pointer)) {
		output_nullt_string("Could not allocate memory to translate an expression into an intermediary form.\n");
		exit_program();
	}
}

attr_nonnull static u get_next_stack_offset(u* cr stack_offset) {
	u old_stack_offset;
	
	assert_comparison(stack_offset, !=, NULL, u* cr, void*);
	assert_comparison(DEREF(stack_offset) % sizeof(f), ==, 0, u, u);
	
	old_stack_offset = DEREF(stack_offset);
	DEREF_ASSIGN(stack_offset) += sizeof(f);
	assert_comparison(DEREF(stack_offset) % sizeof(f), ==, 0, u, u);
	
	return old_stack_offset;
}

attr_nonnull static void create_mov_instruction_for_first_node(
	MidLevelInstructions* cr mid_level_instructions,
	Variable* cr accumulator,
	DataAndCode* cr dac,
	OperandNode* cr operand_node,
	u* cr stack_offset
) {
	MidLevelInstruction* mid_level_instruction;
	
	assert_comparison(mid_level_instructions, !=, NULL, MidLevelInstructions* cr, void*);
	assert_comparison(accumulator, !=, NULL, Variable* cr, void*);
	assert_comparison(dac, !=, NULL, DataAndCode* cr, void*);
	assert_comparison(operand_node, !=, NULL, OperandNode* cr, void*);
	
	mid_level_instruction = (MidLevelInstruction*)m_alloc(sizeof(MidLevelInstruction));
	memory_allocation_error_if_null(mid_level_instruction);
	
	switch (ARROW(operand_node, type)) {
		case TYPE_UINT:
			mid_level_instruction->opcode = MLI_OPCODE_MOV_UVAR_UVAL;
			mid_level_instruction->argument1 = accumulator;
			mid_level_instruction->argument2 = operand_node;
			break;
		case TYPE_SINT:
			output_nullt_string("SINT IN MUL AND DIV\n");
			exit_program();
		case TYPE_FLOAT:
			mid_level_instruction->opcode = MLI_OPCODE_MOV_FVAR_FVAL;
			mid_level_instruction->argument1 = accumulator;
			mid_level_instruction->argument2 = operand_node;
			break;
		case TYPE_BOOL:
			output_nullt_string("BOOL IN MUL AND DIV\n");
			exit_program();
		case TYPE_VARIABLE:
			mid_level_instruction->opcode = MLI_OPCODE_MOV_FVAR_FVAR;
			mid_level_instruction->argument1 = accumulator;
			
			if (!ARROW(ARROW(operand_node, variable), evaluated)) {
				translate_expression_into_mid_level_instructions(dac, mid_level_instructions, ARROW(operand_node, variable), stack_offset);
			}
			mid_level_instruction->argument2 = ARROW(operand_node, variable);
			break;
	}
	
	mid_level_instructions_append(mid_level_instructions, mid_level_instruction, dac);
}

static void create_accumulator_variable(
	DataAndCode* cr dac,
	MidLevelInstructions* cr mid_level_instructions,
	Variable** cr accumulator_variable,
	OperandNode* cr operand_node,
	u* cr stack_offset
) {
	assert_comparison(dac, !=, NULL, DataAndCode* cr, void*);
	assert_comparison(mid_level_instructions, !=, NULL, MidLevelInstructions* cr, void*);
	assert_comparison(accumulator_variable, !=, NULL, Variable** cr, void*);
	
	DEREF_ASSIGN(accumulator_variable) = (Variable*)m_alloc(sizeof(Variable));
	
	if (unlikely(!DEREF(accumulator_variable))) {
		output_nullt_string(
			"Memory error: Unable to allocate memory for a temporary variable needed to store the result of an expression with a unary operator in it.\n"
		);
		
		exit_program();
	}
	
	ARROW_ASSIGN(DEREF(accumulator_variable), name).string = NULL;
	ARROW_ASSIGN(DEREF(accumulator_variable), name).length = 0;
	ARROW_ASSIGN(DEREF(accumulator_variable), line_declared_on) = 0;
	ARROW_ASSIGN(DEREF(accumulator_variable), evaluated) = true;
	ARROW_ASSIGN(DEREF(accumulator_variable), stack_offset) = get_next_stack_offset(stack_offset);
	
	create_mov_instruction_for_first_node(mid_level_instructions, DEREF(accumulator_variable), dac, operand_node, stack_offset);
}

__attribute__((nonnull (1, 2)))
static void create_variable_operand_node(
	Variable* restrict accumulator,
	OperandLinkedList* cr linked_list,
	OperandNode* previous_node,
	OperandNode* next_node
) {
	OperandNode* operand_node;
	
	assert_comparison(accumulator, !=, NULL, Variable* restrict, void*);
	assert_comparison(linked_list, !=, NULL, OperandLinkedList* cr, void*);
	
	#if DEBUG
		if (!(previous_node == NULL && next_node == NULL)) {
			assert_comparison(previous_node, !=, next_node, OperandNode*, OperandNode*);
		}
	#endif
	
	operand_node = (OperandNode*)m_alloc(sizeof(OperandNode));
	memory_allocation_error_if_null(operand_node);
	
	operand_node->type = TYPE_VARIABLE;
	operand_node->variable = accumulator;
	
	/* Link operand_node with the previous node. */
	operand_node->previous_node = previous_node;
	if (previous_node) {
		previous_node->next_node = operand_node;
	} else {
		linked_list->first_node = operand_node;
	}
	
	/* Link operand_node with the next node. */
	operand_node->next_node = next_node;
	if (next_node) {
		next_node->previous_node = operand_node;
	} else {
		linked_list->last_node = operand_node;
	}
}

#if 0
attr_nonnull void output_operands(OperandLinkedList* restrict operands) {
	OperandNode* restrict operand_node;
	
	assert_comparison(operands, !=, NULL, OperandNode*, void*);
	
	operand_node = ARROW(operands, first_node);
	
	output_nullt_string("[\n   (Operands)\n\n");
	
	while (operand_node) {
		output_nullt_string("   ");
		switch (ARROW(operand_node, type)) {
		case TYPE_FLOAT:
			putf(ARROW(operand_node, value).fvalue);
			break;
		case TYPE_VARIABLE:
			output_nullt_string("Variable");
			break;
		default:
			assert_comparison(0, !=, 0, u, u);
		}
		output_newline();
		
		operand_node = ARROW(operand_node, next_node);
	}
	
	output_nullt_string("]\n\n");
}

attr_nonnull void output_operators(OperatorLinkedList* restrict operators) {
	OperatorNode* restrict operator_node;
	
	assert_comparison(operators, !=, NULL, OperatorNode*, void*);
	
	operator_node = ARROW(operators, first_node);
	
	output_nullt_string("[\n   (Operators)\n\n");
	
	while (operator_node) {
		extern const char* cr operators_to_strings[];
		
		output_nullt_string("   ");
		output_nullt_string(operators_to_strings[operator_node->operator_]);
		if (unlikely(ARROW(operator_node, is_unary))) {
			output_nullt_string(" (unary)");
		} else {
			output_nullt_string(" (binary)");
		}
		
		output_newline();
		
		operator_node = ARROW(operator_node, next_node);
	}
	
	output_nullt_string("]\n\n");
}
#endif

attr_nonnull static bool /* reached the end of the entire expression? */
create_instructions_for_multiplication_and_division_subexpression(
	OperatorLinkedList* operators,
	OperandLinkedList* operands,
	Variable* accumulator,
	MidLevelInstructions* cr mid_level_instructions,
	DataAndCode* cr dac,
	OperandNode** operand_node,
	OperatorNode** operator_node,
	u* cr stack_offset
) {
	OperatorNode* operator_node_before_first_operator;
	OperandNode* first_operand_node_before_subexpression;
	OperandNode* previous_operand_node;
	OperatorNode* previous_operator_node;
	OperandNode* first_operand_node;
	OperatorNode* first_operator_node;
	
	assert_comparison(operators, !=, NULL, OperatorLinkedList*, void*);
	assert_comparison(operands, !=, NULL, OperandLinkedList*, void*);
	assert_comparison(accumulator, !=, NULL, Variable*, void*);
	assert_comparison(mid_level_instructions, !=, NULL, MidLevelInstructions*, void*);
	assert_comparison(operand_node, !=, NULL, OperandNode*, void*);
	assert_comparison(operator_node, !=, NULL, OperatorNode*, void*);
	
	first_operator_node = DEREF(operator_node);
	operator_node_before_first_operator = ARROW(first_operator_node, previous_node);
	
	first_operand_node = ARROW(DEREF(operand_node), previous_node);
	first_operand_node_before_subexpression = ARROW(first_operand_node, previous_node);
	
	create_mov_instruction_for_first_node(
		mid_level_instructions,
		accumulator,
		dac,
		ARROW(DEREF(operand_node), previous_node),
		stack_offset
	);
	
	while (
		DEREF(operator_node) /* not yet at the end of the entire expression */ &&
		(DEREF(operator_node)->operator_ == OPERATOR_MULTIPLICATION || DEREF(operator_node)->operator_ == OPERATOR_DIVISION)
	) {
		MidLevelInstruction* mid_level_instruction = (MidLevelInstruction*)m_alloc(sizeof(MidLevelInstruction));
		memory_allocation_error_if_null(mid_level_instruction);
		
		switch (DEREF(operand_node)->type) {
			case TYPE_UINT:
				mid_level_instruction->opcode =
					(DEREF(operator_node)->operator_ == OPERATOR_MULTIPLICATION) ? MLI_OPCODE_MUL_UVAR_UVAL : MLI_OPCODE_DIV_UVAR_UVAL;
				mid_level_instruction->argument1 = accumulator;
				mid_level_instruction->argument2 = DEREF(operand_node);
				break;
			case TYPE_SINT:
				output_nullt_string("SINT IN MUL AND DIV\n");
				exit_program();
			case TYPE_FLOAT:
				mid_level_instruction->opcode =
					(DEREF(operator_node)->operator_ == OPERATOR_MULTIPLICATION) ? MLI_OPCODE_MUL_FVAR_FVAL : MLI_OPCODE_DIV_FVAR_FVAL;
				mid_level_instruction->argument1 = accumulator;
				mid_level_instruction->argument2 = DEREF(operand_node);
				break;
			case TYPE_BOOL:
				output_nullt_string("BOOL IN MUL AND DIV\n");
				exit_program();
			case TYPE_VARIABLE:
				output_nullt_string("VARIABLE IN MUL AND DIV\n");
				exit_program();
		}
		
		mid_level_instructions_append(mid_level_instructions, mid_level_instruction, dac);
		
		previous_operand_node = DEREF(operand_node);
		previous_operator_node = DEREF(operator_node);
		
		DEREF_ASSIGN(operator_node) = DEREF(operator_node)->next_node;
		DEREF_ASSIGN(operand_node) = DEREF(operand_node)->next_node;
		
		/* m_free(previous_operator_node); */
	}
	
	ARROW_ASSIGN(first_operand_node, previous_node) = NULL;
	ARROW_ASSIGN(previous_operand_node, next_node) = NULL;
	ARROW_ASSIGN(accumulator, operands).first_node = first_operand_node;
	ARROW_ASSIGN(accumulator, operands).last_node = previous_operand_node;
	
	ARROW_ASSIGN(first_operator_node, previous_node) = NULL;
	ARROW_ASSIGN(previous_operator_node, next_node) = NULL;
	ARROW_ASSIGN(accumulator, operators).first_node = first_operator_node;
	ARROW_ASSIGN(accumulator, operators).last_node = previous_operator_node;
	
	create_variable_operand_node(
		accumulator, operands, first_operand_node_before_subexpression, DEREF(operand_node)
	);
	
	if (operator_node_before_first_operator) {
		/*
		This is what the following line of code does:
		
		    next_node
		    -----------------↘
		2.0 + 5.0 * 3.0 * 4.0 + 6.0
		
		The above is an example expression.
		*/
		operator_node_before_first_operator->next_node = DEREF(operator_node);
	} else {
		/*
		This brach will be executed if the expression is something like this:
		2.0 * 3.0 + 4.0
		
		This statement sets the operator linked list's first_node
		pointer to point to the plus operator node (or whatever
		operator node is there; this is only an example) after
		the multiplication and/or division subexpression.
		*/
		
		operators->first_node = DEREF(operator_node);
	}
	
	if (DEREF(operator_node)) {
		ARROW_ASSIGN(DEREF(operator_node), previous_node) = NULL;
	}
	
	accumulator->last_instruction_that_uses_this_variable = mid_level_instructions_get_last_instruction(mid_level_instructions);
	
	/* Reached the end of the entire expression? */
	return !DEREF(operator_node);
}

attr_nonnull
static void translate_multiplication_and_division_subexpressions(
	DataAndCode* cr dac,
	MidLevelInstructions* cr block,
	Variable* cr variable,
	u* cr stack_offset
) {
	OperandNode* operand_node = ARROW(variable, operands).first_node->next_node;
	OperatorNode* operator_node = ARROW(variable, operators).first_node;
	
	while (operand_node) {
		if (ARROW(operator_node, operator_) == OPERATOR_MULTIPLICATION || ARROW(operator_node, operator_) == OPERATOR_DIVISION) {
			Variable* accumulator_variable = (Variable*)m_alloc(sizeof(Variable));
			memory_allocation_error_if_null(accumulator_variable);
			
			accumulator_variable->name.string = NULL;
			accumulator_variable->name.length = 0;
			accumulator_variable->line_declared_on = 0;
			accumulator_variable->evaluated = true;
			
			ARROW_ASSIGN(accumulator_variable, stack_offset) = get_next_stack_offset(stack_offset);
			
			/* Reached the end of the entire expression? */
			if (
				create_instructions_for_multiplication_and_division_subexpression(
					&variable->operators,
					&variable->operands,
					accumulator_variable,
					block,
					dac,
					&operand_node,
					&operator_node,
					stack_offset
				)
			) {
				return;
			}
		}
		
		operand_node = operand_node->next_node;
		operator_node = operator_node->next_node;
	}
}

attr_nonnull static bool /* reached the end of the entire expression? */
create_instructions_for_addition_and_subtraction_subexpression(
	OperatorLinkedList* operators,
	OperandLinkedList* operands,
	Variable* accumulator,
	MidLevelInstructions* cr mid_level_instructions,
	DataAndCode* cr dac,
	OperandNode** operand_node,
	OperatorNode** operator_node,
	u* cr stack_offset
) {
	OperatorNode* operator_node_before_first_operator;
	OperandNode* first_operand_node_before_subexpression;
	OperandNode* previous_operand_node;
	OperatorNode* previous_operator_node;
	OperandNode* first_operand_node;
	OperatorNode* first_operator_node;
	
	assert_comparison(operators, !=, NULL, OperatorLinkedList*, void*);
	assert_comparison(operands, !=, NULL, OperandLinkedList*, void*);
	assert_comparison(accumulator, !=, NULL, Variable*, void*);
	assert_comparison(mid_level_instructions, !=, NULL, MidLevelInstructions*, void*);
	assert_comparison(operand_node, !=, NULL, OperandNode*, void*);
	assert_comparison(operator_node, !=, NULL, OperatorNode*, void*);
	
	first_operator_node = DEREF(operator_node);
	operator_node_before_first_operator = ARROW(first_operator_node, previous_node);
	
	first_operand_node = ARROW(DEREF(operand_node), previous_node);
	first_operand_node_before_subexpression = ARROW(first_operand_node, previous_node);
	
	create_mov_instruction_for_first_node(
		mid_level_instructions,
		accumulator,
		dac,
		ARROW(DEREF(operand_node), previous_node),
		stack_offset
	);
	
	while (
		DEREF(operator_node) /* not yet at the end of the entire expression */ &&
		(DEREF(operator_node)->operator_ == OPERATOR_ADDITION || DEREF(operator_node)->operator_ == OPERATOR_SUBTRACTION)
	) {
		MidLevelInstruction* mid_level_instruction = (MidLevelInstruction*)m_alloc(sizeof(MidLevelInstruction));
		memory_allocation_error_if_null(mid_level_instruction);
		
		switch (DEREF(operand_node)->type) {
			case TYPE_UINT:
				mid_level_instruction->opcode =
					(DEREF(operator_node)->operator_ == OPERATOR_ADDITION) ? MLI_OPCODE_ADD_UVAR_UVAL : MLI_OPCODE_SUB_UVAR_UVAL;
				mid_level_instruction->argument1 = accumulator;
				mid_level_instruction->argument2 = DEREF(operand_node);
				break;
			case TYPE_SINT:
				output_nullt_string("SINT IN ADD AND SUB\n");
				exit_program();
			case TYPE_FLOAT:
				mid_level_instruction->opcode =
					(DEREF(operator_node)->operator_ == OPERATOR_ADDITION) ? MLI_OPCODE_ADD_FVAR_FVAL : MLI_OPCODE_SUB_FVAR_FVAL;
				mid_level_instruction->argument1 = accumulator;
				mid_level_instruction->argument2 = DEREF(operand_node);
				break;
			case TYPE_BOOL:
				output_nullt_string("BOOL IN MUL AND DIV\n");
				exit_program();
			case TYPE_VARIABLE:
				ARROW_ASSIGN(mid_level_instruction, opcode) =
					(DEREF(operator_node)->operator_ == OPERATOR_ADDITION) ? MLI_OPCODE_ADD_FVAR_FVAR : MLI_OPCODE_SUB_FVAR_FVAR;
				ARROW_ASSIGN(mid_level_instruction, argument1) = accumulator;
				ARROW_ASSIGN(mid_level_instruction, argument2) = ARROW(DEREF(operand_node), variable);
		}
		
		mid_level_instructions_append(mid_level_instructions, mid_level_instruction, dac);
		
		previous_operand_node = DEREF(operand_node);
		previous_operator_node = DEREF(operator_node);
		
		DEREF_ASSIGN(operator_node) = DEREF(operator_node)->next_node;
		DEREF_ASSIGN(operand_node) = DEREF(operand_node)->next_node;
		
		/* m_free(previous_operator_node); */
	}
	
	ARROW_ASSIGN(first_operand_node, previous_node) = NULL;
	ARROW_ASSIGN(previous_operand_node, next_node) = NULL;
	ARROW_ASSIGN(accumulator, operands).first_node = first_operand_node;
	ARROW_ASSIGN(accumulator, operands).last_node = previous_operand_node;
	
	ARROW_ASSIGN(first_operator_node, previous_node) = NULL;
	ARROW_ASSIGN(previous_operator_node, next_node) = NULL;
	ARROW_ASSIGN(accumulator, operators).first_node = first_operator_node;
	ARROW_ASSIGN(accumulator, operators).last_node = previous_operator_node;
	
	create_variable_operand_node(accumulator, operands, first_operand_node_before_subexpression, DEREF(operand_node));
	
	if (operator_node_before_first_operator) {
		/*
		This is what the following line of code does:
		
		    next_node
		    -----------------↘
		2.0 + 5.0 * 3.0 * 4.0 + 6.0
		
		The above is an example expression.
		*/
		operator_node_before_first_operator->next_node = DEREF(operator_node);
	} else {
		/*
		This brach will be executed if the expression is something like this:
		2.0 * 3.0 + 4.0
		
		This statement sets the operator linked list's first_node
		pointer to point to the plus operator node (or whatever
		operator node is there; this is only an example) after
		the multiplication and/or division subexpression.
		*/
		
		operators->first_node = DEREF(operator_node);
	}
	
	if (DEREF(operator_node)) {
		ARROW_ASSIGN(DEREF(operator_node), previous_node) = NULL;
	}
	
	accumulator->last_instruction_that_uses_this_variable = mid_level_instructions_get_last_instruction(mid_level_instructions);
	
	/* Reached the end of the entire expression? */
	return !DEREF(operator_node);
}

attr_nonnull
static void translate_addition_and_subtraction_subexpressions(
	DataAndCode* cr dac,
	MidLevelInstructions* cr mid_level_instructions,
	Variable* cr variable,
	u* cr stack_offset
) {
	OperandNode* operand_node = ARROW(ARROW(variable, operands).first_node, next_node);
	OperatorNode* operator_node = ARROW(variable, operators).first_node;
	
	while (operand_node) {
		if (operator_node->operator_ == OPERATOR_ADDITION || operator_node->operator_ == OPERATOR_SUBTRACTION) {
			Variable* accumulator_variable = (Variable*)m_alloc(sizeof(Variable));
			memory_allocation_error_if_null(accumulator_variable);
			
			accumulator_variable->name.string = NULL;
			accumulator_variable->name.length = 0;
			accumulator_variable->line_declared_on = 0;
			accumulator_variable->evaluated = true;
			
			ARROW_ASSIGN(accumulator_variable, stack_offset) = get_next_stack_offset(stack_offset);
			
			/* Reached the end of the entire expression? */
			if (
				create_instructions_for_addition_and_subtraction_subexpression(
					&variable->operators,
					&variable->operands,
					accumulator_variable,
					mid_level_instructions,
					dac,
					&operand_node,
					&operator_node,
					stack_offset
				)
			) {
				return;
			}
		}
		
		operand_node = operand_node->next_node;
		operator_node = operator_node->next_node;
	}
}

static void translate_comparison_subexpressions(
	DataAndCode* cr dac,
	MidLevelInstructions* cr mid_level_instructions,
	Variable* cr variable,
	u* cr stack_offset
) {
	OperandNode* operand_node;
	OperatorNode* operator_node;
	
	assert_comparison(mid_level_instructions, !=, NULL, MidLevelInstructions*, void*);
	assert_comparison(variable, !=, NULL, Variable*, void*);
	
	operand_node = ARROW(variable, operands).first_node;
	operator_node = ARROW(variable, operators).first_node;
	
	create_mov_instruction_for_first_node(mid_level_instructions, variable, dac, operand_node, stack_offset);
	
	operand_node = ARROW(operand_node, next_node);
	
	while (operand_node) {
		MidLevelInstruction* mid_level_instruction;
		
		const Operator operator_ = ARROW(operator_node, operator_);
		
		assert_comparison(
			operator_ == OPERATOR_LESS_THAN_OR_EQUAL_TO ||
			operator_ == OPERATOR_LESS_THAN ||
			operator_ == OPERATOR_EQUALS ||
			operator_ == OPERATOR_NOT_EQUAL ||
			operator_ == OPERATOR_GREATER_THAN ||
			operator_ == OPERATOR_GREATER_THAN_OR_EQUAL_TO,
			==,
			true,
			bool, bool
		);
		
		mid_level_instruction = (MidLevelInstruction*)m_alloc(sizeof(MidLevelInstruction));
		memory_allocation_error_if_null(mid_level_instruction);
		
		switch (ARROW(operand_node, type)) {
		case TYPE_UINT:
			switch (operator_) {
				case OPERATOR_LESS_THAN_OR_EQUAL_TO:
					ARROW_ASSIGN(mid_level_instruction, opcode) = MLI_OPCODE_CMP_LE_UVAR_UVAL;
					break;
				case OPERATOR_LESS_THAN:
					ARROW_ASSIGN(mid_level_instruction, opcode) = MLI_OPCODE_CMP_LT_UVAR_UVAL;
					break;
				case OPERATOR_EQUALS:
					ARROW_ASSIGN(mid_level_instruction, opcode) = MLI_OPCODE_CMP_EQ_UVAR_UVAL;
					break;
				case OPERATOR_NOT_EQUAL:
					ARROW_ASSIGN(mid_level_instruction, opcode) = MLI_OPCODE_CMP_NE_UVAR_UVAL;
					break;
				case OPERATOR_GREATER_THAN:
					ARROW_ASSIGN(mid_level_instruction, opcode) = MLI_OPCODE_CMP_GT_UVAR_UVAL;
					break;
				case OPERATOR_GREATER_THAN_OR_EQUAL_TO:
					ARROW_ASSIGN(mid_level_instruction, opcode) = MLI_OPCODE_CMP_GE_UVAR_UVAL;
					break;
				default:
					assert_comparison(ARROW(operand_node, type), !=, ARROW(operand_node, type), Operator, Operator);
			}
			
			ARROW_ASSIGN(mid_level_instruction, opcode) = (operator_ == OPERATOR_ADDITION) ? MLI_OPCODE_ADD_UVAR_UVAL : MLI_OPCODE_SUB_UVAR_UVAL;
			ARROW_ASSIGN(mid_level_instruction, argument1) = variable;
			ARROW_ASSIGN(mid_level_instruction, argument2) = operand_node;
			
			break;
		case TYPE_SINT:
			output_nullt_string("SINT IN MUL AND DIV\n");
			exit_program();
		case TYPE_FLOAT:
			switch (operator_) {
				case OPERATOR_LESS_THAN_OR_EQUAL_TO:
					ARROW_ASSIGN(mid_level_instruction, opcode) = MLI_OPCODE_CMP_LE_FVAR_FVAL;
					break;
				case OPERATOR_LESS_THAN:
					ARROW_ASSIGN(mid_level_instruction, opcode) = MLI_OPCODE_CMP_LT_FVAR_FVAL;
					break;
				case OPERATOR_EQUALS:
					ARROW_ASSIGN(mid_level_instruction, opcode) = MLI_OPCODE_CMP_EQ_FVAR_FVAL;
					break;
				case OPERATOR_NOT_EQUAL:
					ARROW_ASSIGN(mid_level_instruction, opcode) = MLI_OPCODE_CMP_NE_FVAR_FVAL;
					break;
				case OPERATOR_GREATER_THAN:
					ARROW_ASSIGN(mid_level_instruction, opcode) = MLI_OPCODE_CMP_GT_FVAR_FVAL;
					break;
				case OPERATOR_GREATER_THAN_OR_EQUAL_TO:
					ARROW_ASSIGN(mid_level_instruction, opcode) = MLI_OPCODE_CMP_GE_FVAR_FVAL;
					break;
				default:
					assert_comparison(ARROW(operand_node, type), !=, ARROW(operand_node, type), Operator, Operator);
			}
			ARROW_ASSIGN(mid_level_instruction, argument1) = variable;
			ARROW_ASSIGN(mid_level_instruction, argument2) = operand_node;
			
			break;
		case TYPE_BOOL:
			output_nullt_string("BOOL IN MUL AND DIV\n");
			exit_program();
		case TYPE_VARIABLE:
			switch (operator_) {
				case OPERATOR_LESS_THAN_OR_EQUAL_TO:
					ARROW_ASSIGN(mid_level_instruction, opcode) = MLI_OPCODE_CMP_LE_FVAR_FVAR;
					break;
				case OPERATOR_LESS_THAN:
					ARROW_ASSIGN(mid_level_instruction, opcode) = MLI_OPCODE_CMP_LT_FVAR_FVAR;
					break;
				case OPERATOR_EQUALS:
					ARROW_ASSIGN(mid_level_instruction, opcode) = MLI_OPCODE_CMP_EQ_FVAR_FVAR;
					break;
				case OPERATOR_NOT_EQUAL:
					ARROW_ASSIGN(mid_level_instruction, opcode) = MLI_OPCODE_CMP_NE_FVAR_FVAR;
					break;
				case OPERATOR_GREATER_THAN:
					ARROW_ASSIGN(mid_level_instruction, opcode) = MLI_OPCODE_CMP_GT_FVAR_FVAR;
					break;
				case OPERATOR_GREATER_THAN_OR_EQUAL_TO:
					ARROW_ASSIGN(mid_level_instruction, opcode) = MLI_OPCODE_CMP_GE_FVAR_FVAR;
					break;
				default:
					assert_comparison(ARROW(operand_node, type), !=, ARROW(operand_node, type), Operator, Operator);
			}
			ARROW_ASSIGN(mid_level_instruction, argument1) = variable;
			ARROW_ASSIGN(mid_level_instruction, argument2) = operand_node->variable;
			
			break;
		}
		
		mid_level_instructions_append(mid_level_instructions, mid_level_instruction, dac);
		
		operand_node = ARROW(operand_node, next_node);
		operator_node = ARROW(operator_node, next_node);
	}
	
	variable->last_instruction_that_uses_this_variable = mid_level_instructions_get_last_instruction(mid_level_instructions);
}

static void create_change_sign_mid_level_instruction(
	DataAndCode* cr dac,
	MidLevelInstructions* cr mid_level_instructions,
	Variable* cr accumulator_variable
) {
	MidLevelInstruction* mid_level_instruction;
	
	assert_comparison(dac, !=, NULL, DataAndCode* cr, void*);
	assert_comparison(mid_level_instructions, !=, NULL, MidLevelInstructions* cr, void*);
	assert_comparison(accumulator_variable, !=, NULL, Variable* cr, void*);
	
	mid_level_instruction = (MidLevelInstruction*)m_alloc(sizeof(MidLevelInstruction));
	
	if (unlikely(!mid_level_instruction)) {
		output_nullt_string("Unable to allocate memory for an intermediary compiler instruction.\n");
		exit_program();
	}
	
	ARROW_ASSIGN(mid_level_instruction, opcode) = MLI_OPCODE_CHANGE_SIGN;
	
	ARROW_ASSIGN(mid_level_instruction, argument1) = accumulator_variable;
	mid_level_instructions_append(mid_level_instructions, mid_level_instruction, dac);
	
	ARROW_ASSIGN(accumulator_variable, last_instruction_that_uses_this_variable) = mid_level_instruction;
}

attr_nonnull
static void go_through_unary_operators_for_current_operand(
	DataAndCode* cr dac,
	MidLevelInstructions* cr mid_level_instructions,
	OperatorNode** operator_node_in_calling_function,
	OperandNode* operand_node,
	OperandLinkedList* cr operands,
	u* cr stack_offset
) {
	OperatorNode* first_operator_node;
	OperatorNode* operator_node;
	Variable* accumulator_variable = NULL;
	
	assert_comparison(dac, !=, NULL, DataAndCode* cr, void*);
	assert_comparison(mid_level_instructions, !=, NULL, MidLevelInstructions* cr, void*);
	assert_comparison(operator_node_in_calling_function, !=, NULL, OperatorNode**, void*);
	assert_comparison(operand_node, !=, NULL, OperandNode** , void*);
	assert_comparison(operands, !=, NULL, OperandLinkedList*, void*);
	
	first_operator_node = operator_node = DEREF(operator_node_in_calling_function);
	
	while (1) {
		if (!ARROW(operator_node, next_node)) break;
		if (!ARROW(ARROW(operator_node, next_node), is_unary)) break;
		operator_node = ARROW(operator_node, next_node);
	}
	
	assert_comparison(operator_node, !=, NULL, OperatorNode*, void*);
	
	DEREF_ASSIGN(operator_node_in_calling_function) = ARROW(operator_node, next_node);
	
	while (1) {
		switch (ARROW(operator_node, operator_)) {
			case OPERATOR_ADDITION:
				/* FIXME */
				break;
			case OPERATOR_SUBTRACTION:
				if (!accumulator_variable) {
					create_accumulator_variable(dac, mid_level_instructions, &accumulator_variable, operand_node, stack_offset);
					assert_comparison(accumulator_variable, !=, NULL, Variable*, void*);
				}
				
				create_change_sign_mid_level_instruction(dac, mid_level_instructions, accumulator_variable);
				break;
			case OPERATOR_LESS_THAN:
				output_nullt_string("\nunary " OPENING_QUOTE "<" CLOSING_QUOTE " in expression\n");
				exit_program();
			case OPERATOR_GREATER_THAN:
				output_nullt_string("\nunary " OPENING_QUOTE ">" CLOSING_QUOTE " in expression\n");
				exit_program();
			default:
				assert_comparison(ARROW(operator_node, operator_), !=, ARROW(operator_node, operator_), Operator, Operator);
				break;
		}
		
		if (!operator_node || operator_node == first_operator_node) break;
		operator_node = ARROW(operator_node, previous_node);
	}
	
	if (accumulator_variable) {
		create_variable_operand_node(accumulator_variable, operands, ARROW(operand_node, previous_node), ARROW(operand_node, next_node));
	}
}

static void translate_unary_operators(
	DataAndCode* cr dac,
	MidLevelInstructions* cr mid_level_instructions,
	Variable* cr variable,
	u* cr stack_offset
) {
	OperandNode* operand_node;
	OperatorNode* operator_node;
	
	assert_comparison(dac, !=, NULL, DataAndCode* cr, void*);
	assert_comparison(mid_level_instructions, !=, NULL, MidLevelInstructions* cr, void*);
	assert_comparison(variable, !=, NULL, Variable* cr, void*);
	
	operand_node = ARROW(variable, operands).first_node;
	operator_node = ARROW(variable, operators).first_node;
	
	while (operator_node) {
		if (ARROW(operator_node, is_unary)) {
			go_through_unary_operators_for_current_operand(dac, mid_level_instructions, &operator_node, operand_node, &variable->operands, stack_offset);
			continue;
		} else {
			operand_node = ARROW(operand_node, next_node);
		}
		
		operator_node = ARROW(operator_node, next_node);
	}
}

void translate_expression_into_mid_level_instructions(
	DataAndCode* cr dac,
	MidLevelInstructions* cr mli,
	Variable* cr variable,
	u* restrict stack_offset
) {
	assert_comparison(dac, !=, NULL, DataAndCode*, void*);
	assert_comparison(mli, !=, NULL, MidLevelInstructions*, void*);
	assert_comparison(variable, !=, NULL, Variable*, void*);
	
	ARROW_ASSIGN(variable, stack_offset) = get_next_stack_offset(stack_offset);
	
	translate_unary_operators(dac, mli, variable, stack_offset);
	translate_multiplication_and_division_subexpressions(dac, mli, variable, stack_offset);
	translate_addition_and_subtraction_subexpressions(dac, mli, variable, stack_offset);
	translate_comparison_subexpressions(dac, mli, variable, stack_offset);
	
	ARROW_ASSIGN(variable, evaluated) = true;
}
