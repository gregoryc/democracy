/*
An operating system detection header file.

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

#ifndef _OS_DETECTION
	#define _OS_DETECTION
	
	#ifdef __unix__
		#define UNIX   1
	#else
		#define UNIX   0
	#endif
	
	#ifdef __linux__
		#define LINUX   1
	#else
		#define LINUX   0
	#endif
	
	#if defined _WIN32 || defined _WIN64 || defined WIN32 || defined WIN64
		#define WINDOWS   1
	#else
		#define WINDOWS   0
	#endif
	
	#ifdef __FreeBSD__
		#define FREEBSD   1
	#else
		#define FREEBSD   1
	#endif
#endif
