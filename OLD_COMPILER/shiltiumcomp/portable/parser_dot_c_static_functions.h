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

static LoopHasFinished
skip_over_beginning_comments_and_whitespace_and_set_statement_buffer_offset_to_beginning_of_last_line_iteration(
	ParserState* cr state,
	u* restrict number_of_slash_stars_in_block_comment,
	char* restrict last_char
) attr_nonnull;

static inline void setup_the_hash_tables_that_are_in_state(ParserState* cr state) attr_nonnull;
static inline void allocate_statement_buffer(ParserState* cr state) attr_nonnull;
static inline u get_initial_statement_buffer_allocated_length(void) attr_const;

static void
skip_over_beginning_comments_and_whitespace_and_set_statement_buffer_offset_to_beginning_of_last_line(
	ParserState* cr state
) attr_nonnull;

static inline bool reached_eof(ParserState* cr state) attr_nonnull;
static void unclosed_block_comment_error(ParserState* cr state) attrs_cold_and_nonnull;
static inline void main_parser_loop(ParserState* cr state) attr_nonnull;
static LoopHasFinished main_parser_loop_iteration(ParserState* cr state) attr_nonnull;
static inline void put_character_in_statement_buffer(ParserState* cr state) attr_nonnull;
static inline bool utf8_char_is_space(const char utf8_char[7]);

#if DEBUG
	static void output_subroutine(
		const Subroutine* cr subroutine,
		const char* cr subroutine_name,
		const u length_of_subroutine_name
	) attr_nonnull;
	
	static void output_subroutines_in_scope_hash_table(ParserState* cr state) attr_nonnull;
#endif
