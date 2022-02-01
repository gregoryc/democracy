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

#ifdef __cplusplus
	#include <cctype>
#else
	#include <ctype.h>
#endif

#include "../global_headers/headers.h"
#include "ensure_type_is_valid_otherwise_give_type_error.h"
#include "insert_variable_into_scope.h"
#include "get_variable.h"
#include "show_tokens.h"
#include "line_number.h"
#include "read_char.h"

#undef unsigned
#undef short

static Subroutine* get_subroutine(ParserState* cr state, const char* subroutine_name, const u length_of_subroutine_name) {
	Scope* scope;
	
	assert_comparison(state, !=, NULL, ParserState* cr, void*);
	assert_comparison(subroutine_name, !=, NULL, const char*, void*);
	assert_comparison(length_of_subroutine_name, !=, 0, const u, u);
	
	scope = &state->scope;
	
	while (scope) {
		bool in_hash_table;
		Subroutine* subroutine;
		
		subroutine = (Subroutine*)DEREF(
			hash_table_get_value_from_string_key(
				&scope->subroutines_in_scope,
				subroutine_name,
				length_of_subroutine_name,
				&in_hash_table
			)
		);
		
		if (in_hash_table) {
			return subroutine;
		}
		
		scope = ARROW(scope, parent_scope);
	}
	
	return NULL;
}

attr_nonnull
static void ensure_that_argument_counts_are_equal_otherwise_give_type_error(
	const Subroutine* cr subroutine,
	const u length
) {
	assert_comparison(subroutine, !=, NULL, const Subroutine* cr, void*);
	
	if (unlikely(length != ARROW(subroutine, length_of_argument_type_list))) {
		output_nullt_string("Type error: gave ");
		putu(length);
		output_nullt_string(" argument");
		if (length != 1) {
			output_char('s');
		}
		output_nullt_string(" but ");
		putu(ARROW(subroutine, length_of_argument_type_list));
		output_nullt_string(" are expected\n");
		exit_program();
	}
}

static Variable* create_variable_from_argument_expression(
	ParserState* cr state,
	Token** const tokens,
	const u first_argument_expression_index,
	const u last_argument_expression_index
) {
	Variable* variable;
	
	variable = (Variable*)m_alloc(sizeof(Variable));
	
	if (unlikely(!variable)) {
		output_nullt_string("Unable to allocate a temporary variable for an expression in an argument list.\n");
		exit_program();
	}
	
	#if DEBUG
		/*
		This is set to point to -1 so that the program will definitely crash if it's used.
		It's set to this to prevent a debugging nightmare, or even worse, the program working
		in DEBUG mode but not working in release mode.
		*/
		ARROW_ASSIGN(variable, statement_declared_on) = (Statement*)-1;
	#endif
	
	ARROW_ASSIGN(variable, name).length = 0;
	ARROW_ASSIGN(variable, line_declared_on) = ARROW(ARRAY_INDEX(tokens, first_argument_expression_index), line_number);
	ARROW_ASSIGN(variable, is_argument) = false;
	
	parse_expression(
		state,
		variable,
		tokens,
		last_argument_expression_index,
		first_argument_expression_index,
		&state->scope.variables_in_scope,
		TYPE_FLOAT
	);
	
	translate_expression_into_mid_level_instructions(ARROW(state, dac), ARROW(state, mli), variable, ARROW(state, stack_offset));
	
	return variable;
}

