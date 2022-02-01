/*
x86 and x86-64 instruction generation functions and functions that get the length of the generated instructions.

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

#include "../../global_headers/headers.h"
#include "instructions/x86_instructions.h"

#if DEBUG
	bool in_pusha_or_popa = false;
#endif

#if DEBUG
	#if SIXTY_FOUR_BIT
		const char*
		#ifndef __cplusplus
			const
		#endif
		restrict sixty_four_bit_register_numbers_to_strings[16] =
			{"rax", "rcx", "rdx", "rbx", "rsp", "rbp", "rsi", "rdi", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"};
	#endif
	
	const char*
	#ifndef __cplusplus
		const
	#endif
	restrict thirty_two_bit_register_numbers_to_strings[
	#if SIXTY_FOUR_BIT
		16
	#else
		8
	#endif
	] = {
		"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi",
		#if SIXTY_FOUR_BIT
			"r8d", "r9d", "r10d", "r11d", "r12d", "r13d", "r14d", "r15d"
		#endif
	};
#endif

/* ----------------------------------------------------------- Length-getting functions -------------------------------------------------------- */

#if SIXTY_FOUR_BIT
	u dac_get_length_of_mov_reg64_reg64(void) {
		return 3;
	}
	
	u dac_get_length_of_xor_reg64_reg64(void) {
		return 3;
	}
	
	u dac_get_length_of_add_reg64_reg64(void) {
		return 3;
	}
	
	u dac_get_length_of_sub_reg64_reg64(void) {
		return 3;
	}
	
	u dac_get_length_of_mov_reg64_deref_register_as_qword(const Register64 register2) {
		return 3 + (register2 == REGISTER_RSP || register2 == REGISTER_RBP || register2 == REGISTER_R12 || register2 == REGISTER_R13);
	}
	
	attr_const u dac_get_length_of_pusha_except_stack_pointer(void) {
		small_num_type register_ = REGISTER_R15 + 1;
		u total = 0;
		
		while (register_--) {
			if (register_ != REGISTER_RSP) {
				total += dac_get_length_of_push((Register64)register_);
			}
		}
		
		return total;
	}
	
	attr_const u dac_get_length_of_popa_except_stack_pointer(void) {
		small_num_type register_ = REGISTER_R15 + 1;
		u total = 0;
		
		while (register_--) {
			if (register_ != REGISTER_RSP) {
				total += dac_get_length_of_pop((Register64)register_);
			}
		}
		
		return total;
	}
	
	u dac_get_length_of_mov_reg64_lit64(const Register64 register_, const s amount) {
		s32 amount32;
		
		if (!amount) {
			return dac_get_length_of_xor_reg64_reg64();
		}
		
		amount32 = amount;
		
		if (amount32 == amount) {
			return 5 + (register_ >= REGISTER_R8);
		}
		
		return 10;
	}
	
	attr_const static u get_length_of_fpu_deref_register_as_qword_instruction(const Register64 register_) {
		return 2 + (register_ >= REGISTER_R8) + (register_ % 8 == 4 || register_ % 8 == 5);
	}
	
	u dac_get_length_of_fld_deref_register_as_qword(const Register64 register_) {
		return get_length_of_fpu_deref_register_as_qword_instruction(register_);
	}
	
	u dac_get_length_of_fmul_deref_register_as_qword(const Register64 register_) {
		return get_length_of_fpu_deref_register_as_qword_instruction(register_);
	}
	
	u dac_get_length_of_fadd_deref_register_as_qword(const Register64 register_) {
		return get_length_of_fpu_deref_register_as_qword_instruction(register_);
	}
	
	u dac_get_length_of_fsub_deref_register_as_qword(const Register64 register_) {
		return get_length_of_fpu_deref_register_as_qword_instruction(register_);
	}
	
	u dac_get_length_of_fstp_deref_register_as_qword(const Register64 register_) {
		return get_length_of_fpu_deref_register_as_qword_instruction(register_);
	}
