/*
Functions to constant folding expressions.

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

#define CONSTANT_FOLDING_ENABLED   0


#if CONSTANT_FOLDING_ENABLED

const char* cr operators_to_strings[] = {"+", "-", "*", "/", "**", "_/", "<", "<=", "=", "!=", ">=", ">"};

extern const char*
#ifndef __cplusplus
	const
#endif
restrict types_to_strings[4];

static inline u upow(u base, u exponent) {
	return pow(base, exponent);
}

static inline u uroot(u root, u base) {
	if (!root) return 0;
	return pow(base, 1.0 / root);
}

static inline f froot(f base, f exponent) {
	return pow(base, 1.0 / exponent);
}

/* Helper functions */

attr_nonnull
static void replace_two_operand_nodes_with_one_value(
	OperandLinkedList* restrict linked_list,
	OperandNode* restrict node1,
	OperandNode* restrict node2,
	TypeUnion value
) {
	OperandNode* node_after_the_other_two_nodes;
	
	assert_comparison(linked_list, !=, NULL, OperandLinkedList* restrict, void*);
	assert_comparison(linked_list, !=, NULL, OperandNode* restrict, void*);
	assert_comparison(linked_list, !=, NULL, OperandNode* restrict, void*);
	
	node_after_the_other_two_nodes = node2->next_node;
	
	assert_comparison(node1, !=, NULL, void*, void*);
	assert_comparison(node2, !=, NULL, void*, void*);
	assert_comparison(node1, !=, node2, void*, void*);
	
	node1->next_node = node_after_the_other_two_nodes;
	if (node_after_the_other_two_nodes) {
		node_after_the_other_two_nodes->previous_node = node1;
	} else {
		linked_list->last_node = node1;
	}
	
	m_free(node2);
	
	node1->value.uvalue = value.uvalue;
}

attr_nonnull
static void ensure_types_are_equal_otherwise_give_type_error(OperandNode* cr node1, OperandNode* cr node2, OperatorNode* cr operator_node) {
	assert_comparison(node1, !=, NULL, void*, void*);
	assert_comparison(node2, !=, NULL, void*, void*);
	assert_comparison(operator_node, !=, NULL, void*, void*);
	assert_comparison(node1, !=, node2, void*, void*);
	
	if (ARROW(node1, type) != ARROW(node2, type)) {
		output_nullt_string("Type error on line ");
		putu(ARROW(operator_node, line_number));
		output_nullt_string(": invalid operands for binary " OPENING_QUOTE);
		output_nullt_string(ARRAY_INDEX(operators_to_strings, operator_node->operator_));
		output_nullt_string(CLOSING_QUOTE ": ");
		output_nullt_string(ARRAY_INDEX(types_to_strings, node1->type));
		output_nullt_string(" and ");
		output_nullt_string(ARRAY_INDEX(types_to_strings, node2->type));
		output_newline();
		exit_program();
	}
}

