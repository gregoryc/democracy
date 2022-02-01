/*
Main source file for Shiltium.
This file is licensed under the Do Whatever You Want Public License, which is found in the LICENSE file.
*/

#include "headers.h"

#if DEBUG
	u64 number_of_assert_comparison_macro_calls = 0;
	u64 number_of_checked_dereferences = 0;
#endif

__attribute__((nonnull)) static inline FILE* open_file_for_reading(const char* const restrict file) {
	assert_comparison(file, !=, NULL);
	
	#if DEBUG
		update_memory_allocation_for_fopen();
	#endif
	return fopen(file, "r");
}

__attribute__((nonnull)) static inline int close_file(FILE* const restrict file) {
	assert_comparison(file, !=, NULL);
	
	#if DEBUG
		update_memory_allocation_for_fclose();
	#endif
	return fclose(file);
}

#if DEBUG
	static const char* const restrict error_numbers_to_strings[] = {
		/* No error */
		"ERROR_NUMBER_NO_ERROR",
		
		/* Syntax errors */
		"ERROR_NUMBER__SYNTAX_ERROR__EMPTY_STATEMENT",
		"ERROR_NUMBER__SYNTAX_ERROR__MISSING_SEMICOLON",
		"ERROR_NUMBER__SYNTAX_ERROR__UNCLOSED_BLOCK_COMMENT",
		"ERROR_NUMBER__SYNTAX_ERROR__UNCLASSIFIED_SYNTAX_ERROR",
		
		/* Type errors */
		"ERROR_NUMBER__TYPE_ERROR__INVALID_TYPE",
		
		/* Literal errors */
		"ERROR_NUMBER__LITERAL_ERROR__UINT_TOO_LARGE",
		"ERROR_NUMBER__LITERAL_ERROR__INVALID_UINT",
		"ERROR_NUMBER__LITERAL_ERROR__INVALID_FLOAT",
		"ERROR_NUMBER__LITERAL_ERROR__MORE_THAN_ONE_POINT_IN_FLOAT_LITERAL",
		"ERROR_NUMBER__LITERAL_ERROR__INVALID_CHARACTER_IN_BASE",
		"ERROR_NUMBER__LITERAL_ERROR__EMPTY_EXPONENT",
		"ERROR_NUMBER__LITERAL_ERROR__INVALID_EXPONENT",
		"ERROR_NUMBER__LITERAL_ERROR__INVALID_BASE",
		
		/* Name errors */
		"ERROR_NUMBER__NAME_ERROR__INVALID_VARIABLE_NAME",
		"ERROR_NUMBER__NAME_ERROR__VARIABLE_ALREADY_DECLARED",
	};
#endif

/*
[Block:
	[Statement (statement): [Token: "number"] [Token: "="] [Token: "0"]]
	[Statement (block):
		[Block:
			[Statement (statement): [Token: "number"] [Token: "*="] [Token: "123"]]
			...
		]
	[Statement (statement): [Token: "number"] [Token: "+="] [Token: "5"]]
]
*/

#if DEBUG
	static void show_ending_debug_information(void) {
		output_nullt_string("\nNumber of assert_comparison macro calls: ");
		output_u64(number_of_assert_comparison_macro_calls);
		output_newline();
		
		show_memory_usage();
	}
#endif

__attribute__((nonnull)) static void variable_del(void* const variable_) {
	Variable* variable;
	
	assert_comparison(variable_, !=, NULL);
	
	variable = (Variable*)variable_;
	
	operand_linked_list_del(&variable->operands);
	operator_linked_list_del(&variable->operators);
}

static void segfault_handler(int arg) {
	(void)arg;
	
	output_nullt_string("Sorry, internal compiler error. Signal SIGSEGV raised.\nThis is a bug in the compiler, not the program.\n");
	
	#if UNIX
		/* Normal Linux segfault exit status */
		exit(139);
	#elif WINDOWS
		/* Normal Windows segfault exit status */
		exit(3);
	#else
		#pragma message "OS segfault exit status not known, defaulting to 1"
		exit(1);
	#endif
}

static void output_basename(const char* const full_filename) {
	const char* new_full_filename = full_filename;
	u64 i;
	
	assert_comparison(full_filename, !=, NULL);
	
	for (i = 0; ARRAY_INDEX(full_filename, i); ++i) {
		if (i && ARRAY_INDEX(full_filename, i - 1) == '/') {
			new_full_filename = full_filename + i;
		}
	}
	
	output_nullt_string(new_full_filename);
}

