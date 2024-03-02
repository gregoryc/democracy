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
This file contains functions that parse an expression string
into operator and operand structs and do some constant folding
and miscellanious optimizations on them.
*/

#include "../global_headers/headers.h"
#include "get_variable.h"

const char*
#ifndef __cplusplus
	const
#endif
restrict types_to_strings[4] = {"u", "s", "f", "b"};

#define NUMBER_OF_TYPES   4

attr_nonnull static inline void operand_linked_list_init(OperandLinkedList* cr linked_list) {
	assert_comparison(linked_list, !=, NULL, void*, void*);
	
	ARROW_ASSIGN(linked_list, first_node) = NULL;
	ARROW_ASSIGN(linked_list, last_node) = NULL;
}

attr_nonnull static inline void operator_linked_list_init(OperatorLinkedList* cr linked_list) {
	assert_comparison(linked_list, !=, NULL, void*, void*);
	
	ARROW_ASSIGN(linked_list, first_node) = NULL;
	ARROW_ASSIGN(linked_list, last_node) = NULL;
}

attrs_cold_and_noreturn
static void unable_to_allocate_memory_for_an_operand_in_an_expression(void) {
	output_nullt_string("Memory error: unable to allocate memory for an operand in an expression.\n");
	exit_program();
}

attr_nonnull
static void operand_linked_list_append(
	OperandLinkedList* cr linked_list,
	char* const string,
	u length,
	u line_number
) {
	OperandNode* node;
	
	assert_comparison(linked_list, !=, NULL, OperandLinkedList* cr, void*);
	assert_comparison(string, !=, NULL, char* const, void*);
	assert_comparison(length, !=, 0, u, u);
	assert_comparison(line_number, !=, 0, u, u);
	
	if (likely(ARROW(linked_list, first_node))) {
		node = (OperandNode*)m_alloc(sizeof(OperandNode));
		
		if (unlikely(!node)) {
			unable_to_allocate_memory_for_an_operand_in_an_expression();
		}
		
		node->string = string;
		node->string_length = length;
		node->line_number = line_number;
		node->previous_node = linked_list->last_node;
		node->next_node = NULL;
		linked_list->last_node->next_node = node;
		linked_list->last_node = node;
	} else {
		ARROW_ASSIGN(linked_list, first_node) = (OperandNode*)m_alloc(sizeof(OperandNode));
		
		if (unlikely(!ARROW(linked_list, first_node))) {
			unable_to_allocate_memory_for_an_operand_in_an_expression();
		}
		
		linked_list->first_node->string = string;
		linked_list->first_node->string_length = length;
		linked_list->first_node->line_number = line_number;
		linked_list->first_node->previous_node = NULL;
		linked_list->first_node->next_node = NULL;
		linked_list->last_node = linked_list->first_node;
	}
}

attrs_cold_and_noreturn
static void unable_to_allocate_memory_for_an_operator_in_an_expression(void) {
	output_nullt_string("Memory error: unable to allocate memory for an operator in an expression.\n");
	exit_program();
}

attr_nonnull
static void operator_linked_list_append(
	OperatorLinkedList* cr linked_list,
	const Operator operator_,
	const u line_number,
	const bool is_unary
) {
	OperatorNode* node;
	
	assert_comparison(linked_list, !=, NULL, void*, void*);
	assert_comparison(line_number, !=, 0, u, u);
	
	if (likely(ARROW(linked_list, first_node))) {
		node = (OperatorNode*)m_alloc(sizeof(OperatorNode));
		
		if (unlikely(!ARROW(linked_list, first_node))) {
			unable_to_allocate_memory_for_an_operator_in_an_expression();
		}
		
		node->previous_node = linked_list->last_node;
		node->next_node = NULL;
		node->operator_ = operator_;
		node->line_number = line_number;
		node->is_unary = is_unary;
		linked_list->last_node->next_node = node;
		linked_list->last_node = node;
	} else {
		linked_list->first_node = (OperatorNode*)m_alloc(sizeof(OperatorNode));
		
		if (unlikely(!ARROW(linked_list, first_node))) {
			unable_to_allocate_memory_for_an_operator_in_an_expression();
		}
		
		linked_list->first_node->previous_node = NULL;
		linked_list->first_node->next_node = NULL;
		linked_list->first_node->operator_ = operator_;
		linked_list->first_node->line_number = line_number;
		linked_list->first_node->is_unary = is_unary;
		linked_list->last_node = linked_list->first_node;
	}
}

