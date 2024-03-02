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

#include "debug.h"

#define PROGRAM_NAME   "Shiltium"
#define PROGRAM_NAME_LOWERCASE   "shiltium"
#define VERSION   "0.01.11"
#define SHILTO_LANGUAGE_VERSION   VERSION

#if DEBUG
	#define ENABLE_INT3  0
#endif

#include "os_detection.h"

#if __GNUC__
	#define likely(x)    __builtin_expect((x), 1)
	#define unlikely(x)  __builtin_expect((x), 0)
#else
	#define likely(x)   (x)
	#define unlikely(x) (x)
#endif

#if __GNUC__
	#define inline __inline__
	#define restrict __restrict__
#endif

#define TAB_STRING "    "

#include <inttypes.h>

typedef uint64_t u64;
typedef int64_t s64;
typedef uint32_t u32;
typedef int32_t s32;
typedef double f64;

typedef uint8_t uchar;
typedef int8_t schar;
typedef u32 small_num_type;

#define double YOU_SHOULD_NOT_USE_DOUBLE
#define float YOU_SHOULD_NOT_USE_FLOAT
#define auto YOU_SHOULD_NOT_USE_AUTO
#define register YOU_SHOULD_NOT_USE_REGISTER
#define short YOU_SHOULD_NOT_USE_SHORT
#define unsigned YOU_SHOULD_NOT_USE_UNSIGNED
#define signed YOU_SHOULD_NOT_USE_SIGNED
#define int64_t YOU_SHOULD_NOT_USE_INT64_T
#define uint64_t YOU_SHOULD_NOT_USE_UINT64_T

#ifndef OUTPUT_FUNCTIONS_MODULE
	#define abort YOU_SHOULD_NOT_USE_ABORT
	#define strlen YOU_SHOULD_NOT_USE_STRLEN
	
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
		#ifndef OUTPUT_FUNCTIONS_MODULE
			#define free YOU_SHOULD_NOT_USE_FREE
		#endif
	#endif
#endif

#include "assert_comparison.h"

/* Enums */

#ifdef REG_RAX
	#undef REG_RAX
	#undef REG_RCX
	#undef REG_RDX
	#undef REG_RBX
	#undef REG_RSP
	#undef REG_RBP
	#undef REG_RSI
	#undef REG_RDI
	#undef REG_R8
	#undef REG_R9
	#undef REG_R10
	#undef REG_R11
	#undef REG_R12
	#undef REG_R13
	#undef REG_R14
	#undef REG_R15
	
	#define REG_RAX  REG__RAX
	#define REG_RCX  REG__RCX
	#define REG_RDX  REG__RDX
	#define REG_RBX  REG__RBX
	#define REG_RSP  REG__RSP
	#define REG_RBP  REG__RBP
	#define REG_RSI  REG__RSI
	#define REG_RDI  REG__RDI
	#define REG_R8   REG__R8
	#define REG_R9   REG__R9
	#define REG_R10  REG__R10
	#define REG_R11  REG__R11
	#define REG_R12  REG__R12
	#define REG_R13  REG__R13
	#define REG_R14  REG__R14
	#define REG_R15  REG__R15
#endif

typedef enum {
	REG_RAX, REG_RCX, REG_RDX, REG_RBX, REG_RSP, REG_RBP,
	REG_RSI, REG_RDI, REG_R8, REG_R9, REG_R10, REG_R11,
	REG_R12, REG_R13, REG_R14, REG_R15
} Register;

