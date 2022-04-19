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

#include "../global_headers/debug.h"

#if DEBUG
	#ifdef __cplusplus
		#include <climits>
	#else
		#include <limits.h>
	#endif
#endif

#include "../global_headers/headers.h"
#include "parse_block.h"
#include "subroutine_value_deletion_function.h"

WhatToOptimizeFor what_to_optimize_for = OPTIMIZE_FOR_SPEED;

/* The filename of the source code of the program that is being compiled. */
const char* source_filename;

#if UNIX
	bool is_a_terminal;
#endif

Language language = LANGUAGE_ENGLISH;

static enum {
	EXECUTION_LOCATION_NORMAL,
	EXECUTION_LOCATION_IN_GENERATED_CODE,
	EXECUTION_LOCATION_IN_PARSE_BLOCK
} execution_location = EXECUTION_LOCATION_NORMAL;

#if UNIX
	static void* signal_stack;
	
	#define SIGNAL_STACK_SIZE   4096
#endif

#if DEBUG
	u number_of_assert_comparison_macro_calls = 0;
	
	#define SHOW_BITS   0
	#define WRITE_GENERATED_CODE_TO_FILE  0
	
	#if WRITE_GENERATED_CODE_TO_FILE
		#define GENERATED_CODE_FILENAME   "__GENERATED_CODE__.bin"
	#endif
#endif

attr_nonnull static inline FILE* open_file_for_reading(const char* cr filename) {
	assert_comparison(filename, !=, NULL, char*, void*);
	assert_comparison(strlen(filename), !=, 0, u, u);
	
	#if DEBUG
		update_memory_allocations_for_fopen();
	#endif
	
	return fopen(filename, "r");
}

attr_nonnull static inline int close_file(FILE* cr file) {
	assert_comparison(file, !=, NULL, FILE*, void*);
	
	#if DEBUG
		update_memory_allocations_for_fclose();
	#endif
	return fclose(file);
}

#if DEBUG
	static void show_beginning_debug_information(void) {
		output_nullt_string("DEBUG enabled.\n");
		
		if (what_to_optimize_for == OPTIMIZE_FOR_SPEED) {
			output_nullt_string("Optimizing for speed.");
		} else {
			output_nullt_string("Optimizing for memory usage.");
		}
		
		output_newline();
	}
	
	static void show_ending_debug_information(void) {
		output_nullt_string("\nNumber of assert_comparison macro calls: ");
		
		putu(number_of_assert_comparison_macro_calls);
		output_newline();
		
		show_memory_usage();
	}
#endif

static inline void run_generated_code(const DataAndCode* cr dac, const u main_subroutine_offset) {
	union {
		char* char_pointer_type;
		void (*function_type)(void);
	} char_pointer_to_function_union;
	
	assert_comparison(dac, !=, NULL, DataAndCode*, void*);
	assert_comparison(ARROW(dac, start_of_code), !=, NULL, DataAndCode*, void*);
	
	execution_location = EXECUTION_LOCATION_IN_GENERATED_CODE;
	
	char_pointer_to_function_union.char_pointer_type = ARROW(dac, start_of_code) + main_subroutine_offset;
	#if 0
	char_pointer_to_function_union.function_type();
	#endif
	
	execution_location = EXECUTION_LOCATION_NORMAL;
}