void operator_linked_list_delete_node(OperatorLinkedList* cr linked_list, OperatorNode* cr operator_node) {
	assert_comparison(operator_node, !=, NULL, OperatorNode* cr, void*);
	
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

attr_nonnull static void operator_linked_list_link(
	OperatorLinkedList* cr linked_list,
	OperatorNode* cr operator_node1,
	OperatorNode* cr operator_node2
) {
	assert_comparison(linked_list, !=, NULL, OperatorLinkedList* cr, void*);
	
	if (operator_node1) {
		ARROW_ASSIGN(operator_node1, next_node) = operator_node2;
	} else {
		ARROW_ASSIGN(linked_list, first_node) = operator_node1;
	}
	
	if (operator_node2) {
		ARROW_ASSIGN(operator_node2, previous_node) = operator_node1;
	} else {
		ARROW_ASSIGN(linked_list, last_node) = operator_node2;
	}
}

/* Constant folding functions */

attr_nonnull
static void go_through_unary_operators_for_current_operand(
	OperatorNode** operator_node_in_calling_function,
	OperandNode* operand_node,
	OperatorLinkedList* cr operators
) {
	OperatorNode* first_operator_node;
	OperatorNode* operator_node;
	
	assert_comparison(operator_node_in_calling_function, !=, NULL, OperatorNode**, void*);
	assert_comparison(operand_node, !=, NULL, OperandNode** , void*);
	
	first_operator_node = operator_node = DEREF(operator_node_in_calling_function);
	
	while (1) {
		if (!ARROW(operator_node, next_node)) break;
		if (!ARROW(ARROW(operator_node, next_node), is_unary)) break;
		operator_node = ARROW(operator_node, next_node);
	}
	
	assert_comparison(operator_node, !=, NULL, OperatorNode*, void*);
	
	DEREF_ASSIGN(operator_node_in_calling_function) = ARROW(operator_node, next_node);
	
	while (1) {
		OperatorNode* previous_node;
		
		switch (ARROW(operator_node, operator_)) {
			case OPERATOR_ADDITION:
				switch (ARROW(operand_node, type)) {
					case TYPE_UINT:
						#if DEBUG
							output_nullt_string("TYPE_UINT in ");
							output_nullt_string(__func__);
							output_nullt_string(" in " __FILE__ "\n");
						#endif
						exit_program();
					case TYPE_SINT:
						#if DEBUG
							output_nullt_string("TYPE_SINT in ");
							output_nullt_string(__func__);
							output_nullt_string(" in " __FILE__ "\n");
						#endif
						exit_program();
					case TYPE_FLOAT:
						ARROW_ASSIGN(operand_node, value).fvalue = fabs(ARROW(operand_node, value).fvalue);
						break;
					case TYPE_BOOL:
						#if DEBUG
							output_nullt_string("TYPE_BOOL in ");
							output_nullt_string(__func__);
							output_nullt_string(" in " __FILE__ "\n");
						#endif
						exit_program();
					case TYPE_VARIABLE:
						#if DEBUG
							output_nullt_string("TYPE_VARIABLE in ");
							output_nullt_string(__func__);
							output_nullt_string(" in " __FILE__ "\n");
						#endif
						exit_program();
				}
				
				break;
			case OPERATOR_SUBTRACTION:
				switch (ARROW(operand_node, type)) {
					case TYPE_UINT:
						#if DEBUG
							output_nullt_string("TYPE_UINT in ");
							output_nullt_string(__func__);
							output_nullt_string(" in " __FILE__ "\n");
						#endif
						exit_program();
					case TYPE_SINT:
						#if DEBUG
							output_nullt_string("TYPE_SINT in ");
							output_nullt_string(__func__);
							output_nullt_string(" in " __FILE__ "\n");
						#endif
						exit_program();
					case TYPE_FLOAT:
						ARROW_ASSIGN(operand_node, value).fvalue = -ARROW(operand_node, value).fvalue;
						break;
					case TYPE_BOOL:
						#if DEBUG
							output_nullt_string("TYPE_BOOL in ");
							output_nullt_string(__func__);
							output_nullt_string(" in " __FILE__ "\n");
						#endif
						exit_program();
					case TYPE_VARIABLE:
						#if DEBUG
							output_nullt_string("TYPE_VARIABLE in ");
							output_nullt_string(__func__);
							output_nullt_string(" in " __FILE__ "\n");
						#endif
						exit_program();
				}
				
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
		
		previous_node = ARROW(operator_node, previous_node);
		if (!operator_node || operator_node == first_operator_node) break;
		m_free(operator_node);
		operator_node = previous_node;
	}
	
	operator_linked_list_link(operators, ARROW(first_operator_node, previous_node), operator_node);
	m_free(operator_node);
}

attr_nonnull static void constant_folding_for_unary_operators(OperatorLinkedList* cr operators, OperandLinkedList* cr operands) {
	OperatorNode* operator_node;
	OperandNode* operand_node;
	
	assert_comparison(operators, !=, NULL, OperatorLinkedList* cr, void*);
	assert_comparison(operands, !=, NULL, OperandLinkedList* cr, void*);
	
	operand_node = ARROW(operands, first_node);
	operator_node = ARROW(operators, first_node);
	
	while (operator_node) {
		if (ARROW(operator_node, is_unary)) {
			go_through_unary_operators_for_current_operand(&operator_node, operand_node, operators);
			continue;
		} else {
			operand_node = ARROW(operand_node, next_node);
		}
		
		operator_node = ARROW(operator_node, next_node);
	}
}

attr_nonnull static void constant_folding_for_exponentiation_and_roots(OperatorLinkedList* cr operators, OperandLinkedList* cr operands) {
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
				
				value.uvalue = upow(ARROW(operand_node, value).uvalue, ARROW(ARROW(operand_node, next_node), value).uvalue);
				
				replace_two_operand_nodes_with_one_value(operands, operand_node, operand_node->next_node, value);
				operator_linked_list_delete_node(operators, operator_node);
				operator_node = next_node;
				break;
			}
			case TYPE_SINT:
				output_nullt_string("sint ** sint\n");
				exit_program();
			case TYPE_FLOAT: {
				OperatorNode* next_node = operator_node->next_node;
				
				value.fvalue = pow(operand_node->value.fvalue, ARROW(ARROW(operand_node, next_node), value).fvalue);
				
				replace_two_operand_nodes_with_one_value(operands, operand_node, operand_node->next_node, value);
				operator_linked_list_delete_node(operators, operator_node);
				operator_node = next_node;
				break;
			}
			case TYPE_BOOL:
				output_nullt_string("bool ** bool\n");
				exit_program();
			case TYPE_VARIABLE:
				output_nullt_string("variable ** variable\n");
				exit_program();
			}
			
			break;
		}
		case OPERATOR_ROOTS:
			switch (operand_node->type) {
			case TYPE_UINT: {
				OperatorNode* next_node = operator_node->next_node;
				
				value.uvalue = uroot(operand_node->value.uvalue, operand_node->next_node->value.uvalue);
				
				replace_two_operand_nodes_with_one_value(operands, operand_node, operand_node->next_node, value);
				operator_linked_list_delete_node(operators, operator_node);
				operator_node = next_node;
				break;
			}
			case TYPE_SINT:
				output_nullt_string("sint _/ sint\n");
				exit_program();
			case TYPE_FLOAT: {
				OperatorNode* next_node = operator_node->next_node;
				
				value.fvalue = froot(operand_node->next_node->value.fvalue, operand_node->value.fvalue);
				
				replace_two_operand_nodes_with_one_value(operands, operand_node, operand_node->next_node, value);
				operator_linked_list_delete_node(operators, operator_node);
				operator_node = next_node;
				break;
			}
			case TYPE_BOOL:
				output_nullt_string("bool _/ bool\n");
				exit_program();
			case TYPE_VARIABLE:
				output_nullt_string("variable _/ variable\n");
				exit_program();
			}
			
			break;
		default:
			operand_node = operand_node->next_node;
			operator_node = operator_node->next_node;
		}
	}
}