typedef enum {
	/* No error */
	ERROR_NUMBER_NO_ERROR,
	
	/* Syntax errors */
	ERROR_NUMBER__SYNTAX_ERROR__EMPTY_STATEMENT,
	ERROR_NUMBER__SYNTAX_ERROR__MISSING_SEMICOLON,
	ERROR_NUMBER__SYNTAX_ERROR__UNCLOSED_BLOCK_COMMENT_AT_BEGINNING_OF_LINE,
	ERROR_NUMBER__SYNTAX_ERROR__UNCLOSED_BLOCK_COMMENT_NOT_AT_BEGINNING_OF_LINE,
	ERROR_NUMBER__SYNTAX_ERROR__UNCLASSIFIED_SYNTAX_ERROR,
	
	/* Type errors */
	ERROR_NUMBER__TYPE_ERROR__INVALID_TYPE,
	
	/* Literal errors */
	ERROR_NUMBER__LITERAL_ERROR__UINT_TOO_LARGE,
	ERROR_NUMBER__LITERAL_ERROR__INVALID_UINT,
	ERROR_NUMBER__LITERAL_ERROR__INVALID_FLOAT,
	ERROR_NUMBER__LITERAL_ERROR__MORE_THAN_ONE_POINT_IN_FLOAT_LITERAL,
	ERROR_NUMBER__LITERAL_ERROR__INVALID_CHARACTER_IN_BASE,
	ERROR_NUMBER__LITERAL_ERROR__EMPTY_EXPONENT,
	ERROR_NUMBER__LITERAL_ERROR__INVALID_EXPONENT,
	ERROR_NUMBER__LITERAL_ERROR__INVALID_BASE,
	
	/* Name errors */
	ERROR_NUMBER__NAME_ERROR__INVALID_VARIABLE_NAME,
	ERROR_NUMBER__NAME_ERROR__VARIABLE_ALREADY_DECLARED
} ErrorNumber;

#if DEBUG
	#define LAST_ERROR_NUMBER   ERROR_NUMBER__NAME_ERROR__VARIABLE_ALREADY_DECLARED
#endif

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
	MLI_OPCODE_SUB_FVAR_FVAR   /* opcode 14 */
} MidLevelInstructionOpcode;

#if DEBUG
	#define LAST_MLI_OPCODE   MLI_OPCODE_SUB_FVAR_FVAR
#endif

typedef enum {TYPE_UINT, TYPE_SINT, TYPE_FLOAT, TYPE_BOOL, TYPE_VARIABLE} Type;
typedef enum {STATEMENT_OR_BLOCK_TYPE__STATEMENT, STATEMENT_OR_BLOCK_TYPE__BLOCK} StatementOrBlockType;
typedef enum {HASH_TYPE_UINT, HASH_TYPE_STRING} HashType;

typedef enum {
	OPERATOR_ADDITION,
	OPERATOR_SUBTRACTION,
	OPERATOR_MULTIPLICATION,
	OPERATOR_DIVISION,
	OPERATOR_EXPONENTIATION,
	OPERATOR_ROOTS
} Operator;

/* Unions */

/* This union is used to make casting unnecessary. */
typedef union {
	u64 u64_value;
	s64 s64_value;
	f64 f64_value;
	bool bool_value;
	char* string_value;
} TypeUnion;

typedef union {
	u64* u64_pointer;
	s64* s64_pointer;
	f64* float_pointer;
	bool* bool_pointer;
} TypePointerUnion;

/* Structs */

typedef struct {
	char* start_of_code;
	u64 current_instruction_offset;
	u64 code_length;
	
	char* start_of_data;
	u64 data_length;
	
	u64 stack_offset;
} DataAndCode;

typedef struct {
	MidLevelInstructionOpcode opcode;
	void *argument1, *argument2;
	
	/*
	The only time this is true is when this mid-level instruction is a mov
	instruction operating upon a temporary variable generated by the compiler.
	Since all the other variables are in a hash table, they don't need to be
	freed this way.
	*/
	bool should_free_argument2;
} MidLevelInstruction;

typedef struct {
	MidLevelInstruction** mid_level_instructions;
	u64 used_length;
	u64 allocated_length;
} MidLevelInstructions;

typedef struct {
	char* string;
	u64 length;
} String;

typedef struct {
	String string;
	u64 line_number;
} Token;

typedef struct {
	Token* tokens;
	u64 length_of_tokens;
	u64 length_of_statement_line_string;
	/* The original line in the code, for example "i = 5;" */
	char* statement_line;
	StatementOrBlockType type;
} Statement;

typedef struct HashNode {
	TypeUnion key;
	u64 length_of_key;
	HashType type_of_key;
	void* value;
	struct HashNode* next;
} HashItem;

typedef struct {
	HashItem** linked_lists;
	u64 allocated_length;
	u64 length;
	u64 max_amount_of_items_in_a_linked_list;
	f64 multiplication_amount;
} HashTable;