attr_nonnull static void get_type(OperandNode* operand_node) {
	u i;
	
	assert_comparison(operand_node, !=, NULL, void*, void*);
	
	for (i = 0; i < operand_node->string_length; ++i) {
		if (ARRAY_INDEX(operand_node->string, i) == '.') {
			operand_node->type = TYPE_FLOAT;
			return;
		}
	}
	
	if (is_valid_identifier(operand_node->string, operand_node->string_length)) {
		ARROW_ASSIGN(operand_node, type) = TYPE_VARIABLE;
		return;
	}
	
	ARROW_ASSIGN(operand_node, type) = TYPE_UINT;
}

static void get_value(ParserState* cr state, OperandNode* const operand_node) {
	assert_comparison(operand_node, !=, NULL, void*, void*);
	
	switch (ARROW(operand_node, type)) {
		case TYPE_UINT:
			operand_node->value.uvalue = convert_literal_to_uint(
				operand_node->string, operand_node->string_length, operand_node->line_number);
			break;
		case TYPE_SINT:
			output_nullt_string("SINT\n");
			exit_program();
		case TYPE_FLOAT:
			operand_node->value.fvalue = convert_literal_to_float(
				operand_node->string, operand_node->string_length, operand_node->line_number);
			break;
		case TYPE_BOOL:
			output_nullt_string("BOOL\n");
			exit_program();
		case TYPE_VARIABLE: {
			Variable* variable;
			String string;
			
			string.string = ARROW(operand_node, string);
			string.length = ARROW(operand_node, string_length);
			
			variable = get_variable(state, &string); 
			
			if (variable) {
				ARROW_ASSIGN(operand_node, variable) = variable;
			} else {
				output_nullt_string("Name error on line ");
				putu(ARROW(operand_node, line_number));
				output_nullt_string(": " OPENING_QUOTE);
				output_string_with_length(ARROW(operand_node, string), ARROW(operand_node, string_length));
				output_nullt_string(CLOSING_QUOTE " undeclared\n");
				exit_program();
			}
			break;
		}
		default:
			assert_comparison(operand_node->type, !=, operand_node->type, Operator, Operator);
		}
}

attr_nonnull
static void process_parsed_expression(
	ParserState* cr state,
	OperatorLinkedList* cr operators,
	OperandLinkedList* cr operands
) {
	OperandNode* operand_node;
	
	assert_comparison(operators, !=, NULL, void*, void*);
	assert_comparison(operands, !=, NULL, void*, void*);
	
	operand_node = ARROW(operands, first_node);
	
	while (operand_node) {
		get_type(operand_node);
		get_value(state, operand_node);
		
		operand_node = ARROW(operand_node, next_node);
	}
	
	constant_fold_expression(operators, operands);
}

typedef enum {TOKEN_TYPE_OPERATOR, TOKEN_TYPE_OPERAND, TOKEN_TYPE_NONE} TokenType;

attr_nonnull static void give_syntax_error_if_previous_token_type_is_an_operator(
	const TokenType previous_token_type,
	Token* const previous_token,
	Token* const token,
	Statement* cr statement
) {
	assert_comparison(token, !=, NULL, Token* const, void*);
	assert_comparison(statement, !=, NULL, Statement* cr, void*);
	
	if (unlikely(previous_token_type == TOKEN_TYPE_OPERATOR)) {
		char* ptr;
		char* end_of_statement_ptr;
		u new_length_of_statement_line_string;
		
		output_syntax_error_beginning_text(ARROW(previous_token, line_number), 0);
		output_nullt_string(" you cannot have a " OPENING_QUOTE);
		output_string(&previous_token->string);
		output_nullt_string(CLOSING_QUOTE " operator right before a");
		
		if (
			strnequal(
				ARROW(previous_token, string).string,
				ARROW(token, string).string,
				ARROW(previous_token, string).length,
				ARROW(token, string).length
			)
		) {
			output_nullt_string("nother");
		}
		
		output_nullt_string(" " OPENING_QUOTE);
		output_string(&token->string);
		output_nullt_string(CLOSING_QUOTE " operator. You must have an operand between the two.\n");
		
		new_length_of_statement_line_string = ARROW(token, string).string - ARROW(previous_token, string).string;
		ptr = ARROW(previous_token, string).string;
		
		while (DEREF(ptr) != '\n' && ptr != ARROW(statement, statement_line)) {
			++new_length_of_statement_line_string;
			--ptr;
		}
		
		if (DEREF(ptr) == '\n') {
			--new_length_of_statement_line_string;
			++ptr;
		}
		
		end_of_statement_ptr = ARROW(statement, statement_line) + ARROW(statement, length_of_statement_line_string);
		ARROW_ASSIGN(statement, statement_line) = ptr;
		ptr = ARROW(token, string).string;
		
		while (ptr < end_of_statement_ptr && DEREF(ptr) != '\n') {
			++new_length_of_statement_line_string;
			++ptr;
		}
		
		ARROW_ASSIGN(statement, length_of_statement_line_string) = new_length_of_statement_line_string;
		output_source_lines(statement, ARROW(previous_token, line_number));
		
		exit_program();
	}
}