#else
	u dac_get_length_of_pusha_except_stack_pointer(void) {
		small_num_type register_ = REGISTER_EDI + 1;
		u total = 0;
		
		while (register_--) {
			if (register_ != REGISTER_ESP) {
				total += dac_get_length_of_push();
			}
		}
		
		return total;
	}
	
	u dac_get_length_of_popa_except_stack_pointer(void) {
		small_num_type register_ = REGISTER_EDI + 1;
		u total = 0;
		
		while (register_--) {
			if (register_ != REGISTER_ESP) {
				total += dac_get_length_of_pop();
			}
		}
		
		return total;
	}
	
	attr_const static u get_length_of_fpu_deref_register_as_qword_instruction(const Register32 register_) {
		return 2 + (register_ == REGISTER_ESP || register_ == REGISTER_EBP);
	}
	
	u dac_get_length_of_fld_deref_register_as_qword(const Register32 register_) {
		return get_length_of_fpu_deref_register_as_qword_instruction(register_);
	}
	
	u dac_get_length_of_fmul_deref_register_as_qword(const Register32 register_) {
		return get_length_of_fpu_deref_register_as_qword_instruction(register_);
	}
	
	u dac_get_length_of_fadd_deref_register_as_qword(const Register32 register_) {
		return get_length_of_fpu_deref_register_as_qword_instruction(register_);
	}
	
	u dac_get_length_of_fsub_deref_register_as_qword(const Register32 register_) {
		return get_length_of_fpu_deref_register_as_qword_instruction(register_);
	}
	
	u dac_get_length_of_fstp_deref_register_as_qword(const Register32 register_) {
		return get_length_of_fpu_deref_register_as_qword_instruction(register_);
	}
#endif

u dac_get_length_of_fincstp(void) {
	return 2;
}

u dac_get_length_of_fld1(void) {
	return 2;
}

u dac_get_length_of_fldz(void) {
	return 2;
}

u dac_get_length_of_mov_reg32_reg32(void) {
	return 2;
}

u dac_get_length_of_xor_reg32_reg32(void) {
	return 2;
}

u dac_get_length_of_add_reg32_reg32(void) {
	return 2;
}

u dac_get_length_of_sub_reg32_reg32(void) {
	return 2;
}

u dac_get_length_of_mov_reg32_lit32(const u number) {
	if (!number) return dac_get_length_of_xor_reg32_reg32();
	return 5;
}

/* ------------------------------------- The x86 instruction-generation functions (the assembler functions) ------------------------------------ */