typedef struct {
	HashItem* current_hashitem;
	HashTable* hash_table;
	u64 how_far_in;
	TypeUnion key;
	u64 length_of_key;
	HashType type_of_key;
	void** value;
} HashIterator;

typedef struct {
	MidLevelInstructions mid_level_instructions;
	HashTable variables_in_scope;
	Statement** statement_or_block_array;
	u64 length;
} Block;

typedef struct OperandNode {
	const char* string;
	u64 string_length;
	u64 line_number;
	TypeUnion value;
	struct OperandNode *previous_node, *next_node;
	Type type;
	Type type_of_the_variable_that_this_operand_node_references;
	struct Variable* variable;
	
	/*
	This is a pointer to a memory address in the string_and_float_literals_hash_table
	hash table defined in translate_mid_level_instructions_into_cpu_instructions.c
	*/
	void** address_in_memory;
} OperandNode;

typedef struct OperatorNode {
	Operator operator_;
	u64 line_number;
	struct OperatorNode *previous_node, *next_node;
} OperatorNode;

typedef struct {
	OperandNode *first_node, *last_node;
} OperandLinkedList;

typedef struct {
	OperatorNode *first_node, *last_node;
} OperatorLinkedList;

typedef struct Variable {
	String name;
	u64 line_declared_on;
	OperatorLinkedList operators;
	OperandLinkedList operands;
	u64 stack_offset;
	MidLevelInstruction* last_instruction_that_uses_this_variable;
	bool evaluated;
} Variable;

typedef struct {
	u64 number_of_enclosing_loops;
	u64 number_of_enclosing_conditionals;
} GlobalData;

/* Functions / Macros */

#if __GNUC__
	#define attr_nonnull __attribute__((nonnull))
	#define attr_const __attribute__((const))
	#define attr_noreturn __attribute__((noreturn))
	#define attr_flatten __attribute__((flatten))
	#define attrs_nonnull_and_flatten __attribute__((nonnull, flatten))
	#define attrs_nonnull_and_warn_unused_result __attribute__((nonnull, warn_unused_result))
	#define attrs_const_and_flatten __attribute__((const, flatten))
	#define attrs_const_and_nonnull __attribute__((const, nonnull))
#else
	#define attr_nonnull
	#define attr_const
	#define attr_noreturn
	#define attr_flatten
	#define attrs_nonnull_and_flatten
	#define attrs_nonnull_and_warn_unused_result
	#define attrs_const_and_flatten
	#define attrs_const_and_nonnull
#endif

/* memory.c */
#if DEBUG
	void update_memory_allocation_for_fopen(void);
	void update_memory_allocation_for_fclose(void);
	char* m_alloc(size_t size);
	char* m_zero_initialized_alloc(size_t size);
	char* m_realloc(void* ptr, size_t size);
	void m_free(void* const restrict ptr) attr_nonnull;
	void show_memory_usage(void);
#else
	#define m_alloc(size)   ((char*)malloc(size))
	#define m_realloc(ptr, size)   ((char*)realloc(ptr, size))
	#define m_free(size)   free(size)
	#define m_zero_initialized_alloc(size) ((char*)calloc((size), 1))
#endif

void* m_allocate_writable_and_executable_memory(DataAndCode* const restrict dac, size_t length);
void m_make_memory_only_executable(void* restrict address, const size_t length);
void m_free_memory_for_code(void* restrict address, const size_t length);

