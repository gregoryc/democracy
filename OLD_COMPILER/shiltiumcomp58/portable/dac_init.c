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

#include "../global_headers/headers.h"

void dac_init(DataAndCode* cr dac) {
	assert_comparison(dac, !=, NULL, DataAndCode*, void*);
	
	ARROW_ASSIGN(dac, start_of_code) = NULL;
	ARROW_ASSIGN(dac, current_instruction_offset) = 0;
	
	hash_table_init(&dac->string_literals, 25, 7, 3.0);
	hash_table_init(&dac->float_literals, 60, 7, 3.0);
	
	ARROW_ASSIGN(dac, start_of_float_data) = NULL;
	ARROW_ASSIGN(dac, float_data_real_length) = 0;
	ARROW_ASSIGN(dac, float_data_allocated_length) = 0;
	
	ARROW_ASSIGN(dac, start_of_string_data) = 0;
	ARROW_ASSIGN(dac, string_data_real_length) = 0;
	ARROW_ASSIGN(dac, float_data_allocated_length) = 0;
}