attr_nonnull static void constant_folding_for_multiplication_and_division(OperatorLinkedList* cr operators, OperandLinkedList* cr operands) {
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
				
				value.uvalue = operand_node->value.uvalue * operand_node->next_node->value.uvalue;
				
				replace_two_operand_nodes_with_one_value(operands, operand_node, operand_node->next_node, value);
				operator_linked_list_delete_node(operators, operator_node);
				operator_node = next_node;
				break;
			}
			case TYPE_SINT:
				output_nullt_string("sint * sint");
				exit_program();
			case TYPE_FLOAT: {
				OperatorNode* next_node = operator_node->next_node;
				
				value.fvalue = operand_node->value.fvalue * operand_node->next_node->value.fvalue;
				
				replace_two_operand_nodes_with_one_value(operands, operand_node, operand_node->next_node, value);
				operator_linked_list_delete_node(operators, operator_node);
				operator_node = next_node;
				break;
			}
			case TYPE_BOOL:
				output_nullt_string("bool * bool\n");
				exit_program();
			case TYPE_VARIABLE:
				output_nullt_string("variable * variable\n");
				exit_program();
			}
			
			break;
		case OPERATOR_DIVISION:
			switch (ARROW(operand_node, type)) {
			case TYPE_UINT: {
				OperatorNode* next_node = ARROW(operator_node, next_node);
				
				/* Unsigned division by zero */
				if (!operand_node->next_node->value.uvalue) {
					output_nullt_string("Math error on line ");
					putu(operand_node->next_node->line_number);
					output_nullt_string(": unsigned division by zero\n");
					exit_program();
				}
				
				value.uvalue = operand_node->value.uvalue / operand_node->next_node->value.uvalue;
				
				replace_two_operand_nodes_with_one_value(operands, operand_node, operand_node->next_node, value);
				operator_linked_list_delete_node(operators, operator_node);
				operator_node = next_node;
				break;
			}
			case TYPE_SINT:
				output_nullt_string("sint / sint");
				exit_program();
			case TYPE_FLOAT: {
				OperatorNode* next_node = operator_node->next_node;
				
				value.fvalue = operand_node->value.fvalue / operand_node->next_node->value.fvalue;
				
				replace_two_operand_nodes_with_one_value(operands, operand_node, operand_node->next_node, value);
				operator_linked_list_delete_node(operators, operator_node);
				operator_node = next_node;
				break;
			}
			case TYPE_BOOL:
				output_nullt_string("bool / bool\n");
				exit_program();
			case TYPE_VARIABLE:
				output_nullt_string("variable / variable\n");
				exit_program();
			}
			
			break;
		default:
			operand_node = operand_node->next_node;
			operator_node = operator_node->next_node;
		}
	}
}

