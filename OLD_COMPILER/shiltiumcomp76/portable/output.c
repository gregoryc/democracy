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

#include "../global_headers/os_detection.h"

#if UNIX
	#include <unistd.h>
#endif

#ifdef __cplusplus
	#include <cerrno>
#else
	#include <errno.h>
#endif

#define OUTPUT_MODULE
#include "../global_headers/headers.h"
#include "../global_headers/globals.h"
#include "line_number.h"

/* There's much more output in DEBUG mode, so set INITIAL_OUTPUT_BUFFER_SIZE appropriately. */
#if DEBUG
	#define INITIAL_OUTPUT_BUFFER_SIZE  8192
#else
	#define INITIAL_OUTPUT_BUFFER_SIZE  256
#endif

/*
All output is stored in a buffer pointed to by this 'output_buffer' pointer.
The buffer is only flushed only at the end of the program.
*/
static char* restrict output_buffer = NULL;
static u output_buffer_offset = 0;
static u output_buffer_allocated_length = INITIAL_OUTPUT_BUFFER_SIZE;

void setup_output_buffer(void) {
	output_buffer = m_alloc(INITIAL_OUTPUT_BUFFER_SIZE);
	
	if (unlikely(!output_buffer)) {
		/*
		An error message can't be printed here because the system
		has no memory, and the output buffer cannot be allocated.
		
		The only option is to silently exit the program.
		*/
		
		exit_program();
	}
}

/*
This function tries to reallocate with double the necessary amount of memory.
If that fails it then tries 1.5 times the necessary amount, then 1.25x,
then 1.1x, then 1.05x, then 1.0x, and if that fails it will output and error
message and abort.

Doing this prevents an error if the neccessary amount is some very huge amount of memory (such as one
gigabyte). If the neccessary amount were one gigabyte, it would try to allocate two gigabytes, and on a
system with only two gigabytes of RAM and no swap space, it would fail, since it would be requesting to
allocate ALL the memory on the system. If this were the case, it would try smaller and smaller amounts of
memory (about 1.5x, about 1.25x, about 1.1x, about 1.05x and about 1.0x) until it's successful, and if
it isn't it would abort the program.
*/
static void reallocate_output_buffer_if_necessary(const size_t length) {
	assert_comparison(output_buffer, !=, NULL, char*, void*);
	
	if (likely(output_buffer_offset + length < output_buffer_allocated_length)) return;
	
	/* 1 is added because output_buffer_offset is an offset, not a length counter variable. */ 
	output_buffer_allocated_length = (output_buffer_offset + 1 + length) * 2;
	output_buffer = m_realloc(output_buffer, output_buffer_allocated_length);
	if (likely((size_t)output_buffer)) return;
	
	output_buffer_allocated_length = (output_buffer_offset + 1 + length) * 1.5;
	output_buffer = m_realloc(output_buffer, output_buffer_allocated_length);
	if (likely((size_t)output_buffer)) return;
	
	output_buffer_allocated_length = (output_buffer_offset + 1 + length) * 1.25;
	output_buffer = m_realloc(output_buffer, output_buffer_allocated_length);
	if (likely((size_t)output_buffer)) return;
	
	output_buffer_allocated_length = (output_buffer_offset + 1 + length) * 1.1;
	output_buffer = m_realloc(output_buffer, output_buffer_allocated_length);
	if (likely((size_t)output_buffer)) return;
	
	output_buffer_allocated_length = (output_buffer_offset + 1 + length) * 1.05;
	output_buffer = m_realloc(output_buffer, output_buffer_allocated_length);
	if (likely((size_t)output_buffer)) return;
	
	output_buffer_allocated_length = (output_buffer_offset + 1 + length);
	output_buffer = m_realloc(output_buffer, output_buffer_allocated_length);
	if (likely((size_t)output_buffer)) return;
	
	/*
	An error message can't be printed here because the system
	has no memory, and the output buffer cannot be reallocated.
	
	The only option is to silently exit the program.
	*/
	
	exit_program();
}