#if UNIX
	static void segfault_handler(const int signal_, siginfo_t* cr siginfo, void* cr ucontext) {
		(void)signal_;
		(void)ucontext;
		
		switch (execution_location) {
			case EXECUTION_LOCATION_NORMAL:
				output_nullt_string("\nSorry, internal compiler error. Signal SIGSEGV raised.\nDereferenced address ");
				putu((u)ARROW(siginfo, si_addr));
				output_nullt_string(" (decimal)\n");
				break;
			
			case EXECUTION_LOCATION_IN_PARSE_BLOCK:
				switch (language) {
					case LANGUAGE_ENGLISH:
						output_nullt_string("Stack overflow -- too many nested blocks\n");
						break;
					case LANGUAGE_FRENCH:
						output_nullt_string("Débordement de pile -- un trop grand nombre de blocs imbriqués\n");
						break;
					case LANGUAGE_GERMAN:
						output_nullt_string("Stack Overflow -- zu viele verschachtelte Blöcke\n");
						break;
					case LANGUAGE_ITALIAN:
						output_nullt_string("Stack Overflow -- blocchi annidati troppi\n");
						break;
					case LANGUAGE_KOREAN:
						output_nullt_string("스택 오버플로 - 너무 많은 중첩된 블록\n");
						break;
					case LANGUAGE_PORTUGUESE:
						output_nullt_string("Stack Overflow - muitos blocos aninhados\n");
						break;
					case LANGUAGE_SPANISH:
						output_nullt_string("Desbordamiento de pila -- bloques anidados demasiados\n");
						break;
					case LANGUAGE_RUSSIAN:
						output_nullt_string("Переполнение стека -- слишком много вложенных блоков\n");
						break;
				}
				
				break;
			
			case EXECUTION_LOCATION_IN_GENERATED_CODE:
				output_nullt_string("Stack overflow (probably due to infinite recursion) in the generated code.\n");
				break;
		}
		
		exit_program();
	}
	
	#if DEBUG
		static void sigtrap_handler(const int arg) {
			(void)arg;
			
			output_nullt_string("SIGTRAP raised.\n");
			exit_program();
		}
	#endif
	
	static inline void handle_compiler_crashes_properly(void) {
		struct sigaction sa;
		stack_t stack;
		
		signal_stack = stack.ss_sp = m_alloc(SIGNAL_STACK_SIZE);
		
		if (unlikely(!stack.ss_sp)) {
			output_nullt_string("Memory error: Unable to allocate memory for signal stack.\n");
			exit_program();
		}
		
		stack.ss_size = SIGNAL_STACK_SIZE;
		stack.ss_flags = 0;
		
		if (unlikely(sigaltstack(&stack, NULL) == -1)) {
			output_nullt_string("Unable set signal stack: ");
			output_errno_string();
			output_newline();
			exit_program();
		}
		
		sa.sa_flags = SA_SIGINFO | SA_ONSTACK;
		sa.sa_sigaction = segfault_handler;
		sigemptyset(&sa.sa_mask);
		
		if (unlikely(sigaction(SIGSEGV, &sa, NULL) == -1)) {
			output_nullt_string("sigaction() failed to set a handler for the SIGSEGV signal.\nContinuing anyway...\n");
		}
		
		#if DEBUG
			sa.sa_flags = 0;
			sa.sa_handler = sigtrap_handler;
			
			if (unlikely(sigaction(SIGTRAP, &sa, NULL) == -1)) {
				output_nullt_string("sigaction() failed to set a handler for the SIGTRAP signal.\nContinuing anyway...\n");
			}
		#endif
	}
#endif

#if DEBUG
	static inline void do_size_asserts(void) {
		char array0[CHAR_BIT == 8 ? 1 : -1];
		char array1[sizeof(u) == sizeof(size_t) ? 1 : -1];
		char array2[sizeof(void*) == sizeof(u) ? 1 : -1];
		char array3[sizeof(char*) == sizeof(u) ? 1 : -1];
		char array4[sizeof(s) == sizeof(u) ? 1 : -1];
		char array5[sizeof(u64) == 8 ? 1 : -1];
		char array6[sizeof(u32) == 4 ? 1 : -1];
		char array7[sizeof(s32) == 4 ? 1 : -1];
		char array8[sizeof(uchar) == 1 ? 1 : -1];
		char array9[sizeof(schar) == 1 ? 1 : -1];
		#if SIXTY_FOUR_BIT
			char array10[sizeof(u) == 8 ? 1 : -1];
			char array11[sizeof(char*) == 8 ? 1 : -1];
			char array12[sizeof(void*) == 8 ? 1 : -1];
		#else
			char array10[sizeof(u) == 4 ? 1 : -1];
			char array11[sizeof(char*) == 4 ? 1 : -1];
			char array12[sizeof(void*) == 4 ? 1 : -1];
		#endif
		
		(void)array0;
		(void)array1;
		(void)array2;
		(void)array3;
		(void)array4;
		(void)array5;
		(void)array6;
		(void)array7;
		(void)array8;
		(void)array9;
		(void)array10;
		(void)array11;
		(void)array12;
	}
