/*
This file is licensed under the WTFPL which is found in the LICENSE file.
*/

#include "headers.h"

static const char* const restrict types_to_strings[4] = {"u", "s", "f", "b"};

typedef enum {
	TYPE_OF_LAST_OPERATOR_OR_OPERAND_APPEND___OPERATOR,
	TYPE_OF_LAST_OPERATOR_OR_OPERAND_APPEND___OPERAND,
	TYPE_OF_LAST_OPERATOR_OR_OPERAND_APPEND___NONE
} TypeOfAppend;

const char* const restrict operators_to_strings[6] = {"+", "-", "*", "/", "**", "_/"};

static inline u64 u64_pow(u64 base, u64 exponent) {
	return pow(base, exponent);
}

static inline u64 u64_root(u64 root, u64 base) {
	if (!root) return 0;
	return pow(base, 1.0 / root);
}

static inline f64 f64_root(f64 base, f64 exponent) {
	return pow(base, 1.0 / exponent);
}

attr_nonnull static inline void operand_linked_list_init(OperandLinkedList* linked_list) {
	assert_comparison(linked_list, !=, NULL);
	
	ARROW_ASSIGN(linked_list, first_node) = NULL;
	ARROW_ASSIGN(linked_list, last_node) = NULL;
}

attr_nonnull static inline void operator_linked_list_init(OperatorLinkedList* linked_list) {
	assert_comparison(linked_list, !=, NULL);
	
	ARROW_ASSIGN(linked_list, first_node) = NULL;
	ARROW_ASSIGN(linked_list, last_node) = NULL;
}

static inline void operand_linked_list_append(OperandLinkedList* linked_list, const char* string, u64 length, u64 line_number) {
	OperandNode* node;
	
	assert_comparison(linked_list, !=, NULL);
	assert_comparison(string, !=, NULL);
	assert_comparison(length, >, 0);
	
	if (likely((u64)linked_list->first_node)) {
		node = (OperandNode*)m_alloc(sizeof(OperandNode));
		node->string = string;
		node->string_length = length;
		node->line_number = line_number;
		node->previous_node = linked_list->last_node;
		node->next_node = NULL;
		linked_list->last_node->next_node = node;
		linked_list->last_node = node;
	} else {
		linked_list->first_node = (OperandNode*)m_alloc(sizeof(OperandNode));
		linked_list->first_node->string = string;
		linked_list->first_node->string_length = length;
		linked_list->first_node->line_number = line_number;
		linked_list->first_node->previous_node = NULL;
		linked_list->first_node->next_node = NULL;
		linked_list->last_node = linked_list->first_node;
	}
}

static inline void operator_linked_list_append(OperatorLinkedList* linked_list, Operator operator_, u64 line_number) {
	OperatorNode* node;
	
	assert_comparison(linked_list, !=, NULL);
	
	if (likely((u64)linked_list->first_node)) {
		node = (OperatorNode*)m_alloc(sizeof(OperatorNode));
		node->operator_ = operator_;
		node->line_number = line_number;
		node->previous_node = linked_list->last_node;
		node->next_node = NULL;
		linked_list->last_node->next_node = node;
		linked_list->last_node = node;
	} else {
		linked_list->first_node = (OperatorNode*)m_alloc(sizeof(OperatorNode));
		linked_list->first_node->operator_ = operator_;
		linked_list->first_node->line_number = line_number;
		linked_list->first_node->previous_node = NULL;
		linked_list->first_node->next_node = NULL;
		linked_list->last_node = linked_list->first_node;
	}
}

__attribute__((nonnull))
static void operator_linked_list_delete_node(OperatorLinkedList* linked_list, OperatorNode* operator_node) {
	assert_comparison(operator_node, !=, NULL);
	
	if (ARROW(operator_node, previous_node)) {
		ARROW_ASSIGN(ARROW(operator_node, previous_node), next_node) = ARROW(operator_node, next_node);
	}
	
	if (ARROW(operator_node, next_node)) {
		ARROW_ASSIGN(ARROW(operator_node, next_node), previous_node) = ARROW(operator_node, previous_node);
	}
	
	if (ARROW(linked_list, first_node) == operator_node) {
		ARROW_ASSIGN(linked_list, first_node) = ARROW(operator_node, next_node);
	}
	
	if (ARROW(linked_list, last_node) == operator_node) {
		ARROW_ASSIGN(linked_list, last_node) = ARROW(operator_node, previous_node);
	}
	
	m_free(operator_node);
}