__attribute__((nonnull (1, 4)))
static void create_variables_from_argument_expressions_and_push_them_onto_the_stack(
	ParserState* cr state,
	Subroutine* cr subroutine,
	SubroutineForwardReference* cr subroutine_forward_reference,
	Token** const tokens,
	const u number_of_tokens
) {
	MidLevelInstruction* mid_level_instruction;
	u beginning_of_argument_expression_index;
	LinkedListHead argument_type_list_head;
	bool previous_token_is_a_comma;
	Variable* variable;
	u i;
	u length;
	
	assert_comparison(state, !=, NULL, ParserState* cr, void*);
	assert_comparison(tokens, !=, NULL, Token** const, void*);
	
	if (subroutine) {
		length = 0;
		
		if (!number_of_tokens) {
			ensure_that_argument_counts_are_equal_otherwise_give_type_error(subroutine, length);
			return;
		}
	} else {
		/* If this is removed, gcc gives an error. */
		argument_type_list_head = NULL;
		
		if (!number_of_tokens) return;
	}
	
	if (unlikely(
		ARROW(ARRAY_INDEX(tokens, 0), string).length == 1 &&
		DEREF(ARROW(ARRAY_INDEX(tokens, 0), string).string) == ','
	)) {
		output_nullt_string("Syntax error: an argument list cannot begin with a comma\n");
		exit_program();
	}
	
	if (subroutine) {
		argument_type_list_head = ARROW(subroutine, argument_type_list);
	}
	
	previous_token_is_a_comma = false;
	beginning_of_argument_expression_index = false;
	
	for (i = 0; i < number_of_tokens; ++i) {
		/* if token is a comma */
		if (
			ARROW(ARRAY_INDEX(tokens, i), string).length == 1 &&
			DEREF(ARROW(ARRAY_INDEX(tokens, i), string).string) == ','
		) {
			if (unlikely(previous_token_is_a_comma)) {
				output_nullt_string("Syntax error: two commas in a row in an argument list\n");
				exit_program();
			}
			
			in:
			
			variable = create_variable_from_argument_expression(state, tokens, beginning_of_argument_expression_index, i);
			
			if (subroutine) {
				if (likely(argument_type_list_head)) {
					if (unlikely((Type)(u)ARROW(argument_type_list_head, value) != TYPE_FLOAT)) {
						output_nullt_string("Type error: argument needs to be a float\n");
						exit_program();
					}
				} else {
					output_nullt_string("Type error: calling subroutine with too many arguments.\n");
					exit_program();
				}
				
				++length;
			} else {
				linked_list_append(&subroutine_forward_reference->argument_type_list, (void*)TYPE_FLOAT);
				ARROW_ASSIGN(subroutine_forward_reference, length_of_argument_type_list)++;
			}
			
			mid_level_instruction = (MidLevelInstruction*)m_alloc(sizeof(MidLevelInstruction));
			
			if (unlikely(!mid_level_instruction)) {
				output_nullt_string(
					"Memory error: unable to allocate memory for a " OPENING_QUOTE
					"push_argument" CLOSING_QUOTE " mid-level instruction.\n"
				);
			}
			
			ARROW_ASSIGN(mid_level_instruction, opcode) = MLI_OPCODE_PUSH_ARGUMENT;
			ARROW_ASSIGN(mid_level_instruction, argument1) = variable;
			
			mid_level_instructions_append(ARROW(state, mli), mid_level_instruction, ARROW(state, dac));
			
			beginning_of_argument_expression_index = i + 1;
			previous_token_is_a_comma = true;
		} else {
			previous_token_is_a_comma = false;
		}
	}
	
	if (
		i == number_of_tokens &&
		!(
			ARROW(ARRAY_INDEX(tokens, number_of_tokens - 1), string).length == 1 &&
			DEREF(ARROW(ARRAY_INDEX(tokens, number_of_tokens - 1), string).string) == ','
		)
	) {
		goto in;
	}
	
	if (subroutine) {
		ensure_that_argument_counts_are_equal_otherwise_give_type_error(subroutine, length);
	}
}

attr_nonnull static void variable_free_function(void* cr variable_) {
	Variable* variable;
	
	assert_comparison(variable_, !=, NULL, void*, void*);
	
	variable = (Variable*)variable_;
	
	if (unlikely(!ARROW(variable, evaluated))) {
		output_nullt_string("Variable error: unused variable " OPENING_QUOTE);
		output_string(ARROW(variable, name).string, ARROW(variable, name).length);
		output_nullt_string(CLOSING_QUOTE " (it was declared on line ");
		putu(ARROW(variable, line_declared_on));
		output_nullt_string(")\n");
		output_source_lines(ARROW(variable, statement_declared_on), ARROW(variable, line_declared_on));
		
		exit_program();
	}
	
	operand_linked_list_del(&variable->operands);
	operator_linked_list_del(&variable->operators);
	m_free(variable);
}

