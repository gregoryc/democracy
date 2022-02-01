#include "../../../global_headers/headers.h"
#include "x86_instructions.h"
#include "output_short_branch_instruction.h"

u dac_get_length_of_jae_short(void) {
	return 2;
}

void dac_append_jae_short(DataAndCode* cr dac, const schar displacement) {
	assert_comparison(dac, !=, NULL, DataAndCode*, void*);
	
	#if DEBUG
		output_short_branch_instruction(displacement, "jae");
	#endif
	
	ARRAY_INDEX_ASSIGN(ARROW(dac, start_of_code), ARROW(dac, current_instruction_offset)) = (uchar)115;
	ARRAY_INDEX_ASSIGN(ARROW(dac, start_of_code), ARROW(dac, current_instruction_offset + 1)) = displacement;
	ARROW_ASSIGN(dac, current_instruction_offset) += 2;
}