attr_nonnull static void clean_up(
	FILE* const restrict file,
	Block* const restrict main_block,
	const ErrorNumber error_number
) {
	u64 i;
	
	assert_comparison(file, !=, NULL);
	assert_comparison(main_block, !=, NULL);
	
	close_file(file);
	
	i = ARROW(main_block, length);
	
	while (i--) {
		m_free(ARRAY_INDEX(ARROW(main_block, statement_or_block_array), i)->statement_line);
		m_free(ARRAY_INDEX(ARROW(main_block, statement_or_block_array), i)->tokens);
		m_free(ARRAY_INDEX(ARROW(main_block, statement_or_block_array), i));
	}
	
	if (
		error_number == ERROR_NUMBER__SYNTAX_ERROR__EMPTY_STATEMENT ||
		error_number == ERROR_NUMBER__SYNTAX_ERROR__UNCLOSED_BLOCK_COMMENT_NOT_AT_BEGINNING_OF_LINE
	) {
		m_free(ARROW(ARRAY_INDEX(ARROW(main_block, statement_or_block_array), ARROW(main_block, length)), statement_line));
		m_free(ARRAY_INDEX(ARROW(main_block, statement_or_block_array), ARROW(main_block, length)));
	}
	
	m_free(ARROW(main_block, statement_or_block_array));
	
	if (ARROW(main_block, mid_level_instructions.mid_level_instructions)) {
		hash_table_del_with_values(&main_block->variables_in_scope, variable_del);
		mid_level_instructions_del(&main_block->mid_level_instructions);
	}
	
	m_free(main_block);
}

static inline void run_generated_code(const DataAndCode* const restrict dac) {
	/*
	This is a hack, but it works, and it's valid ISO C. If a cast
	were used instead, gcc would give an error message because
	casts from char*'s to function pointers are forbidden in ISO C.
	*/
	union {
		char* char_pointer_type;
		void (*function_type)(void);
	} u;
	
	assert_comparison(dac, !=, NULL);
	
	u.char_pointer_type = ARROW(dac, start_of_code);
	u.function_type();
}

static inline void enable_alignment_checking(void) {
	#ifdef __GNUC__
		__asm__("pushf\n");
		#ifdef __i386__
			__asm__("orl $0x40000, (%esp)\n");
		#elif defined __x86_64__
			__asm__("orl $0x40000, (%rsp)\n");
		#endif
		__asm__("popf\n");
	#else
		#pragma message "Alignment checking disabled"
	#endif
}

static inline void close_stdin(void) {
	if (unlikely(fclose(stdin) == EOF)) {
		output_nullt_string("Cannot close stdin\n");
		exit(1);
	}
}

static inline void output_everything_at_exit(void) {
	if (atexit(output_everything)) {
		output_nullt_string("atexit() failed\n");
		output_everything();
		exit(1);
	}
}

static inline void handle_compiler_crashes_properly(void) {
	if (unlikely(signal(SIGSEGV, segfault_handler) == SIG_ERR)) {
		output_nullt_string("signal() failed");
		exit(1);
	}
}

#if DEBUG
	static inline void do_size_asserts(void) {
		assert_comparison(sizeof(u64), ==, 8);
		assert_comparison(sizeof(s64), ==, 8);
		assert_comparison(sizeof(u32), ==, 4);
		assert_comparison(sizeof(s32), ==, 4);
		assert_comparison(sizeof(uchar), ==, 1);
		assert_comparison(sizeof(schar), ==, 1);
	}
#endif

