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
	#include <cstring>
	#include <cstdlib>
	#include <cmath>
	#include <cstdio>
	#include <csignal>
#else
	#include <string.h>
	#include <stdlib.h>
	#include <math.h>
	#include <signal.h>
	#include <stdio.h>
	#include <stdbool.h>
#endif

#include <inttypes.h>

#include "debug.h"
#include "os_detection.h"

#define PROGRAM_NAME   "Shiltiumcomp"
#define PROGRAM_NAME_LOWERCASE   "shiltiumcomp"
#define VERSION   "0.0.26"
#define LANGUAGE_VERSION   VERSION
#define LANGUAGE_NAME  "Shiltolang"
#define TAB_STRING "    "

/*
Should stack usage be minimized?

If enabled, the stack usage will be minimized, but the heap usage will increase.
This is enabled by default to enable a greater maximum number of nested blocks.
*/
#define MINIMIZE_STACK_USAGE   1

/*
Should mmap() be used?

USE_MMAP should equal 1 if mmap() should be used or 0 if
a combination of m_alloc() and mprotect() should be used.
*/
#if UNIX
	#define USE_MMAP   1
#else
	#define USE_MMAP   0
#endif

#if WINDOWS
	#define NICE_QUOTES   0
#else
	#define NICE_QUOTES   1
#endif

#if NICE_QUOTES
	#define OPENING_QUOTE "‘"
	#define CLOSING_QUOTE "’"
#else
	#define OPENING_QUOTE "'"
	#define CLOSING_QUOTE "'"
#endif

#undef NICE_QUOTES

#ifdef __GNUC__
	#ifdef __x86_64__
		#define SIXTY_FOUR_BIT  1
	#elif defined __i386__
		#define SIXTY_FOUR_BIT  0
	#else
		#error "Neither __i386__ nor __x86_64__ is defined."
	#endif
#else
	#define SIXTY_FOUR_BIT  0
#endif

#if __GNUC__
	#define likely(x)    __builtin_expect(!!(x), 1)
	#define unlikely(x)  __builtin_expect(!!(x), 0)
#else
	#define likely(x)   (x)
	#define unlikely(x) (x)
#endif

#if __GNUC__
	#define inline __inline__
	#define restrict __restrict__
#endif

typedef uint64_t u64;
typedef uint32_t u32;
typedef int32_t s32;

typedef uint8_t uchar;
typedef int8_t schar;
typedef u32 small_num_type;

typedef size_t u;
typedef signed long s;
typedef double f;

#define cr   const restrict

#define double YOU_SHOULD_NOT_USE_DOUBLE
#define float YOU_SHOULD_NOT_USE_FLOAT
#define auto YOU_SHOULD_NOT_USE_AUTO
#define register YOU_SHOULD_NOT_USE_REGISTER
#define short YOU_SHOULD_NOT_USE_SHORT
#define unsigned YOU_SHOULD_NOT_USE_UNSIGNED
#define signed YOU_SHOULD_NOT_USE_SIGNED
#define int64_t YOU_SHOULD_NOT_USE_INT64_T
#define uint64_t YOU_SHOULD_NOT_USE_UINT64_T

#define abort YOU_SHOULD_NOT_USE_ABORT
#define exit YOU_SHOULD_NOT_USE_EXIT