void operand_linked_list_del(OperandLinkedList* linked_list) {
	OperandNode* node;
	
	assert_comparison(linked_list, !=, NULL);
	node = linked_list->first_node;
	
	while (node) {
		OperandNode* next_node = node->next_node;
		m_free(node);
		node = next_node;
	}
}

void operator_linked_list_del(OperatorLinkedList* linked_list) {
	OperatorNode* node;
	
	assert_comparison(linked_list, !=, NULL);
	node = linked_list->first_node;
	
	while (node) {
		OperatorNode* next_node = node->next_node;
		m_free(node);
		node = next_node;
	}
}

__attribute__((nonnull)) static void get_type(OperandNode* operand_node) {
	u64 i;
	
	assert_comparison(operand_node, !=, NULL);
	
	for (i = 0; i < operand_node->string_length; ++i) {
		if (ARRAY_INDEX(operand_node->string, i) == '.') {
			operand_node->type = TYPE_FLOAT;
			return;
		}
	}
	
	if (is_valid_identifier(operand_node->string, operand_node->string_length)) {
		operand_node->type = TYPE_VARIABLE;
		return;
	}
	
	operand_node->type = TYPE_UINT;
}

__attribute__((nonnull, warn_unused_result))
static ErrorNumber get_value(OperandNode* const operand_node, Block* const block) {
	ErrorNumber error_number = ERROR_NUMBER_NO_ERROR;
	
	assert_comparison(operand_node, !=, NULL);
	
	switch (operand_node->type) {
	case TYPE_UINT:
		operand_node->value.u64_value = convert_literal_to_uint(
			operand_node->string, operand_node->string_length, operand_node->line_number, &error_number);
		break;
	case TYPE_SINT:
		output_nullt_string("SINT\n");
		exit(1);
	case TYPE_FLOAT:
		operand_node->value.f64_value = convert_literal_to_float(
			operand_node->string, operand_node->string_length, operand_node->line_number, &error_number);
		break;
	case TYPE_BOOL:
		output_nullt_string("BOOL\n");
		exit(1);
	case TYPE_VARIABLE: {
		Variable* variable = (Variable*)(*hash_table_get_value_with_string_key(&block->variables_in_scope, operand_node->string, operand_node->string_length));
		if (variable) {
			operand_node->variable = variable;
			#if 0
			operand_node->type_of_the_variable_that_this_operand_node_references = variable->type;
			if (variable->is_value_known) {
				switch (variable->type) {
				case TYPE_UINT:
					operand_node->value.u64_value = variable->value.u64_value;
				case TYPE_SINT:
					output_nullt_string("SINT\n");
					exit(1);
				case TYPE_FLOAT:
					operand_node->value.f64_value = variable->value.f64_value;
					break;
				case TYPE_BOOL:
					output_nullt_string("BOOL\n");
					exit(1);
				case TYPE_VARIABLE:
					assert_comparison(0, !=, 0, u64);
				}
			}
			#endif
		} else {
			output_nullt_string("Name error on line ");
			output_u64(operand_node->line_number);
			output_nullt_string(": ‘");
			output_string(operand_node->string, operand_node->string_length);
			output_nullt_string("’ undeclared\n");
			exit(1);
		}
		break;
	}
	#if DEBUG
	default:
		assert_comparison(operand_node->type, !=, operand_node->type);
	#endif
	}
	
	return error_number;
}

__attribute__((nonnull))
static void replace_two_operand_nodes_with_one_value(
	OperandLinkedList* restrict linked_list,
	OperandNode* restrict node1,
	OperandNode* restrict node2,
	TypeUnion value
) {
	OperandNode* node_after_the_other_two_nodes = node2->next_node;
	
	assert_comparison(node1, !=, NULL);
	assert_comparison(node2, !=, NULL);
	assert_comparison(node1, !=, node2);
	
	node1->next_node = node_after_the_other_two_nodes;
	if (node_after_the_other_two_nodes) {
		node_after_the_other_two_nodes->previous_node = node1;
	} else {
		linked_list->last_node = node1;
	}
	
	m_free(node2);
	
	node1->value.u64_value = value.u64_value;
}