void parse_expression(
	ParserState* cr state,
	Variable* cr variable,
	Token** cr tokens,
	const u length_of_tokens,
	const u beginning_token_offset,
	const Type type
) {
	u i;
	OperatorLinkedList* operators;
	OperandLinkedList* operands;
	TokenType previous_token_type = TOKEN_TYPE_NONE;
	Token* previous_token = NULL;
	Token* current_token;
	Type expression_type;
	
	assert_comparison(state, !=, NULL, ParserState* cr, void*);
	assert_comparison(variable, !=, NULL, void*, void*);
	assert_comparison(tokens, !=, NULL, void*, void*);
	assert_comparison(length_of_tokens, !=, 0, u, u);
	
	operators = &variable->operators;
	operands = &variable->operands;
	
	assert_comparison(operators, !=, NULL, OperatorLinkedList*, void*);
	assert_comparison(operands, !=, NULL, OperandLinkedList*, void*);
	
	operator_linked_list_init(operators);
	operand_linked_list_init(operands);
	
	for (i = beginning_token_offset; i < length_of_tokens; ++i, previous_token = current_token) {
		char* string;
		u length, line_number;
		
		current_token = ARRAY_INDEX(tokens, i);
		
		/* just easier names to refer to */
		string = ARROW(current_token, string).string;
		length = ARROW(current_token, string).length;
		line_number = ARROW(current_token, line_number);
		
		assert_comparison(string, !=, NULL, char*, void*);
		assert_comparison(length, !=, 0, u, u);
		assert_comparison(line_number, !=, 0, u, u);
		
		if (length == 1) {
			switch (DEREF(string)) {
				/* The operators that can be unary or binary depending on their context */
				case '+':
					operator_linked_list_append(operators, OPERATOR_ADDITION, line_number, previous_token_type == TOKEN_TYPE_OPERATOR);
					goto unary_operator_continue;
				case '-':
					operator_linked_list_append(operators, OPERATOR_SUBTRACTION, line_number, previous_token_type == TOKEN_TYPE_OPERATOR);
					goto unary_operator_continue;
				case '<':
					operator_linked_list_append(operators, OPERATOR_LESS_THAN, line_number, previous_token_type == TOKEN_TYPE_OPERATOR);
					goto unary_operator_continue;
				case '>':
					operator_linked_list_append(operators, OPERATOR_GREATER_THAN, line_number, previous_token_type == TOKEN_TYPE_OPERATOR);
					goto unary_operator_continue;
				
				/* The binary operators */
				
				case '*':
					give_syntax_error_if_previous_token_type_is_an_operator(
						previous_token_type,
						previous_token,
						current_token,
						ARROW(variable, statement_declared_on)
					);
					
					operator_linked_list_append(operators, OPERATOR_MULTIPLICATION, line_number, false);
					goto operator_continue;
				case '/':
					give_syntax_error_if_previous_token_type_is_an_operator(
						previous_token_type,
						previous_token,
						current_token,
						ARROW(variable, statement_declared_on)
					);
					
					operator_linked_list_append(operators, OPERATOR_DIVISION, line_number, false);
					goto operator_continue;
				case '=':
					give_syntax_error_if_previous_token_type_is_an_operator(
						previous_token_type,
						previous_token,
						current_token,
						ARROW(variable, statement_declared_on)
					);
					
					operator_linked_list_append(operators, OPERATOR_EQUALS, line_number, false);
					goto operator_continue;
			}
		} else if (length == 2) {
			if (DEREF(string) == '*' && ARRAY_INDEX(string, 1) == '*') {
				give_syntax_error_if_previous_token_type_is_an_operator(
					previous_token_type,
					previous_token,
					current_token,
					ARROW(variable, statement_declared_on)
				);	
				
				operator_linked_list_append(operators, OPERATOR_EXPONENTIATION, line_number, false);
				goto operator_continue;
			} else if (DEREF(string) == '_' && ARRAY_INDEX(string, 1) == '/') {
				give_syntax_error_if_previous_token_type_is_an_operator(
					previous_token_type,
					previous_token,
					current_token,
					ARROW(variable, statement_declared_on)
				);	
				
				operator_linked_list_append(operators, OPERATOR_ROOTS, line_number, false);
				goto operator_continue;
			} else if (DEREF(string) == '<' && ARRAY_INDEX(string, 1) == '=') {
				give_syntax_error_if_previous_token_type_is_an_operator(
					previous_token_type,
					previous_token,
					current_token,
					ARROW(variable, statement_declared_on)
				);	
				
				operator_linked_list_append(operators, OPERATOR_LESS_THAN_OR_EQUAL_TO, line_number, false);
				goto operator_continue;
			} else if (DEREF(string) == '!' && ARRAY_INDEX(string, 1) == '=') {
				give_syntax_error_if_previous_token_type_is_an_operator(
					previous_token_type,
					previous_token,
					current_token,
					ARROW(variable, statement_declared_on)
				);	
				
				operator_linked_list_append(operators, OPERATOR_NOT_EQUAL, line_number, false);
				goto operator_continue;
			} else if (DEREF(string) == '>' && ARRAY_INDEX(string, 1) == '=') {
				give_syntax_error_if_previous_token_type_is_an_operator(
					previous_token_type,
					previous_token,
					current_token,
					ARROW(variable, statement_declared_on)
				);	
				
				operator_linked_list_append(operators, OPERATOR_GREATER_THAN_OR_EQUAL_TO, line_number, false);
				goto operator_continue;
			}
		}
		
		if (unlikely(previous_token_type == TOKEN_TYPE_OPERAND)) {
			output_nullt_string("Syntax error: you cannot have two operands in a row.\n");
			exit_program();
		}
		
		previous_token_type = TOKEN_TYPE_OPERAND;
		operand_linked_list_append(operands, string, length, line_number);
		
		continue;
		
		operator_continue:
			if (unlikely(i == beginning_token_offset)) {
				output_syntax_error_beginning_text(ARROW(current_token, line_number), 0);
				output_nullt_string("an expression cannot start with a " OPENING_QUOTE);
				output_string(&current_token->string);
				output_nullt_string(CLOSING_QUOTE " operator\n");
				output_source_lines(ARROW(variable, statement_declared_on), ARROW(current_token, line_number));
				exit_program();
			}
			
			if (unlikely(i == length_of_tokens - 1)) {
				output_syntax_error_beginning_text(ARROW(current_token, line_number), 0);
				output_nullt_string("an expression cannot end with a " OPENING_QUOTE);
				output_string(&current_token->string);
				output_nullt_string(CLOSING_QUOTE " operator\n");
				output_source_lines(ARROW(variable, statement_declared_on), ARROW(current_token, line_number));
				exit_program();
			}
			
			previous_token_type = TOKEN_TYPE_OPERATOR;
			continue;
		
		unary_operator_continue:
			if (unlikely(i == length_of_tokens - 1)) {
				output_syntax_error_beginning_text(ARROW(current_token, line_number), 0);
				output_nullt_string("an expression cannot end with a " OPENING_QUOTE);
				output_string(&current_token->string);
				output_nullt_string(CLOSING_QUOTE " operator\n");
				output_source_lines(ARROW(variable, statement_declared_on), ARROW(current_token, line_number));
				exit_program();
			}
			
			if (unlikely(i == beginning_token_offset)) {
				ARROW_ASSIGN(ARROW(operators, last_node), is_unary) = true;
			}
			
			previous_token_type = TOKEN_TYPE_OPERATOR;
	}
	
	process_parsed_expression(state, operators, operands);
	
	expression_type = ARROW3(operands, last_node, type);
	
	if (expression_type == TYPE_VARIABLE) {
		expression_type = ARROW4(operands, last_node, variable, type);
	}
	
	assert_comparison(expression_type, <, NUMBER_OF_TYPES, Type, u);
	
	if (expression_type != type) {
		output_nullt_string("Assignment Error: assigning expression of type " OPENING_QUOTE);
		output_nullt_string(ARRAY_INDEX(types_to_strings, expression_type));
		output_nullt_string(CLOSING_QUOTE " to variable of type " OPENING_QUOTE);
		output_nullt_string(ARRAY_INDEX(types_to_strings, type));
		output_nullt_string(CLOSING_QUOTE "\n");
		exit_program();
	}
}