attr_nonnull static void constant_folding_for_addition_and_subtraction(OperatorLinkedList* cr operators, OperandLinkedList* cr operands) {
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
				
				value.uvalue = operand_node->value.uvalue + operand_node->next_node->value.uvalue;
				
				replace_two_operand_nodes_with_one_value(operands, operand_node, operand_node->next_node, value);
				operator_linked_list_delete_node(operators, operator_node);
				operator_node = next_node;
				break;
			}
			case TYPE_SINT:
				output_nullt_string("sint + sint");
				exit_program();
			case TYPE_FLOAT: {
				OperatorNode* next_node = operator_node->next_node;
				
				value.fvalue = operand_node->value.fvalue + operand_node->next_node->value.fvalue;
				
				replace_two_operand_nodes_with_one_value(operands, operand_node, operand_node->next_node, value);
				operator_linked_list_delete_node(operators, operator_node);
				operator_node = next_node;
				break;
			}
			case TYPE_BOOL:
				output_nullt_string("bool + bool\n");
				exit_program();
			case TYPE_VARIABLE:
				output_nullt_string("variable + variable\n");
				exit_program();
			}
			
			break;
		case OPERATOR_SUBTRACTION:
			switch (operand_node->type) {
			case TYPE_UINT: {
				OperatorNode* next_node = operator_node->next_node;
				
				value.uvalue = operand_node->value.uvalue - operand_node->next_node->value.uvalue;
				
				replace_two_operand_nodes_with_one_value(operands, operand_node, operand_node->next_node, value);
				operator_linked_list_delete_node(operators, operator_node);
				operator_node = next_node;
				break;
			}
			case TYPE_SINT:
				output_nullt_string("sint - sint\n");
				exit_program();
			case TYPE_FLOAT: {
				OperatorNode* next_node = operator_node->next_node;
				
				value.fvalue = operand_node->value.fvalue - operand_node->next_node->value.fvalue;
				
				replace_two_operand_nodes_with_one_value(operands, operand_node, operand_node->next_node, value);
				operator_linked_list_delete_node(operators, operator_node);
				operator_node = next_node;
				break;
			}
			case TYPE_BOOL:
				output_nullt_string("bool - bool\n");
				exit_program();
			case TYPE_VARIABLE:
				output_nullt_string("variable - variable\n");
				exit_program();
			}
			
			break;
		default:
			operand_node = operand_node->next_node;
			operator_node = operator_node->next_node;
		}
	}
}

void constant_fold_expression(OperatorLinkedList* cr operators, OperandLinkedList* cr operands) {
	assert_comparison(operators, !=, NULL, OperatorLinkedList* cr, void*);
	assert_comparison(operands, !=, NULL, OperandLinkedList* cr, void*);
	
	/* Constant fold operators with higher precedences first. */
	constant_folding_for_unary_operators(operators, operands);
	constant_folding_for_exponentiation_and_roots(operators, operands);
	constant_folding_for_multiplication_and_division(operators, operands);
	constant_folding_for_addition_and_subtraction(operators, operands);
}

#else /* CONSTANT_FOLDING_ENABLED */
	void constant_fold_expression(OperatorLinkedList* cr operators, OperandLinkedList* cr operands) {
		assert_comparison(operators, !=, NULL, OperatorLinkedList* cr, void*);
		assert_comparison(operands, !=, NULL, OperandLinkedList* cr, void*);
		
		(void)operators;
		(void)operands;
	}
#endif