#endif

#if DEBUG && SHOW_BITS
	/*
	Shows the bits of the generated program.
	The only reason this is in here is because it looks cool  ;)
	This is disabled by default of course.
	*/
	static void show_bits(DataAndCode* cr dac) {
		u i;
		const char zero = '0';
		const char newline = '\n';
		
		for (i = 0; i < ARROW(dac, current_instruction_offset); ++i) {
			char buffer[8 /* number of bits in a byte */ + 1 /* null terminator */];
			u j, len;
			
			u_to_string((uchar)ARRAY_INDEX(ARROW(dac, start_of_code), i), buffer, 2);
			
			len = strlen(buffer);
			
			j = 8 - len;
			
			while (j--) {
				write(STDERR_FILENO, &zero, 1);
				usleep(7500);
			}
			
			for (j = 0; j < len; ++j) {
				write(STDERR_FILENO, &buffer[j], 1);
				usleep(10000);
			}
		}
		
		write(STDERR_FILENO, &newline, 1);
	}
#endif

#if DEBUG && WRITE_GENERATED_CODE_TO_FILE
	#undef perror
	#undef fwrite
	
	/*
	Write the generated code to a file.
	This is useful to ensure that this program is generating the correct code by disassembling the file.
	This is disabled by default.
	*/
	static void write_generated_code_to_file(DataAndCode* cr dac) {
		FILE* file = fopen(GENERATED_CODE_FILENAME, "w");
		
		update_memory_allocations_for_fopen();
		
		if (fwrite(ARROW(dac, start_of_code), ARROW(dac, current_instruction_offset), 1, file) != 1) {
			perror("Unable to write to file " OPENING_QUOTE "output" CLOSING_QUOTE);
			exit_program();
		}
		
		if (fclose(file) == EOF) {
			perror("Unable to close file " OPENING_QUOTE "output" CLOSING_QUOTE);
			exit_program();
		}
		
		update_memory_allocations_for_fclose();
	}
	
	#define perror YOU_SHOULD_NOT_USE_PERROR
	#define fwrite YOU_SHOULD_NOT_USE_FWRITE
#endif

static void init(void) {
	setup_output_buffer();
	
	#if UNIX
		handle_compiler_crashes_properly();
	#endif
	
	#if DEBUG
		do_size_asserts();
	#endif
	
	#if UNIX
		is_a_terminal = isatty(STDERR_FILENO);
	#endif
	
	#if DEBUG
		show_beginning_debug_information();
	#endif
}

static void open_source_file(FILE** cr file, char** cr argv) {
	assert_comparison(file, !=, NULL, FILE** cr, void*);
	assert_comparison(argv, !=, NULL, char** cr, void*);
	
	DEREF_ASSIGN(file) = open_file_for_reading(source_filename);
	
	if (unlikely(!DEREF(file))) {
		output_nullt_string(PROGRAM_NAME_LOWERCASE ": Cannot open file " OPENING_QUOTE);
		output_nullt_string(ARRAY_INDEX(argv, 1));
		output_nullt_string(CLOSING_QUOTE ": ");
		output_errno_string();
		output_newline();
		exit_program();
	}
}

attr_nonnull
static void give_an_error_if_there_are_subroutines_that_were_referenced_but_arent_defined(
	HashTable* cr subroutine_forward_references
) {
	HashIterator iterator;
	
	hash_iterator_init(subroutine_forward_references, &iterator);
	
	while (hash_iterator_next(&iterator)) {
		output_nullt_string("Undefined subroutine " OPENING_QUOTE);
		output_string(iterator.key.string_value, iterator.length_of_key);
		output_nullt_string(CLOSING_QUOTE "\n");
	}
	
	if (ARROW(subroutine_forward_references, length)) {
		exit_program();
	}
}