/* x86_instructions.c */
u64 dac_get_length_of_push(const Register register_) attrs_const_and_flatten;
u64 dac_get_length_of_pop(const Register register_) attrs_const_and_flatten;
u64 dac_get_length_of_ret(void) attr_const;
u64 dac_get_length_of_add_reg_lit32(const Register register_, const s32 amount) attrs_const_and_flatten;
u64 dac_get_length_of_sub_reg_lit32(const Register register_, const s32 amount) attrs_const_and_flatten;
u64 dac_get_length_of_mov_reg_reg(void) attr_const;
u64 dac_get_length_of_xor_reg_reg(void) attr_const;
u64 dac_get_length_of_add_reg_reg(void) attr_const;
u64 dac_get_length_of_sub_reg_reg(void) attr_const;
u64 dac_get_length_of_pusha(void) attrs_const_and_flatten;
u64 dac_get_length_of_popa(void) attrs_const_and_flatten;
u64 dac_get_length_of_mov_reg_lit64(const s64 amount) attrs_const_and_flatten;
u64 dac_get_length_of_fld_deref_register_as_qword(const Register register_) attrs_const_and_flatten;
u64 dac_get_length_of_fmul_deref_register_as_qword(const Register register_) attrs_const_and_flatten;
u64 dac_get_length_of_fadd_deref_register_as_qword(const Register register_) attrs_const_and_flatten;
u64 dac_get_length_of_fsub_deref_register_as_qword(const Register register_) attrs_const_and_flatten;
u64 dac_get_length_of_fstp_deref_register_as_qword(const Register register_) attrs_const_and_flatten;
u64 dac_get_length_of_fld1(void) attr_const;
u64 dac_get_length_of_fldz(void) attr_const;
#if DEBUG
	u64 dac_get_length_of_int3(void) attr_const;
#endif
u64 dac_get_length_of_starting_instructions(void) attrs_const_and_flatten;
u64 dac_get_length_of_ending_instructions(void) attrs_const_and_flatten;

void dac_append_push(DataAndCode* const restrict dac, const Register register_) attrs_nonnull_and_flatten;
void dac_append_pop(DataAndCode* const restrict dac, const Register register_) attrs_nonnull_and_flatten;
void dac_append_ret(DataAndCode* const restrict dac) attr_nonnull;
void dac_append_add_reg_lit32(DataAndCode* const restrict dac, const Register register_, const s32 amount) attrs_nonnull_and_flatten;
void dac_append_sub_reg_lit32(DataAndCode* const restrict dac, const Register register_, const s32 amount) attrs_nonnull_and_flatten;
void dac_append_mov_reg_reg(DataAndCode* const restrict dac, const Register register1, const Register register2) attr_nonnull;
void dac_append_xor_reg_reg(DataAndCode* const restrict dac, const Register register1, const Register register2) attr_nonnull;
void dac_append_add_reg_reg(DataAndCode* const restrict dac, const Register register1, const Register register2) attr_nonnull;
void dac_append_sub_reg_reg(DataAndCode* const restrict dac, const Register register1, const Register register2) attr_nonnull;
void dac_append_pusha(DataAndCode* const restrict dac) attr_nonnull;
void dac_append_popa(DataAndCode* const restrict dac) attr_nonnull;
void dac_append_mov_reg_lit64(DataAndCode* const restrict dac, const Register register_, const s64 amount) attr_nonnull;
void dac_append_fld_deref_register_as_qword(DataAndCode* const restrict dac, const Register register_) attrs_nonnull_and_flatten;
void dac_append_fmul_deref_register_as_qword(DataAndCode* const restrict dac, const Register register_) attrs_nonnull_and_flatten;
void dac_append_fadd_deref_register_as_qword(DataAndCode* const restrict dac, const Register register_) attrs_nonnull_and_flatten;
void dac_append_fsub_deref_register_as_qword(DataAndCode* const restrict dac, const Register register_);
void dac_append_fstp_deref_register_as_qword(DataAndCode* const restrict dac, const Register register_) attrs_nonnull_and_flatten;
void dac_append_fld1(DataAndCode* const restrict dac) attr_nonnull;
void dac_append_fldz(DataAndCode* const restrict dac) attr_nonnull;
#if DEBUG
	void dac_append_int3(DataAndCode* const restrict dac) attr_nonnull;
#endif
void dac_append_starting_instructions(DataAndCode* const restrict dac) attrs_nonnull_and_flatten;
void dac_append_ending_instructions(DataAndCode* const restrict dac) attrs_nonnull_and_flatten;

/* conversion.c */
void add_commas_to_u64(u64 number, char* const string) attr_nonnull;
void u64_to_string(u64 number, char* const string, small_num_type base) attr_nonnull;
void f64_to_string(f64 number);

/* expression_parser.c */
void operand_linked_list_del(OperandLinkedList* linked_list) attr_nonnull;
void operator_linked_list_del(OperatorLinkedList* linked_list) attr_nonnull;
void expression_parser_parse_expression(
	Variable* const restrict variable,
	Statement* const statement,
	ErrorNumber* const restrict error_number,
	Block* block,
	Type type
) attr_nonnull;