#ifndef OUTPUT_MODULE
	#define puts YOU_SHOULD_NOT_USE_PUTS
	#define fputs YOU_SHOULD_NOT_USE_FPUTS
	#define fputc YOU_SHOULD_NOT_USE_FPUTC
	#ifdef putchar
		#undef putchar
	#endif
	#define putchar YOU_SHOULD_NOT_USE_PUTCHAR
	#define printf YOU_SHOULD_NOT_USE_PRINTF
	#define fprintf YOU_SHOULD_NOT_USE_FPRINTF
	#define sprintf YOU_SHOULD_NOT_USE_SPRINTF
	#define snprintf YOU_SHOULD_NOT_USE_SNPRINTF
	#define vprintf YOU_SHOULD_NOT_USE_VPRINTF
	#define vfprintf YOU_SHOULD_NOT_USE_VFPRINTF
	#define vsprintf YOU_SHOULD_NOT_USE_VSPRINTF
	#define vsnprintf YOU_SHOULD_NOT_USE_VSNPRINTF
	#ifdef putc
		#undef putc
	#endif
	#define putc YOU_SHOULD_NOT_USE_PUTC
	#define fflush YOU_SHOULD_NOT_USE_FFLUSH
	#define gets YOU_SHOULD_NOT_USE_GETS
	#define fgets YOU_SHOULD_NOT_USE_GETS
	#define fread YOU_SHOULD_NOT_USE_FREAD
	#define fwrite YOU_SHOULD_NOT_USE_FWRITE
	#ifdef getchar
		#undef getchar
	#endif
	#define getchar YOU_SHOULD_NOT_USE_GETCHAR
	#define getw YOU_SHOULD_NOT_USE_GETW
	#define perror YOU_SHOULD_NOT_USE_PERROR
#endif

#if DEBUG
	#ifndef MEMORY_ALLOCATION_MODULE
		#define malloc YOU_SHOULD_NOT_USE_MALLOC
		#define calloc YOU_SHOULD_NOT_USE_CALLOC
		#define realloc YOU_SHOULD_NOT_USE_REALLOC
		#define free YOU_SHOULD_NOT_USE_FREE
	#endif
#endif

/* Enums */

typedef enum {
	LANGUAGE_ENGLISH,
	LANGUAGE_FRENCH,
	LANGUAGE_GERMAN,
	LANGUAGE_ITALIAN,
	LANGUAGE_KOREAN,
	LANGUAGE_PORTUGUESE,
	LANGUAGE_SPANISH,
	LANGUAGE_RUSSIAN
} Language;