__attribute__((nonnull))
static void ensure_types_are_equal_otherwise_give_type_error(OperandNode* node1, OperandNode* node2, OperatorNode* operator_node) {
	assert_comparison(node1, !=, NULL);
	assert_comparison(node2, !=, NULL);
	assert_comparison(operator_node, !=, NULL);
	assert_comparison(node1, !=, node2);
	
	if (node1->type != node2->type) {
		output_nullt_string("Type error on line ");
		output_u64(operator_node->line_number);
		output_nullt_string(": invalid operands for binary ‘");
		output_nullt_string(ARRAY_INDEX(operators_to_strings, operator_node->operator_));
		output_nullt_string("’: ");
		output_nullt_string(ARRAY_INDEX(types_to_strings, node1->type));
		output_nullt_string(" and ");
		output_nullt_string(ARRAY_INDEX(types_to_strings, node2->type));
		output_char('\n');
		exit(1);
	}
}

__attribute__((nonnull)) static void constant_folding_for_exponentiation_and_roots(OperatorLinkedList* operators, OperandLinkedList* operands) {
	OperandNode* operand_node = operands->first_node;
	OperatorNode* operator_node = operators->first_node;
	TypeUnion value;
	
	while (operator_node) {
		ensure_types_are_equal_otherwise_give_type_error(operand_node, operand_node->next_node, operator_node);
		
		switch (operator_node->operator_) {
		case OPERATOR_EXPONENTIATION: {
			switch (operand_node->type) {
			case TYPE_UINT: {
				OperatorNode* next_node = operator_node->next_node;
				
				value.u64_value = u64_pow(operand_node->value.u64_value, operand_node->next_node->value.u64_value);
				
				replace_two_operand_nodes_with_one_value(operands, operand_node, operand_node->next_node, value);
				operator_linked_list_delete_node(operators, operator_node);
				operator_node = next_node;
				break;
			}
			case TYPE_SINT:
				output_nullt_string("sint ** sint\n");
				exit(1);
			case TYPE_FLOAT: {
				OperatorNode* next_node = operator_node->next_node;
				
				value.f64_value = pow(operand_node->value.f64_value, operand_node->next_node->value.f64_value);
				
				replace_two_operand_nodes_with_one_value(operands, operand_node, operand_node->next_node, value);
				operator_linked_list_delete_node(operators, operator_node);
				operator_node = next_node;
				break;
			}
			case TYPE_BOOL:
				output_nullt_string("bool ** bool\n");
				exit(1);
			case TYPE_VARIABLE:
				output_nullt_string("variable ** variable\n");
				exit(1);
			}
			
			break;
		}
		case OPERATOR_ROOTS:
			switch (operand_node->type) {
			case TYPE_UINT: {
				OperatorNode* next_node = operator_node->next_node;
				
				value.u64_value = u64_root(operand_node->value.u64_value, operand_node->next_node->value.u64_value);
				
				replace_two_operand_nodes_with_one_value(operands, operand_node, operand_node->next_node, value);
				operator_linked_list_delete_node(operators, operator_node);
				operator_node = next_node;
				break;
			}
			case TYPE_SINT:
				output_nullt_string("sint _/ sint\n");
				exit(1);
			case TYPE_FLOAT: {
				OperatorNode* next_node = operator_node->next_node;
				
				value.f64_value = f64_root(operand_node->next_node->value.f64_value, operand_node->value.f64_value);
				
				replace_two_operand_nodes_with_one_value(operands, operand_node, operand_node->next_node, value);
				operator_linked_list_delete_node(operators, operator_node);
				operator_node = next_node;
				break;
			}
			case TYPE_BOOL:
				output_nullt_string("bool _/ bool\n");
				exit(1);
			case TYPE_VARIABLE:
				output_nullt_string("variable _/ variable\n");
				exit(1);
			}
			
			break;
		default:
			operand_node = operand_node->next_node;
			operator_node = operator_node->next_node;
		}
	}
}

