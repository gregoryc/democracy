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
#include "resize_statement_buffer_if_necessary.h"
#include "read_and_parse_statement.h"
#include "subroutine_value_deletion_function.h"
#include "parse_block.h"
#include "line_number.h"
#include "show_tokens.h"
#include "utf8.h"

#include "parser_dot_c_static_functions.h"

/*
 *  This function parses a block, that is to say, it parses a list of
 *  statements surrounded by curly braces. This function can be considered
 *  to be the core of this program, because each module is surrounded a
 *  large block that contains all functions and declarations defined in
 *  the module, and so this function does most of the work of this
 *  program.
 */
attr_nonnull void parse_block(ParserState* cr state) {
	assert_comparison(state, !=, NULL, ParserState* cr, void*);
	
	ARROW_ASSIGN(state, before_statement) = true;
	
	setup_the_hash_tables_that_are_in_state(state);
	allocate_statement_buffer(state);
	main_parser_loop(state);
	
	#if DEBUG
		output_subroutines_in_scope_hash_table(state);
	#endif
	
	m_free(ARROW(state, statement_buffer));
	hash_table_del(&state->scope.variables_in_scope);
	
	if (!ARROW(state, in_subroutine)) {
		MidLevelInstructions new_mli;
		
		new_mli.mid_level_instructions = ARROW(ARROW(state, mli), mid_level_instructions) + ARROW(state, previous_mid_level_instruction_offset);
		new_mli.used_length = ARROW(ARROW(state, mli), used_length) - ARROW(state, previous_mid_level_instruction_offset);
		
		DEREF_ASSIGN(ARROW(state, previous_subroutine_offset)) += get_length_of_mid_level_instructions(&new_mli);
	}
	
	hash_table_del_with_keys_and_values(&state->scope.subroutines_in_scope, subroutine_value_deletion_function);
	m_free(state);
}

attr_nonnull static inline void setup_the_hash_tables_that_are_in_state(ParserState* cr state) {
	assert_comparison(state, !=, NULL, ParserState* cr, void*);
	
	hash_table_init(&state->scope.variables_in_scope, 20, 4, 3.0);
	hash_table_init(&state->scope.subroutines_in_scope, 10, 4, 3.0);
}

attr_nonnull static inline void allocate_statement_buffer(ParserState* cr state) {
	assert_comparison(state, !=, NULL, ParserState* cr, void*);
	
	ARROW_ASSIGN(state, statement_buffer_allocated_length) = get_initial_statement_buffer_allocated_length();
	
	ARROW_ASSIGN(state, statement_buffer) = m_alloc(ARROW(state, statement_buffer_allocated_length));
	output_memory_error_if_null(ARROW(state, statement_buffer), state, "Unable to allocate memory to parse block");
}

attr_const static inline u get_initial_statement_buffer_allocated_length(void) {
	/*
	 *  OPTIMIZE_FOR_SPEED is likely because it's default.
	 *
	 *  If optimizing for speed, more initial memory should be allocated
	 *  for the buffer because it's faster. But if optimizing for memory
	 *  usage, less memory (i.e., 20 bytes) should be allocated.
	 */
	if (likely(OPTIMIZE_FOR_SPEED)) {
		return 140;
	} else {
		return 20;
	}
}

attr_nonnull static inline void main_parser_loop(ParserState* cr state) {
	LoopHasFinished loop_finished;
	
	assert_comparison(state, !=, NULL, ParserState* cr, void*);
	
	do {
		loop_finished = main_parser_loop_iteration(state);
	} while (loop_finished == LOOP_HASNT_FINISHED_YET);
}

