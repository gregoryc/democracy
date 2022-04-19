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
	extern bool in_pusha_or_popa;
	
	#if SIXTY_FOUR_BIT
		extern const char*
		#ifndef __cplusplus
			const
		#endif
		restrict sixty_four_bit_register_numbers_to_strings[16];
	#endif
	
	extern const char*
	#ifndef __cplusplus
		const
	#endif
	restrict thirty_two_bit_register_numbers_to_strings[
	#if SIXTY_FOUR_BIT
		16
	#else
		8
	#endif
	];
#endif
