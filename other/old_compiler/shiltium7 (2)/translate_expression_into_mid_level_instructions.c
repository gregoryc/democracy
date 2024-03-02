#include "headers.h"

static inline void memory_allocation_error_if_null(const void* const restrict pointer) {
	if (unlikely(!pointer)) {
		output_nullt_string("Could not allocate memory to translate an expression into an intermediary form.\n");
		exit(1);
	}
}

attr_nonnull static void create_mov_instruction_for_first_node(
	Block* block, Variable* accumulator, OperandNode* operand_node, const bool should_free_variable
) {
	MidLevelInstruction* mid_level_instruction = (MidLevelInstruction*)m_alloc(sizeof(MidLevelInstruction));
	memory_allocation_error_if_null(mid_level_instruction);
	
	switch (operand_node->type) {
	case TYPE_UINT:
		mid_level_instruction->opcode = MLI_OPCODE_MOV_UVAR_UVAL;
		mid_level_instruction->argument1 = accumulator;
		mid_level_instruction->argument2 = operand_node;
		mid_level_instruction->should_free_argument2 = should_free_variable;
		break;
	case TYPE_SINT:
		output_nullt_string("SINT IN MUL AND DIV\n");
		exit(1);
	case TYPE_FLOAT:
		mid_level_instruction->opcode = MLI_OPCODE_MOV_FVAR_FVAL;
		mid_level_instruction->argument1 = accumulator;
		mid_level_instruction->argument2 = operand_node;
		mid_level_instruction->should_free_argument2 = should_free_variable;
		break;
	case TYPE_BOOL:
		output_nullt_string("BOOL IN MUL AND DIV\n");
		exit(1);
	case TYPE_VARIABLE:
		mid_level_instruction->opcode = MLI_OPCODE_MOV_FVAR_FVAR;
		mid_level_instruction->argument1 = accumulator;
		mid_level_instruction->argument2 = operand_node->variable;
		mid_level_instruction->should_free_argument2 = true;
		break;
	default:
		assert_comparison(0, !=, 0);
	}
	
	mid_level_instructions_append(&block->mid_level_instructions, mid_level_instruction);
}

static OperandNode* create_result_of_subexpression_operand_node(Variable* accumulator, OperandNode* previous_node, OperandNode* next_node) {
	OperandNode* operand_node = (OperandNode*)m_alloc(sizeof(OperandNode));
	memory_allocation_error_if_null(operand_node);
	
	/* These can be removed in the future as they're just here for safety. */
	operand_node->line_number = 0;
	operand_node->value.u64_value = 0;
	
	operand_node->type = TYPE_VARIABLE;
	operand_node->variable = accumulator;
	
	/* Link operand_node with the previous node. */
	operand_node->previous_node = previous_node;
	if (previous_node) {
		previous_node->next_node = operand_node;
	}
	
	/* Link operand_node with the next node. */
	operand_node->next_node = next_node;
	if (next_node) {
		next_node->previous_node = operand_node;
	}
	
	return operand_node;
}

