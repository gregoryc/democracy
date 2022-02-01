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
#include "../global_headers/globals.h"
#include "read_and_parse_statement.h"
#include "parse_block.h"
#include "line_number.h"
#include "show_tokens.h"
#include "read_char.h"

#undef unsigned
#undef short

#include "parser_dot_c_static_functions.h"

attr_nonnull void parse_block(ParserState* cr state) {
	assert_comparison(state, !=, NULL, ParserState* cr, void*);
	
	ARROW_ASSIGN(state, before_statement) = true;
	
	setup_hash_tables(state);
	
	ARROW_ASSIGN(state, statement_buffer_allocated_length) = get_initial_statement_buffer_allocated_length();
	
	/* Allocate memory for the statement buffer and do error handling if memory can't be allocated. */
	ARROW_ASSIGN(state, statement_buffer) = m_alloc(ARROW(state, statement_buffer_allocated_length));
	if (unlikely(!ARROW(state, statement_buffer))) unable_to_allocate_memory_to_parse_block();
	
	main_parser_loop(state);
	
	#if DEBUG
		output_subroutines_in_scope_hash_table(state);
	#endif
	
	m_free(ARROW(state, statement_buffer));
	hash_table_del(&state->scope.variables_in_scope);
}

attr_cold_and_noreturn static void unable_to_allocate_memory_to_parse_block(void) {
	output_nullt_string("Unable to allocate memory to parse block.\n");
	exit_program();
}

attr_nonnull
static u /* loop finished? */
skip_over_beginning_comments_and_whitespace_iteration(
	ParserState* cr state,
	u* restrict number_of_slash_stars_in_block_comment,
	int* restrict last_char
) {
	ARROW_ASSIGN(state, char_) = read_char(state);
	
	if (ARROW(state, statement_buffer_offset) == ARROW(state, statement_buffer_allocated_length)) {
		char* old_buffer_pointer = ARROW(state, statement_buffer);
		
		ARROW_ASSIGN(state, statement_buffer_allocated_length) *= 2;
		
		ARROW_ASSIGN(state, statement_buffer) = m_realloc(ARROW(state, statement_buffer), ARROW(state, statement_buffer_allocated_length));
		if (unlikely(!ARROW(state, statement_buffer))) {
			m_free(old_buffer_pointer);
			unable_to_allocate_memory_to_parse_block();
		}
	}
	
	ARRAY_INDEX_ASSIGN(ARROW(state, statement_buffer), ARROW(state, statement_buffer_offset)) = ARROW(state, char_);
	
	if (unlikely(ARROW(state, char_) == EOF)) {
		if (ARROW(state, block_comment_starting_line)) {
			output_nullt_string("Syntax error: unclosed block comment");
			
			if (ARROW(state, block_comment_starting_line) != get_line_number(state) - 1) {
	 			output_nullt_string(" (it was opened on line ");
				putu(ARROW(state, block_comment_starting_line));
				output_char(')');
			}
			
			output_newline();
			
			exit_program();
		}
		
		/* The loop has finished. */
		return true;
	}
	
	if (ARROW(state, char_) == '\n') {
		increment_line_number(state);
		DEREF_ASSIGN(last_char) = ARROW(state, char_);
		ARROW_ASSIGN(state, statement_buffer_offset) = 0;
		
		/* The loop hasn't finished yet. */
		return false;
	}
	
	if (unlikely(isspace(ARROW(state, char_)))) {
		goto end_of_iteration;
	}
	
	if (DEREF(last_char) == '*' && ARROW(state, char_) == '/') {
		ARROW_ASSIGN(state, block_comment_starting_line) = 0;
		number_of_slash_stars_in_block_comment = 0;
		goto end_of_iteration;
	}
	
	if (unlikely(ARROW(state, char_) == '/')) {
		int next_char = read_char(state);
		
		if (unlikely(next_char == EOF)) {
			assert_comparison(0, !=, 0, u, u);
		}
		
		if (unlikely(ungetc(next_char, ARROW(state, file)) == EOF)) {
			output_nullt_string("ungetc() failed.\n");
			exit_program();
		}
		
		if (next_char == '*') {
			if (DEREF(number_of_slash_stars_in_block_comment)) {
				output_nullt_string("Warning on line ");
				putu(get_line_number(state));
				output_nullt_string(": ");
				
				switch (DEREF(number_of_slash_stars_in_block_comment)) {
				case 1:
					output_nullt_string(OPENING_QUOTE "/*" CLOSING_QUOTE " in block comment\n");
					break;
				case 2:
					output_nullt_string("another " OPENING_QUOTE "/*" CLOSING_QUOTE " in block comment\n");
					break;
				case 3:
					output_nullt_string("yet another " OPENING_QUOTE "/*" CLOSING_QUOTE " in block comment\n");
					break;
				default:
					output_nullt_string("a ");
					putu(DEREF(number_of_slash_stars_in_block_comment));
					
					if (DEREF(number_of_slash_stars_in_block_comment) == 3) {
						output_nullt_string("rd");
					} else {
						output_nullt_string("th");
					}
					
					output_nullt_string(" " OPENING_QUOTE "/*" CLOSING_QUOTE " in block comment\n");
				}
			} else {
				ARROW_ASSIGN(state, block_comment_starting_line) = get_line_number(state);
			}
			
			++number_of_slash_stars_in_block_comment;
			goto end_of_iteration;
		} else {
			/* Fall through to next the if statement. */
		}
	}
	
	/* Stop iterating if not in a comment. */
	if (!ARROW(state, block_comment_starting_line)) {
		if (ARROW(state, char_) == '*') {
			int next_char = read_char(state);
			
			if (next_char == '/') {
				output_nullt_string("Syntax error on line ");
				putu(get_line_number(state));
				output_nullt_string(": block comment never opened.\n");
				exit_program();
			}
			
			if (ungetc(next_char, ARROW(state, file)) == EOF) {
				output_nullt_string("ungetc() failed.\n");
				exit_program();
			}
		}
		
		/* The loop has finished. */
		return true;
	}
	
	end_of_iteration:
		DEREF_ASSIGN(last_char) = ARROW(state, char_);
		ARROW_ASSIGN(state, statement_buffer_offset)++;
	
	/* The loop hasn't finished yet. */
	return false;
}