int main(int argc, const char** const restrict argv) {
	DataAndCode dac;
	GlobalData global_data;
	Block* restrict block;
	ErrorNumber error_number;
	FILE* restrict file;
	
	enable_alignment_checking();
	setup_output_buffer();
	output_everything_at_exit();
	close_stdin();
	handle_compiler_crashes_properly();
	#if DEBUG
		do_size_asserts();
	#endif
	
	global_data.number_of_enclosing_loops = 0;
	global_data.number_of_enclosing_conditionals = 0;
	
	if (
		argc == 1 ||
		strequal(ARRAY_INDEX(argv, 1), "h") || strequal(ARRAY_INDEX(argv, 1), "-h") || strequal(ARRAY_INDEX(argv, 1), "--h") ||
		strequal(ARRAY_INDEX(argv, 1), "help") || strequal(ARRAY_INDEX(argv, 1), "-help") || strequal(ARRAY_INDEX(argv, 1), "--help")
	) {
		output_nullt_string("Usage: ");
		
		output_basename(DEREF(argv));
		output_nullt_string(
			" [option] [file.sl]\n"
			"Options:\n\n"
			"    d, D, -d, -D, --d, --D, doc, -doc, --doc, documentation, -documentation,\n"
			"    --documentation, shilto-language-documentation,\n"
			"    -shilto-language-documentation, --shilto-language-documentation:\n\n"
			"        display the Shilto Language documentation\n\n"
			"    h, -h, --h, help, -help, --help:\n"
			"        display this help and exit\n\n"
			"    v, -v, --v, version, -version, --version:\n"
			"        display version information and exit\n"
		);
		return 0;
	}
	
	if (
		strequal(ARRAY_INDEX(argv, 1), "d") ||
		strequal(ARRAY_INDEX(argv, 1), "D") ||
		strequal(ARRAY_INDEX(argv, 1), "-d") ||
		strequal(ARRAY_INDEX(argv, 1), "-D") ||
		strequal(ARRAY_INDEX(argv, 1), "--d") ||
		strequal(ARRAY_INDEX(argv, 1), "--D") ||
		strequal(ARRAY_INDEX(argv, 1), "doc") ||
		strequal(ARRAY_INDEX(argv, 1), "-doc") ||
		strequal(ARRAY_INDEX(argv, 1), "--doc") ||
		strequal(ARRAY_INDEX(argv, 1), "documentation") ||
		strequal(ARRAY_INDEX(argv, 1), "-documentation") ||
		strequal(ARRAY_INDEX(argv, 1), "--documentation") ||
		strequal(ARRAY_INDEX(argv, 1), "shilto-language-documentation") ||
		strequal(ARRAY_INDEX(argv, 1), "-shilto-language-documentation") ||
		strequal(ARRAY_INDEX(argv, 1), "--shilto-language-documentation")
	) {
		/*
		"less language_documentation" sounds funny ;)
		It makes me imagine someone shouting "LESS LANGUAGE DOCUMENTATION!!! TOO MUCH DOCUMENTATION!!!! AAAA!!!"
		*/
		int return_value = system("less -~ -x4 --prompt='Shilto Language Language Documentation' language_documentation");
		if (return_value == -1) {
			output_nullt_string(PROGRAM_NAME ": system() failed\n");
			return 1;
		}
		if (return_value) {
			output_nullt_string(PROGRAM_NAME ": less failed\n");
			return 1;
		}
		return 0;
	}
	
	if (
		strequal(ARRAY_INDEX(argv, 1), "v") ||
		strequal(ARRAY_INDEX(argv, 1), "-v") ||
		strequal(ARRAY_INDEX(argv, 1), "--v") ||
		strequal(ARRAY_INDEX(argv, 1), "version") ||
		strequal(ARRAY_INDEX(argv, 1), "-version") ||
		strequal(ARRAY_INDEX(argv, 1), "--version")
	) {
		output_nullt_string("This program is " PROGRAM_NAME " version ");
		output_nullt_string(VERSION);
		output_nullt_string(" and it can compile and execute code in Shilto Language ");
		output_nullt_string(SHILTO_LANGUAGE_VERSION);
		output_nullt_string(".\n");
		
		return 1;
	}
	
	file = open_file_for_reading(ARRAY_INDEX(argv, 1));
	if (!file) {
		output_nullt_string(PROGRAM_NAME_LOWERCASE ": Cannot open file ‘");
		output_nullt_string(ARRAY_INDEX(argv, 1));
		output_nullt_string("’: ");
		output_errno_string();
		output_newline();
		return 1;
	}
	
	#if DEBUG
		output_nullt_string("DEBUG enabled.\nAll debug messages go to stderr.\n");
	#endif
	
	dac.current_instruction_offset = 0;
	dac.start_of_data = 0;
	dac.stack_offset = 0;
	
	block = parse_block(file, &dac, &error_number, &global_data);
	assert_comparison(error_number, <=, LAST_ERROR_NUMBER);
	assert_comparison(error_number, <=, 255);
	
	/* For now, everything is stored in one big block, a "global function" */
	if (error_number != ERROR_NUMBER_NO_ERROR) {
		#if DEBUG
			output_newline();
			output_nullt_string(PROGRAM_NAME " JIT failed to compile code.\n");
		#endif
		
		clean_up(file, block, error_number);
		
		if (dac.start_of_data) {
			m_free(dac.start_of_data);
		}
		
		#if DEBUG
			show_ending_debug_information();
		#endif
		
		return error_number;
	}
	
	m_make_memory_only_executable(dac.start_of_code, dac.code_length);
	
	#if DEBUG
		output_nullt_string("Error: ");
		output_nullt_string(ARRAY_INDEX(error_numbers_to_strings, error_number));
		output_newline();
	#endif
	
	#if DEBUG
		output_nullt_string(PROGRAM_NAME " JIT successfully compiled code.\n\n");
		
		output_nullt_string("Stack offset: ");
		output_u64(dac.stack_offset);
		output_newline();
		
		output_nullt_string("Generated program data section size: ");
		output_u64(dac.data_length);
		output_nullt_string(" byte");
		if (dac.data_length != 1) {
			output_char('s');
		}
		output_newline();
		
		output_nullt_string("Generated program code section size: ");
		output_u64(dac.code_length);
		output_nullt_string(" byte");
		if (dac.code_length != 1) {
			output_char('s');
		}
		output_nullt_string("\n\n");
	#endif
	
	clean_up(file, block, error_number);
	
	run_generated_code(&dac);
	
	#if DEBUG
		output_nullt_string(PROGRAM_NAME " JIT successfully ran code.\n");
	#endif
	
	if (dac.start_of_data) {
		m_free(dac.start_of_data);
	}
	
	m_free_memory_for_code(dac.start_of_code, dac.code_length);
	
	#if DEBUG
		show_ending_debug_information();
	#endif
	
	return error_number;
}