/* hash_table.c */
void hash_table_new(
	HashTable* const restrict ht, const u64 number_of_linked_lists, const u64 max_amount_of_items_in_a_linked_list, const f64 multiplication_amount
) attr_nonnull;
bool hash_table_insert_string(HashTable* const restrict ht, char* key, const u64 length_of_key, void* value, const bool replace) attr_nonnull;
bool hash_table_insert_uint(HashTable* const restrict ht, const u64 key, void* value, const bool replace) attr_nonnull;
void** hash_table_get_value_with_string_key(HashTable* ht, const char* key, u64 length_of_key) attr_nonnull;
void** hash_table_get_value_with_uint_key(HashTable* ht, u64 key) attr_nonnull;
void hash_iterator_init(HashTable* const restrict hash_table, HashIterator* const restrict hash_iterator) attr_nonnull;
bool hash_iterator_next(HashIterator* const restrict iterator) attr_nonnull;
void hash_table_del(HashTable* const restrict ht) attr_nonnull;
void hash_table_del_with_values(HashTable* const restrict ht, void (*value_deletion_function)(void*)) attr_nonnull;

/* is_valid_identifer.c */
bool is_valid_identifier(const char* const restrict string, const u64 string_length) attr_nonnull;

/* literal_conversion.c */
u64 convert_literal_to_uint(const char* string, u64 string_length, u64 line_number, ErrorNumber* error_number)
	attrs_nonnull_and_warn_unused_result;
f64 convert_literal_to_float(const char* string, u64 string_length, u64 line_number, ErrorNumber* error_number)
	attrs_nonnull_and_warn_unused_result;

/* mid_level_instructions.c */
void mid_level_instructions_init(MidLevelInstructions* const restrict mli) attr_nonnull;
void mid_level_instructions_append(
	MidLevelInstructions* const restrict mli, MidLevelInstruction* const restrict instruction
) attr_nonnull;
MidLevelInstruction* mid_level_instructions_get_last_instruction(
	MidLevelInstructions* const restrict mli
) attrs_const_and_nonnull;
void mid_level_instructions_del(MidLevelInstructions* const restrict mli) attr_nonnull;

/* string_comparison.c */
bool strequal(const char* restrict string1, const char* restrict string2) attr_nonnull;
bool strnequal(const char* restrict str1, const char* restrict str2, const u64 length1, const u64 length2) attr_nonnull;

/* output_functions.c */
void setup_output_buffer(void);
void output_string(const char* const restrict string, const u64 length) attr_nonnull;
void output_nullt_string(const char* const restrict string) attr_nonnull;
void output_u64(const u64 number) attr_nonnull;
void output_u64_without_commas(const u64 number);
void output_errno_string(void);
void output_char(const int char_);
void output_newline(void);
void output_f64(const f64 number);
void open_error_dialog_and_exit_when_it_gets_closed(void (*callback_function)(int), const int argument);
void output_everything(void);

/* parser.c */
Block* parse_block(FILE* restrict file, DataAndCode* restrict dac, ErrorNumber* error_number, GlobalData* global_data);

/* translate_mid_level_instructions_into_cpu_instructions.c */
void translate_mid_level_instructions_to_cpu_instructions(
	DataAndCode* const restrict dac,
	MidLevelInstructions* const restrict mli
) attr_nonnull;

/* translate_expression_into_mid_level_instructions.c */
void translate_expression_into_mid_level_instructions(
	DataAndCode* const restrict dac,
	Block* const restrict block,
	Variable* const restrict variable
) attr_nonnull;

#include "deref.h"

/* misc_utils.c */
attrs_const_and_nonnull static inline size_t str_len(const char* const restrict string) {
	size_t i;
	for (i = 0; ARRAY_INDEX(string, i); ++i) {}
	return i;
}

attr_nonnull static inline void __memcopy(void* const restrict dest, const void* const restrict src, size_t num_bytes) {
	while (num_bytes--) {
		ARRAY_INDEX_ASSIGN((char* const restrict)dest, num_bytes) = ARRAY_INDEX((const char* restrict)src, num_bytes);
	}
}

#define memcpy __memcopy
