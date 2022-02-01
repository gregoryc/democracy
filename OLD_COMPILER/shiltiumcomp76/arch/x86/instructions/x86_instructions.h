/*
A header file for x86_instructions.c

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

#if SIXTY_FOUR_BIT
	typedef enum {
		REGISTER_RAX, REGISTER_RCX, REGISTER_RDX, REGISTER_RBX, REGISTER_RSP, REGISTER_RBP,
		REGISTER_RSI, REGISTER_RDI, REGISTER_R8, REGISTER_R9, REGISTER_R10, REGISTER_R11,
		REGISTER_R12, REGISTER_R13, REGISTER_R14, REGISTER_R15
	} Register64;
#endif

typedef enum {
	REGISTER_EAX, REGISTER_ECX, REGISTER_EDX, REGISTER_EBX,
	REGISTER_ESP, REGISTER_EBP, REGISTER_ESI, REGISTER_EDI
	
	#if SIXTY_FOUR_BIT
		,
		REGISTER_R8D, REGISTER_R9D, REGISTER_R10D, REGISTER_R11D,
		REGISTER_R12D, REGISTER_R13D, REGISTER_R14D, REGISTER_R15d
	#endif
} Register32;

typedef enum {
	REGISTER_ST0, REGISTER_ST1, REGISTER_ST2, REGISTER_ST3,
	REGISTER_ST4, REGISTER_ST5, REGISTER_ST6, REGISTER_ST7
} FPU_Register;

u dac_get_length_of_pusha(void) attrs_const_and_flatten;
u dac_get_length_of_popa(void) attrs_const_and_flatten;
u dac_get_length_of_mov_reg32_reg32(void) attr_const;
u dac_get_length_of_xor_reg32_reg32(void) attr_const;
u dac_get_length_of_add_reg32_reg32(void) attr_const;
u dac_get_length_of_sub_reg32_reg32(void) attr_const;

#if SIXTY_FOUR_BIT
	u dac_get_length_of_push(const Register64 register_) attrs_const_and_flatten;
	u dac_get_length_of_pop(const Register64 register_) attrs_const_and_flatten;
	void dac_append_push(DataAndCode* cr dac, const Register64 register_) attrs_nonnull_and_flatten;
	void dac_append_pop(DataAndCode* cr dac, const Register64 register_) attrs_nonnull_and_flatten;
#else
	u dac_get_length_of_push(void) attrs_const_and_flatten;
	u dac_get_length_of_pop(void) attrs_const_and_flatten;
	void dac_append_push(DataAndCode* cr dac, const Register32 register_) attrs_nonnull_and_flatten;
	void dac_append_pop(DataAndCode* cr dac, const Register32 register_) attrs_nonnull_and_flatten;
#endif

u dac_get_length_of_ret(void) attr_const;
void dac_append_ret(DataAndCode* cr dac) attr_nonnull;

void dac_append_mov_reg32_reg32(DataAndCode* cr dac, const Register32 register1, const Register32 register2);
void dac_append_xor_reg32_reg32(DataAndCode* cr dac, const Register32 register1, const Register32 register2);
void dac_append_add_reg32_reg32(DataAndCode* cr dac, const Register32 register1, const Register32 register2);
void dac_append_sub_reg32_reg32(DataAndCode* cr dac, const Register32 register1, const Register32 register2);

#if DEBUG
	void dac_append_int3(DataAndCode* cr dac) attr_nonnull;
#endif

#if SIXTY_FOUR_BIT
	u dac_get_length_of_fld_deref_register_as_qword(const Register64 register_) attrs_const_and_flatten;
	u dac_get_length_of_fmul_deref_register_as_qword(const Register64 register_) attrs_const_and_flatten;
	u dac_get_length_of_fadd_deref_register_as_qword(const Register64 register_) attrs_const_and_flatten;
	u dac_get_length_of_fsub_deref_register_as_qword(const Register64 register_) attrs_const_and_flatten;
	u dac_get_length_of_fstp_deref_register_as_qword(const Register64 register_) attrs_const_and_flatten;
	
	void dac_append_fld_deref_register_as_qword(DataAndCode* cr dac, const Register64 register_) attrs_nonnull_and_flatten;
	void dac_append_fmul_deref_register_as_qword(DataAndCode* cr dac, const Register64 register_) attrs_nonnull_and_flatten;
	void dac_append_fadd_deref_register_as_qword(DataAndCode* cr dac, const Register64 register_) attrs_nonnull_and_flatten;
	void dac_append_fsub_deref_register_as_qword(DataAndCode* cr dac, const Register64 register_) attrs_nonnull_and_flatten;
	void dac_append_fstp_deref_register_as_qword(DataAndCode* cr dac, const Register64 register_) attrs_nonnull_and_flatten;
#else
	u dac_get_length_of_fld_deref_register_as_qword(const Register32 register_) attrs_const_and_flatten;
	u dac_get_length_of_fmul_deref_register_as_qword(const Register32 register_) attrs_const_and_flatten;
	u dac_get_length_of_fadd_deref_register_as_qword(const Register32 register_) attrs_const_and_flatten;
	u dac_get_length_of_fsub_deref_register_as_qword(const Register32 register_) attrs_const_and_flatten;
	u dac_get_length_of_fstp_deref_register_as_qword(const Register32 register_) attrs_const_and_flatten;
	
	void dac_append_fld_deref_register_as_qword(DataAndCode* cr dac, const Register32 register_) attrs_nonnull_and_flatten;
	void dac_append_fmul_deref_register_as_qword(DataAndCode* cr dac, const Register32 register_) attrs_nonnull_and_flatten;
	void dac_append_fadd_deref_register_as_qword(DataAndCode* cr dac, const Register32 register_) attrs_nonnull_and_flatten;
	void dac_append_fsub_deref_register_as_qword(DataAndCode* cr dac, const Register32 register_) attrs_nonnull_and_flatten;
	void dac_append_fstp_deref_register_as_qword(DataAndCode* cr dac, const Register32 register_) attrs_nonnull_and_flatten;
#endif

u dac_get_length_of_pusha_except_stack_pointer(void) attr_const;
void dac_append_pusha_except_stack_pointer(DataAndCode* cr dac) attr_nonnull;

u dac_get_length_of_popa_except_stack_pointer(void) attr_const;
void dac_append_popa_except_stack_pointer(DataAndCode* cr dac) attr_nonnull;

#if SIXTY_FOUR_BIT
	u dac_get_length_of_add_reg64_lit32(const Register64 register_, const s32 amount) attrs_const_and_flatten;
	u dac_get_length_of_sub_reg64_lit32(const Register64 register_, const s32 amount) attrs_const_and_flatten;
	u dac_get_length_of_mov_reg64_reg64(void) attr_const;
	u dac_get_length_of_xor_reg64_reg64(void) attr_const;
	u dac_get_length_of_add_reg64_reg64(void) attr_const;
	u dac_get_length_of_sub_reg64_reg64(void) attr_const;
	u dac_get_length_of_mov_deref_register_as_qword_reg64(const Register64 register1) attr_const;
	u dac_get_length_of_mov_reg64_deref_register_as_qword(const Register64 register2) attr_const;
	
	u dac_get_length_of_mov_reg64_lit64(const Register64 register_, const s amount) attrs_const_and_flatten;
	
	void dac_append_mov_reg64_reg64(DataAndCode* cr dac, const Register64 register1, const Register64 register2) attr_nonnull;
	void dac_append_xor_reg64_reg64(DataAndCode* cr dac, const Register64 register1, const Register64 register2) attr_nonnull;
	void dac_append_add_reg64_reg64(DataAndCode* cr dac, const Register64 register1, const Register64 register2) attr_nonnull;
	void dac_append_sub_reg64_reg64(DataAndCode* cr dac, const Register64 register1, const Register64 register2) attr_nonnull;
	
	void dac_append_mov_deref_register_as_qword_reg64(
		DataAndCode* cr dac,
		const Register64 register1,
		const Register64 register2
	) attr_nonnull;
	
	void dac_append_mov_reg64_deref_register_as_qword(
		DataAndCode* cr dac,
		const Register64 register1,
		const Register64 register2
	) attr_nonnull;
	
	void dac_append_add_reg64_lit32(DataAndCode* cr dac, const Register64 register_, const s32 amount) attrs_nonnull_and_flatten;
	void dac_append_sub_reg64_lit32(DataAndCode* cr dac, const Register64 register_, const s32 amount) attrs_nonnull_and_flatten;
	void dac_append_mov_reg64_lit64(DataAndCode* cr dac, const Register64 register_, s amount) attr_nonnull;
#endif

u dac_get_length_of_fcomi(void) attr_const;
u dac_get_length_of_fcomip(void) attr_const;
u dac_get_length_of_fxch(void) attr_const;
u dac_get_length_of_fld1(void) attr_const;
u dac_get_length_of_fldz(void) attr_const;
u dac_get_length_of_ffree(void) attr_const;
u dac_get_length_of_fincstp(void) attr_const;

void dac_append_fcomi(DataAndCode* cr dac, const FPU_Register register_) attr_nonnull;
void dac_append_fcomip(DataAndCode* cr dac, const FPU_Register register_) attr_nonnull;
void dac_append_fxch(DataAndCode* cr dac, const FPU_Register register_) attr_nonnull;
void dac_append_ffree(DataAndCode* cr dac, const FPU_Register register_) attr_nonnull;
void dac_append_fincstp(DataAndCode* cr dac) attr_nonnull;
void dac_append_fld1(DataAndCode* cr dac) attr_nonnull;
void dac_append_fldz(DataAndCode* cr dac) attr_nonnull;

u dac_get_length_of_mov_reg32_lit32(const u number) attr_const;
void dac_append_mov_reg32_lit32(DataAndCode* cr dac, const Register32 register_, u number) attr_nonnull;

u dac_get_length_of_jmp_short(void) attr_const;
u dac_get_length_of_jb_short(void) attr_const;
u dac_get_length_of_ja_short(void) attr_const;
u dac_get_length_of_jae_short(void) attr_const;
u dac_get_length_of_jne_short(void) attr_const;

void dac_append_jmp_short(DataAndCode* cr dac, const schar displacement) attr_nonnull;
void dac_append_jb_short(DataAndCode* cr dac, const schar displacement) attr_nonnull;
void dac_append_ja_short(DataAndCode* cr dac, const schar displacement) attr_nonnull;
void dac_append_jae_short(DataAndCode* cr dac, const schar displacement) attr_nonnull;
void dac_append_jne_short(DataAndCode* cr dac, const schar displacement) attr_nonnull;

#if SIXTY_FOUR_BIT
	u dac_get_length_of_syscall(void) attr_const;
	void dac_append_syscall(DataAndCode* cr dac) attr_nonnull;
#endif

u dac_get_length_of_fchs(void) attr_const;
void dac_append_fchs(DataAndCode* cr dac) attr_nonnull;

u dac_get_length_of_int(void) attr_const;
void dac_append_int(DataAndCode* cr dac, const uchar interrupt_number) attr_nonnull;

u dac_get_length_of_mov_deref_register_as_dword_reg32(const Register32 register1) attr_const;
void dac_append_mov_deref_register_as_dword_reg32(DataAndCode* cr dac, const Register32 register1, const Register32 register2) attr_nonnull;

#if SIXTY_FOUR_BIT
	u dac_get_length_of_add_deref_register_as_qword_reg64(const Register64 register1) attr_const;
	void dac_append_add_deref_register_as_qword_reg64(DataAndCode* cr dac, const Register64 register1, const Register64 register2) attr_nonnull;
	
	u dac_get_length_of_mov_deref_register_as_qword_lit32(const Register64 register_) attr_const;
	void dac_append_mov_deref_register_as_qword_lit32(DataAndCode* cr dac, const Register64 register_, const u number) attr_nonnull;
	
	u dac_get_length_of_mov_deref_register_as_qword_plus_signed_byte_offset_lit32(const Register64 register_) attr_const;
	void dac_append_mov_deref_register_as_qword_plus_signed_byte_offset_lit32(
		DataAndCode* cr dac,
		const Register64 register_,
		const schar offset,
		const u number
	) attr_nonnull;
#endif

attr_const u dac_get_length_of_mov_deref_register_as_dword_lit32(const Register32 register_);
attr_nonnull void dac_append_mov_deref_register_as_dword_lit32(DataAndCode* cr dac, const Register32 register_, const u number);

u dac_get_length_of_mov_deref_register_as_dword_plus_signed_byte_offset_lit32(const Register32 register_) attr_const;
void dac_append_mov_deref_register_as_dword_plus_signed_byte_offset_lit32(
	DataAndCode* cr dac,
	const Register32 register_,
	const schar offset,
	const u number
) attr_nonnull;

u dac_get_length_of_add_reg32_lit32(const Register32 register_, const s32 amount) attr_const;
void dac_append_add_reg32_lit32(DataAndCode* cr dac, const Register32 register_, const s32 amount) attr_nonnull;

#if SIXTY_FOUR_BIT
	u dac_get_length_of_call_reg64(const Register64 register_) attr_const;
	void dac_append_call_reg64(DataAndCode* cr dac, const Register64 register_) attr_nonnull;
#else
	u dac_get_length_of_call_reg32(void) attr_const;
	void dac_append_call_reg32(DataAndCode* cr dac, const Register32 register_) attr_nonnull;
#endif

void dac_append_nop(DataAndCode* cr dac) attr_nonnull;

#if SIXTY_FOUR_BIT
	u dac_get_length_of_mov_reg64_lit64_ten_byte_encoding(void) attr_const;
	void dac_append_mov_reg64_lit64_ten_byte_encoding(DataAndCode* cr dac, const Register64 register_, const s amount) attr_nonnull;
#endif

u dac_get_length_of_faddp(void) attr_const;
void dac_append_faddp(DataAndCode* cr dac) attr_nonnull;

u dac_get_length_of_fsubp(void) attr_const;
void dac_append_fsubp(DataAndCode* cr dac) attr_nonnull;

#if SIXTY_FOUR_BIT
	u dac_get_length_of_jmp_reg(Register64 register_) attr_const;
	void dac_append_jmp_reg(DataAndCode* cr dac, const Register64 register_) attr_nonnull;
#else
	u dac_get_length_of_jmp_reg(void) attr_const;
	void dac_append_jmp_reg(DataAndCode* cr dac, const Register32 register_) attr_nonnull;
#endif

u dac_get_length_of_mov_reg32_deref_register_as_dword(const Register32 register2) attr_const;
void dac_append_mov_reg32_deref_register_as_dword(
	DataAndCode* cr dac,
	const Register32 register1,
	const Register32 register2
) attr_nonnull;

u dac_get_length_of_fstp_fpu_register(void) attr_const;
void dac_append_fstp_fpu_register(DataAndCode* cr dac, const FPU_Register register_) attr_nonnull;

u dac_get_length_of_fld_qword_address(void) attr_const;
void dac_append_fld_qword_address(DataAndCode* cr dac, const s rip_relative_address) attr_nonnull;

u dac_get_length_of_fadd_qword_address(void) attr_const;
void dac_append_fadd_qword_address(DataAndCode* cr dac, const s rip_relative_address) attr_nonnull;

#if SIXTY_FOUR_BIT
	u dac_get_length_of_fstp_deref_register_as_qword_plus_signed_byte_offset(const Register64 register_) attr_const;
	void dac_append_fstp_deref_register_as_qword_plus_signed_byte_offset(
		DataAndCode* cr dac,
		const Register64 register_,
		const char offset
	) attr_nonnull;
#endif

/* je.c */
u dac_get_length_of_je_short(void) attr_const;
void dac_append_je_short(DataAndCode* cr dac, const schar displacement) attr_nonnull;

u dac_get_length_of_je_near(void) attr_const;
void dac_append_je_near(DataAndCode* cr dac, const s32 displacement) attr_nonnull;

u dac_get_length_of_je(const s displacement) attr_const;
void dac_append_je(DataAndCode* cr dac, const s displacement) attr_nonnull;

/* jbe.c */
u dac_get_length_of_jbe_short(void) attr_const;
void dac_append_jbe_short(DataAndCode* cr dac, const schar displacement) attr_nonnull;

u dac_get_length_of_jbe_near(void) attr_const;
void dac_append_jbe_near(DataAndCode* cr dac, const s32 displacement) attr_nonnull;

u dac_get_length_of_jbe(const s displacement) attr_const;
void dac_append_jbe(DataAndCode* cr dac, const s displacement) attr_nonnull;