#if SIXTY_FOUR_BIT
	__attribute__((nonnull (1)))
	void dac_reg64_reg64(
		DataAndCode* cr dac,
		const Register64 register1,
		const Register64 register2,
		const small_num_type instruction_number,
		const small_num_type base
		#if DEBUG
			,
			const char* cr instruction_string1,
			const char* cr instruction_string2,
			const char* cr instruction_string3,
			const char* cr instruction_string4
		#endif
	);
	
	void dac_append_mov_reg64_reg64(DataAndCode* cr dac, const Register64 register1, const Register64 register2) {
		assert_comparison(dac, !=, NULL, DataAndCode*, void*);
		
		dac_reg64_reg64(dac, register1, register2, 137, 192
		#if DEBUG
		, "mov ", "", "", ""
		#endif
		);
	}
	
	void dac_append_xor_reg64_reg64(DataAndCode* cr dac, const Register64 register1, const Register64 register2) {
		assert_comparison(dac, !=, NULL, DataAndCode*, void*);
		
		dac_reg64_reg64(dac, register1, register2, 49, 192
		#if DEBUG
		, "xor ", "", "", ""
		#endif
		);
	}
	
	void dac_append_add_reg64_reg64(DataAndCode* cr dac, const Register64 register1, const Register64 register2) {
		assert_comparison(dac, !=, NULL, DataAndCode*, void*);
		
		dac_reg64_reg64(dac, register1, register2, 1, 192
		#if DEBUG
		, "add ", "", "", ""
		#endif
		);
	}
	
	void dac_append_sub_reg64_reg64(DataAndCode* cr dac, const Register64 register1, const Register64 register2) {
		assert_comparison(dac, !=, NULL, DataAndCode*, void*);
		
		dac_reg64_reg64(dac, register1, register2, 41, 192
		#if DEBUG
		, "sub ", "", "", ""
		#endif
		);
	}
	
	/*
	This function, which generates the instruction
	
	mov reg64, qword[reg64]
	
	is basically equivalent to dac_append_mov_deref_register_as_qword_reg64(), which generates the instruction
	
	mov qword[reg64], reg64
	
	except the instruction number is 139 instead of 137 and all the registers are backwards.
	
	So
	
	foo(register1, register2)
	
	in dac_append_mov_deref_register_as_qword_reg64() would be
	
	foo(register2, register1)
	
	in this function.
	*/
	void dac_append_mov_reg64_deref_register_as_qword(DataAndCode* cr dac, const Register64 register1, const Register64 register2) {
		assert_comparison(dac, !=, NULL, DataAndCode*, void*);
		assert_comparison(register1, <=, REGISTER_R15, Register64, Register64);
		assert_comparison(register2, <=, REGISTER_R15, Register64, Register64);
		
		#if DEBUG
			output_nullt_string(TAB_STRING TAB_STRING);
			output_nullt_string("mov ");
			output_nullt_string(sixty_four_bit_register_numbers_to_strings[register1]);
			output_nullt_string(", qword[");
			output_nullt_string(sixty_four_bit_register_numbers_to_strings[register2]);
			output_nullt_string("]\n");
		#endif
		
		if (register2 == REGISTER_RBP || register2 == REGISTER_R13) {
			/*
			REGISTER_RAX is being given as an argument to this function
			because REGISTER_RAX == 0, and it needs to be zero to generate
			the right instruction.
			*/
			dac_reg64_reg64(dac, REGISTER_RAX, register1, 139, 69
			#if DEBUG
			, NULL, NULL, NULL, NULL
			#endif
			);
			
			dac->start_of_code[dac->current_instruction_offset++] = 0;
			return;
		}
		
		dac_reg64_reg64(dac, register2, register1, 139, 0
		#if DEBUG
			, NULL, NULL, NULL, NULL
		#endif
		);
		
		if (register2 == REGISTER_RSP || register2 == REGISTER_R12) {
			dac->start_of_code[dac->current_instruction_offset++] = 36;
		}
	}
	
	void dac_append_pusha_except_stack_pointer(DataAndCode* cr dac) {
		small_num_type register_;
		
		assert_comparison(dac, !=, NULL, DataAndCode*, void*);
		
		#if DEBUG
			in_pusha_or_popa = true;
			output_nullt_string(TAB_STRING TAB_STRING);
			output_nullt_string("push ");
		#endif
		
		for (register_ = REGISTER_RAX; register_ <= REGISTER_R15; ++register_) {
			if (register_ != REGISTER_RSP) {
				#if DEBUG
					output_nullt_string(sixty_four_bit_register_numbers_to_strings[register_]);
					output_space();
				#endif
				dac_append_push(dac, (Register64)register_);
			}
		}
		
		#if DEBUG
			output_newline();
			in_pusha_or_popa = false;
		#endif
	}
	
	void dac_append_popa_except_stack_pointer(DataAndCode* cr dac) {
		small_num_type register_ = REGISTER_R15 + 1;
		
		assert_comparison(dac, !=, NULL, DataAndCode*, void*);
		
		#if DEBUG
			in_pusha_or_popa = true;
			output_nullt_string(TAB_STRING TAB_STRING);
			output_nullt_string("pop ");
		#endif
		
		while (register_--) {
			if (register_ != REGISTER_RSP) {
				#if DEBUG
					output_nullt_string(sixty_four_bit_register_numbers_to_strings[register_]);
					output_space();
				#endif
				dac_append_pop(dac, (Register64)register_);
			}
		}
		
		#if DEBUG
			output_newline();
			in_pusha_or_popa = false;
		#endif
	}
	
	void dac_append_mov_reg64_lit64(DataAndCode* cr dac, const Register64 register_, s amount) {
		s32 amount32;
		
		assert_comparison(dac, !=, NULL, DataAndCode*, void*);
		assert_comparison(register_, <=, REGISTER_R15, Register64, Register64);
		
		if (!amount) {
			/* xor reg64, reg64 optimization */
			dac_append_xor_reg64_reg64(dac, register_, register_);
			return;
		}
		
		amount32 = amount;
		
		if (amount32 == amount) {
			#if DEBUG
				output_nullt_string(TAB_STRING TAB_STRING);
				output_nullt_string("mov ");
				output_nullt_string(thirty_two_bit_register_numbers_to_strings[register_]);
				output_nullt_string(", ");
				putu_without_commas(amount);
				output_newline();
			#endif
			
			if (register_ >= REGISTER_R8) {
				dac->start_of_code[dac->current_instruction_offset++] = 65;
			}
			dac->start_of_code[dac->current_instruction_offset] = (uchar)184 + (register_ % 8);
			dac->start_of_code[dac->current_instruction_offset + 1] = ((uchar*)&amount32)[0];
			dac->start_of_code[dac->current_instruction_offset + 2] = ((uchar*)&amount32)[1];
			dac->start_of_code[dac->current_instruction_offset + 3] = ((uchar*)&amount32)[2];
			dac->start_of_code[dac->current_instruction_offset + 4] = ((uchar*)&amount32)[3];
			ARROW_ASSIGN(dac, current_instruction_offset) += 5;
		} else {
			dac_append_mov_reg64_lit64_ten_byte_encoding(dac, register_, amount);
		}
	}
	
	attr_nonnull static void
	dac_append_fpu_deref_register_as_qword_instruction(
		DataAndCode* cr dac,
		const Register64 register_,
		const uchar instruction_number,
		const uchar magic_number1,
		const uchar magic_number2,
		const uchar base
		#if DEBUG
			, const char* cr instruction_string
		#endif
	) {
		assert_comparison(dac, !=, NULL, DataAndCode*, void*);
		assert_comparison(register_, <=, REGISTER_R15, Register64, Register64);
		
		#if DEBUG
			output_nullt_string(TAB_STRING TAB_STRING);
			output_nullt_string(instruction_string);
			output_nullt_string(" qword[");
			output_nullt_string(sixty_four_bit_register_numbers_to_strings[register_]);
			output_nullt_string("]\n");
		#endif
		
		if (register_ >= REGISTER_R8) {
			dac->start_of_code[dac->current_instruction_offset++] = 65;
		}
		
		dac->start_of_code[dac->current_instruction_offset++] = instruction_number;
		
		if (register_ == REGISTER_RSP || register_ == REGISTER_R12) {
			dac->start_of_code[dac->current_instruction_offset] = magic_number1;
			dac->start_of_code[dac->current_instruction_offset + 1] = 36;
			dac->current_instruction_offset += 2;
		}
		else if (register_ == REGISTER_RBP || register_ == REGISTER_R13) {
			dac->start_of_code[dac->current_instruction_offset] = magic_number2;
			dac->start_of_code[dac->current_instruction_offset + 1] = 0;
			dac->current_instruction_offset += 2;
		} else {
			dac->start_of_code[dac->current_instruction_offset++] = base + (register_ % 8);
		}
	}
	
	/* fld qword[reg64] */
	void dac_append_fld_deref_register_as_qword(DataAndCode* cr dac, const Register64 register_) {
		assert_comparison(dac, !=, NULL, DataAndCode*, void*);
		assert_comparison(register_, <=, REGISTER_R15, Register64, Register64);
		
		dac_append_fpu_deref_register_as_qword_instruction(dac, register_, 221, 4, 69, 0
		#if DEBUG
			, "fld"
		#endif
		);
	}
	
	/* fmul qword[reg64] */
	void dac_append_fmul_deref_register_as_qword(DataAndCode* cr dac, const Register64 register_) {
		assert_comparison(dac, !=, NULL, DataAndCode*, void*);
		assert_comparison(register_, <=, REGISTER_R15, Register64, Register64);
		
		dac_append_fpu_deref_register_as_qword_instruction(dac, register_, 220, 12, 77, 8
		#if DEBUG
			, "fmul"
		#endif
		);
	}
	
	/* fadd qword[reg64] */
	void dac_append_fadd_deref_register_as_qword(DataAndCode* cr dac, const Register64 register_) {
		assert_comparison(dac, !=, NULL, DataAndCode*, void*);
		assert_comparison(register_, <=, REGISTER_R15, Register64, Register64);
		
		dac_append_fpu_deref_register_as_qword_instruction(dac, register_, 220, 4, 69, 0
		#if DEBUG
			, "fadd"
		#endif
		);
	}
	
	/* fsub qword[reg64] */
	void dac_append_fsub_deref_register_as_qword(DataAndCode* cr dac, const Register64 register_) {
		assert_comparison(dac, !=, NULL, DataAndCode*, void*);
		assert_comparison(register_, <=, REGISTER_R15, Register64, Register64);
		
		dac_append_fpu_deref_register_as_qword_instruction(dac, register_, 220, 36, 101, 32
		#if DEBUG
			, "fsub"
		#endif
		);
	}
	
	/* fstp qword[reg64] */
	void dac_append_fstp_deref_register_as_qword(DataAndCode* cr dac, const Register64 register_) {
		assert_comparison(dac, !=, NULL, DataAndCode*, void*);
		assert_comparison(register_, <=, REGISTER_R15, Register64, Register64);
		
		dac_append_fpu_deref_register_as_qword_instruction(dac, register_, 221, 28, 93, 24
		#if DEBUG
			, "fstp"
		#endif
		);
	}
