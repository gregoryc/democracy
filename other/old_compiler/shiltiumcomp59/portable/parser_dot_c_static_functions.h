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

static void unable_to_allocate_memory_to_parse_block(void) attr_cold_and_noreturn;

static u /* loop finished? */
skip_over_beginning_comments_and_whitespace_iteration(
	ParserState* cr state,
	u* restrict number_of_slash_stars_in_block_comment,
	int* restrict last_char
) attr_nonnull;

static void skip_over_beginning_comments_and_whitespace(ParserState* cr state) attr_nonnull;

#if DEBUG
	static void output_subroutine(
		const Subroutine* cr subroutine,
		const char* cr subroutine_name,
		const u length_of_subroutine_name
	) attr_nonnull;
	
	static void output_subroutines_in_scope_hash_table(ParserState* cr state) attr_nonnull;
#endif

static inline void setup_hash_tables(ParserState* cr state) attr_nonnull;
static inline u get_initial_statement_buffer_allocated_length(void);

static u /* loop finished? */
main_parser_loop_iteration(ParserState* cr state) attr_nonnull;

static inline void main_parser_loop(ParserState* cr state);