attr_nonnull void output_string_with_length(const char* cr string, const u length) {
	assert_comparison(string, !=, NULL, const char*, void*);
	assert_comparison(length, !=, 0, u, u);
	assert_comparison(output_buffer, !=, NULL, char*, void*);
	
	reallocate_output_buffer_if_necessary(length);
	memcpy(output_buffer + output_buffer_offset, string, length);
	output_buffer_offset += length;
}

attr_nonnull void output_string(String* cr string) {
	assert_comparison(string, !=, NULL, const char*, void*);
	
	output_string_with_length(ARROW(string, string), ARROW(string, length));
}

void output_char(const char char_) {
	assert_comparison(output_buffer, !=, NULL, char*, void*);
	
	reallocate_output_buffer_if_necessary(1);
	output_buffer[output_buffer_offset++] = char_;
}

void output_space(void) {
	assert_comparison(output_buffer, !=, NULL, char*, void*);
	
	output_char(' ');
}

void output_newline(void) {
	assert_comparison(output_buffer, !=, NULL, char*, void*);
	
	output_char('\n');
}

void output_nullt_string(const char* const restrict string) {
	u length_of_string;
	
	assert_comparison(string, !=, NULL, const char*, void*);
	assert_comparison(output_buffer, !=, NULL, char*, void*);
	
	length_of_string = strlen(string);
	assert_comparison(length_of_string, !=, 0, u, u);
	reallocate_output_buffer_if_necessary(length_of_string);
	memcpy(output_buffer + output_buffer_offset, string, length_of_string);
	
	output_buffer_offset += length_of_string;	
}

void putu(const u number) {
	char buffer[27];
	
	assert_comparison(output_buffer, !=, NULL, char*, void*);
	
	add_separators_to_u(number, buffer);
	output_nullt_string(buffer);
}

void putu_without_separators(const u number) {
	char buffer[21];
	
	assert_comparison(output_buffer, !=, NULL, char*, void*);
	
	u_to_string(number, buffer, 10);
	output_nullt_string(buffer);
}

void puts_without_separators(const s number) {
	char buffer[21];
	
	assert_comparison(output_buffer, !=, NULL, char*, void*);
	
	s_to_string(number, buffer, 10);
	output_nullt_string(buffer);
}

void output_source_lines(const Statement* cr statement, u first_line_number) {
	char char_;
	u i;
	
	assert_comparison(output_buffer, !=, NULL, char*, void*);
	
	if (first_line_number != 1) {
		output_nullt_string("...");
	}
	
	char_ = '\n';
	
	i = -1;
	
	while (1) {
		if (unlikely(char_ == '\n')) {
			output_nullt_string("\n   ");
			
			#if UNIX
				if (unlikely(is_a_terminal)) {
					output_nullt_string("\033[1m");
				}
			#endif
			
			putu(first_line_number++);
			
			#if UNIX
				if (unlikely(is_a_terminal)) {
					output_nullt_string("\033[0m");
				}
			#endif
			
			output_nullt_string("  ");
		} else {
			output_char(char_);
		}
		
		++i;
		
		if (i >= ARROW(statement, length_of_statement_line_string)) {
			break;
		}
		
		char_ = ARRAY_INDEX(ARROW(statement, statement_line), i);
	}
	
	output_nullt_string("\n   ...\n");
}

attrs_cold_and_nonnull
static void output_error_or_warning(
	const u starting_line,
	const u ending_line,
	const char* cr error_or_warning_type
) {
	assert_comparison(error_or_warning_type, !=, NULL, const char* cr, void*);
	
	output_nullt_string(PROGRAM_NAME_LOWERCASE ": ");
	output_nullt_string(source_filename);
	output_nullt_string(": ");
	output_nullt_string(error_or_warning_type);
	
	if (likely(starting_line)) {
		output_nullt_string(" on");
		
		if (!ending_line || ending_line - starting_line == 0) {
			output_nullt_string(" line ");
			putu(starting_line);
		} else if (ending_line - starting_line == 1) {
			output_nullt_string(" lines ");
			putu(starting_line);
			output_nullt_string(" and ");
			putu(ending_line);
		} else {
			output_nullt_string(" lines ");
			putu(starting_line);
			output_nullt_string("-");
			putu(ending_line);
		}
	}
	
	output_nullt_string(":\n\n   ");
}