__attribute__((nonnull)) static void constant_folding_for_multiplication_and_division(OperatorLinkedList* operators, OperandLinkedList* operands) {
	OperandNode* operand_node = operands->first_node;
	OperatorNode* operator_node = operators->first_node;
	TypeUnion value;
	
	while (operator_node) {
		ensure_types_are_equal_otherwise_give_type_error(operand_node, operand_node->next_node, operator_node);
		
		switch (operator_node->operator_) {
		case OPERATOR_MULTIPLICATION:
			switch (operand_node->type) {
			case TYPE_UINT: {
				OperatorNode* next_node = operator_node->next_node;
				
				value.u64_value = operand_node->value.u64_value * operand_node->next_node->value.u64_value;
				
				replace_two_operand_nodes_with_one_value(operands, operand_node, operand_node->next_node, value);
				operator_linked_list_delete_node(operators, operator_node);
				operator_node = next_node;
				break;
			}
			case TYPE_SINT:
				output_nullt_string("sint * sint");
				exit(1);
			case TYPE_FLOAT: {
				OperatorNode* next_node = operator_node->next_node;
				
				value.f64_value = operand_node->value.f64_value * operand_node->next_node->value.f64_value;
				
				replace_two_operand_nodes_with_one_value(operands, operand_node, operand_node->next_node, value);
				operator_linked_list_delete_node(operators, operator_node);
				operator_node = next_node;
				break;
			}
			case TYPE_BOOL:
				output_nullt_string("bool * bool\n");
				exit(1);
			case TYPE_VARIABLE:
				output_nullt_string("variable * variable\n");
				exit(1);
			}
			
			break;
		case OPERATOR_DIVISION:
			switch (operand_node->type) {
			case TYPE_UINT: {
				OperatorNode* next_node = operator_node->next_node;
				
				/* Unsigned division by zero */
				if (!operand_node->next_node->value.u64_value) {
					output_nullt_string("Math error on line ");
					output_u64(operand_node->next_node->line_number);
					output_nullt_string(": unsigned division by zero\n");
					exit(1);
				}
				
				value.u64_value = operand_node->value.u64_value / operand_node->next_node->value.u64_value;
				
				replace_two_operand_nodes_with_one_value(operands, operand_node, operand_node->next_node, value);
				operator_linked_list_delete_node(operators, operator_node);
				operator_node = next_node;
				break;
			}
			case TYPE_SINT:
				output_nullt_string("sint / sint");
				exit(1);
			case TYPE_FLOAT: {
				OperatorNode* next_node = operator_node->next_node;
				
				value.f64_value = operand_node->value.f64_value / operand_node->next_node->value.f64_value;
				
				replace_two_operand_nodes_with_one_value(operands, operand_node, operand_node->next_node, value);
				operator_linked_list_delete_node(operators, operator_node);
				operator_node = next_node;
				break;
			}
			case TYPE_BOOL:
				output_nullt_string("bool / bool\n");
				exit(1);
			case TYPE_VARIABLE:
				output_nullt_string("variable / variable\n");
				exit(1);
			}
			
			break;
		default:
			operand_node = operand_node->next_node;
			operator_node = operator_node->next_node;
		}
	}
}

__attribute__((nonnull)) static void constant_folding_for_addition_and_subtraction(OperatorLinkedList* operators, OperandLinkedList* operands) {
	OperandNode* operand_node = operands->first_node;
	OperatorNode* operator_node = operators->first_node;
	TypeUnion value;
	
	while (operator_node) {
		ensure_types_are_equal_otherwise_give_type_error(operand_node, operand_node->next_node, operator_node);
		
		switch (operator_node->operator_) {
		case OPERATOR_ADDITION:
			switch (operand_node->type) {
			case TYPE_UINT: {
				OperatorNode* next_node = operator_node->next_node;
				
				value.u64_value = operand_node->value.u64_value + operand_node->next_node->value.u64_value;
				
				replace_two_operand_nodes_with_one_value(operands, operand_node, operand_node->next_node, value);
				operator_linked_list_delete_node(operators, operator_node);
				operator_node = next_node;
				break;
			}
			case TYPE_SINT:
				output_nullt_string("sint + sint");
				exit(1);
			case TYPE_FLOAT: {
				OperatorNode* next_node = operator_node->next_node;
				
				value.f64_value = operand_node->value.f64_value + operand_node->next_node->value.f64_value;
				
				replace_two_operand_nodes_with_one_value(operands, operand_node, operand_node->next_node, value);
				operator_linked_list_delete_node(operators, operator_node);
				operator_node = next_node;
				break;
			}
			case TYPE_BOOL:
				output_nullt_string("bool + bool\n");
				exit(1);
			case TYPE_VARIABLE:
				output_nullt_string("variable + variable\n");
				exit(1);
			}
			
			break;
		case OPERATOR_SUBTRACTION:
			switch (operand_node->type) {
			case TYPE_UINT: {
				OperatorNode* next_node = operator_node->next_node;
				
				value.u64_value = operand_node->value.u64_value - operand_node->next_node->value.u64_value;
				
				replace_two_operand_nodes_with_one_value(operands, operand_node, operand_node->next_node, value);
				operator_linked_list_delete_node(operators, operator_node);
				operator_node = next_node;
				break;
			}
			case TYPE_SINT:
				output_nullt_string("sint - sint\n");
				exit(1);
			case TYPE_FLOAT: {
				OperatorNode* next_node = operator_node->next_node;
				
				value.f64_value = operand_node->value.f64_value - operand_node->next_node->value.f64_value;
				
				replace_two_operand_nodes_with_one_value(operands, operand_node, operand_node->next_node, value);
				operator_linked_list_delete_node(operators, operator_node);
				operator_node = next_node;
				break;
			}
			case TYPE_BOOL:
				output_nullt_string("bool - bool\n");
				exit(1);
			case TYPE_VARIABLE:
				output_nullt_string("variable - variable\n");
				exit(1);
			}
			
			break;
		default:
			operand_node = operand_node->next_node;
			operator_node = operator_node->next_node;
		}
	}
}

