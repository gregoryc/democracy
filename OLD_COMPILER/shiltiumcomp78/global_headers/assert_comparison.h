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

#if DEBUG
		#define assert_comparison(operand1, comparison_operator, operand2, type1, type2) \
			{ \
			extern u number_of_assert_comparison_macro_calls; \
			++number_of_assert_comparison_macro_calls; \
			if (unlikely(!((operand1) comparison_operator (operand2)))) { \
					output_nullt_string( \
					"--------------------------------------------------------------------------\n" \
					PROGRAM_NAME " internal error in file " OPENING_QUOTE __FILE__ CLOSING_QUOTE ", on line "); \
				putu(__LINE__); \
				output_nullt_string(", in\n"); \
				output_nullt_string(__func__); \
				output_nullt_string("(): assertion\n    "); \
				output_nullt_string(#operand1 " " #comparison_operator " " #operand2); \
				output_nullt_string("\nfailed.\n"); \
				\
				if ( \
					(!strequal(#operand1, "NULL") && (strlen(#operand1) != 1 || #operand1[0] < '0' || #operand1[0] > '9')) || \
					(!strequal(#operand2, "NULL") && (strlen(#operand2) != 1 || #operand2[0] < '0' || #operand2[0] > '9')) \
				) { \
					output_nullt_string("--------------------------------------------------------------------------\n"); \
					if (!strequal(#operand1, "NULL") && (strlen(#operand1) != 1 || #operand1[0] < '0' || #operand1[0] > '9')) { \
						output_nullt_string("Value of " OPENING_QUOTE #operand1 CLOSING_QUOTE ": "); \
						putu((u)(type1)operand1); \
						output_char('\n'); \
					} \
					\
					if (!strequal(#operand2, "NULL") && (strlen(#operand2) != 1 || #operand2[0] < '0' || #operand2[0] > '9')) { \
						output_nullt_string("Value of " OPENING_QUOTE #operand2 CLOSING_QUOTE ": "); \
						putu((u)(type2)operand2); \
						output_char('\n'); \
					} \
					output_nullt_string("--------------------------------------------------------------------------\n"); \
				} \
				output_nullt_string("Try valgrinding this program to get a stack trace. This program is about to purposely " \
					"crash so you can get a stack trace in valgrind.\n" \
					"--------------------------------------------------------------------------\n" \
					"Goodbye\n" \
					"--------------------------------------------------------------------------\n"); \
				*(char*)0 = 0; \
				exit_program(); \
			} \
		}
#else
	#define assert_comparison(operand1, comparison_operator, operand2, type1, type2)
#endif
