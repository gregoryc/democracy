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

void get_next_char(ParserState* cr state, char utf8_char[7]) attr_nonnull;

attrs_const_and_nonnull
static inline uchar get_length_of_utf8_char(const char utf8_char[7]) {
	assert_comparison(utf8_char, !=, NULL, const char*, void*);
	
	return ARRAY_INDEX(utf8_char, 6);
}

/*
 *  Tests whether utf8_char is equal to ascii_char and returns
 *  a boolean value indicating whether or not it is equal.
 */
attrs_const_and_nonnull
static inline bool chreq_utf8_ascii(const char utf8_char[7], const char ascii_char) {
	assert_comparison(utf8_char, !=, NULL, const char*, void*);
	
	return get_length_of_utf8_char(utf8_char) == 1 && DEREF(utf8_char) == ascii_char;
}