__attribute__((nonnull)) static void process_parsed_expression(
	OperatorLinkedList* operators, OperandLinkedList* operands, Block* block, ErrorNumber* error_number)
{
	OperandNode* operand_node;
	
	assert_comparison(error_number, !=, NULL);
	assert_comparison(operators, !=, NULL);
	assert_comparison(operands, !=, NULL);
	
	operand_node = operands->first_node;
	
	while (operand_node) {
		get_type(operand_node);
		*error_number = get_value(operand_node, block);
		if (*error_number != ERROR_NUMBER_NO_ERROR) {
			return;
		}
		
		operand_node = operand_node->next_node;
	}
	
	/* Constant fold operators with higher precedences first. */
	(void)constant_folding_for_exponentiation_and_roots;
	(void)constant_folding_for_multiplication_and_division;
	(void)constant_folding_for_addition_and_subtraction;
	(void)operators;
	
	#if 0
	constant_folding_for_exponentiation_and_roots(operators, operands);
	constant_folding_for_multiplication_and_division(operators, operands);
	constant_folding_for_addition_and_subtraction(operators, operands);
	#endif
}

void expression_parser_parse_expression(
	Variable* const restrict variable, Statement* const statement, ErrorNumber* error_number, Block* block, Type type
) {
	u64 i;
	TypeOfAppend type_of_last_append;
	OperatorLinkedList* restrict operators;
	OperandLinkedList* restrict operands;
	
	assert_comparison(statement, !=, NULL);
	assert_comparison(error_number, !=, NULL);
	assert_comparison(block, !=, NULL);
	
	operators = &variable->operators;
	operands = &variable->operands;
	
	assert_comparison(operators, !=, NULL);
	assert_comparison(operands, !=, NULL);
	
	operator_linked_list_init(operators);
	operand_linked_list_init(operands);
	
	type_of_last_append = TYPE_OF_LAST_OPERATOR_OR_OPERAND_APPEND___NONE;
	
	*error_number = ERROR_NUMBER_NO_ERROR;
	
	for (i = 3; i < statement->length_of_tokens; ++i) {
		u64 j, start_of_token;
		
		/* just easier names to refer to */
		const char* const string = ARRAY_INDEX(statement->tokens, i).string.string;
		const u64 length = ARRAY_INDEX(statement->tokens, i).string.length;
		const u64 line_number = ARRAY_INDEX(statement->tokens, i).line_number;
		
		for (j = 0, start_of_token = 0; j < length; ++j) {
			/* Addition */
			if (ARRAY_INDEX(string, j) == '+') {
				if (type_of_last_append != TYPE_OF_LAST_OPERATOR_OR_OPERAND_APPEND___OPERAND) continue;
				
				/*
				Don't append an empty string since a a normal parser token can begin with an operand.
				All the operands append the expression token before them, but if an operand is before
				any other expression token in a normal parser token, the expression token it outputs
				will empty, and that is not wanted.
				*/
				if (j) {
					operand_linked_list_append(operands, string + start_of_token, j - start_of_token, line_number);
				}
				operator_linked_list_append(operators, OPERATOR_ADDITION, line_number);
				start_of_token = j + 1;
				type_of_last_append = TYPE_OF_LAST_OPERATOR_OR_OPERAND_APPEND___OPERATOR;
			}
			
			/* Subtraction */
			else if (ARRAY_INDEX(string, j) == '-') {
				if (type_of_last_append != TYPE_OF_LAST_OPERATOR_OR_OPERAND_APPEND___OPERAND) continue;
				
				if (j) {
					operand_linked_list_append(operands, string + start_of_token, j - start_of_token, line_number);
				}
				operator_linked_list_append(operators, OPERATOR_SUBTRACTION, line_number);
				start_of_token = j + 1;
				type_of_last_append = TYPE_OF_LAST_OPERATOR_OR_OPERAND_APPEND___OPERATOR;
			}
			
			/* Exponentiation */
			else if (ARRAY_INDEX(string, j) == '*' && j + 1 < length && ARRAY_INDEX(string, j + 1) == '*') {
				if (type_of_last_append != TYPE_OF_LAST_OPERATOR_OR_OPERAND_APPEND___OPERAND) {
					++j;
					continue;
				}
				
				if (j) {
					operand_linked_list_append(operands, string + start_of_token, j - start_of_token, line_number);
				}
				operator_linked_list_append(operators, OPERATOR_EXPONENTIATION, line_number);
				type_of_last_append = TYPE_OF_LAST_OPERATOR_OR_OPERAND_APPEND___OPERATOR;
				start_of_token = j + 2;
				++j;
			}
			
			/* Multiplication */
			else if (ARRAY_INDEX(string, j) == '*') {
				if (type_of_last_append != TYPE_OF_LAST_OPERATOR_OR_OPERAND_APPEND___OPERAND) {
					output_nullt_string("No unary multiplication operator.\n");
					exit(1);
				}
				
				if (j) {
					operand_linked_list_append(operands, string + start_of_token, j - start_of_token, line_number);
				}
				operator_linked_list_append(operators, OPERATOR_MULTIPLICATION, line_number);
				start_of_token = j + 1;
				type_of_last_append = TYPE_OF_LAST_OPERATOR_OR_OPERAND_APPEND___OPERATOR;
			}
			
			/* Roots */
			else if (ARRAY_INDEX(string, j) == '_' && j + 1 < length && ARRAY_INDEX(string, j + 1) == '/') {
				if (type_of_last_append != TYPE_OF_LAST_OPERATOR_OR_OPERAND_APPEND___OPERAND) {
					++j;
					continue;
				}
				
				if (j) {
					operand_linked_list_append(operands, string + start_of_token, j - start_of_token, line_number);
				}
				operator_linked_list_append(operators, OPERATOR_ROOTS, line_number);
				start_of_token = j + 2;
				++j;
				type_of_last_append = TYPE_OF_LAST_OPERATOR_OR_OPERAND_APPEND___OPERATOR;
			}
			
			/* Division */
			else if (ARRAY_INDEX(string, j) == '/') {
				if (type_of_last_append == TYPE_OF_LAST_OPERATOR_OR_OPERAND_APPEND___OPERATOR) {
					output_nullt_string("No unary division operator.\n");
					exit(1);
				}
				
				if (j) {
					operand_linked_list_append(operands, string + start_of_token, j - start_of_token, line_number);
				}
				operator_linked_list_append(operators, OPERATOR_DIVISION, line_number);
				start_of_token = j + 1;
				type_of_last_append = TYPE_OF_LAST_OPERATOR_OR_OPERAND_APPEND___OPERATOR;
			}
			
			else if (j == length - 1) {
				operand_linked_list_append(operands, string + start_of_token, j - start_of_token + 1, line_number);
				type_of_last_append = TYPE_OF_LAST_OPERATOR_OR_OPERAND_APPEND___OPERAND;
			}
		}
	}
	
	process_parsed_expression(operators, operands, block, error_number);
	if (*error_number != ERROR_NUMBER_NO_ERROR) {
		operand_linked_list_del(operands);
		return;
	}
	
	if (
		(operands->last_node->type == TYPE_VARIABLE ?
			operands->last_node->type_of_the_variable_that_this_operand_node_references :
			operands->last_node->type
		) != type
	) {
		output_nullt_string("Assignment Error: assigning expression of type ‘");
		output_nullt_string(ARRAY_INDEX(types_to_strings, operands->last_node->type));
		output_nullt_string("’ to variable of type ‘");
		output_nullt_string(ARRAY_INDEX(types_to_strings, type));
		output_nullt_string("’\n");
		exit(1);
	}
}