#else
	void dac_append_pusha_except_stack_pointer(DataAndCode* cr dac) {
		small_num_type register_;
		
		assert_comparison(dac, !=, NULL, DataAndCode*, void*);
		
		#if DEBUG
			in_pusha_or_popa = true;
			output_nullt_string(TAB_STRING TAB_STRING);
			output_nullt_string("push ");
		#endif
		
		for (register_ = REGISTER_EAX; register_ <= REGISTER_EDI; ++register_) {
			if (register_ != REGISTER_ESP) {
				#if DEBUG
					output_nullt_string(thirty_two_bit_register_numbers_to_strings[register_]);
					output_space();
				#endif
				dac_append_push(dac, (Register32)register_);
			}
		}
		
		#if DEBUG
			output_newline();
			in_pusha_or_popa = false;
		#endif
	}
	
	void dac_append_popa_except_stack_pointer(DataAndCode* cr dac) {
		small_num_type register_ = REGISTER_EDI + 1;
		
		assert_comparison(dac, !=, NULL, DataAndCode*, void*);
		
		#if DEBUG
			in_pusha_or_popa = true;
			output_nullt_string(TAB_STRING TAB_STRING);
			output_nullt_string("pop ");
		#endif
		
		while (register_--) {
			if (register_ != REGISTER_ESP) {
				#if DEBUG
					output_nullt_string(thirty_two_bit_register_numbers_to_strings[register_]);
					output_space();
				#endif
				dac_append_pop(dac, (Register32)register_);
			}
		}
		
		#if DEBUG
			output_newline();
			in_pusha_or_popa = false;
		#endif
	}
	
	attr_nonnull static void
	dac_append_fpu_deref_register_as_dword_instruction(
		DataAndCode* cr dac,
		const Register32 register_,
		const uchar instruction_number,
		const uchar magic_number1,
		const uchar magic_number2,
		const uchar base
		#if DEBUG
			, const char* cr instruction_string
		#endif
	) {
		assert_comparison(dac, !=, NULL, DataAndCode*, void*);
		assert_comparison(register_, <=, REGISTER_EDI, Register32, Register32);
		
		#if DEBUG
			output_nullt_string(TAB_STRING TAB_STRING);
			output_nullt_string(instruction_string);
			output_nullt_string(" qword[");
			output_nullt_string(thirty_two_bit_register_numbers_to_strings[register_]);
			output_nullt_string("]\n");
		#endif
		
		dac->start_of_code[dac->current_instruction_offset++] = instruction_number;
		
		if (register_ == REGISTER_ESP) {
			dac->start_of_code[dac->current_instruction_offset] = magic_number1;
			dac->start_of_code[dac->current_instruction_offset + 1] = 36;
			dac->current_instruction_offset += 2;
		} else if (register_ == REGISTER_EBP) {
			dac->start_of_code[dac->current_instruction_offset] = magic_number2;
			dac->start_of_code[dac->current_instruction_offset + 1] = 0;
			dac->current_instruction_offset += 2;
		} else {
			dac->start_of_code[dac->current_instruction_offset++] = base + register_;
		}
	}
	
	/* fld qword[reg32] */
	void dac_append_fld_deref_register_as_qword(DataAndCode* cr dac, const Register32 register_) {
		assert_comparison(dac, !=, NULL, DataAndCode*, void*);
		assert_comparison(register_, <=, REGISTER_EDI, Register32, Register32);
		
		dac_append_fpu_deref_register_as_dword_instruction(dac, register_, 217, 4, 69, 0
		#if DEBUG
			, "fld"
		#endif
		);
	}
	
	/* fmul qword[reg32] */
	void dac_append_fmul_deref_register_as_qword(DataAndCode* cr dac, const Register32 register_) {
		assert_comparison(dac, !=, NULL, DataAndCode*, void*);
		assert_comparison(register_, <=, REGISTER_EDI, Register32, Register32);
		
		dac_append_fpu_deref_register_as_dword_instruction(dac, register_, 216, 12, 77, 8
		#if DEBUG
			, "fmul"
		#endif
		);
	}
	
	/* fadd dword[reg32] */
	void dac_append_fadd_deref_register_as_qword(DataAndCode* cr dac, const Register32 register_) {
		assert_comparison(dac, !=, NULL, DataAndCode*, void*);
		assert_comparison(register_, <=, REGISTER_EDI, Register32, Register32);
		
		dac_append_fpu_deref_register_as_dword_instruction(dac, register_, 216, 4, 69, 0
		#if DEBUG
			, "fadd"
		#endif
		);
	}
	
	/* fsub dword[reg32] */
	void dac_append_fsub_deref_register_as_qword(DataAndCode* cr dac, const Register32 register_) {
		assert_comparison(dac, !=, NULL, DataAndCode*, void*);
		assert_comparison(register_, <=, REGISTER_EDI, Register32, Register32);
		
		dac_append_fpu_deref_register_as_dword_instruction(dac, register_, 216, 36, 101, 32
		#if DEBUG
			, "fsub"
		#endif
		);
	}
	
	/* fstp dword[reg32] */
	void dac_append_fstp_deref_register_as_qword(DataAndCode* cr dac, const Register32 register_) {
		assert_comparison(dac, !=, NULL, DataAndCode*, void*);
		assert_comparison(register_, <=, REGISTER_EDI, Register32, Register32);
		
		dac_append_fpu_deref_register_as_dword_instruction(dac, register_, 217, 28, 93, 24
		#if DEBUG
			, "fstp"
		#endif
		);
	}
