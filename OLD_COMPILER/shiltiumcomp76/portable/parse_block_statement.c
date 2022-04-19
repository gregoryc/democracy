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
#include "subroutine_value_deletion_function.h"
#include "parse_statement_string_into_tokens.h"
#include "insert_variable_into_scope.h"
#include "line_number.h"
#include "show_tokens.h"
#include "parse_block.h"
#include "tokens_del.h"

#undef unsigned
#undef short

typedef enum {
	BLOCK_STATEMENT_TYPE_IF_STATEMENT,
	BLOCK_STATEMENT_TYPE_UNLESS_STATEMENT,
	BLOCK_STATEMENT_TYPE_SUB_STATEMENT
} BlockStatementType;

attr_nonnull
static void create_argument(
	ParserState* cr state,
	char* const string,
	const u length,
	const u number_of_arguments,
	const u argument_number
) {
	Variable* argument;
	
	assert_comparison(state, !=, NULL, ParserState* cr, void*);
	
	argument = (Variable*)m_alloc(sizeof(Variable));
	
	output_memory_error_if_null(argument, state, "unable to allocate memory for temporary variable in block statement expression");
	
	ARROW_ASSIGN(argument, name).string = string;
	ARROW_ASSIGN(argument, name).length = length;
	ARROW_ASSIGN(argument, statement_declared_on) = NULL;
	ARROW_ASSIGN(argument, line_declared_on) = 0;
	ARROW_ASSIGN(argument, stack_offset) = (number_of_arguments - argument_number + 1) * 8;
	ARROW_ASSIGN(argument, type) = TYPE_FLOAT;
	ARROW_ASSIGN(argument, evaluated) = true;
	ARROW_ASSIGN(argument, is_argument) = true;
	
	free_list_append(ARROW(state, free_list_head), argument, m_free);
	
	insert_variable_into_scope(state, string, length, argument);
}