attr_nonnull
static void create_program(
	FILE* cr file,
	DataAndCode* cr dac,
	MidLevelInstructions* cr mli,
	u* cr main_subroutine_offset
	#if USE_MMAP
		, u* cr code_length
	#endif
) {
	FreeListNode* free_list_head = NULL;
	HashTable subroutine_forward_references;
	u stack_offset = 0;
	ParserState* state;
	u line_number = 1;
	
	#if !USE_MMAP
		u code_length = 0;
	#endif
	
	assert_comparison(file, !=, NULL, FILE* cr, void*);
	assert_comparison(dac, !=, NULL, DataAndCode* cr, void*);
	assert_comparison(mli, !=, NULL, MidLevelInstructions* cr, void*);
	
	dac_init(dac);
	hash_table_init(&subroutine_forward_references, 10, 4, 2.5);
	
	mid_level_instructions_init(mli);
	
	state = (ParserState*)m_alloc(sizeof(ParserState));
	
	/* Set some of the members in the 'state' structure. */
	ARROW_ASSIGN(state, line_number) = &line_number;
	ARROW_ASSIGN(state, line_number_at_start_of_statement) = 1;
	ARROW_ASSIGN(state, file) = file;
	ARROW_ASSIGN(state, dac) = dac;
	ARROW_ASSIGN(state, mli) = mli;
	ARROW_ASSIGN(state, free_list_head) = &free_list_head;
	ARROW_ASSIGN(state, previous_mid_level_instruction_offset) = 0;
	
	ARROW_ASSIGN(state, previous_subroutine_offset) =
		#if !USE_MMAP
			&
		#endif
		code_length;
	
	ARROW_ASSIGN(state, scope).parent_scope = NULL;
	ARROW_ASSIGN(state, subroutine_forward_references) = &subroutine_forward_references;
	ARROW_ASSIGN(state, stack_offset) = &stack_offset;
	ARROW_ASSIGN(state, in_subroutine) = false;
	
	execution_location = EXECUTION_LOCATION_IN_PARSE_BLOCK;
	
	parse_block(state);
	
	execution_location = EXECUTION_LOCATION_NORMAL;
	
	{
		MidLevelInstructions new_mli;
		Subroutine* subroutine;
		bool in_hash_table;
		
		subroutine = DEREF((Subroutine**)hash_table_get_value_from_string_key(&state->scope.subroutines_in_scope, "main", 4, &in_hash_table));
		
		if (unlikely(!in_hash_table)) {
			output_nullt_string("Subroutine error: no main subroutine defined\n");
			exit_program();
		}
		
		DEREF_ASSIGN(main_subroutine_offset) = ARROW(subroutine, code_offset);
		
		new_mli.mid_level_instructions = ARROW(ARROW(state, mli), mid_level_instructions) + ARROW(state, previous_mid_level_instruction_offset);
		new_mli.used_length = ARROW(ARROW(state, mli), used_length) - ARROW(state, previous_mid_level_instruction_offset);
		
		DEREF_ASSIGN(ARROW(state, previous_subroutine_offset)) += get_length_of_mid_level_instructions(&new_mli);
	}
	
	hash_table_del_with_keys_and_values(&state->scope.subroutines_in_scope, subroutine_value_deletion_function);
	m_free(state);
	
	give_an_error_if_there_are_subroutines_that_were_referenced_but_arent_defined(&subroutine_forward_references);
	hash_table_del(&subroutine_forward_references);
	
	translate_mid_level_instructions_to_cpu_instructions(dac, mli,
		#if USE_MMAP
			DEREF(code_length)
		#else
			code_length
		#endif
	);
	
	free_list_del(free_list_head);
	
	#if DEBUG
		output_nullt_string(PROGRAM_NAME " successfully compiled code.\n\n");
		
		output_nullt_string("Generated program code section size: ");
		
		putu(
			#if USE_MMAP
				DEREF(code_length)
			#else
				code_length
			#endif
		);
		
		output_newline();
		
		output_nullt_string("Generated program float section size: ");
		
		putu(ARROW(dac, float_data_real_length));
		output_nullt_string(" byte");
		if (likely(ARROW(dac, float_data_real_length) != 1)) {
			output_char('s');
		}
		
		output_nullt_string("\nGenerated program string section size: ");
		
		putu(ARROW(dac, string_data_real_length));
		output_nullt_string(" byte");
		
		if (likely(ARROW(dac, string_data_real_length) != 1)) {
			output_char('s');
		}
		
		output_nullt_string("\n\n");
	#endif
	
	#if DEBUG
		#if SHOW_BITS
			show_bits(dac);
		#endif
		
		#if WRITE_GENERATED_CODE_TO_FILE
			write_generated_code_to_file(dac);
		#endif
	#endif
}

