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
#include "resize_statement_buffer_if_necessary.h"
#include "parse_statement_string_into_tokens.h"
#include "parse_block_statement.h"
#include "parse_statement.h"
#include "line_number.h"
#include "tokens_del.h"
#include "utf8.h"

#undef unsigned
#undef short

static void read_characters_until_newline_or_eof(ParserState* cr state) {
	u original_statement_buffer_offset;
	
	assert_comparison(state, !=, NULL, ParserState* cr, void*);
	
	original_statement_buffer_offset = ARROW(state, statement_buffer_offset);
	
	while (1) {
		char char_[7];
		
		get_next_char(state, char_);
		
		if (chreq_utf8_ascii(char_, '\n') || is_eof(ARROW(state, file))) {
			rewind_file(ARROW(state, file), ARROW(state, statement_buffer_offset) - original_statement_buffer_offset);
			break;
		}
		
		memcpy(ARROW(state, statement_buffer) + ARROW(state, statement_buffer_offset), char_, get_length_of_utf8_char(char_));
		ARROW_ASSIGN(state, statement_buffer_offset) += get_length_of_utf8_char(char_);
		
		resize_statement_buffer_if_necessary(state);
	}
}

/*
Here's what this function does:

If the statement is like this

1.  foo bar baz foo bar
2.  bla bla bla bla
3.  quux foo bar baz
4.  
5.  
6.  

this function would set the statement buffer offset to equal the offset of the last non-newline character on line 3 (a 'z').

This function sets the statement buffer offset to be the offset of the last non-whitespace
character in the statement buffer, in order to make error messages nicer.  It doesn't look
good to output an error message with unnecessary blank lines.
*/
static void set_statement_buffer_offset_to_be_the_offset_of_the_last_nonempty_line_in_statement(ParserState* cr state) {
	ARROW_ASSIGN(state, statement_buffer_offset)--;
	increment_line_number(state);
	
	while (isspace(ARRAY_INDEX(ARROW(state, statement_buffer), ARROW(state, statement_buffer_offset)))) {
		if (ARRAY_INDEX(ARROW(state, statement_buffer), ARROW(state, statement_buffer_offset)) == '\n') {
			decrement_line_number(state);
		}
		
		ARROW_ASSIGN(state, statement_buffer_offset)--;
	}
	
	while (ARRAY_INDEX(ARROW(state, statement_buffer), ARROW(state, statement_buffer_offset)) != '\n') {
		ARROW_ASSIGN(state, statement_buffer_offset)++;
	}
}

attrs_cold_and_nonnull
static void empty_statement_function_for_normal_statements(
	Statement* cr statement,
	const u line_number_at_start_of_statement
) {
	output_nullt_string("Syntax error on line ");
	putu(line_number_at_start_of_statement);
	output_nullt_string(": unlike C, C++, Java, C#, D and other C-like languages, in " LANGUAGE_NAME " an empty statement is invalid.\n");
	output_source_lines(statement, line_number_at_start_of_statement);
}

attrs_cold_and_nonnull
static void give_missing_semicolon_error_message(ParserState* cr state) {
	assert_comparison(state, !=, NULL, ParserState* cr, void*);
	
	set_statement_buffer_offset_to_be_the_offset_of_the_last_nonempty_line_in_statement(state);

	output_syntax_error_beginning_text(0, 0);
	output_nullt_string("missing semicolon at the end of line ");
	
	/* 1 is subtracted because the newline has already been reached. */
	putu(get_line_number(state) - 1);
	output_nullt_string("\n\n   ");
	
	output_string_with_length(ARROW(state, statement_buffer), ARROW(state, statement_buffer_offset));
	
	/* Nice unicode arrow, isn't it? :) */
	output_nullt_string(" ◀――― ;\n");
	
	exit_program();
}

static void handle_opening_curly(ParserState* cr state) {
	assert_comparison(state, !=, NULL, ParserState* cr, void*);
	
	read_characters_until_newline_or_eof(state);
	
	parse_block_statement(state);

	ARROW_ASSIGN(state, line_number_at_start_of_statement) = get_line_number(state);
	ARROW_ASSIGN(state, before_statement) = true;
}

static void handle_semicolon(ParserState* cr state) {
	Statement* statement;
	
	assert_comparison(state, !=, NULL, ParserState* cr, void*);
	
	statement = (Statement*)m_alloc(sizeof(Statement));
	output_memory_error_if_null(statement, state, "Unable to allocate memory for statement");

	ARROW_ASSIGN(statement, statement_line) = m_alloc(ARROW(state, statement_buffer_offset));
	output_memory_error_if_null(ARROW(statement, statement_line), state, "Unable to allocate memory for statement buffer");

	ARROW_ASSIGN(statement, length_of_statement_line_string) = ARROW(state, statement_buffer_offset);
	memcpy(ARROW(statement, statement_line), ARROW(state, statement_buffer), ARROW(state, statement_buffer_offset));

	parse_statement_string_into_tokens(
		statement,
		ARROW(state, first_meaningful_character_offset),
		ARROW(state, line_number_at_start_of_statement),
		empty_statement_function_for_normal_statements
	);
	
	ARROW_ASSIGN(state, line_number_at_start_of_statement) = get_line_number(state);
	parse_statement(state, statement);
	tokens_del(statement);
	
	free_list_append(ARROW(state, free_list_head), ARROW(statement, statement_line), m_free);
	free_list_append(ARROW(state, free_list_head), statement, m_free);
	
	ARROW_ASSIGN(state, before_statement) = true;
}

static LoopHasFinished handle_current_character(ParserState* cr state) {
	if (chreq_utf8_ascii(state->utf8_char, '\n')) {
		increment_line_number(state);
	}
	
	if (chreq_utf8_ascii(state->utf8_char, '{')) {
		handle_opening_curly(state);
		return LOOP_HAS_FINISHED;
	}
	
	if (chreq_utf8_ascii(state->utf8_char, ';')) {
		handle_semicolon(state);
		return LOOP_HAS_FINISHED;
	}
	
	ARROW_ASSIGN(state, before_statement) = false;
	get_next_char(state, state->utf8_char);
	
	return LOOP_HASNT_FINISHED_YET;
}

static void add_current_character_to_statement_buffer(ParserState* cr state) {
	assert_comparison(state, !=, NULL, ParserState* cr, void*);
	
	resize_statement_buffer_if_necessary(state);
	
	memcpy(
		ARROW(state, statement_buffer) + ARROW(state, statement_buffer_offset),
		state->utf8_char,
		get_length_of_utf8_char(state->utf8_char)
	);
	
	ARROW_ASSIGN(state, statement_buffer_offset) += get_length_of_utf8_char(state->utf8_char);
}

attr_nonnull
static LoopHasFinished read_and_parse_statement_iteration(ParserState* cr state) {
	assert_comparison(state, !=, NULL, ParserState* cr, void*);
	
	/*
	 * If the end of the file was reached, the statement was never properly
	 * ended by a semicolon, so give a "missing semicolon" error message.
	 */
	if (unlikely(is_eof(ARROW(state, file)))) {
		give_missing_semicolon_error_message(state);
	}
	
	add_current_character_to_statement_buffer(state);
	
	return handle_current_character(state);
}

attr_nonnull void read_and_parse_statement(ParserState* cr state) {
	LoopHasFinished loop_has_finished;
	
	assert_comparison(state, !=, NULL, ParserState* cr, void*);
	
	loop_has_finished = false;
	
	do {
		loop_has_finished = read_and_parse_statement_iteration(state);
	} while (loop_has_finished == LOOP_HASNT_FINISHED_YET);
}
