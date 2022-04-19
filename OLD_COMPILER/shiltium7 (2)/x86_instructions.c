/*
x86 instruction generation functions and functions that get the length of x86 instructions.
*/

#include "headers.h"

#if DEBUG
	static bool in_pusha_or_popa = false;
#endif

#if DEBUG
	static const char* const restrict register_numbers_to_strings[16] =
		{"rax", "rcx", "rdx", "rbx", "rsp", "rbp", "rsi", "rdi", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"};
#endif

/* ----------------------------------------------------------- Length-getting functions -------------------------------------------------------- */

attr_const static inline u64 dac_get_length_of_push_or_pop(const Register register_) {
	return 1 + (register_ >= REG_R8);
}

u64 dac_get_length_of_push(const Register register_) {
	return dac_get_length_of_push_or_pop(register_);
}

u64 dac_get_length_of_pop(const Register register_) {
	return dac_get_length_of_push_or_pop(register_);
}

u64 dac_get_length_of_ret(void) {
	return 1;
}

attr_const static u64 dac_get_length_of_add_or_sub_reg_lit32(const Register register_, const s32 amount) {
	if ((schar)amount != amount && register_ == REG_RAX) return 6;
	return 4 + ((schar)amount != amount) * 3;
}

u64 dac_get_length_of_add_reg_lit32(const Register register_, const s32 amount) {
	return dac_get_length_of_add_or_sub_reg_lit32(register_, amount);
}

u64 dac_get_length_of_sub_reg_lit32(const Register register_, const s32 amount) {
	return dac_get_length_of_add_or_sub_reg_lit32(register_, amount);
}

u64 dac_get_length_of_mov_reg_reg(void) {
	return 3;
}

u64 dac_get_length_of_xor_reg_reg(void) {
	return 3;
}

u64 dac_get_length_of_add_reg_reg(void) {
	return 3;
}

u64 dac_get_length_of_sub_reg_reg(void) {
	return 3;
}

u64 dac_get_length_of_pusha(void) {
	small_num_type register_ = REG_R15 + 1;
	u64 total = 0;
	
	while (register_--) {
		total += dac_get_length_of_push((Register)register_);
	}
	
	return total;
}

u64 dac_get_length_of_popa(void) {
	small_num_type register_ = REG_R15 + 1;
	u64 total = 0;
	
	while (register_--) {
		total += dac_get_length_of_pop((Register)register_);
	}
	
	return total;
}

u64 dac_get_length_of_mov_reg_lit64(const s64 amount) {
	s32 amount32;
	
	if (!amount) {
		return dac_get_length_of_xor_reg_reg();
	}
	
	amount32 = amount;
	
	/*
	This is what the following code does:
	
	if (amount32 == amount) {
		return 7;
	} else {
		return 10;
	}
	*/
	return 7 + (amount32 != amount) * 3;
}

__attribute__((const)) static u64 get_length_of_fpu_deref_register_as_qword_instruction(const Register register_) {
	return 2 + (register_ >= REG_R8) + (register_ % 8 == 4 || register_ % 8 == 5);
}

u64 dac_get_length_of_fld_deref_register_as_qword(const Register register_) {
	return get_length_of_fpu_deref_register_as_qword_instruction(register_);
}

u64 dac_get_length_of_fmul_deref_register_as_qword(const Register register_) {
	return get_length_of_fpu_deref_register_as_qword_instruction(register_);
}

u64 dac_get_length_of_fadd_deref_register_as_qword(const Register register_) {
	return get_length_of_fpu_deref_register_as_qword_instruction(register_);
}

u64 dac_get_length_of_fsub_deref_register_as_qword(const Register register_) {
	return get_length_of_fpu_deref_register_as_qword_instruction(register_);
}

u64 dac_get_length_of_fstp_deref_register_as_qword(const Register register_) {
	return get_length_of_fpu_deref_register_as_qword_instruction(register_)
	#if DEBUG
		+ dac_get_length_of_int3()
	#endif
	;
}

u64 dac_get_length_of_fld1(void) {
	return 2;
}

u64 dac_get_length_of_fldz(void) {
	return 2;
}

#if DEBUG
	u64 dac_get_length_of_int3(void) {
		#if ENABLE_INT3
			return 1;
		#else
			return 0;
		#endif
	}
#endif

u64 dac_get_length_of_starting_instructions(void) {
	return dac_get_length_of_pusha();
}

u64 dac_get_length_of_ending_instructions(void) {
	return dac_get_length_of_popa() + dac_get_length_of_ret();
}


/* ------------------------------------- The x86 instruction-generation functions (the assembler functions) ------------------------------------ */


attr_nonnull static inline void dac_append_push_or_pop(
	DataAndCode* const restrict dac,
	const Register register_,
	const small_num_type instruction_code_base
) {
	assert_comparison(dac, !=, NULL);
	assert_comparison(register_, <=, REG_R15);
	
	if (register_ >= REG_R8) {
		dac->start_of_code[dac->current_instruction_offset++] = 65;
	}
	
	dac->start_of_code[dac->current_instruction_offset++] = instruction_code_base + register_ - ((register_ >= REG_R8) * 8);
}

void dac_append_push(DataAndCode* const restrict dac, const Register register_) {
	assert_comparison(dac, !=, NULL);
	assert_comparison(register_, <=, REG_R15);
	
	#if DEBUG
		if (!in_pusha_or_popa) {
			output_nullt_string(TAB_STRING "push ");
			output_nullt_string(register_numbers_to_strings[register_]);
			output_newline();
		}
	#endif
	
	dac_append_push_or_pop(dac, register_, 80);
}

void dac_append_pop(DataAndCode* const restrict dac, const Register register_) {
	assert_comparison(dac, !=, NULL);
	assert_comparison(register_, <=, REG_R15);
	
	#if DEBUG
		if (!in_pusha_or_popa) {
			output_nullt_string(TAB_STRING "pop ");
			output_nullt_string(register_numbers_to_strings[register_]);
			output_newline();
		}
	#endif
	
	dac_append_push_or_pop(dac, register_, 88);
}

void dac_append_ret(DataAndCode* const restrict dac) {
	assert_comparison(dac, !=, NULL);
	
	#if DEBUG
		output_nullt_string(TAB_STRING "ret\n");
	#endif
	dac->start_of_code[dac->current_instruction_offset++] = (uchar)195;
}

attr_nonnull static inline void dac_append_add_or_sub_reg_lit32(
	DataAndCode* const restrict dac,
	const Register register_,
	s32 amount,
	const small_num_type register_code_base,
	const small_num_type special_rax_code
) {
	assert_comparison(dac, !=, NULL);
	assert_comparison(register_, <=, REG_R15);
	
	dac->start_of_code[dac->current_instruction_offset] = 72 + (register_ >= REG_R8);
	
	if ((schar)amount == amount) {
		dac->start_of_code[dac->current_instruction_offset + 1] = (uchar)131;
		dac->start_of_code[dac->current_instruction_offset + 3] = (schar)amount;
	} else {
		dac->start_of_code[dac->current_instruction_offset + 2 + (register_ != REG_RAX)] = ((uchar*)&amount)[0];
		dac->start_of_code[dac->current_instruction_offset + 3 + (register_ != REG_RAX)] = ((uchar*)&amount)[1];
		dac->start_of_code[dac->current_instruction_offset + 4 + (register_ != REG_RAX)] = ((uchar*)&amount)[2];
		dac->start_of_code[dac->current_instruction_offset + 5 + (register_ != REG_RAX)] = ((uchar*)&amount)[3];
		
		if (register_ == REG_RAX) {
			dac->start_of_code[dac->current_instruction_offset + 1] = special_rax_code;
			dac->current_instruction_offset += 6;
			return;
		} else {
			dac->start_of_code[dac->current_instruction_offset + 1] = (uchar)129;
		}
	}
	dac->start_of_code[dac->current_instruction_offset + 2] = register_code_base + register_ - ((register_ >= REG_R8) * 8);
	dac->current_instruction_offset += 4 + ((schar)amount != amount) * 3;
}

void dac_append_add_reg_lit32(DataAndCode* const restrict dac, const Register register_, const s32 amount) {
	assert_comparison(dac, !=, NULL);
	assert_comparison(register_, <=, REG_R15);
	
	#if DEBUG
		output_nullt_string(TAB_STRING "add ");
		output_nullt_string(register_numbers_to_strings[register_]);
		output_nullt_string(", ");
		output_u64_without_commas(amount);
		output_newline();
	#endif
	
	dac_append_add_or_sub_reg_lit32(dac, register_, amount, 192, 5);
}

void dac_append_sub_reg_lit32(DataAndCode* const restrict dac, const Register register_, const s32 amount) {
	assert_comparison(dac, !=, NULL);
	assert_comparison(register_, <=, REG_R15);
	
	#if DEBUG
		output_nullt_string(TAB_STRING "sub ");
		output_nullt_string(register_numbers_to_strings[register_]);
		output_nullt_string(", ");
		output_u64_without_commas(amount);
		output_newline();
	#endif
	
	dac_append_add_or_sub_reg_lit32(dac, register_, amount, 232, 45);
}

attr_nonnull static void dac_reg_reg(
	DataAndCode* const restrict dac,
	const Register register1,
	const Register register2,
	const small_num_type instruction_number
	#if DEBUG
		, const char* const restrict instruction_string
	#endif
) {
	assert_comparison(dac, !=, NULL);
	assert_comparison(register1, <=, REG_R15);
	assert_comparison(register2, <=, REG_R15);
	
	#if DEBUG
		output_nullt_string(TAB_STRING);
		output_nullt_string(instruction_string);
		output_char(' ');
		output_nullt_string(register_numbers_to_strings[register1]);
		output_nullt_string(", ");
		output_nullt_string(register_numbers_to_strings[register2]);
		output_newline();
	#endif
	
	if (register1 <= REG_RDI) {
		if (register2 <= REG_RDI) {
			dac->start_of_code[dac->current_instruction_offset] = 72;
		} else {
			dac->start_of_code[dac->current_instruction_offset] = 76;
		}
	} else {
		if (register2 <= REG_RDI) {
			dac->start_of_code[dac->current_instruction_offset] = 73;
		} else {
			dac->start_of_code[dac->current_instruction_offset] = 77;
		}
	}
	
	dac->start_of_code[dac->current_instruction_offset + 1] = instruction_number;
	dac->start_of_code[dac->current_instruction_offset + 2] = 192 + ((register1 % 8) + ((register2 % 8) * 8));
	dac->current_instruction_offset += 3;
}

void dac_append_mov_reg_reg(DataAndCode* const restrict dac, const Register register1, const Register register2) {
	assert_comparison(dac, !=, NULL);
	
	dac_reg_reg(dac, register1, register2, 137
	#if DEBUG
	, "mov"
	#endif
	);
}

void dac_append_xor_reg_reg(DataAndCode* const restrict dac, const Register register1, const Register register2) {
	assert_comparison(dac, !=, NULL);
	
	dac_reg_reg(dac, register1, register2, 49
	#if DEBUG
	, "xor"
	#endif
	);
}

void dac_append_add_reg_reg(DataAndCode* const restrict dac, const Register register1, const Register register2) {
	assert_comparison(dac, !=, NULL);
	
	dac_reg_reg(dac, register1, register2, 1
	#if DEBUG
	, "add"
	#endif
	);
}

void dac_append_sub_reg_reg(DataAndCode* const restrict dac, const Register register1, const Register register2) {
	assert_comparison(dac, !=, NULL);
	
	dac_reg_reg(dac, register1, register2, 41
	#if DEBUG
	, "sub"
	#endif
	);
}

/* Reimplementation of the pusha instruction for x86-64 using multiple push instructions. */
void dac_append_pusha(DataAndCode* const restrict dac) {
	small_num_type register_;
	
	assert_comparison(dac, !=, NULL);
	
	#if DEBUG
		in_pusha_or_popa = true;
		output_nullt_string(TAB_STRING);
		output_nullt_string("push ");
	#endif
	
	for (register_ = REG_RAX; register_ <= REG_R15; ++register_) {
		#if DEBUG
			output_nullt_string(register_numbers_to_strings[register_]);
			output_char(' ');
		#endif
		dac_append_push(dac, (Register)register_);
	}
	
	#if DEBUG
		output_newline();
		in_pusha_or_popa = false;
	#endif
}

/* Reimplementation of the popa instruction for x86-64 using multiple pop instructions. */
void dac_append_popa(DataAndCode* const restrict dac) {
	small_num_type register_ = REG_R15 + 1;
	
	assert_comparison(dac, !=, NULL);
	
	#if DEBUG
		in_pusha_or_popa = true;
		output_nullt_string(TAB_STRING);
		output_nullt_string("pop ");
	#endif
	
	while (register_--) {
		#if DEBUG
			output_nullt_string(register_numbers_to_strings[register_]);
			output_char(' ');
		#endif
		dac_append_pop(dac, (Register)register_);
	}
	
	#if DEBUG
		output_newline();
		in_pusha_or_popa = false;
	#endif
}

void dac_append_mov_reg_lit64(DataAndCode* const restrict dac, const Register register_, s64 amount) {
	s32 amount32;
	
	assert_comparison(dac, !=, NULL);
	
	if (!amount) {
		/* xor reg, reg optimization */
		dac_append_xor_reg_reg(dac, register_, register_);
		return;
	}
	
	assert_comparison(register_, <=, REG_R15);
	amount32 = amount;
	
	#if DEBUG
		output_nullt_string(TAB_STRING "mov ");
		output_nullt_string(register_numbers_to_strings[register_]);
		output_nullt_string(", ");
		output_u64_without_commas(amount);
		output_newline();
	#endif
	
	dac->start_of_code[dac->current_instruction_offset] = 72 + (register_ >= REG_R8);
	if (amount32 == amount) {
		dac->start_of_code[dac->current_instruction_offset + 1] = (uchar)199;
		dac->start_of_code[dac->current_instruction_offset + 2] = (uchar)192 + (register_ % 8);
		dac->start_of_code[dac->current_instruction_offset + 3] = ((uchar*)&amount32)[0];
		dac->start_of_code[dac->current_instruction_offset + 4] = ((uchar*)&amount32)[1];
		dac->start_of_code[dac->current_instruction_offset + 5] = ((uchar*)&amount32)[2];
		dac->start_of_code[dac->current_instruction_offset + 6] = ((uchar*)&amount32)[3];
		dac->current_instruction_offset += 7;
	} else {
		dac->start_of_code[dac->current_instruction_offset + 1] = 184 + (register_ % 8);
		dac->start_of_code[dac->current_instruction_offset + 2] = ((uchar*)&amount)[0];
		dac->start_of_code[dac->current_instruction_offset + 3] = ((uchar*)&amount)[1];
		dac->start_of_code[dac->current_instruction_offset + 4] = ((uchar*)&amount)[2];
		dac->start_of_code[dac->current_instruction_offset + 5] = ((uchar*)&amount)[3];
		dac->start_of_code[dac->current_instruction_offset + 6] = ((uchar*)&amount)[4];
		dac->start_of_code[dac->current_instruction_offset + 7] = ((uchar*)&amount)[5];
		dac->start_of_code[dac->current_instruction_offset + 8] = ((uchar*)&amount)[6];
		dac->start_of_code[dac->current_instruction_offset + 9] = ((uchar*)&amount)[7];
		dac->current_instruction_offset += 10;
	}
}

attr_nonnull static void
dac_append_fpu_deref_register_as_qword_instruction(
	DataAndCode* const restrict dac,
	const Register register_,
	const uchar instruction_number,
	const uchar magic_number1,
	const uchar magic_number2,
	const uchar base
	#if DEBUG
		, const char* const restrict instruction_string
	#endif
) {
	assert_comparison(dac, !=, NULL);
	assert_comparison(register_, <=, REG_R15);
	
	#if DEBUG
		output_nullt_string(TAB_STRING);
		output_nullt_string(instruction_string);
		output_nullt_string(" qword[");
		output_nullt_string(register_numbers_to_strings[register_]);
		output_nullt_string("]\n");
	#endif
	
	if (register_ >= REG_R8) {
		dac->start_of_code[dac->current_instruction_offset++] = 65;
	}
	
	dac->start_of_code[dac->current_instruction_offset++] = instruction_number;
	
	/* rsp or r12 */
	if (register_ % 8 == 4) {
		dac->start_of_code[dac->current_instruction_offset++] = magic_number1;
		dac->start_of_code[dac->current_instruction_offset++] = 36;
		dac->current_instruction_offset += 2;
	}
	/* rbp or r13 */
	else if (register_ % 8 == 5) {
		dac->start_of_code[dac->current_instruction_offset] = magic_number2;
		dac->start_of_code[dac->current_instruction_offset] = 0;
		dac->current_instruction_offset += 2;
	} else {
		dac->start_of_code[dac->current_instruction_offset++] = base + (register_ % 8);
	}
}

/* fld qword[reg] */
void dac_append_fld_deref_register_as_qword(DataAndCode* const restrict dac, const Register register_) {
	assert_comparison(dac, !=, NULL);
	assert_comparison(register_, <=, REG_R15);
	
	dac_append_fpu_deref_register_as_qword_instruction(dac, register_, 221, 4, 69, 0
	#if DEBUG
		, "fld"
	#endif
	);
}

/* fmul qword[reg] */
void dac_append_fmul_deref_register_as_qword(DataAndCode* const restrict dac, const Register register_) {
	assert_comparison(dac, !=, NULL);
	assert_comparison(register_, <=, REG_R15);
	
	dac_append_fpu_deref_register_as_qword_instruction(dac, register_, 220, 12, 77, 8
	#if DEBUG
		, "fmul"
	#endif
	);
}

/* fadd qword[reg] */
void dac_append_fadd_deref_register_as_qword(DataAndCode* const restrict dac, const Register register_) {
	assert_comparison(dac, !=, NULL);
	assert_comparison(register_, <=, REG_R15);
	
	dac_append_fpu_deref_register_as_qword_instruction(dac, register_, 220, 4, 69, 0
	#if DEBUG
		, "fadd"
	#endif
	);
}

/* fsub qword[reg] */
void dac_append_fsub_deref_register_as_qword(DataAndCode* const restrict dac, const Register register_) {
	assert_comparison(dac, !=, NULL);
	assert_comparison(register_, <=, REG_R15);
	
	dac_append_fpu_deref_register_as_qword_instruction(dac, register_, 220, 36, 101, 32
	#if DEBUG
		, "fsub"
	#endif
	);
}

/* fstp qword[reg] */
void dac_append_fstp_deref_register_as_qword(DataAndCode* const restrict dac, const Register register_) {
	assert_comparison(dac, !=, NULL);
	assert_comparison(register_, <=, REG_R15);
	
	#if DEBUG
		dac_append_int3(dac);
	#endif
	
	dac_append_fpu_deref_register_as_qword_instruction(dac, register_, 221, 28, 93, 24
	#if DEBUG
		, "fstp"
	#endif
	);
}

void dac_append_fld1(DataAndCode* const restrict dac) {
	assert_comparison(dac, !=, NULL);
	
	#if DEBUG
		output_nullt_string(TAB_STRING "fld1\n");
	#endif
	
	dac->start_of_code[dac->current_instruction_offset++] = (uchar)217;
	dac->start_of_code[dac->current_instruction_offset++] = (uchar)232;
}

void dac_append_fldz(DataAndCode* const restrict dac) {
	assert_comparison(dac, !=, NULL);
	
	#if DEBUG
		output_nullt_string(TAB_STRING "fldz\n");
	#endif
	
	dac->start_of_code[dac->current_instruction_offset++] = (uchar)217;
	dac->start_of_code[dac->current_instruction_offset++] = (uchar)238;
}

#if DEBUG
	void dac_append_int3(DataAndCode* const restrict dac) {	
		assert_comparison(dac, !=, NULL);
		
		#if ENABLE_INT3
			output_nullt_string(TAB_STRING "int3\n");
			dac->start_of_code[dac->current_instruction_offset++] = (uchar)204;
		#endif
	}
#endif

void dac_append_starting_instructions(DataAndCode* const restrict dac) {
	assert_comparison(dac, !=, NULL);
	
	#if DEBUG
		output_nullt_string("\nstart:\n");
		output_nullt_string(TAB_STRING "; Save all the registers\n");
	#endif
	
	dac_append_pusha(dac);
	output_newline();
}

void dac_append_ending_instructions(DataAndCode* const restrict dac) {
	assert_comparison(dac, !=, NULL);
	
	#if DEBUG
		output_nullt_string(TAB_STRING "; Restore all the registers\n");
	#endif
	
	dac_append_popa(dac);
	
	#if DEBUG
		output_newline();
		output_nullt_string(TAB_STRING "; Return back to C\n");
	#endif
	
	dac_append_ret(dac);
	
	#if DEBUG
		output_newline();
	#endif
}