typedef enum {
	/* Assignment */
	MLI_OPCODE_MOV_UVAR_UVAL,  /* opcode 0 */
	MLI_OPCODE_MOV_FVAR_FVAL,  /* opcode 1 */
	MLI_OPCODE_MOV_FVAR_FVAR,  /* opcode 2 */
	
	/* Multiplication */
	MLI_OPCODE_MUL_UVAR_UVAL,  /* opcode 3 */
	MLI_OPCODE_MUL_FVAR_FVAL,  /* opcode 4 */
	
	/* Division */
	MLI_OPCODE_DIV_UVAR_UVAL,  /* opcode 5 */
	MLI_OPCODE_DIV_FVAR_FVAL,  /* opcode 6 */
	
	/* Addition */
	MLI_OPCODE_ADD_UVAR_UVAL,  /* opcode 7 */
	MLI_OPCODE_ADD_FVAR_FVAL,  /* opcode 8 */
	MLI_OPCODE_ADD_UVAR_UVAR,  /* opcode 9 */
	MLI_OPCODE_ADD_FVAR_FVAR,  /* opcode 10 */
	
	/* Subtraction */
	MLI_OPCODE_SUB_UVAR_UVAL,  /* opcode 11 */
	MLI_OPCODE_SUB_FVAR_FVAL,  /* opcode 12 */
	MLI_OPCODE_SUB_UVAR_UVAR,  /* opcode 13 */
	MLI_OPCODE_SUB_FVAR_FVAR,  /* opcode 14 */
	
	/* Comparison */
	MLI_OPCODE_CMP_LE_UVAR_UVAL,  /* opcode 15 */
	MLI_OPCODE_CMP_LT_UVAR_UVAL,  /* opcode 16 */
	MLI_OPCODE_CMP_EQ_UVAR_UVAL,  /* opcode 17 */
	MLI_OPCODE_CMP_NE_UVAR_UVAL,  /* opcode 18 */
	MLI_OPCODE_CMP_GT_UVAR_UVAL,  /* opcode 19 */
	MLI_OPCODE_CMP_GE_UVAR_UVAL,  /* opcode 20 */
	
	MLI_OPCODE_CMP_LE_FVAR_FVAL,  /* opcode 21 */
	MLI_OPCODE_CMP_LT_FVAR_FVAL,  /* opcode 22 */
	MLI_OPCODE_CMP_EQ_FVAR_FVAL,  /* opcode 23 */
	MLI_OPCODE_CMP_NE_FVAR_FVAL,  /* opcode 24 */
	MLI_OPCODE_CMP_GT_FVAR_FVAL,  /* opcode 25 */
	MLI_OPCODE_CMP_GE_FVAR_FVAL,  /* opcode 26 */
	
	MLI_OPCODE_CMP_LE_UVAR_UVAR,  /* opcode 27 */
	MLI_OPCODE_CMP_LT_UVAR_UVAR,  /* opcode 28 */
	MLI_OPCODE_CMP_EQ_UVAR_UVAR,  /* opcode 29 */
	MLI_OPCODE_CMP_NE_UVAR_UVAR,  /* opcode 30 */
	MLI_OPCODE_CMP_GT_UVAR_UVAR,  /* opcode 31 */
	MLI_OPCODE_CMP_GE_UVAR_UVAR,  /* opcode 32 */
	
	MLI_OPCODE_CMP_LE_FVAR_FVAR,  /* opcode 33 */
	MLI_OPCODE_CMP_LT_FVAR_FVAR,  /* opcode 34 */
	MLI_OPCODE_CMP_EQ_FVAR_FVAR,  /* opcode 35 */
	MLI_OPCODE_CMP_NE_FVAR_FVAR,  /* opcode 36 */
	MLI_OPCODE_CMP_GT_FVAR_FVAR,  /* opcode 37 */
	MLI_OPCODE_CMP_GE_FVAR_FVAR,  /* opcode 38 */
	
	/* Miscellaneous */
	
	MLI_OPCODE_CHANGE_SIGN,
	
	MLI_OPCODE_TEST_AND_JMP_IF_ZERO_FVAR_MEMADDR,  /* opcode 40 */
	
	MLI_OPCODE_SEND_HELLO_WORLD_MESSAGE_TO_UI,    /* opcode 41 */
	MLI_OPCODE_SEND_OUTPUT_UINT_MESSAGE_TO_UI,    /* opcode 43 */
	MLI_OPCODE_SEND_OUTPUT_FLOAT_MESSAGE_TO_UI,   /* opcode 44 */
	
	MLI_OPCODE_ADD_MAIN_SUBROUTINE_STARTING_INSTRUCTIONS,   /* opcode 44 */
	MLI_OPCODE_ADD_MAIN_SUBROUTINE_ENDING_INSTRUCTIONS,     /* opcode 45 */
	
	MLI_OPCODE_ADD_SUBROUTINE_STARTING_INSTRUCTIONS,    /* opcode 46 */
	MLI_OPCODE_ADD_SUBROUTINE_ENDING_INSTRUCTIONS,      /* opcode 47 */
	
	MLI_OPCODE_PUSH_ARGUMENT,     /* opcode 48 */
	MLI_OPCODE_CALL_SUBROUTINE    /* opcode 49 */
} MidLevelInstructionOpcode;

#if DEBUG
	#define LAST_MLI_OPCODE   MLI_OPCODE_CALL_SUBROUTINE
#endif

typedef enum {TYPE_UINT, TYPE_SINT, TYPE_FLOAT, TYPE_BOOL, TYPE_VARIABLE} Type;
typedef enum {HASH_TYPE_UINT, HASH_TYPE_STRING, HASH_TYPE_FLOATING_POINT_NUMBER} HashType;
typedef enum {OPTIMIZE_FOR_MEMORY, OPTIMIZE_FOR_SPEED} WhatToOptimizeFor;