attr_nonnull
static LoopHasFinished main_parser_loop_iteration(ParserState* cr state) {
	assert_comparison(state, !=, NULL, ParserState* cr, void*);
	
	/*
	 *  ARROW(state, block_comment_starting_line) is set to 0 because if
	 *  block_comment_starting_line is zero, it indicates that the parser
	 *  is not yet in a block comment.
	 */
	ARROW_ASSIGN(state, block_comment_starting_line) = 0;
	ARROW_ASSIGN(state, statement_buffer_offset) = 0;
	
	/*
	 *  The statement buffer offset is set to the beginning
	 *  last line in order to make error messages look nicer.
	 */
	skip_over_beginning_comments_and_whitespace_and_set_statement_buffer_offset_to_beginning_of_last_line(state);
	
	output_nullt_string("Chars: ");
	
	while (!reached_eof(state)) {
		char char_[7];
		
		get_next_char(state, char_);
		
		output_string_with_length(char_, get_length_of_utf8_char(char_));
	}
	
	exit_program();
	
	if (reached_eof(state)) {
		if (ARROW(state, in_subroutine)) {
			output_syntax_error_beginning_text(0, 0);
			output_nullt_string("Missing " OPENING_QUOTE "}" CLOSING_QUOTE " at end of file\n");
			exit_program();
		}
		
		return LOOP_HAS_FINISHED;
	}
	
	if (chreq_utf8_ascii(state->utf8_char, '}')) {
		if (unlikely(!ARROW(state, in_subroutine))) {
			output_syntax_error_beginning_text(get_line_number(state), 0);
			output_nullt_string("Extra " OPENING_QUOTE "}" CLOSING_QUOTE "\n");
			exit_program();
		}
		
		return LOOP_HAS_FINISHED;
	}
	
	/*
	 *  Since we already skipped over any beginning comments and whitespace, the current
	 *  statement_buffer_offset (which is right now 1 more than the last valid offset)
	 *  will be (but not is) the first meaningful character offset. It "will be" because
	 *  ARRAY_INDEX(state.statement_buffer, state.statement_buffer_offset) is uninitialized,
	 *  but will be initialized later in the read_and_parse_statement() function.
	 */
	ARROW_ASSIGN(state, first_meaningful_character_offset) = ARROW(state, statement_buffer_offset);
	ARROW_ASSIGN(state, line_number_at_start_of_statement) = get_line_number(state);
	
	read_and_parse_statement(state);
	
	return LOOP_HASNT_FINISHED_YET;
}

/* This function has got to have the longest name in existence. */
attr_nonnull
static LoopHasFinished skip_over_beginning_comments_and_whitespace_and_set_statement_buffer_offset_to_beginning_of_last_line_iteration(
	ParserState* cr state,
	u* restrict number_of_slash_stars_in_block_comment,
	char last_char[7]
) {
	assert_comparison(state, !=, NULL, ParserState* cr, void*);
	assert_comparison(number_of_slash_stars_in_block_comment, !=, NULL, u* restrict, void*);
	assert_comparison(last_char, !=, NULL, char*, void*);
	
	get_next_char(state, state->utf8_char);
	
	output_nullt_string("!");
	output_string_with_length(state->utf8_char, get_length_of_utf8_char(state->utf8_char));
	
	if (unlikely(reached_eof(state))) {
		if (ARROW(state, block_comment_starting_line)) {
			unclosed_block_comment_error(state);
		}
		
		return LOOP_HAS_FINISHED;
	}
	
	resize_statement_buffer_if_necessary(state);
	put_character_in_statement_buffer(state);
	
	if (chreq_utf8_ascii(state->utf8_char, '\n')) {
		increment_line_number(state);
		memcpy(last_char, state->utf8_char, get_length_of_utf8_char(state->utf8_char));
		ARROW_ASSIGN(state, statement_buffer_offset) = 0;
		
		return LOOP_HASNT_FINISHED_YET;
	}
	
	if (utf8_char_is_space(state->utf8_char)) {
		goto end_of_iteration;
	}
	
	if (chreq_utf8_ascii(last_char, '*') && chreq_utf8_ascii(state->utf8_char, '/')) {
		if (unlikely(!ARROW(state, block_comment_starting_line))) {
			output_syntax_error_beginning_text(0, 0);
			output_nullt_string("Block comment never opened.\n");
			
			exit_program();
		}
		
		ARROW_ASSIGN(state, block_comment_starting_line) = 0;
		DEREF_ASSIGN(number_of_slash_stars_in_block_comment) = 0;
		goto end_of_iteration;
	}
	
	if (chreq_utf8_ascii(state->utf8_char, '/')) {
		char next_char[7];
		get_next_char(state, next_char);
		
		if (unlikely(reached_eof(state))) {
			output_nullt_string("End of file reached when it wasn't expected.\n");
			exit_program();
		}
		
		rewind_file(ARROW(state, file), get_length_of_utf8_char(next_char));
		
		if (chreq_utf8_ascii(next_char, '*')) {
			if (unlikely(DEREF(number_of_slash_stars_in_block_comment))) {
				output_comment_warning_beginning_text(get_line_number(state), 0);
				
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
						output_nullt_string("th " OPENING_QUOTE "/*" CLOSING_QUOTE " in block comment\n");
				}
			} else {
				ARROW_ASSIGN(state, block_comment_starting_line) = get_line_number(state);
			}
			
			DEREF_ASSIGN(number_of_slash_stars_in_block_comment)++;
			goto end_of_iteration;
		}
	}
	
	/* Stop iterating if not in a comment. */
	if (!ARROW(state, block_comment_starting_line)) {
		if (chreq_utf8_ascii(state->utf8_char, '*')) {
			char next_char[7];
			get_next_char(state, next_char);
			
			if (unlikely(chreq_utf8_ascii(next_char, '/'))) {
				output_syntax_error_beginning_text(get_line_number(state), 0);
				output_nullt_string("Block comment never opened.\n");
				exit_program();
			}
			
			rewind_file(ARROW(state, file), get_length_of_utf8_char(next_char));
		}
		
		return LOOP_HAS_FINISHED;
	}
	
	end_of_iteration:
		memcpy(last_char, state->utf8_char, get_length_of_utf8_char(state->utf8_char));
		ARROW_ASSIGN(state, statement_buffer_offset) += get_length_of_utf8_char(state->utf8_char);
	
	return LOOP_HASNT_FINISHED_YET;
}

