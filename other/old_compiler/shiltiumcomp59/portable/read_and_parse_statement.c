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
#include "parse_statement_string_into_tokens.h"
#include "parse_block_statement.h"
#include "parse_statement.h"
#include "line_number.h"
#include "tokens_del.h"
#include "read_char.h"

#undef unsigned
#undef short

static void resize_buffer_if_necessary(ParserState* cr state) {
	assert_comparison(state, !=, NULL, ParserState* cr, void*);
	
	if (unlikely(ARROW(state, statement_buffer_offset) == ARROW(state, statement_buffer_allocated_length))) {
		char* old_buffer_pointer = ARROW(state, statement_buffer);
		ARROW_ASSIGN(state, statement_buffer_allocated_length) *= 2;
		
		ARROW_ASSIGN(state, statement_buffer) =
			m_realloc(
				ARROW(state, statement_buffer),
				ARROW(state, statement_buffer_allocated_length)
			);
		
		if (unlikely(!ARROW(state, statement_buffer))) {
			m_free(old_buffer_pointer);
			output_nullt_string("Memory error: unable to allocate memory to resize statement buffer.\n");
			exit_program();
		}
	}
}

static void read_characters_until_newline_or_eof(ParserState* cr state) {
	u original_statement_buffer_offset;
	
	assert_comparison(state, !=, NULL, ParserState* cr, void*);
	
	original_statement_buffer_offset = ARROW(state, statement_buffer_offset);
	
	while (1) {
		int char_ = read_char(state);
		
		if (char_ == '\n' || char_ == EOF) {
			long number_of_bytes = -(long)(ARROW(state, statement_buffer_offset) - original_statement_buffer_offset);
			
			/* This is needed in Windows, or at least when using MinGW32 + Wine, for some reason. */
			#if WINDOWS
			if (number_of_bytes) {
			#endif
				if (unlikely(fseek(ARROW(state, file), number_of_bytes, SEEK_CUR) == -1)) {
					output_nullt_string("fseek() failed: ");
					output_errno_string();
					output_newline();
					exit_program();
				}
			#if WINDOWS
			}
			#endif
			
			break;
		}
		
		ARRAY_INDEX_ASSIGN(ARROW(state, statement_buffer), ARROW(state, statement_buffer_offset)) = char_;
		ARROW_ASSIGN(state, statement_buffer_offset)++;
		
		resize_buffer_if_necessary(state);
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

This functions sets the statement buffer offset to equal the last non-empty line in the statement, in order to
make error messages nicer. It doesn't look good to output an error message with unnecessary blank lines.
*/
static void set_statement_buffer_offset_to_equal_the_last_nonempty_line_in_statement(ParserState* cr state) {
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

attr_nonnull
static void empty_statement_function_for_normal_statements(
	Statement* cr statement,
	const u line_number_at_start_of_statement
) {
	output_nullt_string("Syntax error on line ");
	putu(line_number_at_start_of_statement);
	output_nullt_string(": unlike C, C++, Java, C#, D and other C-like languages, in " LANGUAGE_NAME " an empty statement is invalid.\n");
	output_source_lines(statement, line_number_at_start_of_statement);
}

attr_nonnull void read_and_parse_statement(ParserState* cr state) {
	assert_comparison(state, !=, NULL, ParserState* cr, void*);
	
	while (1) {
		/* If the current character is a newline, increment the line number. */
		if (unlikely(ARROW(state, char_) == '\n')) {
			increment_line_number(state);
		}
		
		/*
		If the current charcter is EOF, the statement was never properly
		ended by a semicolon, so give a "missing semicolon" error message.
		*/
		if (unlikely(ARROW(state, char_) == EOF)) {
			set_statement_buffer_offset_to_equal_the_last_nonempty_line_in_statement(state);
			
			output_syntax_error_beginning_text(0, 0);
			output_nullt_string("missing semicolon at the end of line ");
			putu(get_line_number(state) - 1);
			output_newline();
			output_string(ARROW(state, statement_buffer), ARROW(state, statement_buffer_offset));
			
			/* Nice unicode arrow, isn't it? :) */
			output_nullt_string(" ◀――― ;\n");
			
			exit_program();
		}
		
		resize_buffer_if_necessary(state);
		
		ARRAY_INDEX_ASSIGN(ARROW(state, statement_buffer), ARROW(state, statement_buffer_offset)) = ARROW(state, char_);
		ARROW_ASSIGN(state, statement_buffer_offset)++;
		
		if (unlikely(ARROW(state, char_) == '{')) {
			read_characters_until_newline_or_eof(state);
			
			parse_block_statement(state);
			
			ARROW_ASSIGN(state, line_number_at_start_of_statement) = get_line_number(state);
			ARROW_ASSIGN(state, before_statement) = true;
			
			break;
		}
		
		if (unlikely(ARROW(state, char_) == ';')) {
			Statement* statement = (Statement*)m_alloc(sizeof(Statement));
			
			if (unlikely(!statement)) {
				output_nullt_string("Memory error: unable to allocate memory for statement.\n");
				exit_program();
			}
			
			ARROW_ASSIGN(statement, statement_line) = m_alloc(ARROW(state, statement_buffer_offset));
			
			if (unlikely(!statement)) {
				output_nullt_string("Memory error: unable to allocate memory for statement buffer.\n");
				exit_program();
			}
			
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
			
			break;
		}
		
		ARROW_ASSIGN(state, before_statement) = false;
		ARROW_ASSIGN(state, char_) = read_char(state);
	}
}
