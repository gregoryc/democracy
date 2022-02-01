#!/usr/bin/python
#
# A python script to create a large Shiltolang file.
#
# Copyright (C) 2010 Gregory Cohen <gregorycohen2@gmail.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

from sys import argv

characters = "abcdefghijklmnopqrstuvwxyz"
characters = characters

filename = argv[1]

with open(filename, "w") as f:
	for a in characters:
		for b in characters:
			for c in characters:
				string = a + b + c
				f.write("f " + string + " = 1.0;\n")
				f.write("output " + string + ";\n")
