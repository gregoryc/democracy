#!/usr/bin/python
#
# A python script to test Shiltiumcomp.
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

from os import chdir, system, remove
from sys import stderr, exit

source_files = (
	# Empty source file
	
	# Test 1
	"",
	
	# Test 2
	'''
	f x = 3.0;
	output x;
	''',
	
	# Test 3
	'''
	f x = 3.0;
	output x;
	f y = 2.0;
	f z = 4.0;
	output z;
	output y;
	''',
	
	# Test 4
	'''
	if 2.0 {
		f number = 3.0;
		output number;
	}
	''',
	
	# Test 5
	'''
	f number = 2.0;
	
	if 3.0 = 5.0 {
		f foobar = 5.0;
		output foobar;
	}
	
	output number;
	''',
	
	# Test 6
	'''
	f number = 2.0;
	
	if 3.0 = 5.0 {
		f foobar = 5.0;
		output foobar;
	}
	
	output number;
	f x = 5.0;
	output x;
	''',
	
	# Test 7
	'''
	f foo = 5.0;
	
	if 3.0 = 5.5 {
		f fooz = 4.0;
		output fooz;
	}
	
	f x = 3.0;
	f y = 3.0 * 4.0;
	
	output foo;
	output x;
	output y;
	''',
	
	# Test 8
	'''
	f foo = 5_323_423_231.0;
	
	if 32_343__43_2.0 = _5.5 {
		f fooz = 4_.0;
		output fooz;
	}
	
	output /* comment */ foo;
	
	f x = 3.0;
	output x;
	
	f y = 3.0 * 4.0;
	output y;
	''',
	
	# Test 9
	'''
	f foo = 53234__________________________________________23231.32;
	
	if 32_343__43_2.0 = _5.5 {
		f fooz = 4_.0;
		output fooz;
	}
	
	f x = 3.0;
	f y = 3.0 * 4.0;
	
	output x;
	output foo;
	output y;
	''',
	
	# Test 10
	'''
	f foo = 5323423231.32;
	
	output foo;
	
	if32_343__43_2.0 = _5.5 {
		f fooz = 4_.0;
		output fooz;
	}
	
	f x = 3.0;
	f y = 3.0 * 4.0;
	
	output y;
	output x;
	''',
	
	# Test 11
	'''
	f foo = 5323423231.32;
	
	output foo;
	
	if 32_343__43_2.0 = _5.5{
		f fooz = 4_.0;
		output fooz;
	}
	
	f x = 3.0;
	f y = 3.0 * 4.0;
	
	output x;
	output y;
	''',
	
	# Test 12
	'''
	f foo = 5323423231.32;
	
	if32_343__43_2.0 = _5.5{
		f fooz = 4_.0;
		output fooz;
	}
	
	f x = 3.0;
	output x;
	
	f y = 3.0 * 4.0;
	
	output y;
	output foo;
	''',
	
	# Test 13
	'''
	f  foo = 5323423231.32;
	
	 if   32_343__43_2.0   =  _5.5  	 	{
				f  fooz		 =		 4_.0;
   output 		fooz;
	  }
	
	f  x  =  3.0;
	f   y  =  3.0 			*  4.0;
	
	output y;
	output x;
	output foo;
	''',
	
	# Test 14
	'''
	f number$ = 3.0;
	output number$;
	''',
	
	# Test 15
	'''
	f number$ = 1.0;
	output number$;
	''',
	
	# Test 16
	'''
	f number$
		=  
			1.0;
	output number$;
	''',
	
	# Test 17
	'''
	f x2ds3f = 5.0;
	output x2ds3f;
	''',
	
	# Test 18
	'''
	f x3 = 4.0;
	output x3;
	''',
	
	# Test 19
	'''
	f x0 = 4.0e-233;
	f x1 = 4.00001e33;
	f x2 = 4.9865e1;
	f x3 = 4.12345e-1;
	f x4 = 4.33e0;
	f x5 = 0.5e+0;
	f x6 = 1.0e-0;
	f x7 = 2.0e+50;
	f x8 = 3.0e+13;
	f x9 = 4.3232e+13;
	f x10 = 4.3232e-5000;
	f x11 = 4.3232e+5000;
	f x12 = 4.3232e5000;
	
	output x0;  output x1;  output x2;  output x3;
	output x4;  output x5;  output x6;  output x7;
	output x8; output x9;  output x10;  output x11;
	output x12;
	''',
	
	# Test 20
	'''
	f x0 = 3.0:16;
	f x1 = ZZZ.0:36;
	f x2 = ZZZ.ZZYFZGYZZ:36;
	f x3 = ZZZ.ZZYFZGYZZe-2:36;
	f x4 = ZZZ.ZZYFZGYZZe+203:36;
	f x5 = AAA.0:11;
	f x6 = AAA.0e-304:11;
	f x7 = 037425.0e0:8;
	f x8 = 037425.0e-0:8;
	f x9 = 037425.0e+0:8;
	
	output x0;  output x1;  output x2;  output x3;
	output x4;  output x5;  output x6;  output x7;
	output x8; output x9;
	''',
	
	# Test 21
	'''
	f x0 = 3.0:16;
	f x1 = ZZZ.0:36;
	f x2 = ZZZ.ZZYFZGYZZ:36;
	f x3 = ZZZ.ZZYFZGYZZe-2:36;
	f x4 = ZZZ.ZZYFZGYZZe+203:36;
	f x5 = AAA.0:11;
	f x6 = AAA.0e-304:11;
	f x7 = 037425.0e0:8;
	f x8 = 037425.0e-0:8;
	f x9 = 037425.0e+0:8;
	
	output x0;  output x1;  output x2;  output x3;
	output x4;  output x5;  output x6;  output x7;
	output x8; output x9;
	''',
	
	# Test 22
	'''
	/*
	Foo
	bar
	*/
	
	f /* foo */ x0 /* foo */ = /* foo */ 3.0:16;
	f x1 = ZZZ.0:36;
	f x2 = ZZZ.ZZYFZGYZZ:36;
	f x3 = ZZZ.ZZYFZGYZZe-2:36;
	f x4 = ZZZ.ZZYFZGYZZe+203:36;
	f x5 = AAA.0:11;
	f x6 = AAA.0e-304:11;
	f x7 = 037425.0e0:8;
	f x8 = 037425.0e-0:8;
	f x9 = 037425.0e+0:8;
	
	output /* f sdfsd sf
	fsdfs df  
	fsdfsd 	
	fsdfsd ad  
	*/ x1; /* fooz */
	
	output x0;  output x2;  output x3;
	output x4; output x5;  output x6;
	output x7;  output x8; output x9;
	'''
)

source_filename = "./test/__TEST_DOT_PY_TEST_PROGRAM__.sl"

chdir("..")

def test(test_number):
	exit_status = system("./shiltiumcomp %s > /dev/null 2>/dev/null" % source_filename)
	if exit_status:
		print >> stderr, "Test %d failed." % test_number
		exit(exit_status)
	else:
		print "Test %d passed." % test_number

def write_source_to_file(string):
	with open(source_filename, "w") as file:
		file.write(string)

def test_all():
	for test_number, source_code in enumerate(source_files):
		write_source_to_file(source_code)
		test(test_number + 1)

def main():
	for i in ("my_make clean && my_make", "my_make rel", "my_make clean && my_make mcpp"):
		exit_status = system(i)
		if exit_status:
			exit(exit_status)
		
		test_all()
	
	print "\33cCreating large file..."
	system("./test/create_large_file.py " + source_filename)
	print "Done\nTesting..."
	test(len(source_files) + 1)
	
	print "\33cAll %d tests passed!" % (len(source_files) + 1)
	remove(source_filename)

main()