typedef enum {
	OPERATOR_ADDITION,
	OPERATOR_SUBTRACTION,
	OPERATOR_MULTIPLICATION,
	OPERATOR_DIVISION,
	OPERATOR_EXPONENTIATION,
	OPERATOR_ROOTS,
	OPERATOR_LESS_THAN,
	OPERATOR_LESS_THAN_OR_EQUAL_TO,
	OPERATOR_EQUALS,
	OPERATOR_NOT_EQUAL,
	OPERATOR_GREATER_THAN_OR_EQUAL_TO,
	OPERATOR_GREATER_THAN
} Operator;

/* Unions */

/* This union is used to make casting unnecessary. */
typedef union {
	u uvalue;
	s svalue;
	f fvalue;
	bool bool_value;
	char* string_value;
	void* void_pointer_value;
} TypeUnion;

typedef union {
	u* u_pointer;
	s* s_pointer;
	f* f_pointer;
	bool* bool_pointer;
} TypePointerUnion;

/* Structs */

/*
One of the advantages of garbage-collected languages is that you
don't have to create structures like this. The only purpose of this
structure is to free already-allocated blocks when it is safe to
free them, after compiling the generated program, right before it
runs. It's a singly-linked list of blocks to free.
*/
typedef struct FreeListNode {
	void* block_to_free;
	void (*free_function)(void* block_to_free);
	struct FreeListNode* next_node;
} FreeListNode;

typedef FreeListNode* FreeListHead;

typedef struct HashNode {
	TypeUnion key;
	u length_of_key;
	HashType type_of_key;
	void* value;
	struct HashNode* next_node;
} HashItem;

typedef struct {
	HashItem** linked_lists;
	u allocated_length;
	u length;
	u max_amount_of_items_in_a_linked_list;
	f multiplication_amount;
} HashTable;

typedef struct {
	HashItem* current_hashitem;
	HashTable* hash_table;
	u how_far_in;
	TypeUnion key;
	u length_of_key;
	HashType type_of_key;
	void** value;
} HashIterator;

typedef struct {
	char* start_of_code;
	u current_instruction_offset;
	
	HashTable string_literals;
	HashTable float_literals;
	
	char* start_of_float_data;
	u float_data_real_length;
	u float_data_allocated_length;
	
	char* start_of_string_data;
	u string_data_real_length;
	u string_data_allocated_length;
} DataAndCode;

typedef struct Scope {
	HashTable subroutines_in_scope;
	HashTable variables_in_scope;
	struct Scope* parent_scope;
} Scope;

typedef struct {
	MidLevelInstructionOpcode opcode;
	void* argument1;
	void* argument2;
} MidLevelInstruction;

typedef struct {
	MidLevelInstruction** mid_level_instructions;
	u used_length;
	u allocated_length;
} MidLevelInstructions;

typedef struct {
	Scope scope;
	
	char* statement_buffer;
	u statement_buffer_allocated_length;
	u statement_buffer_offset;
	u line_number_at_start_of_statement;
	
	#if 0
	The offset of the first non-whitespace characer that isn't inside of a comment.  '
	
	For example, in the statement
	
	/* comment */  f number = 3.0;
	
	the 'first meaningful character offset' would be the offset of the character 'f'.
	
	/* comment */  f number = 3.0;
	               ^
	
	#endif
	u first_meaningful_character_offset;
	
	
	u* line_number;
	u block_comment_starting_line;
	FILE* file;
	DataAndCode* dac;
	MidLevelInstructions* mli;
	FreeListHead* free_list_head;
	u previous_mid_level_instruction_offset;
	u* previous_subroutine_offset;
	HashTable* subroutine_forward_references;
	u* stack_offset;
	
	int char_;
	bool in_subroutine;
	bool before_statement;
}	ParserState;

typedef struct LinkedListNode {
	void* value;
	struct LinkedListNode* next_node;
} LinkedListNode;

typedef LinkedListNode* LinkedListHead;

typedef struct {
	u code_offset;
	LinkedListHead argument_type_list;
	u length_of_argument_type_list;
	u line_declared_on;
	bool used;
} Subroutine;

