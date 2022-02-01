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

#undef unsigned
#undef short

static void skip_over_comment(Statement* cr statement, u* cr i) {
	assert_comparison(statement, !=, NULL, Statement* cr, void*);
	assert_comparison(i, !=, NULL, u* cr, void*);
	
	while (DEREF(i) < ARROW(statement, length_of_statement_line_string) - 1) {
		const char char_ = ARRAY_INDEX(ARROW(statement, statement_line), DEREF(i));
		
		if (char_ == '/' && DEREF(i) > 0 && ARRAY_INDEX(ARROW(statement, statement_line), DEREF(i) - 1) == '*') {
			return;
		}
		
		DEREF_ASSIGN(i)++;
	}
}

static void append_token(
	u* cr token_number,
	u* cr allocated_length,
	Statement* cr statement,
	const u starting_token_offset,
	const u i,
	const u line_number
) {
	Token* token;
	
	if (unlikely(DEREF(token_number) == DEREF(allocated_length))) {
		Token** new_allocation;
		
		DEREF_ASSIGN(allocated_length) *= 2;
		
		new_allocation = (Token**)m_realloc(ARROW(statement, tokens), sizeof(Token*) * DEREF(allocated_length));
		if (unlikely(!new_allocation)) {
			m_free(ARROW(statement, tokens));
			output_nullt_string("Unable to allocate memory for tokens\n");
			exit_program();
		}
		ARROW_ASSIGN(statement, tokens) = new_allocation;
	}
	
	token = (Token*)m_alloc(sizeof(Token));
	if (unlikely(!token)) {
		output_nullt_string("Unable to allocate memory for token\n");
		exit_program();
	}
	
	ARROW_ASSIGN(token, string).string = ARROW(statement, statement_line) + starting_token_offset;
	ARROW_ASSIGN(token, string).length = i - starting_token_offset;
	ARROW_ASSIGN(token, line_number) = line_number;
	
	ARRAY_INDEX_ASSIGN(ARROW(statement, tokens), DEREF(token_number)) = token;
	
	DEREF_ASSIGN(token_number)++;
}

static inline bool is_one_character_operator(const char char_) {
	return
		char_ == '+' ||
		char_ == '-' ||
		char_ == '*' ||
		char_ == '/' ||
		char_ == '<' ||
		char_ == '>' ||
		char_ == '=' ||
		char_ == '(' ||
		char_ == ')' ||
		char_ == ',';
}

static inline bool is_unary_operator(const char first_char_in_token) {
	return
		first_char_in_token == '+' ||
		first_char_in_token == '-' ||
		first_char_in_token == '<' ||
		first_char_in_token == '>';
}

/* Parses the statement parameter's statement_line member and sets the statement parameter appropriately. */
attr_nonnull
void parse_statement_string_into_tokens(
	Statement* cr statement,
	const u first_meaningful_character_offset,
	const u line_number_at_start_of_statement,
	void (*const empty_statement_function)(Statement* cr statement, const u line_number_at_start_of_startment)
) {
	u i = first_meaningful_character_offset;
	u token_number = 0;
	u allocated_length;
	u starting_token_offset = 0;
	u line_number = line_number_at_start_of_statement;
	bool in_token = false;
	
	/*
	Allocate 7 initial tokens when optimizing for speed, 2 when optimizing for memory usage.
	
	Optimize for speed is likely because it's default.
	*/
	if (likely(what_to_optimize_for) == OPTIMIZE_FOR_SPEED) {
		allocated_length = 7;
	} else {
		allocated_length = 2;
	}
	
	ARROW_ASSIGN(statement, tokens) = (Token**)m_alloc(sizeof(Token*) * allocated_length);
	
	while (i < ARROW(statement, length_of_statement_line_string) - 1) {
		const char char_ = ARRAY_INDEX(ARROW(statement, statement_line), i);
		
		if (char_ == '\n') {
			++line_number;
		}
		
		if (isspace(char_)) {
			if (in_token) {
				in_token = false;
				append_token(&token_number, &allocated_length, statement, starting_token_offset, i, line_number);
			}
			
			goto after;
		}
		
		if (
			char_ == '/' &&
			i + 2 < ARROW(statement, length_of_statement_line_string) &&
			ARRAY_INDEX(ARROW(statement, statement_line), i + 1) == '*'
		) {
			if (i - starting_token_offset) {
				if (in_token) {
					append_token(&token_number, &allocated_length, statement, starting_token_offset, i, line_number);
				}
			}
			skip_over_comment(statement, &i);
			in_token = false;
		}
		/* else if the charcter is the start of a two-character operator */
		else if (
			unlikely(i + 2 < ARROW(statement, length_of_statement_line_string) &&
				(
					(char_ == '*' && ARRAY_INDEX(ARROW(statement, statement_line), i + 1) == '*') ||
					(char_ == '_' && ARRAY_INDEX(ARROW(statement, statement_line), i + 1) == '/') ||
					((char_ == '<' || char_ == '>' || char_ == '!') && ARRAY_INDEX(ARROW(statement, statement_line), i + 1) == '='))
				)
			) {
			if (in_token) {
				append_token(&token_number, &allocated_length, statement, starting_token_offset, i, line_number);
			}
			append_token(&token_number, &allocated_length, statement, i, i + 2, line_number);
			++i;
			starting_token_offset = i + 1;
			in_token = false;
		}
		else if (is_one_character_operator(char_)) {
			if (in_token) {
				const char first_char_in_token = ARRAY_INDEX(ARROW(statement, statement_line), starting_token_offset);
				
				if (is_unary_operator(first_char_in_token)) {
					goto else_;
				}
				
				append_token(&token_number, &allocated_length, statement, starting_token_offset, i, line_number);
			}
			append_token(&token_number, &allocated_length, statement, i, i + 1, line_number);
			starting_token_offset = i;
			in_token = false;
		} else {
			else_:
			if (!in_token) {
				starting_token_offset = i;
				in_token = true;
			}
		}
		
		after:
			++i;
	}
	
	if (in_token) {
		append_token(&token_number, &allocated_length, statement, starting_token_offset, i, line_number);
	}
	
	ARROW_ASSIGN(statement, length_of_tokens) = token_number;
	
	if (unlikely(!ARROW(statement, length_of_tokens))) {
		empty_statement_function(statement, line_number_at_start_of_statement);
		exit_program();
	}
}