attr_nonnull static void cleanup_before_running_generated_code(FILE* cr file, MidLevelInstructions* cr mli) {
	assert_comparison(file, !=, NULL, FILE* cr, void*);
	assert_comparison(mli, !=, NULL, MidLevelInstructions* cr, void*);
	
	close_file(file);
	mid_level_instructions_del(mli);
}

attr_nonnull static void cleanup(DataAndCode* dac
#if USE_MMAP
	, u code_length
#endif
) {
	assert_comparison(dac, !=, NULL, DataAndCode*, void*);
	#if USE_MMAP
		assert_comparison(code_length, !=, 0, u, u);
	#endif
	
	hash_table_del(&dac->string_literals);
	hash_table_del(&dac->float_literals);
	
	/*
	In DEBUG mode, m_free() cannot be called with a NULL pointer
	as an argument because that normally would be a bug. That's
	why there is checking that m_free()'s argument is not equal
	to NULL in DEBUG mode. But in release mode, m_free() can be
	called with NULL as its argument, so the '#else' is a minor
	optimization for release mode.
	*/
	#if DEBUG
		if (ARROW(dac, start_of_float_data)) {
			m_free(ARROW(dac, start_of_float_data));
		}
		
		if (ARROW(dac, start_of_string_data)) {
			m_free(ARROW(dac, start_of_string_data));
		}
	#else
		m_free(ARROW(dac, start_of_float_data));
		m_free(ARROW(dac, start_of_string_data));
	#endif
	
	m_free_memory_for_code(ARROW(dac, start_of_code)
	#if USE_MMAP
		, code_length
	#endif
	);
}

#if DEBUG
	static void show_debug_information_that_is_shown_before_running_the_generated_code(DataAndCode* cr dac) {
		HashIterator iterator;
		u length;
		
		output_nullt_string(PROGRAM_NAME " successfully ran code.\n\nFloat literals hash table:\n{\n");
		
		length = 0;
		
		hash_iterator_init(&dac->float_literals, &iterator);
		
		while (hash_iterator_next(&iterator)) {
			output_nullt_string("   ");
			putf(iterator.key.fvalue);
			output_nullt_string(" => ");
			putu(DEREF((u*)iterator.value));
			output_newline();
			++length;
		}
		
		assert_comparison(length, ==, ARROW(dac, float_literals).length, u, u);
		
		output_nullt_string("}\n\nString literals hash table:\n{\n   Unimplemented\n}\n");
	}
#endif

void real_main(char** const argv) {
	FILE* file;
	MidLevelInstructions mli;
	DataAndCode dac;
	u main_subroutine_offset;
	#if USE_MMAP
		u code_length = 0;
	#endif
	
	process_command_line_options(argv);
	open_source_file(&file, argv);
	
	create_program(file, &dac, &mli, &main_subroutine_offset
		#if USE_MMAP
			, &code_length
		#endif
	);
	
	#if DEBUG
		show_debug_information_that_is_shown_before_running_the_generated_code(&dac);
	#endif
	
	cleanup_before_running_generated_code(file, &mli);
	
	run_generated_code(&dac, main_subroutine_offset);
	
	cleanup(&dac
		#if USE_MMAP
			, code_length
		#endif
	);
}

static u number_of_main_calls = 0;

int main(const int argc, char** const argv) {
	++number_of_main_calls;
	
	init();
	
	/* argc is not needed because argv[argc] is always equal to NULL */
	(void)argc;
	
	real_main(argv);
	
	--number_of_main_calls;
	
	if (!number_of_main_calls) {
		#if UNIX
			m_free(signal_stack);
		#endif
		
		#if DEBUG
			show_ending_debug_information();
		#endif
		
		exit_program();
	}
	
	return 0;
}