#endif

void dac_append_fincstp(DataAndCode* cr dac) {
	#if DEBUG
		output_nullt_string(TAB_STRING TAB_STRING "fincstp\n");
	#endif
	
	dac->start_of_code[dac->current_instruction_offset] = (uchar)217;
	dac->start_of_code[dac->current_instruction_offset + 1] = (uchar)247;
	dac->current_instruction_offset += 2;
}

void dac_append_fld1(DataAndCode* cr dac) {
	assert_comparison(dac, !=, NULL, DataAndCode*, void*);
	
	#if DEBUG
		output_nullt_string(TAB_STRING TAB_STRING "fld1\n");
	#endif
	
	dac->start_of_code[dac->current_instruction_offset] = (uchar)217;
	dac->start_of_code[dac->current_instruction_offset + 1] = (uchar)232;
	dac->current_instruction_offset += 2;
}

void dac_append_fldz(DataAndCode* cr dac) {
	assert_comparison(dac, !=, NULL, DataAndCode*, void*);
	
	#if DEBUG
		output_nullt_string(TAB_STRING TAB_STRING "fldz\n");
	#endif
	
	dac->start_of_code[dac->current_instruction_offset] = (uchar)217;
	dac->start_of_code[dac->current_instruction_offset + 1] = (uchar)238;
	dac->current_instruction_offset += 2;
}