typedef struct {
	LinkedListHead argument_type_list;
	LinkedListHead mid_level_instruction_list;
	u length_of_argument_type_list;
} SubroutineForwardReference;

typedef struct {
	char* string;
	u length;
} String;

typedef struct {
	String string;
	u line_number;
} Token;

typedef struct {
	Token** tokens;
	u length_of_tokens;
	/* The original line in the code, for example "number = 5.0;" */
	char* statement_line;
	u length_of_statement_line_string;
} Statement;

typedef struct OperandNode {
	char* string;
	u string_length;
	u line_number;
	TypeUnion value;
	Type type;
	struct Variable* variable;
	void** address_in_memory;
	struct OperandNode *previous_node, *next_node;
} OperandNode;

typedef struct OperatorNode {
	struct OperatorNode *previous_node, *next_node;
	Operator operator_;
	u line_number;
	bool is_unary;
} OperatorNode;

typedef struct {
	OperandNode *first_node, *last_node;
} OperandLinkedList;

typedef struct {
	OperatorNode *first_node, *last_node;
} OperatorLinkedList;

typedef struct Variable {
	String name;
	Statement* statement_declared_on;
	OperatorLinkedList operators;
	OperandLinkedList operands;
	u line_declared_on;
	u stack_offset;
	Type type;
	MidLevelInstruction* last_instruction_that_uses_this_variable;
	bool evaluated;
	bool is_argument;
} Variable;

/* Functions / Macros */

#ifdef __GNUC__
	#define attr_nonnull   __attribute__((nonnull))
	#define attr_const     __attribute__((const))
	
	/*
	If __GNUC__ is defined and the program is being compiled
	under Windows, the compiler is MinGW, which doesn't support
	the 'cold' or 'hot' attributes. Also the default version
	of gcc on FreeBSD doesn't support these attributes either.
	So the attributes are disabled if the program is being
	compiled with gcc under Windows or FreeBSD.
	*/
	#if WINDOWS || FREEBSD
		#define attr_cold
		#define attrs_cold_and_nonnull
		#define attr_cold_and_noreturn
		#define attr_hot
	#else
		#define attr_cold  __attribute__((cold))
		#define attrs_cold_and_nonnull   __attribute__((cold, nonnull))
		#define attr_cold_and_noreturn  __attribute__((cold, noreturn))
		#define attr_hot  __attribute__((hot))
	#endif
	
	#define attr_noreturn  __attribute__((noreturn))
	#define attr_flatten   __attribute__((flatten))
	#define attrs_nonnull_and_flatten __attribute__((nonnull, flatten))
	#define attrs_nonnull_and_warn_unused_result __attribute__((nonnull, warn_unused_result))
	#define attrs_const_and_flatten __attribute__((const, flatten))
	#define attrs_const_and_nonnull __attribute__((const, nonnull))
#else
	#define attr_nonnull
	#define attr_const
	#define attr_hot
	#define attr_cold
	#define attrs_cold_and_nonnull
	#define attr_cold_and_noreturn
	#define attr_noreturn
	#define attr_flatten
	#define attrs_nonnull_and_flatten
	#define attrs_nonnull_and_warn_unused_result
	#define attrs_const_and_flatten
	#define attrs_const_and_nonnull
#endif

/* memory.c */
#if DEBUG
	char* m_alloc(const size_t size);
	char* m_zero_initialized_alloc(const size_t size);
	char* m_realloc(void* cr ptr, const size_t size);
	void m_free(void* cr ptr) attr_nonnull;
	void show_memory_usage(void);
#else
	#define m_alloc(size)   ((char*)malloc(size))
	#define m_realloc(ptr, size)   ((char*)realloc(ptr, size))
	#define m_free   free
	#define m_zero_initialized_alloc(size) ((char*)calloc((size), 1))
#endif

void* m_allocate_code_section(size_t length);

void m_free_memory_for_code(void* restrict address
#if USE_MMAP
	, u length
#endif
) attr_nonnull;