attr_cold void output_syntax_error_beginning_text(const u starting_line, const u ending_line) {
	output_error_or_warning(starting_line, ending_line, "syntax error");
}

attr_cold void output_type_error_beginning_text(const u starting_line, const u ending_line) {
	output_error_or_warning(starting_line, ending_line, "type error");
}

attr_cold void output_comment_warning_beginning_text(const u starting_line, const u ending_line) {
	output_error_or_warning(starting_line, ending_line, "comment warning");
}

__attribute__((nonnull (1), cold))
void output_memory_error(ParserState* cr state, const char* cr string) {
	assert_comparison(string, !=, NULL, const char* cr, void*);
	
	output_nullt_string("Memory error");
	if (likely(state)) {
		output_nullt_string(" on line ");
		putu(get_line_number(state));
	}
	output_nullt_string(": ");
	output_nullt_string(string);
	output_nullt_string(".\n");
}

__attribute__((nonnull (3)))
void output_memory_error_if_null(const bool value, ParserState* cr state, const char* cr string) {
	assert_comparison(string, !=, NULL, const char* cr, void*);
	
	if (unlikely(!value)) {
		output_memory_error(state, string);
	}
}

void output_errno_string(void) {
	assert_comparison(output_buffer, !=, NULL, char*, void*);
	
	output_nullt_string(strerror(errno));
}

#if DEBUG
	void putf(const f number) {
		char buffer[
			10 /* max number of digits in string */ +
			1 /* for possible decimal point */ +
			5 /* for possible e+DDD, or e-DDD, where the D's are digits */ +
			1 /* for null terminator */
		];
		
		int result;
		
		assert_comparison(output_buffer, !=, NULL, char* restrict, void*);
		
		result = snprintf(buffer, sizeof(buffer), "%.10g", number);
		
		if (unlikely(result >= (s)sizeof(buffer) || result == -1)) {
			output_nullt_string("snprintf() failed");
			exit_program();
		}
		
		output_nullt_string(buffer);
	}
	
	void output_variable(Variable* cr variable) {
		assert_comparison(variable, !=, NULL, Variable*, void*);
		
		if (ARROW(variable, name).length) {
			output_string(&variable->name);
		} else {
			output_nullt_string("(internal compiler variable");
		}
		
		output_nullt_string(" [stack offset: ");
		putu(ARROW(variable, stack_offset));
		output_char(']');
		
		if (!ARROW(variable, name).length) {
			output_char(')');
		}
	}
	
	void rewind_output_pointer(const u number_of_bytes) {
		assert_comparison(output_buffer, !=, NULL, char*, void*);
		
		output_buffer_offset -= number_of_bytes;
	}
#endif

attr_noreturn void exit_program(void) {
	#undef unsigned
	#undef short
	#undef exit
	#undef free
	
	const unsigned short exit_message_code = 3;
	
	assert_comparison(output_buffer, !=, NULL, char*, void*);
	
	#if UNIX
		write(STDERR_FILENO, output_buffer, output_buffer_offset);
	#else
		fwrite(output_buffer, output_buffer_offset, 1, stderr);
	#endif
	
	free(output_buffer);
	
	/* This tells the UI to exit the program. */
	#if UNIX
		write(STDOUT_FILENO, &exit_message_code, sizeof(unsigned short));
	#else
		fwrite(&exit_message_code, sizeof(unsigned short), 1, stdout);
	#endif
	
	exit(0);
}