attr_nonnull static bool /* reached the end of the entire expression? */
create_instructions_for_multiplication_and_division_subexpression(
	OperatorLinkedList* operators,
	OperandLinkedList* operands,
	Variable* accumulator,
	Block* block,
	OperandNode** operand_node,
	OperatorNode** operator_node
) {
	OperatorNode* operator_node_before_first_operator;
	OperandNode* first_operand_node_before_subexpression;
	
	assert_comparison(operators, !=, NULL);
	assert_comparison(operands, !=, NULL);
	assert_comparison(accumulator, !=, NULL);
	assert_comparison(block, !=, NULL);
	assert_comparison(operand_node, !=, NULL);
	assert_comparison(operator_node, !=, NULL);
	
	operator_node_before_first_operator = DEREF(operator_node)->previous_node;
	first_operand_node_before_subexpression = DEREF(operand_node)->previous_node->previous_node;
	
	create_mov_instruction_for_first_node(block, accumulator, DEREF(operand_node)->previous_node, true /* should free variable? */);
	
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
				mid_level_instruction->should_free_argument2 = true;
				break;
			case TYPE_SINT:
				output_nullt_string("SINT IN MUL AND DIV\n");
				exit(1);
			case TYPE_FLOAT:
				mid_level_instruction->opcode =
					(DEREF(operator_node)->operator_ == OPERATOR_MULTIPLICATION) ? MLI_OPCODE_MUL_FVAR_FVAL : MLI_OPCODE_DIV_FVAR_FVAL;
				mid_level_instruction->argument1 = accumulator;
				mid_level_instruction->argument2 = DEREF(operand_node);
				mid_level_instruction->should_free_argument2 = true;
				break;
			case TYPE_BOOL:
				output_nullt_string("BOOL IN MUL AND DIV\n");
				exit(1);
			case TYPE_VARIABLE:
				output_nullt_string("VARIABLE IN MUL AND DIV\n");
				exit(1);
		}
		
		mid_level_instructions_append(&block->mid_level_instructions, mid_level_instruction);
		
		{
			OperatorNode* old_operator_node = DEREF(operator_node);
			
			DEREF_ASSIGN(operator_node) = DEREF(operator_node)->next_node;
			DEREF_ASSIGN(operand_node) = DEREF(operand_node)->next_node;
			
			m_free(old_operator_node);
		}
	}
	
	{
		OperandNode* result = create_result_of_subexpression_operand_node(
			accumulator, first_operand_node_before_subexpression, DEREF(operand_node)
		);
		
		if (!first_operand_node_before_subexpression) {
			operands->first_node = result;
		}
	}
	
	if (operator_node_before_first_operator) {
		operator_node_before_first_operator->next_node = DEREF(operator_node);
	} else {
		operators->first_node = DEREF(operator_node);
	}
	
	accumulator->last_instruction_that_uses_this_variable = mid_level_instructions_get_last_instruction(&block->mid_level_instructions);
	
	/* Reached the end of the entire expression? */
	return !DEREF(operator_node);
}

__attribute__((nonnull))
static void translate_multiplication_and_division(
	DataAndCode* const restrict dac,
	Block* const restrict block,
	Variable* const restrict variable
) {
	OperandNode* operand_node = ARROW(variable, operands).first_node->next_node;
	OperatorNode* operator_node = ARROW(variable, operators).first_node;
	
	while (operand_node) {
		if (operator_node->operator_ == OPERATOR_MULTIPLICATION || operator_node->operator_ == OPERATOR_DIVISION) {
			Variable* accumulator_variable = (Variable*)m_alloc(sizeof(Variable));
			memory_allocation_error_if_null(accumulator_variable);
			
			accumulator_variable->name.string = NULL;
			accumulator_variable->name.length = 0;
			accumulator_variable->line_declared_on = 0;
			accumulator_variable->evaluated = true;
			
			dac->stack_offset += sizeof(u64);
			accumulator_variable->stack_offset = dac->stack_offset;
			
			/* Reached the end of the entire expression? */
			if (create_instructions_for_multiplication_and_division_subexpression(
				&variable->operators, &variable->operands, accumulator_variable, block, &operand_node, &operator_node)
			) {
				return;
			}
		}
		
		operand_node = operand_node->next_node;
		operator_node = operator_node->next_node;
	}
}

