/*
A header file with macros for safe dereferencing.

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

#if DEBUG && __GNUC__
	attr_noreturn static inline void* __null_pointer_deref_error_message(
		const int line,
		const char* cr file,
		const char* cr function
	) {
		output_nullt_string("\nError in file ");
		output_nullt_string(file);
		output_nullt_string(", in ");
		output_nullt_string(function);
		output_nullt_string("(), on line ");
		putu(line);
		output_nullt_string(": dereferenced NULL pointer\n");
		exit_program();
	}
	
	#define DEREF(x) (((x) ? (*(x)) : (__null_pointer_deref_error_message(__LINE__, __FILE__, __func__), *((__typeof__(x))NULL))))
	#define DEREF_ASSIGN(x) (void)((!(x)) && __null_pointer_deref_error_message(__LINE__, __FILE__, __func__)), (*x)
	#define ARRAY_INDEX(x, i) DEREF((x) + (i))
	
	/* i is guaranteed to be evaluated only once */
	#define ARRAY_INDEX_ASSIGN(x, i) (void)((!(x)) && __null_pointer_deref_error_message(__LINE__, __FILE__, __func__)), (x)[i]
	#define ARROW(x, y) DEREF(x).y
	
	/* y is guaranteed to be evaluated only once */
	#define ARROW_ASSIGN(x, y) DEREF_ASSIGN(x).y
#else
	#define DEREF(x) (*(x))
	#define DEREF_ASSIGN(x) (*(x))
	#define ARRAY_INDEX(x, i) (x)[i]
	#define ARRAY_INDEX_ASSIGN(x, i) (x)[i]
	#define ARROW(x, y) (x)->y
	#define ARROW_ASSIGN(x, y) (x)->y
#endif

#define ARROW3(a, b, c) ARROW(ARROW(a, b), c)
#define ARROW4(a, b, c, d) ARROW(ARROW(ARROW(a, b), c), d)