attr_nonnull void dac_reg32_reg32(
	DataAndCode* cr dac,
	const Register32 register1,
	const Register32 register2,
	const small_num_type instruction_number,
	const small_num_type base
	#if DEBUG
		,
		const char* cr instruction_string1,
		const char* cr instruction_string2,
		const char* cr instruction_string3,
		const char* cr instruction_string4
	#endif
);

void dac_append_mov_reg32_reg32(DataAndCode* cr dac, const Register32 register1, const Register32 register2) {
	assert_comparison(dac, !=, NULL, DataAndCode*, void*);
	
	dac_reg32_reg32(dac, register1, register2, 137, 192
	#if DEBUG
		, "mov ", "", "", ""
	#endif
	);
}

void dac_append_xor_reg32_reg32(DataAndCode* cr dac, const Register32 register1, const Register32 register2) {
	assert_comparison(dac, !=, NULL, DataAndCode*, void*);
	
	dac_reg32_reg32(dac, register1, register2, 49, 192
	#if DEBUG
		, "xor ", "", "", ""
	#endif
	);
}

void dac_append_add_reg32_reg32(DataAndCode* cr dac, const Register32 register1, const Register32 register2) {
	assert_comparison(dac, !=, NULL, DataAndCode*, void*);
	
	dac_reg32_reg32(dac, register1, register2, 1, 192
	#if DEBUG
		, "add ", "", "", ""
	#endif
	);
}