void update_memory_allocations_for_fopen(void);
void update_memory_allocations_for_fclose(void);

/* constant_folding.c */
void constant_fold_expression(OperatorLinkedList* cr operators, OperandLinkedList* cr operands) attr_nonnull;
void operator_linked_list_delete_node(OperatorLinkedList* cr linked_list, OperatorNode* cr operator_node) attr_nonnull;

/* conversion.c */
void add_separators_to_u(u number, char* cr string) attr_nonnull;
void u_to_string(u number, char* const string, small_num_type base) attr_nonnull;
void f_to_string(f number);

/* dac_init.c */
void dac_init(DataAndCode* cr dac) attr_nonnull;

/* process_command_line_options.c */
void process_command_line_options(char** argv) attr_nonnull;

/* parse_expression.c */
void operand_linked_list_del(OperandLinkedList* cr linked_list) attr_nonnull;
void operator_linked_list_del(OperatorLinkedList* cr linked_list) attr_nonnull;
void parse_expression(
	ParserState* cr state,
	Variable* cr variable,
	Token** cr tokens,
	const u length_of_tokens,
	const u beginning_token_offset,
	HashTable* variables_in_scope,
	const Type type
) attr_nonnull;

/* free_list.c */
void free_list_append(
	FreeListHead* head,
	void* block_to_free,
	void (*free_function)(void* pointer)
) __attribute__((nonnull (2, 3)));

void free_list_del(FreeListNode* head);

/* get_length_of_mid_level_instructions.c */
u get_length_of_mid_level_instructions(MidLevelInstructions* cr mli) attr_nonnull;

/* hash_table.c */
void hash_table_init(
	HashTable* cr ht,
	const size_t number_of_linked_lists,
	const size_t max_amount_of_items_in_a_linked_list,
	const f multiplication_amount
) attr_nonnull;

bool hash_table_insert_with_string_key(
	HashTable* cr ht,
	char* key,
	const u length_of_key,
	void* const value,
	const bool replace,
	void*** cr new_value
) __attribute__((nonnull (1, 6)));

bool hash_table_insert_with_u_key(
	HashTable* cr ht,
	const u key,
	void* value,
	const bool replace,
	void*** cr new_value
) __attribute__((nonnull (1, 5)));

bool hash_table_insert_with_f_key(
	HashTable* cr ht,
	const f key,
	void* value,
	const bool replace,
	void*** cr new_value
) __attribute__((nonnull (1, 5)));

void** hash_table_get_value_from_string_key(
	HashTable* ht,
	const char* key,
	u length_of_key,
	bool* cr in_hash_table
) attr_nonnull;

void** hash_table_get_value_from_u_key(HashTable* ht, u key, bool* cr in_hash_table) attr_nonnull;
void** hash_table_get_value_from_f_key(HashTable* cr ht, f key, bool* cr in_hash_table) attr_nonnull;
void hash_table_delete_string_key(HashTable* cr ht, const char* cr key, const u length_of_key) attr_nonnull;
void hash_iterator_init(HashTable* cr hash_table, HashIterator* cr hash_iterator) attr_nonnull;
bool hash_iterator_next(HashIterator* cr iterator) attr_nonnull;
void hash_table_del(HashTable* cr ht) attr_nonnull;

void hash_table_del_with_keys_and_values(
	HashTable* cr ht,
	void (*value_deletion_function)(const TypeUnion key, const u length_of_key, void* value)
) attr_nonnull;

/* is_valid_identifer.c */
bool is_valid_identifier(const char* cr string, u string_length) attr_nonnull;

/* linked_list.c */
void linked_list_append(LinkedListHead* const head, void* const value) __attribute__((nonnull (1)));

/* literal_conversion.c */
u convert_literal_to_uint(const char* cr string, u string_length, u line_number) attrs_nonnull_and_warn_unused_result;
f convert_literal_to_float(const char* cr string, u string_length, u line_number) attrs_nonnull_and_warn_unused_result;