attr_nonnull
void parse_statement(ParserState* cr state, Statement* cr statement) {
	#if DEBUG && SHOW_TOKENS
		u j;
	#endif
	
	/* just easier names to refer to */
	Token** tokens;
	u number_of_tokens;
	
	assert_comparison(state, !=, NULL, ParserState* cr, void*);
	
	if (unlikely(!ARROW(state, in_subroutine))) {
		output_syntax_error_beginning_text(get_line_number(state), 0);
		output_nullt_string("normal statement not in subroutine\n");
		exit_program();
	}
	
	number_of_tokens = ARROW(statement, length_of_tokens);
	tokens = ARROW(statement, tokens);
	
	#if DEBUG && SHOW_TOKENS
		output_nullt_string("Tokens in statement:\n");
		
		for (j = 0; j < number_of_tokens; ++j) {
			output_nullt_string("    ");
			output_string(ARROW(ARRAY_INDEX(tokens, j), string).string, ARROW(ARRAY_INDEX(tokens, j), string).length);
			output_newline();
		}
	#endif
	
	/* This branch handles declaring initialized variables, for example:  i = 0; */
	if (number_of_tokens >= 4 && ARROW(ARRAY_INDEX(tokens, 2), string).length == 1 && ARROW(ARRAY_INDEX(tokens, 2), string).string[0] == '=') {
		Variable* variable;
		
		ensure_type_is_valid_otherwise_give_type_error(
			&ARRAY_INDEX(tokens, 0)->string,
			ARROW(ARRAY_INDEX(tokens, 0), line_number)
		);
		
		if (unlikely(!is_valid_identifier(ARROW(ARRAY_INDEX(tokens, 1), string).string, ARROW(ARRAY_INDEX(tokens, 1), string).length))) {
			output_nullt_string("Identifier error on line ");
			putu(ARROW(ARRAY_INDEX(tokens, 1), line_number));
			output_nullt_string(": invalid variable name " OPENING_QUOTE);
			output_string(ARROW(ARRAY_INDEX(tokens, 1), string).string, ARROW(ARRAY_INDEX(tokens, 1), string).length);
			output_nullt_string(CLOSING_QUOTE "\n");
			exit_program();
		}
		
		variable = (Variable*)m_alloc(sizeof(Variable));
		if (unlikely(!variable)) {
			output_nullt_string("Memory error: Unable to allocate memory for variable.\n");
			exit_program();
		}
		
		free_list_append(ARROW(state, free_list_head), variable, variable_free_function);
		
		ARROW_ASSIGN(variable, statement_declared_on) = statement;
		ARROW_ASSIGN(variable, line_declared_on) = ARROW(ARRAY_INDEX(tokens, 1), line_number);
		ARROW_ASSIGN(variable, name).string = ARROW(ARRAY_INDEX(tokens, 1), string).string;
		ARROW_ASSIGN(variable, name).length = ARROW(ARRAY_INDEX(tokens, 1), string).length;
		ARROW_ASSIGN(variable, evaluated) = false;
		ARROW_ASSIGN(variable, is_argument) = false;
		
		insert_variable_into_scope(
			state,
			ARROW(ARRAY_INDEX(tokens, 1), string).string,
			ARROW(ARRAY_INDEX(tokens, 1), string).length,
			variable
		);
		
		switch (DEREF(ARROW(ARRAY_INDEX(tokens, 0), string).string)) {
			case 'u': {
				ARROW_ASSIGN(variable, type) = TYPE_UINT;
				
				parse_expression(
					state,
					variable,
					tokens,
					ARROW(statement, length_of_tokens),
					3,
					&state->scope.variables_in_scope,
					TYPE_UINT
				);
				
				break;
			}
			case 'f': {
				ARROW_ASSIGN(variable, type) = TYPE_FLOAT;
				
				parse_expression(
					state,
					variable,
					tokens,
					ARROW(statement, length_of_tokens),
					3,
					&state->scope.variables_in_scope,
					TYPE_FLOAT
				);
				
				break;
			}
			
			default:
				output_nullt_string("Not implemented yet.\n");
				exit_program();
		}
	} else if (
		number_of_tokens == 2 &&
		strnequal(ARROW(ARRAY_INDEX(tokens, 0), string).string, "output", ARROW(ARRAY_INDEX(tokens, 0), string).length, 6)
	) {
		MidLevelInstruction* mid_level_instruction;
		Variable* variable;
		
		variable = get_variable(state, &ARRAY_INDEX(tokens, 1)->string);
		
		if (unlikely(!variable)) {
			output_nullt_string("Name error on line ");
			putu(ARROW(ARRAY_INDEX(tokens, 1), line_number));
			output_nullt_string(": " OPENING_QUOTE);
			output_string(ARROW(ARRAY_INDEX(tokens, 1), string).string, ARROW(ARRAY_INDEX(tokens, 1), string).length);
			output_nullt_string(CLOSING_QUOTE " undeclared\n");
			exit_program();
		}
		
		mid_level_instruction = (MidLevelInstruction*)m_alloc(sizeof(MidLevelInstruction));
		
		if (unlikely(!mid_level_instruction)) {
			output_nullt_string("Unable to allocate memory for " OPENING_QUOTE "send_message_to_ui" CLOSING_QUOTE " mid-level instruction.\n");
			exit_program();
		}
		
		if (!ARROW(variable, evaluated)) {
			translate_expression_into_mid_level_instructions(ARROW(state, dac), ARROW(state, mli), variable, ARROW(state, stack_offset));
		}
		
		switch (ARROW(variable, type)) {
			case TYPE_UINT:
				ARROW_ASSIGN(mid_level_instruction, opcode) = MLI_OPCODE_SEND_OUTPUT_UINT_MESSAGE_TO_UI;
				break;
			case TYPE_SINT:
				output_nullt_string("SINT\n");
				exit_program();
			case TYPE_FLOAT:
				ARROW_ASSIGN(mid_level_instruction, opcode) = MLI_OPCODE_SEND_OUTPUT_FLOAT_MESSAGE_TO_UI;
				break;
			case TYPE_BOOL:
				output_nullt_string("SINT\n");
				exit_program();
				break;
			default:
				assert_comparison(0, !=, 0, u, u);
		}
		
		ARROW_ASSIGN(mid_level_instruction, argument1) = (void*)DEREF(ARROW(state, stack_offset));
		ARROW_ASSIGN(mid_level_instruction, argument2) = (void*)variable;
		DEREF_ASSIGN(ARROW(state, stack_offset)) += sizeof(f) * 2;
		
		mid_level_instructions_append(ARROW(state, mli), mid_level_instruction, ARROW(state, dac));
	} else if (
		number_of_tokens == 1 &&
		strnequal(ARROW(ARRAY_INDEX(tokens, 0), string).string, "hello_world", ARROW(ARRAY_INDEX(tokens, 0), string).length, 11)
	) {
		MidLevelInstruction* mid_level_instruction = (MidLevelInstruction*)m_alloc(sizeof(MidLevelInstruction));
		if (unlikely(!mid_level_instruction)) {
			output_nullt_string("Unable to allocate memory for mid-level instruction.\n");
			exit_program();
		}
		
		ARROW_ASSIGN(mid_level_instruction, opcode) = MLI_OPCODE_SEND_HELLO_WORLD_MESSAGE_TO_UI;
		ARROW_ASSIGN(mid_level_instruction, argument1) = (void*)DEREF(ARROW(state, stack_offset));
		
		mid_level_instructions_append(ARROW(state, mli), mid_level_instruction, ARROW(state, dac));
		
		DEREF_ASSIGN(ARROW(state, stack_offset)) += 8;
	}
	/* This branch handles calling subroutines. */
	else if (
		number_of_tokens >= 3 &&
		ARROW(ARRAY_INDEX(tokens, 1), string).length == 1 &&
		DEREF(ARROW(ARRAY_INDEX(tokens, 1), string).string) == '(' &&
		ARROW(ARRAY_INDEX(tokens, number_of_tokens - 1), string).length == 1 &&
		DEREF(ARROW(ARRAY_INDEX(tokens, number_of_tokens - 1), string).string) == ')'
	) {
		MidLevelInstruction* mid_level_instruction;
		Subroutine* subroutine;
		
		mid_level_instruction = (MidLevelInstruction*)m_alloc(sizeof(MidLevelInstruction));
		
		if (unlikely(!mid_level_instruction)) {
			output_nullt_string(
				"Memory error: unable to allocate memory for a " OPENING_QUOTE
				"call_subroutine" CLOSING_QUOTE " mid-level instruction.\n"
			);
			
			exit_program();
		}
		
		subroutine = get_subroutine(state, ARROW(ARRAY_INDEX(tokens, 0), string).string, ARROW(ARRAY_INDEX(tokens, 0), string).length);
		
		/* if the subroutine has already been defined */
		if (subroutine) {
			create_variables_from_argument_expressions_and_push_them_onto_the_stack(
				state,
				subroutine,
				NULL,
				tokens + 2,
				number_of_tokens - 3
			);
			
			ARROW_ASSIGN(subroutine, used) = true;
			ARROW_ASSIGN(mid_level_instruction, argument1) = (void*)ARROW(subroutine, code_offset);
		} else {
			SubroutineForwardReference* subroutine_forward_reference = NULL;
			void** new_value;
			
			hash_table_insert_with_string_key(
				ARROW(state, subroutine_forward_references),
				ARROW(ARRAY_INDEX(tokens, 0), string).string,
				ARROW(ARRAY_INDEX(tokens, 0), string).length,
				NULL,
				false,
				&new_value
			);
			
			if (!DEREF(new_value)) {
				/*
				m_zero_initialized_alloc() is called because all of the members of
				the SubroutineForwardReference struct should be initialized to zero.
				*/
				subroutine_forward_reference = (SubroutineForwardReference*)m_zero_initialized_alloc(sizeof(SubroutineForwardReference));
				
				if (unlikely(!subroutine_forward_reference)) {
					output_nullt_string("Memory error: unable to allocate memory for a SubroutineForwardReference\n");
					exit_program();
				}
				
				DEREF_ASSIGN(new_value) = subroutine_forward_reference;
			}
			
			create_variables_from_argument_expressions_and_push_them_onto_the_stack(
				state,
				NULL,
				subroutine_forward_reference,
				tokens + 2,
				number_of_tokens - 3
			);
			
			linked_list_append(&subroutine_forward_reference->mid_level_instruction_list, mid_level_instruction);
		}
		
		ARROW_ASSIGN(mid_level_instruction, opcode) = MLI_OPCODE_CALL_SUBROUTINE;
		
		mid_level_instructions_append(ARROW(state, mli), mid_level_instruction, ARROW(state, dac));
	}
	else if (
		number_of_tokens == 2 &&
		strnequal(ARROW(ARRAY_INDEX(tokens, 0), string).string, "import", ARROW(ARRAY_INDEX(tokens, 0), string).length, 6)
	) {
		char* argv[3];
		char* string;
		void real_main(char** const argv);
		
		string = m_alloc(ARROW(ARRAY_INDEX(tokens, 1), string).length + 1);
		memcpy(string, ARROW(ARRAY_INDEX(tokens, 1), string).string, ARROW(ARRAY_INDEX(tokens, 1), string).length);
		ARRAY_INDEX_ASSIGN(string, ARROW(ARRAY_INDEX(tokens, 1), string).length) = '\0';
		
		argv[0] = NULL;
		argv[1] = string;
		argv[2] = NULL;
		
		real_main(argv);
		
		m_free(string);
	}
	
	/* Unknown syntax error */
	else {
		output_syntax_error_beginning_text(ARROW(ARRAY_INDEX(tokens, 0), line_number), ARROW(ARRAY_INDEX(tokens, number_of_tokens - 1), line_number));
		output_newline();
		output_source_lines(statement, ARROW(ARRAY_INDEX(tokens, 0), line_number));
		output_newline();
		
		exit_program();
	}
}