static void translate_addition_and_subtraction(
	DataAndCode* const restrict dac,
	Block* const restrict block,
	Variable* const restrict variable
) {
	OperandNode* operand_node;
	OperatorNode* operator_node;
	
	assert_comparison(block, !=, NULL);
	assert_comparison(variable, !=, NULL);
	
	operand_node = ARROW(variable, operands).first_node;
	operator_node = ARROW(variable, operators).first_node;
	
	ARROW_ASSIGN(dac, stack_offset) += sizeof(u64);
	ARROW_ASSIGN(variable, stack_offset) = ARROW(dac, stack_offset);
	
	create_mov_instruction_for_first_node(block, variable, operand_node, false /* should free variable? */);
	
	operand_node = ARROW(operand_node, next_node);
	
	while (operand_node) {
		MidLevelInstruction* mid_level_instruction;
		
		const Operator operator_ = ARROW(operator_node, operator_);
		
		assert_comparison(operator_ == OPERATOR_ADDITION || operator_ == OPERATOR_SUBTRACTION, ==, true);
		
		mid_level_instruction = (MidLevelInstruction*)m_alloc(sizeof(MidLevelInstruction));
		memory_allocation_error_if_null(mid_level_instruction);
		
		/*
		mid_level_instruction->should_free_argument2 equals false in all
		of these cases except the variable case because the operand node will
		be freed by operand_linked_list_del(). This will happen because the
		operator and operand nodes for addition and subtraction are not going
		to be removed from the operator and operand linked lists, because
		addition and subtraction have the lowest precedence, and therefore do
		not need to have their own subexpression. The reason it is true in the
		variable case is because argument2 in the variable case is not an
		operand node, but a variable which needs to be freed.
		*/
		switch (ARROW(operand_node, type)) {
		case TYPE_UINT:
			ARROW_ASSIGN(mid_level_instruction, opcode) = (operator_ == OPERATOR_ADDITION) ? MLI_OPCODE_ADD_UVAR_UVAL : MLI_OPCODE_SUB_UVAR_UVAL;
			ARROW_ASSIGN(mid_level_instruction, argument1) = variable;
			ARROW_ASSIGN(mid_level_instruction, argument2) = operand_node;
			ARROW_ASSIGN(mid_level_instruction, should_free_argument2) = false;
			break;
		case TYPE_SINT:
			output_nullt_string("SINT IN MUL AND DIV\n");
			exit(1);
		case TYPE_FLOAT:
			ARROW_ASSIGN(mid_level_instruction, opcode) = (operator_ == OPERATOR_ADDITION) ? MLI_OPCODE_ADD_FVAR_FVAL : MLI_OPCODE_SUB_FVAR_FVAL;
			ARROW_ASSIGN(mid_level_instruction, argument1) = variable;
			ARROW_ASSIGN(mid_level_instruction, argument2) = operand_node;
			ARROW_ASSIGN(mid_level_instruction, should_free_argument2) = false;
			break;
		case TYPE_BOOL:
			output_nullt_string("BOOL IN MUL AND DIV\n");
			exit(1);
		case TYPE_VARIABLE:
			ARROW_ASSIGN(mid_level_instruction, opcode) = (operator_ == OPERATOR_ADDITION) ? MLI_OPCODE_ADD_FVAR_FVAR : MLI_OPCODE_SUB_FVAR_FVAR;
			ARROW_ASSIGN(mid_level_instruction, argument1) = variable;
			ARROW_ASSIGN(mid_level_instruction, argument2) = operand_node->variable;
			ARROW_ASSIGN(mid_level_instruction, should_free_argument2) = true;
			break;
		}
		
		mid_level_instructions_append(&block->mid_level_instructions, mid_level_instruction);
		
		operand_node = operand_node->next_node;
		operator_node = operator_node->next_node;
	}
	
	variable->last_instruction_that_uses_this_variable = mid_level_instructions_get_last_instruction(&block->mid_level_instructions);
}

void translate_expression_into_mid_level_instructions(
	DataAndCode* const restrict dac,
	Block* const restrict block,
	Variable* const restrict variable
) {
	assert_comparison(dac, !=, NULL);
	assert_comparison(block, !=, NULL);
	assert_comparison(variable, !=, NULL);
	
	translate_multiplication_and_division(dac, block, variable);
	
	#if 0
	{
		OperatorNode* operator_node = ARROW(variable, operators).first_node;
		
		while (operator_node) {
			output_u64(operator_node->operator_);
			output_newline();
			operator_node = operator_node->next_node;
		}
		
		exit(1);
	}
	#endif
	
	translate_addition_and_subtraction(dac, block, variable);
}