/* mid_level_instructions.c */
void mid_level_instructions_init(MidLevelInstructions* cr mli) attr_nonnull;
void mid_level_instructions_append(
	MidLevelInstructions* cr mli, MidLevelInstruction* cr instruction, DataAndCode* cr dac
) attr_nonnull;
MidLevelInstruction* mid_level_instructions_get_last_instruction(
	MidLevelInstructions* cr mli
) attrs_const_and_nonnull;
void mid_level_instructions_del(MidLevelInstructions* cr mli) attr_nonnull;

/* output.c */
void setup_output_buffer(void);
void output_string(const char* cr string, const u length) attr_nonnull;
void output_nullt_string(const char* cr string) attr_nonnull;
void putu(const u number) attr_nonnull;
void putu_without_commas(const u number);

void output_syntax_error_beginning_text(
	const u starting_line,
	const u ending_line
) attrs_cold_and_nonnull;

void output_source_lines(const Statement* cr statement, u first_line_number) attrs_cold_and_nonnull;
void output_errno_string(void) attr_cold;
void output_char(const int char_) attr_flatten;
void output_space(void) attr_flatten;
void output_newline(void);
void putf(const f number);
#if DEBUG
	void output_variable(Variable* cr variable) attr_nonnull;
	void rewind_output_pointer(const u number_of_bytes);
#endif
void exit_program(void) attr_noreturn;

/* put_instructions_into_code_section.c */
void put_instructions_into_code_section(DataAndCode* cr dac, MidLevelInstructions* cr mli) attr_nonnull;

/* string_comparison.c */
bool strequal(const char* restrict string1, const char* restrict string2) attr_nonnull;
bool strnequal(const char* restrict str1, const char* restrict str2, const u length1, const u length2) attr_nonnull;

/* translate_mid_level_instructions_into_cpu_instructions.c */
void translate_mid_level_instructions_to_cpu_instructions(
	DataAndCode* cr dac,
	MidLevelInstructions* cr mli,
	const u code_length
) attr_nonnull;

/* translate_expression_into_mid_level_instructions.c */
void translate_expression_into_mid_level_instructions(
	DataAndCode* cr dac,
	MidLevelInstructions* cr mid_level_instructions,
	Variable* cr variable,
	u* restrict stack_offset
) attr_nonnull;

#include "assert_comparison.h"
#include "deref.h"

/*
strlen() needs to be reimplemented in DEBUG mode because gcc gives an error
message in the assert_comparison macro if it's not. This happens because of
compiler optimization. gcc first substitutes any strlen calls with string
literals as arguments with constants that equal the value that the strlen
call would return at runtime (e.g., gcc would substitute strlen(#foo, "LITERAL")
with 7). gcc then sees that the '&&' operator is being used with a non-zero
constant and it would normally give a warning, but it gives an error instead
because -Werror is enabled.

strlen() doesn't need to be reimplemented in release mode because the
assert_comparison macro does nothing when in release mode.
*/
#if DEBUG
	attrs_const_and_nonnull static inline u reimplemented_strlen(const char* cr string) {
		u i;
		for (i = 0; ARRAY_INDEX(string, i); ++i) {}
		return i;
	}
	
	#define strlen reimplemented_strlen
#endif

#if !DEBUG
	/*
	If memcpy isn't reimplemented, gcc gives the error:
	
	./portable/output.c: In function ‘illegal_instruction_handler’:
	/usr/include/bits/string3.h:49: sorry, unimplemented: inlining failed in call to ‘memcpy’: function body not available
	*/
	attr_nonnull static inline void reimplemented_memcpy(void* cr dest, const void* restrict src, u num_bytes) {
		while (num_bytes--) {
			ARRAY_INDEX_ASSIGN((char* cr)dest, num_bytes) = ARRAY_INDEX((const char* restrict)src, num_bytes);
		}
	}
	
	#define memcpy reimplemented_memcpy
#endif