__attribute__((nonnull (1, 2, 5)))
static void parse_subroutine_arguments_in_declaration(
	ParserState* cr state,
	Subroutine* cr subroutine,
	LinkedListHead argument_type_list_head,
	u number_of_arguments,
	Token** cr tokens,
	const u number_of_tokens
) {
	u beginning_of_argument_expression_index;
	bool getting_number_of_arguments;
	u argument_number;
	u i;
	
	LinkedListHead original_argument_type_list_head;
	
	assert_comparison(state, !=, NULL, ParserState* cr, void*);
	assert_comparison(subroutine, !=, NULL, Subroutine* cr, void*);
	assert_comparison(tokens, !=, NULL, Token** cr, void*);
	
	original_argument_type_list_head = argument_type_list_head;
	
	if (!number_of_tokens) {
		if (likely(!argument_type_list_head)) return;
		
		output_nullt_string("Type error: too few arguments given to subroutine\n");
		exit_program();
	}
	
	getting_number_of_arguments = !argument_type_list_head;
	
	if (
		unlikely(
			ARROW(ARRAY_INDEX(tokens, 0), string).length == 1 &&
			DEREF(ARROW(ARRAY_INDEX(tokens, 0), string).string) == ','
		)
	) {
		output_nullt_string("Syntax error: an argument declaration list cannot begin with a comma\n");
		exit_program();
	}
	
	top:
	
	argument_number = 0;
	beginning_of_argument_expression_index = false;
	
	for (i = 0; i < number_of_tokens; ++i) {
		/* if token is a comma */
		if (
			ARROW(ARRAY_INDEX(tokens, i), string).length == 1 &&
			DEREF(ARROW(ARRAY_INDEX(tokens, i), string).string) == ','
		) {
			Type type;
			char* new_argument_name;
			
			in:
			
			switch (i - beginning_of_argument_expression_index) {
				case 0:
					output_nullt_string("Syntax error on line ");
					putu(get_line_number(state));
					output_nullt_string(": two commas in a row in argument list\n");
					exit_program();
				case 1:
					output_nullt_string("Syntax error on line ");
					putu(get_line_number(state));
					output_nullt_string(": argument name omitted\n");
					exit_program();
				case 2:
					/* Not an error; the normal behaviour for a valid Shiltolang program. */
					break;
				default:
					output_nullt_string("Syntax error on line ");
					putu(get_line_number(state));
					output_nullt_string(": expected a <type> <argument name> pair, but got something else\n");
					exit_program();
			}
			
			ensure_type_is_valid_otherwise_give_type_error(
				&ARRAY_INDEX(tokens, i - 2)->string,
				ARROW(ARRAY_INDEX(tokens, i - 2), line_number)
			);
			
			if (unlikely(!is_valid_identifier(ARROW(ARRAY_INDEX(tokens, i - 1), string).string, ARROW(ARRAY_INDEX(tokens, i - 1), string).length))) {
				output_nullt_string("Identifier error: invalid argument name " OPENING_QUOTE);
				output_string(&ARRAY_INDEX(tokens, i - 1)->string);
				output_nullt_string(CLOSING_QUOTE "\n");
			}
			
			switch (DEREF(ARROW(ARRAY_INDEX(tokens, i - 2), string).string)) {
				case 'u':
					type = TYPE_UINT;
					break;
				case 's':
					type = TYPE_SINT;
					break;
				case 'f':
					type = TYPE_FLOAT;
					break;
				case 'b':
					type = TYPE_BOOL;
					break;
				default:
					/* This "can't happen" so assert that. */
					assert_comparison(
						DEREF(ARROW(ARRAY_INDEX(tokens, i - 2), string).string),
						!=,
						DEREF(ARROW(ARRAY_INDEX(tokens, i - 2), string).string),
						u,
						u
					);
					
					/* gcc gives an error if this is not here. */
					type = TYPE_UINT;
			}
			
			if (original_argument_type_list_head) {
				if (unlikely(!argument_type_list_head)) {
					output_nullt_string("Type error: too many arguments given to subroutine\n");
					exit_program();
				}
				
				argument_type_list_head = ARROW(argument_type_list_head, next_node);
			} else if (!getting_number_of_arguments) {
				linked_list_append(&subroutine->argument_type_list, (void*)type);
				ARROW_ASSIGN(subroutine, length_of_argument_type_list)++;
			}
			
			if (!getting_number_of_arguments) {
				new_argument_name = m_alloc(ARROW(ARRAY_INDEX(tokens, i - 1), string).length);
				memcpy(new_argument_name, ARROW(ARRAY_INDEX(tokens, i - 1), string).string, ARROW(ARRAY_INDEX(tokens, i - 1), string).length);
				
				free_list_append(ARROW(state, free_list_head), new_argument_name, m_free);
				create_argument(state, new_argument_name, ARROW(ARRAY_INDEX(tokens, i - 1), string).length, number_of_arguments, argument_number);
			}
			
			++argument_number;
			
			beginning_of_argument_expression_index = i + 1;
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
	
	if (getting_number_of_arguments) {
		number_of_arguments = argument_number;
		getting_number_of_arguments = false;
		argument_type_list_head = original_argument_type_list_head;
		
		goto top;
	}
	
	if (unlikely(argument_type_list_head)) {
		output_nullt_string("Type error: too few arguments given to subroutine\n");
		exit_program();
	}
}

attr_nonnull
static void empty_statement_function_for_if_statements(
	Statement* cr statement,
	const u line_number_at_start_of_statement
) {
	output_syntax_error_beginning_text(line_number_at_start_of_statement, 0);
	output_nullt_string("If statement condition omitted.\n");
	output_source_lines(statement, line_number_at_start_of_statement);
}

attr_nonnull
static void empty_statement_function_for_unless_statements(
	Statement* cr statement,
	const u line_number_at_start_of_statement
) {
	output_syntax_error_beginning_text(line_number_at_start_of_statement, 0);
	output_nullt_string("Unless statement condition omitted.\n");
	output_source_lines(statement, line_number_at_start_of_statement);
}

attr_nonnull static void remove_any_spaces_after_if_of_unless_statement(Statement* cr statement) {
	assert_comparison(statement, !=, NULL, Statement*, void*);
	
	while (isspace(DEREF(ARROW(statement, statement_line)))) {
		++statement->statement_line;
		--statement->length_of_statement_line_string;
	}
}

attrs_cold_and_nonnull
static void invalid_statement_error(Statement* cr statement, ParserState* cr state) {
	/*
	Is the loop is a 'loop' loop?
	Yes, this variable name sounds idiotic, but it's appropriate.
	*/
	bool loop_is_loop_loop = false;
	u beginning_line_number;
	
	assert_comparison(statement, !=, NULL, Statement* cr, void*);
	assert_comparison(state, !=, NULL, ParserState* cr, void*);
	
	beginning_line_number = ARROW(state, line_number_at_start_of_statement);
	
	output_syntax_error_beginning_text(beginning_line_number, get_line_number(state));
	
	if (
		ARROW(statement, length_of_statement_line_string) >= 3 &&
		strnequal(ARROW(statement, statement_line), "for", 3, 3)
	) {
		output_nullt_string(
			"There is no for loop in " LANGUAGE_NAME " because "
			LANGUAGE_NAME " does not have\nloops. Use recursion instead."
		);
	} else if (
		ARROW(statement, length_of_statement_line_string) >= 5 &&
		strnequal(ARROW(statement, statement_line), "while", 5, 5)
	) {
		output_nullt_string("There is no while loop in " LANGUAGE_NAME " because "
			LANGUAGE_NAME " does not have\nloops. Use recursion instead."
		);
	} else if (
		ARROW(statement, length_of_statement_line_string) >= 4 &&
		strnequal(ARROW(statement, statement_line), "loop", 4, 4)
	) {
		loop_is_loop_loop = true;
		
		output_nullt_string(
			"There is no " OPENING_QUOTE "loop" CLOSING_QUOTE " loop in " LANGUAGE_NAME
			" because " LANGUAGE_NAME " does not\n   have loops. Use recursion instead."
		);
	} else {
		char* char_;
		
		if (DEREF(ARROW(statement, statement_line) + ARROW(state, first_meaningful_character_offset)) == '{') {
			output_nullt_string(
				"you cannot blocks that are not after an " OPENING_QUOTE "if"
				CLOSING_QUOTE ", " OPENING_QUOTE "unless" CLOSING_QUOTE " or "
				OPENING_QUOTE "sub" CLOSING_QUOTE " statement\n"
			);
			exit_program();
		}
		
		output_nullt_string("There is no " OPENING_QUOTE);
		
		for (
			char_ = ARROW(statement, statement_line) + ARROW(state, first_meaningful_character_offset);
			!isspace(DEREF(char_));
			++char_
		) {
			output_char(DEREF(char_));
		}
		
		output_nullt_string(
			CLOSING_QUOTE " block statement in the current version of " LANGUAGE_NAME ",\n"
			"   but this may change in future versions of the language.\n   \n   Currently, the"
			" only valid block statements are "
			OPENING_QUOTE "if" CLOSING_QUOTE ", " OPENING_QUOTE "unless" CLOSING_QUOTE " and " OPENING_QUOTE "sub" CLOSING_QUOTE "."
		);
	}
	
	output_nullt_string("\n\n");
	output_source_lines(statement, beginning_line_number);
	
	if (unlikely(loop_is_loop_loop)) {
		/* Easter egg #2 */
		output_nullt_string(
			"\nYou appear to be a lost Ruby programmer.\n"
			"If so, here's home: ――――▶  http://www.ruby-lang.org  ◀――――\n"
		);
	}
	
	exit_program();
}

attr_nonnull
static void empty_statement_function_for_sub_statements(
	Statement* cr statement,
	const u line_number_at_start_of_statement
) {
	assert_comparison(statement, !=, NULL, Statement* cr, void*);
	
	output_syntax_error_beginning_text(line_number_at_start_of_statement, 0);
	output_nullt_string("Subroutine name omitted.\n");
	output_source_lines(statement, line_number_at_start_of_statement);
}

attr_nonnull
static Variable* allocate_temporary_variable_in_block_statement_expression(ParserState* cr state, Statement* const statement) {
	Variable* variable;
	
	assert_comparison(state, !=, NULL, ParserState* cr, void*);
	assert_comparison(statement, !=, NULL, Statement* cr, void*);
	
	variable = (Variable*)m_alloc(sizeof(Variable));
	
	output_memory_error_if_null(variable, state, "unable to allocate memory for temporary variable in block statement expression");
	
	ARROW_ASSIGN(variable, name).length = 0;
	ARROW_ASSIGN(variable, statement_declared_on) = statement;
	ARROW_ASSIGN(variable, line_declared_on) = 0;
	ARROW_ASSIGN(variable, evaluated) = true;
	ARROW_ASSIGN(variable, is_argument) = false;
	
	free_list_append(ARROW(state, free_list_head), variable, m_free);
	
	return variable;
}

#if DEBUG && SHOW_TOKENS
	attr_nonnull
	static void show_tokens_in_statement(Statement* cr statement) {
		u i;
		
		assert_comparison(statement, !=, NULL, Statement* cr, void*);
		
		output_nullt_string("Tokens in statement:\n");
		
		for (i = 0; i < ARROW(statement, length_of_tokens); ++i) {
			Token* current_token = ARRAY_INDEX(ARROW(statement, tokens), i);
			output_nullt_string("    ");
			output_string(&current_token->string);
			output_newline();
		}
	}
#endif

attr_nonnull
static BlockStatementType get_block_statement_type(ParserState* cr state, Statement* cr statement) {
	BlockStatementType block_statement_type;
	
	assert_comparison(state, !=, NULL, ParserState* cr, void*);
	assert_comparison(statement, !=, NULL, Statement* cr, void*);
	
	if (
		ARROW(state, statement_buffer_offset) - ARROW(state, first_meaningful_character_offset) >= 2 &&
		strnequal(ARROW(state, statement_buffer) + ARROW(state, first_meaningful_character_offset), "if", 2, 2)
	) {
		ARROW_ASSIGN(statement, statement_line) = ARROW(state, statement_buffer) + ARROW(state, first_meaningful_character_offset) + 2;
		ARROW_ASSIGN(statement, length_of_statement_line_string) =
			ARROW(state, statement_buffer_offset) -
			2 /* strlen("if") */ -
			ARROW(state, first_meaningful_character_offset);
		
		block_statement_type = BLOCK_STATEMENT_TYPE_IF_STATEMENT;
		remove_any_spaces_after_if_of_unless_statement(statement);
	} else if (
		ARROW(state, statement_buffer_offset) - ARROW(state, first_meaningful_character_offset) >= 6 &&
		strnequal(ARROW(state, statement_buffer) + ARROW(state, first_meaningful_character_offset), "unless", 6, 6)
	) {
		ARROW_ASSIGN(statement, statement_line) = ARROW(state, statement_buffer) + ARROW(state, first_meaningful_character_offset) + 6;
		
		ARROW_ASSIGN(statement, length_of_statement_line_string) =
			ARROW(state, statement_buffer_offset) -
			6 /* strlen("unless") */ -
			ARROW(state, first_meaningful_character_offset);
		
		block_statement_type = BLOCK_STATEMENT_TYPE_UNLESS_STATEMENT;
		remove_any_spaces_after_if_of_unless_statement(statement);
	} else if (
		ARROW(state, statement_buffer_offset) - ARROW(state, first_meaningful_character_offset) >= 3 &&
		strnequal(ARROW(state, statement_buffer) + ARROW(state, first_meaningful_character_offset), "sub", 3, 3)
	) {
		ARROW_ASSIGN(statement, statement_line) = ARROW(state, statement_buffer) + ARROW(state, first_meaningful_character_offset) + 3;
		ARROW_ASSIGN(statement, length_of_statement_line_string) =
			ARROW(state, statement_buffer_offset) -
			3 /* strlen("sub") */ -
			ARROW(state, first_meaningful_character_offset);
		
		block_statement_type = BLOCK_STATEMENT_TYPE_SUB_STATEMENT;
		remove_any_spaces_after_if_of_unless_statement(statement);
	} else {
		ARROW_ASSIGN(statement, statement_line) = ARROW(state, statement_buffer);
		ARROW_ASSIGN(statement, length_of_statement_line_string) = ARROW(state, statement_buffer_offset);
		
		invalid_statement_error(statement, state);
	}
	
	return block_statement_type;
}

attr_nonnull
static ParserState* create_new_state_from_old_state(ParserState* cr state) {
	ParserState* new_state;
	
	assert_comparison(state, !=, NULL, ParserState* cr, void*);
	
	new_state = (ParserState*)m_alloc(sizeof(ParserState));
	
	output_memory_error_if_null(new_state, state, "Unable to allocate memory for a ParserState structure");
	
	ARROW_ASSIGN(new_state, file) = ARROW(state, file);
	ARROW_ASSIGN(new_state, dac) = ARROW(state, dac);
	ARROW_ASSIGN(new_state, line_number) = ARROW(state, line_number);
	ARROW_ASSIGN(new_state, free_list_head) = ARROW(state, free_list_head);
	ARROW_ASSIGN(new_state, mli) = ARROW(state, mli);
	ARROW_ASSIGN(new_state, previous_mid_level_instruction_offset) = ARROW(state, previous_mid_level_instruction_offset);
	ARROW_ASSIGN(new_state, previous_subroutine_offset) = ARROW(state, previous_subroutine_offset);
	ARROW_ASSIGN(new_state, scope).parent_scope = &state->scope;
	ARROW_ASSIGN(new_state, subroutine_forward_references) = ARROW(state, subroutine_forward_references);
	ARROW_ASSIGN(new_state, stack_offset) = ARROW(state, stack_offset);
	ARROW_ASSIGN(new_state, in_subroutine) = true;
	
	return new_state;
}

attr_nonnull
static void create_contents_of_the_block_statement(ParserState* cr state) {
	ParserState* new_state;
	
	assert_comparison(state, !=, NULL, ParserState* cr, void*);
	
	new_state = create_new_state_from_old_state(state);
	
	parse_block(new_state);
}

attr_nonnull
static void process_subroutine_forward_reference(
	ParserState* cr state,
	Subroutine* cr subroutine,
	SubroutineForwardReferences* cr subroutine_forward_references,
	const char* cr subroutine_name,
	const u subroutine_name_length,
	Statement* cr statement
) {
	LinkedListHead mli_list_head;
	LinkedListHead argument_type_list_head;
	
	assert_comparison(state, !=, NULL, ParserState* cr, void*);
	assert_comparison(subroutine, !=, NULL, Subroutine* cr, void*);
	assert_comparison(subroutine_forward_references, !=, NULL, SubroutineForwardReferences* cr, void*);
	
	mli_list_head = ARROW(subroutine_forward_references, mid_level_instruction_list);
	
	while (mli_list_head) {
		LinkedListNode* next_node;

		ARROW_ASSIGN((MidLevelInstruction*)ARROW(mli_list_head, value), arguments)[0] = (void*)ARROW(subroutine, code_offset);

		next_node = ARROW(mli_list_head, next_node);
		m_free(mli_list_head);
		mli_list_head = next_node;
	}
	
	hash_table_delete_string_key(ARROW(state, subroutine_forward_references), subroutine_name, subroutine_name_length);
	
	argument_type_list_head = ARROW(subroutine_forward_references, argument_type_list);
	
	ARROW_ASSIGN(subroutine, argument_type_list) = argument_type_list_head;
	ARROW_ASSIGN(subroutine, length_of_argument_type_list) = ARROW(subroutine_forward_references, length_of_argument_type_list);
	
	m_free(subroutine_forward_references);
	
	if (ARROW(statement, length_of_tokens) == 1) {
		if (unlikely(ARROW(subroutine, length_of_argument_type_list))) {
			output_nullt_string("Type error: defining a subroutine with 0 arguments where ");
			putu(ARROW(subroutine, length_of_argument_type_list));
			output_space();
	
			if (ARROW(subroutine, length_of_argument_type_list) == 1) {
				output_nullt_string("was");
			} else {
				output_nullt_string("were");
			}
	
			output_nullt_string(" expected\n");
			exit_program();
		}
	} else {
		parse_subroutine_arguments_in_declaration(
			state,
			subroutine,
			ARROW(subroutine, argument_type_list),
			ARROW(subroutine, length_of_argument_type_list),
			ARROW(statement, tokens) + 2,
			ARROW(statement, length_of_tokens) - 3
		);
		
		#if DEBUG
			output_nullt_string("Arguments: ");
	
			while (argument_type_list_head) {
				static const char* types[] = {"TYPE_UINT", "TYPE_SINT", "TYPE_FLOAT", "TYPE_BOOL"};
				const Type type = (Type)(u)ARROW(argument_type_list_head, value);
		
				assert_comparison(type, <=, TYPE_BOOL, Type, Type);
				output_nullt_string(ARRAY_INDEX(types, type));
				output_nullt_string(", ");
		
				argument_type_list_head = ARROW(argument_type_list_head, next_node);
			}
	
			rewind_output_pointer(2);
			output_newline();
		#endif
	}
	
	ARROW_ASSIGN(subroutine, used) = true;
}

attr_nonnull
static void parse_sub_statement(ParserState* cr state, Statement* cr statement) {
	SubroutineForwardReferences* subroutine_forward_references;
	u number_of_mid_level_instructions_in_block_statement;
	MidLevelInstruction* mid_level_instruction;
	MidLevelInstructions new_mli;
	u number_of_instructions;
	u subroutine_name_length;
	Subroutine* subroutine;
	char* subroutine_name;
	bool main_subroutine;
	bool in_hash_table;
	u old_line_number;
	void** new_value;
	
	assert_comparison(state, !=, NULL, ParserState* cr, void*);
	assert_comparison(statement, !=, NULL, Statement* cr, void*);
	
	subroutine_name_length = ARROW(ARRAY_INDEX(ARROW(statement, tokens), 0), string).length;
	subroutine_name = m_alloc(subroutine_name_length);
	memcpy(subroutine_name, ARROW(ARRAY_INDEX(ARROW(statement, tokens), 0), string).string, subroutine_name_length);
	
	if (!is_valid_identifier(subroutine_name, subroutine_name_length)) {
		output_nullt_string("Name error: " OPENING_QUOTE);
		output_string_with_length(subroutine_name, subroutine_name_length);
		output_nullt_string(CLOSING_QUOTE " is not a valid subroutine name.\n");
		exit_program();
	}
	
	main_subroutine = false;
	
	new_mli.mid_level_instructions = ARROW(ARROW(state, mli), mid_level_instructions) + ARROW(state, previous_mid_level_instruction_offset);
	new_mli.used_length = ARROW(ARROW(state, mli), used_length) - ARROW(state, previous_mid_level_instruction_offset);
	
	subroutine = (Subroutine*)m_alloc(sizeof(Subroutine));
	
	if (unlikely(!subroutine)) {
		output_nullt_string("Memory error: unable to allocate memory for subroutine.\n");
		exit_program();
	}
	
	ARROW_ASSIGN(subroutine, code_offset) =
		DEREF(ARROW(state, previous_subroutine_offset)) +
		get_length_of_mid_level_instructions(&new_mli);
	
	ARROW_ASSIGN(subroutine, line_declared_on) = get_line_number(state);
	ARROW_ASSIGN(subroutine, used) = false;
	
	#if DEBUG
		output_nullt_string("Subroutine offset: ");
		putu(ARROW(subroutine, code_offset));
		output_newline();
	#endif
	
	if (!hash_table_insert_with_string_key(
		&state->scope.subroutines_in_scope,
		subroutine_name,
		subroutine_name_length,
		subroutine,
		false,
		&new_value
	)) {
		output_nullt_string("Name error: subroutine " OPENING_QUOTE);
		output_string(&ARRAY_INDEX(ARROW(statement, tokens), 0)->string);
		output_nullt_string(CLOSING_QUOTE " already defined on line ");
		putu(ARROW((Subroutine*)DEREF(new_value), line_declared_on));
		output_newline();
		exit_program();
	}
	
	subroutine_forward_references =
		(SubroutineForwardReferences*)DEREF(
			hash_table_get_value_from_string_key(
				ARROW(state, subroutine_forward_references),
				subroutine_name,
				subroutine_name_length,
				&in_hash_table
			)
		);
	
	/* if there are 1 or more forward references */
	if (in_hash_table) {
		process_subroutine_forward_reference(
			state,
			subroutine,
			subroutine_forward_references,
			subroutine_name,
			subroutine_name_length,
			statement
		);
	} else {
		ARROW_ASSIGN(subroutine, argument_type_list) = NULL;
		ARROW_ASSIGN(subroutine, length_of_argument_type_list) = 0;
		
		if (ARROW(statement, length_of_tokens) != 1) {
			if (
				strnequal(
					ARROW(ARRAY_INDEX(ARROW(statement, tokens), 0), string).string,
					"main",
					ARROW(ARRAY_INDEX(ARROW(statement, tokens), 0), string).length,
					4
				)
			) {
				output_nullt_string("Type error: subroutine " OPENING_QUOTE "main" CLOSING_QUOTE " must be declared with 0 arguments.\n");
				exit_program();
			}
			
			parse_subroutine_arguments_in_declaration(
				state,
				subroutine,
				NULL,
				0,
				ARROW(statement, tokens) + 2,
				ARROW(statement, length_of_tokens) - 3
			);
		}
	}
	
	ARROW_ASSIGN(state, previous_mid_level_instruction_offset) = ARROW(ARROW(state, mli), used_length);
	DEREF_ASSIGN(ARROW(state, previous_subroutine_offset)) = ARROW(subroutine, code_offset);
	
	if (strnequal(subroutine_name, "main", subroutine_name_length, 4)) {
		main_subroutine = true;
		
		mid_level_instruction = (MidLevelInstruction*)m_alloc(sizeof(MidLevelInstruction));
		
		output_memory_error_if_null(
			mid_level_instruction,
			state,
			"Unable to allocate memory for a " OPENING_QUOTE "add_main_subroutine_"
			"starting_instructions" CLOSING_QUOTE " mid-level instruction.\n"
		);
		
		ARROW_ASSIGN(mid_level_instruction, opcode) = MLI_OPCODE_ADD_MAIN_SUBROUTINE_STARTING_INSTRUCTIONS;
		ARROW_ASSIGN(mid_level_instruction, arguments)[0] = (void*)DEREF(ARROW(state, stack_offset));
		mid_level_instructions_append(ARROW(state, mli), mid_level_instruction, ARROW(state, dac));
	} else {
		mid_level_instruction = (MidLevelInstruction*)m_alloc(sizeof(MidLevelInstruction));
		
		output_memory_error_if_null(
			mid_level_instruction,
			state,
			"Unable to allocate memory for a " OPENING_QUOTE "add_subroutine_"
			"starting_instructions" CLOSING_QUOTE " mid-level instruction.\n"
		);
		
		ARROW_ASSIGN(mid_level_instruction, opcode) = MLI_OPCODE_ADD_SUBROUTINE_STARTING_INSTRUCTIONS;
		ARROW_ASSIGN(mid_level_instruction, arguments)[0] = (void*)DEREF(ARROW(state, stack_offset));
		mid_level_instructions_append(ARROW(state, mli), mid_level_instruction, ARROW(state, dac));
	}
	
	tokens_del(statement);
	
	number_of_instructions = ARROW(ARROW(state, mli), used_length);
	
	#if DEBUG
		output_nullt_string("Number of mid-level-instructions: ");
		putu(number_of_instructions);
		output_nullt_string("\n\n");
	#endif
	
	old_line_number = get_line_number(state);
	assert_comparison(old_line_number, !=, 0, u, u);
	
	create_contents_of_the_block_statement(state);
	
	number_of_mid_level_instructions_in_block_statement = ARROW(ARROW(state, mli), used_length) - number_of_instructions;
	
	#if DEBUG
		output_nullt_string("Number of mid-level instructions in block statement: ");
		putu(number_of_mid_level_instructions_in_block_statement);
		output_newline();
	#endif
	
	new_mli.mid_level_instructions = ARROW(ARROW(state, mli), mid_level_instructions) + number_of_instructions;
	new_mli.used_length = number_of_mid_level_instructions_in_block_statement;
	
	{
		#if DEBUG
			u length = get_length_of_mid_level_instructions(&new_mli);
			
			output_nullt_string("Length of block statement: ");
			putu(length);
			output_nullt_string(" byte");
			if (length != 1) {
				output_char('s');
			}
			output_newline();
		#endif
	}
	
	ARROW_ASSIGN(mid_level_instruction, arguments)[0] = (void*)DEREF(ARROW(state, stack_offset));
	
	mid_level_instruction = (MidLevelInstruction*)m_alloc(sizeof(MidLevelInstruction));
	
	if (main_subroutine) {
		if (unlikely(!mid_level_instruction)) {
			output_nullt_string(
				"Memory error: unable to allocate memory for " OPENING_QUOTE
				"add_main_subroutine_ending_instructions" CLOSING_QUOTE " mid-level instruction.\n"
			);
		}
		
		ARROW_ASSIGN(mid_level_instruction, opcode) = MLI_OPCODE_ADD_MAIN_SUBROUTINE_ENDING_INSTRUCTIONS;
		ARROW_ASSIGN(mid_level_instruction, arguments)[0] = (void*)DEREF(ARROW(state, stack_offset));
	} else {
		if (unlikely(!mid_level_instruction)) {
			output_nullt_string(
				"Memory error: unable to allocate memory for " OPENING_QUOTE
				"add_subroutine_ending_instructions" CLOSING_QUOTE " mid-level instruction.\n"
			);
		}
	
		ARROW_ASSIGN(mid_level_instruction, opcode) = MLI_OPCODE_ADD_SUBROUTINE_ENDING_INSTRUCTIONS;
		ARROW_ASSIGN(mid_level_instruction, arguments)[0] = (void*)ARROW(subroutine, length_of_argument_type_list);
	}
	
	mid_level_instructions_append(ARROW(state, mli), mid_level_instruction, ARROW(state, dac));
}

static void parse_if_or_unless_statement(ParserState* cr state, Statement* cr statement) {
	u number_of_mid_level_instructions_in_block_statement;
	MidLevelInstruction* mid_level_instruction;
	MidLevelInstructions new_mli;
	u number_of_instructions;
	Variable* variable;
	u old_line_number;
	
	variable = allocate_temporary_variable_in_block_statement_expression(state, statement);
	
	assert_comparison(state, !=, NULL, ParserState* cr, void*);
	assert_comparison(statement, !=, NULL, Statement* cr, void*);
	
	parse_expression(
		state,
		variable,
		ARROW(statement, tokens),
		ARROW(statement, length_of_tokens),
		0,
		TYPE_FLOAT
	);
	
	translate_expression_into_mid_level_instructions(state, variable);
	
	mid_level_instruction = (MidLevelInstruction*)m_alloc(sizeof(MidLevelInstruction));
	
	if (unlikely(!mid_level_instruction)) {
		output_nullt_string(
			"Memory error: unable to allocate memory for a " OPENING_QUOTE
			"test_and_jmp_if_zero_fvar_memaddr" CLOSING_QUOTE " mid-level instruction.\n"
		);
		
		exit_program();
	}
	
	ARROW_ASSIGN(mid_level_instruction, opcode) = MLI_OPCODE_TEST_AND_JMP_IF_ZERO_FVAR_MEMADDR;
	ARROW_ASSIGN(mid_level_instruction, arguments)[0] = variable;
	
	mid_level_instructions_append(ARROW(state, mli), mid_level_instruction, ARROW(state, dac));
	
	tokens_del(statement);
	
	number_of_instructions = ARROW(ARROW(state, mli), used_length);
	
	#if DEBUG
		output_nullt_string("Number of mid-level-instructions: ");
		putu(number_of_instructions);
		output_nullt_string("\n\n");
	#endif
	
	old_line_number = get_line_number(state);
	
	create_contents_of_the_block_statement(state);
	
	number_of_mid_level_instructions_in_block_statement = ARROW(ARROW(state, mli), used_length) - number_of_instructions;
	
	#if DEBUG
		output_nullt_string("Number of mid-level instructions in block statement: ");
		putu(number_of_mid_level_instructions_in_block_statement);
		output_newline();
	#endif
	
	new_mli.mid_level_instructions = ARROW(ARROW(state, mli), mid_level_instructions) + number_of_instructions;
	new_mli.used_length = number_of_mid_level_instructions_in_block_statement;
	
	{
		u length = get_length_of_mid_level_instructions(&new_mli);
		
		#if DEBUG
			output_nullt_string("Length of block statement: ");
			putu(length);
			output_nullt_string(" byte");
			if (length != 1) {
				output_char('s');
			}
			output_newline();
		#endif
		
		ARROW_ASSIGN(mid_level_instruction, arguments)[1] = (void*)length;
	}
}

attr_nonnull void parse_block_statement(ParserState* cr state) {
	Statement* statement;
	BlockStatementType block_statement_type;
	
	static void (*empty_statement_functions[])(Statement* cr statement, const u line_number_at_start_of_statement) = {
		empty_statement_function_for_if_statements,
		empty_statement_function_for_unless_statements,
		empty_statement_function_for_sub_statements
	};
	
	assert_comparison(state, !=, NULL, ParserState* cr, void*);
	
	statement = (Statement*)m_alloc(sizeof(Statement));
	output_memory_error_if_null(statement, state, "Unable to allocate memory to parse a block statement");
	
	free_list_append(ARROW(state, free_list_head), statement, m_free);
	
	block_statement_type = get_block_statement_type(state, statement);
	
	parse_statement_string_into_tokens(
		statement,
		0,
		ARROW(state, line_number_at_start_of_statement),
		ARRAY_INDEX(empty_statement_functions, block_statement_type)
	);
	
	#if DEBUG && SHOW_TOKENS
		show_tokens_in_statement(statement);
	#endif
	
	if (block_statement_type == BLOCK_STATEMENT_TYPE_SUB_STATEMENT) {
		parse_sub_statement(state, statement);
	} else {
		parse_if_or_unless_statement(state, statement);
	}
}