attr_nonnull static void skip_over_beginning_comments_and_whitespace(ParserState* cr state) {
	u number_of_slash_stars_in_block_comment;
	int last_char;
	bool loop_finished;
	
	assert_comparison(state, !=, NULL, ParserState* cr, void*);
	
	number_of_slash_stars_in_block_comment = 0;
	last_char = '\0';
	
	do {
		loop_finished = skip_over_beginning_comments_and_whitespace_iteration(
			state,
			&number_of_slash_stars_in_block_comment,
			&last_char
		);
	} while (likely(!loop_finished));
}

#if DEBUG
	static void output_subroutine(const Subroutine* cr subroutine, const char* cr subroutine_name, const u length_of_subroutine_name) {
		u i;
		
		assert_comparison(subroutine, !=, NULL, const Subroutine* cr, void*);
		assert_comparison(subroutine_name, !=, NULL, const char* cr, void*);
		assert_comparison(length_of_subroutine_name, !=, 0, const u, u);
		
		output_nullt_string("   ");
		output_string(subroutine_name, length_of_subroutine_name);
		output_nullt_string(" =>  Subroutine {\n");
		
		i = length_of_subroutine_name + 11;
		
		while (i--) {
			output_char(' ');
		}
		
		output_nullt_string("code_offset = ");
		putu(ARROW(subroutine, code_offset));
		output_nullt_string(",\n");
		
		i = length_of_subroutine_name + 11;
		
		while (i--) {
			output_char(' ');
		}
		
		output_nullt_string("line_declared_on = ");
		putu(ARROW(subroutine, line_declared_on));
		output_newline();
		
		i = length_of_subroutine_name + 8;
		
		while (i--) {
			output_char(' ');
		}
		
		output_nullt_string("}\n\n");
	}
	
	static void output_subroutines_in_scope_hash_table(ParserState* cr state) {
		HashIterator iterator;
		u length;
		
		output_nullt_string("\nSubroutines in scope hash table:\n\n");
		
		length = 0;
		
		hash_iterator_init(&state->scope.subroutines_in_scope, &iterator);
		
		while (hash_iterator_next(&iterator)) {
			Subroutine* subroutine = (Subroutine*)DEREF(iterator.value);
			
			output_subroutine(subroutine, iterator.key.string_value, iterator.length_of_key);
			
			++length;
		}
		
		assert_comparison(length, ==, ARROW(state, scope).subroutines_in_scope.length, u, u);
	}
#endif

static inline void setup_hash_tables(ParserState* cr state) {
	assert_comparison(state, !=, NULL, ParserState* cr, void*);
	
	hash_table_init(&state->scope.variables_in_scope, 20, 4, 3.0);
	hash_table_init(&state->scope.subroutines_in_scope, 10, 4, 3.0);
}

static inline u get_initial_statement_buffer_allocated_length(void) {
	/*
	OPTIMIZE_FOR_SPEED is likely because it's default.
	
	More initial memory should be allocated for the buffer if optimizing for
	speed because it's faster. But if optimizing for memory usage, less memory
	(i.e., 20 bytes) should be allocated.
	*/
	if (likely(OPTIMIZE_FOR_SPEED)) {
		return 140;
	} else {
		return 20;
	}
}

static inline void main_parser_loop(ParserState* cr state) {
	bool loop_finished;
	
	assert_comparison(state, !=, NULL, ParserState* cr, void*);
	
	do {
		loop_finished = main_parser_loop_iteration(state);
	} while (likely(!loop_finished));
}

attr_nonnull
static u /* loop finished? */
main_parser_loop_iteration(ParserState* cr state) {
	assert_comparison(state, !=, NULL, ParserState* cr, void*);
	
	/*
	ARROW(state, block_comment_starting_line) is set to 0 because if
	block_comment_starting_line is zero, it indicates that the parser
	is not in a block comment yet.
	*/
	ARROW_ASSIGN(state, block_comment_starting_line) = 0;
	ARROW_ASSIGN(state, statement_buffer_offset) = 0;
	
	skip_over_beginning_comments_and_whitespace(state);
	
	if (unlikely(feof(ARROW(state, file)) || ARROW(state, char_) == '}')) {
		/* The main parser loop is finished. */
		return true;
	}
	
	/*
	Since we already skipped over any beginning comments and whitespace, the current
	statement_buffer_offset (which is right now 1 more than the last valid offset)
	will be (but not is) the first meaningful character offset. It "will be" because
	ARRAY_INDEX(state.statement_buffer, state.statement_buffer_offset) is uninitialized,
	but will be initialized later in the read_and_parse_statement() function.
	*/
	ARROW_ASSIGN(state, first_meaningful_character_offset) = ARROW(state, statement_buffer_offset);
	ARROW_ASSIGN(state, line_number_at_start_of_statement) = get_line_number(state);
	
	read_and_parse_statement(state);
	
	/* The main parser loop is not finished yet. */
	return false;
}
