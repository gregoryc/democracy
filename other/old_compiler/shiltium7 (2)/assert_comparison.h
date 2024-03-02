#if DEBUG
	#define assert_comparison(operand1, comparison_operator, operand2) \
		{ \
		extern u64 number_of_assert_comparison_macro_calls; \
		++number_of_assert_comparison_macro_calls; \
		if (unlikely(!((operand1) comparison_operator (operand2)))) { \
			volatile u64 x; \
			\
			output_nullt_string( \
				"--------------------------------------------------------------------------\n" \
				PROGRAM_NAME " internal error in file ‘" __FILE__ "’, on line "); \
			output_u64(__LINE__); \
			output_nullt_string(", in\n"); \
			output_nullt_string(__func__); \
			output_nullt_string("(): assertion\n    "); \
			output_nullt_string(#operand1 " " #comparison_operator " " #operand2); \
			output_nullt_string("\nfailed.\n"); \
			\
			if ( \
				(!strequal(#operand1, "NULL") && (str_len(#operand1) != 1 || #operand1[0] < '0' || #operand1[0] > '9')) || \
				(!strequal(#operand2, "NULL") && (str_len(#operand2) != 1 || #operand2[0] < '0' || #operand2[0] > '9')) \
			) { \
				output_nullt_string("--------------------------------------------------------------------------\n"); \
				if (!strequal(#operand1, "NULL") && (str_len(#operand1) != 1 || #operand1[0] < '0' || #operand1[0] > '9')) { \
					output_nullt_string("Value of ‘" #operand1 "’: "); \
					output_u64((u64)operand1); \
					output_char('\n'); \
				} \
				\
				if (!strequal(#operand2, "NULL") && (str_len(#operand2) != 1 || #operand2[0] < '0' || #operand2[0] > '9')) { \
					output_nullt_string("Value of ‘" #operand2 "’: "); \
					output_u64((u64)operand2); \
					output_char('\n'); \
				} \
				output_nullt_string("--------------------------------------------------------------------------\n"); \
			} \
			output_nullt_string("Try valgrinding this program to get a stack trace. This program is about to purposely " \
				"crash so you can get a stack trace in valgrind.\n"); \
			output_nullt_string("--------------------------------------------------------------------------\n"); \
			output_nullt_string("Goodbye\n"); \
			output_nullt_string("--------------------------------------------------------------------------\n"); \
			x = *(char*)0; \
			exit(1); \
		} \
		}
#else
	#define assert_comparison(operand1, comparison_operator, operand2)
#endif