void dac_append_sub_reg32_reg32(DataAndCode* cr dac, const Register32 register1, const Register32 register2) {
	assert_comparison(dac, !=, NULL, DataAndCode*, void*);
	
	dac_reg32_reg32(dac, register1, register2, 41, 192
	#if DEBUG
		, "sub ", "", "", ""
	#endif
	);
}

void dac_append_mov_reg32_lit32(DataAndCode* cr dac, const Register32 register_, u number) {
	assert_comparison(dac, !=, NULL, DataAndCode*, void*);
	assert_comparison(register_, <=, REGISTER_EDI, Register32, Register32);
	
	if (!number) {
		/* xor reg32, reg32 optimization */
		dac_append_xor_reg32_reg32(dac, register_, register_);
		return;
	}
	
	#if DEBUG
		output_nullt_string(TAB_STRING TAB_STRING);
		output_nullt_string("mov ");
		output_nullt_string(thirty_two_bit_register_numbers_to_strings[register_]);
		output_nullt_string(", ");
		putu_without_commas(number);
		output_newline();
	#endif
	
	dac->start_of_code[dac->current_instruction_offset] = (uchar)184 + register_;
	dac->start_of_code[dac->current_instruction_offset + 1] = ((uchar*)&number)[0];
	dac->start_of_code[dac->current_instruction_offset + 2] = ((uchar*)&number)[1];
	dac->start_of_code[dac->current_instruction_offset + 3] = ((uchar*)&number)[2];
	dac->start_of_code[dac->current_instruction_offset + 4] = ((uchar*)&number)[3];
	ARROW_ASSIGN(dac, current_instruction_offset) += 5;
}
