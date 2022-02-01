#include "headers.h"

void mid_level_instructions_init(MidLevelInstructions* const restrict mli) {
	assert_comparison(mli, !=, NULL);
	
	mli->mid_level_instructions = (MidLevelInstruction**)m_alloc(sizeof(MidLevelInstruction*) * 10);
	mli->used_length = 0;
	mli->allocated_length = 10;
}

void mid_level_instructions_append(
	MidLevelInstructions* const restrict mli,
	MidLevelInstruction* const restrict instruction
) {
	assert_comparison(mli, !=, NULL);
	assert_comparison(instruction, !=, NULL);
	
	if (ARROW(mli, used_length) == ARROW(mli, allocated_length)) {
		ARROW_ASSIGN(mli, mid_level_instructions) =
			(MidLevelInstruction**)m_realloc(mli->mid_level_instructions, sizeof(MidLevelInstruction*) * (mli->allocated_length *= 2));
	}
	mli->mid_level_instructions[mli->used_length++] = instruction;
}

MidLevelInstruction* mid_level_instructions_get_last_instruction(
	MidLevelInstructions* const restrict mli
) {
	assert_comparison(mli, !=, NULL);
	assert_comparison(ARROW(mli, used_length), !=, 0);
	
	return ARRAY_INDEX(ARROW(mli, mid_level_instructions), ARROW(mli, used_length) - 1);
}

void mid_level_instructions_del(MidLevelInstructions* const restrict mli) {
	u64 i;
	
	assert_comparison(mli, !=, NULL);
	
	i = mli->used_length;
	
	while (i--) {
		if (mli->mid_level_instructions[i]->should_free_argument2) {
			m_free(mli->mid_level_instructions[i]->argument2);
		}
		m_free(mli->mid_level_instructions[i]);
	}
	m_free(mli->mid_level_instructions);
}
