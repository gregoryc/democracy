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
	static void output_short_branch_instruction(const schar displacement, const char* cr string) {
		output_nullt_string(TAB_STRING TAB_STRING "; displacement is ");
		
		{
			uchar displacement_ = displacement;
			
			if (displacement_ > 127) {
				output_space();
				displacement_ = -displacement_;
			}
			
			/*
			This is a minor optimization.
			putu() can be used instead, but putu_without_commas() is slightly faster.
			*/
			putu_without_commas(displacement_);
		}
		
		output_nullt_string("\n" TAB_STRING TAB_STRING);
		output_nullt_string(string);
		
		output_nullt_string(" short $");
		
		if (displacement + 2) {
			uchar displacement_;
			
			if (displacement + 2 > 0) {
				output_nullt_string(" + ");
			} else {
				output_nullt_string(" - ");
			}
			
			displacement_ = displacement + 2;
			if (displacement_ > 127) {
				displacement_ = -displacement_;
			}
			
			/*
			This is a minor optimization.
			putu() can be used instead, but putu_without_commas() is slightly faster.
			*/
			putu_without_commas(displacement_);
		}
		
		output_newline();
	}
#endif