/*
 *  A little helper function to check whether or not the file
 *  that is being read from has reached the end of the file.
 */
attr_nonnull static inline bool reached_eof(ParserState* cr state) {
	assert_comparison(state, !=, NULL, ParserState* cr, void*);
	
	return is_eof(ARROW(state, file));
}

attrs_cold_and_nonnull
static void unclosed_block_comment_error(ParserState* cr state) {
	assert_comparison(state, !=, NULL, ParserState* cr, void*);
	
	output_syntax_error_beginning_text(get_line_number(state) - 1, 0);
	output_nullt_string("Unclosed block comment");

	if (ARROW(state, block_comment_starting_line) != get_line_number(state) - 1) {
		output_nullt_string(" (it was opened on line ");
		putu(ARROW(state, block_comment_starting_line));
		output_char(')');
	}

	output_newline();

	exit_program();
}

attr_nonnull static void
skip_over_beginning_comments_and_whitespace_and_set_statement_buffer_offset_to_beginning_of_last_line(
	ParserState* cr state
) {
	u number_of_slash_stars_in_block_comment;
	char last_char[7];
	LoopHasFinished loop_finished;
	
	assert_comparison(state, !=, NULL, ParserState* cr, void*);
	
	number_of_slash_stars_in_block_comment = 0;
	
	last_char[0] = '\0';
	last_char[6] = 1;
	
	do {
		loop_finished = skip_over_beginning_comments_and_whitespace_and_set_statement_buffer_offset_to_beginning_of_last_line_iteration(
			state,
			&number_of_slash_stars_in_block_comment,
			last_char
		);
	} while (loop_finished == LOOP_HASNT_FINISHED_YET);
}

attr_nonnull
static inline void put_character_in_statement_buffer(ParserState* cr state) {
	assert_comparison(state, !=, NULL, ParserState* cr, void*);
	
	memcpy(
		ARROW(state, statement_buffer) + ARROW(state, statement_buffer_offset),
		state->utf8_char,
		get_length_of_utf8_char(state->utf8_char)
	);
}

#undef unsigned
#undef short

attrs_const_and_nonnull
static inline bool utf8_char_is_space(const char utf8_char[7]) {
	return get_length_of_utf8_char(utf8_char) == 1 && isspace(DEREF(utf8_char));
}

#define short YOU_SHOULD_NOT_USE_SHORT
#define unsigned YOU_SHOULD_NOT_USE_UNSIGNED

#if DEBUG
	static void output_subroutine(const Subroutine* cr subroutine, const char* cr subroutine_name, const u length_of_subroutine_name) {
		u i;
		
		assert_comparison(subroutine, !=, NULL, const Subroutine* cr, void*);
		assert_comparison(subroutine_name, !=, NULL, const char* cr, void*);
		assert_comparison(length_of_subroutine_name, !=, 0, const u, u);
		
		output_nullt_string("   ");
		output_string_with_length(subroutine_name, length_of_subroutine_name);
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
