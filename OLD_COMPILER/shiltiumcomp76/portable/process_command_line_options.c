/*
A function to go through the command line options.

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

#include "../global_headers/os_detection.h"

#if UNIX
	#include <unistd.h>
#endif

#include "../global_headers/headers.h"
#include "../global_headers/globals.h"

#if UNIX
	#define SEPARATOR_CHAR '/'
#elif WINDOWS
	#define SEPARATOR_CHAR '\\'
#else
	#error "Unknown separator character"
#endif

static void output_basename(const char* const filename) {
	const char* new_filename;
	u i;
	
	if (unlikely(!DEREF(filename))) {
		output_nullt_string("(unknown)");
		return;
	}
	
	new_filename = filename;
	
	for (i = 0; ARRAY_INDEX(filename, i); ++i) {
		if (i && ARRAY_INDEX(filename, i - 1) == SEPARATOR_CHAR) {
			new_filename = filename + i;
		}
	}
	
	output_nullt_string(new_filename);
}

attr_cold static inline void system_failed(void) {
	output_nullt_string(PROGRAM_NAME ": system() failed\n");
	exit_program();
}

attr_nonnull static void show_help(const char* cr program_name) {
	assert_comparison(program_name, !=, NULL, const char* cr, void*);
	
	output_nullt_string("Usage: ");
	output_basename(program_name);
	
	/* The ISO C90 character limit is stupid. */
	
	output_nullt_string(
		" [options] file [options]\n"
		OPENING_QUOTE "file" CLOSING_QUOTE " is the " LANGUAGE_NAME " source file to compile and run.\n"
		"Options can be before or after " OPENING_QUOTE "file" CLOSING_QUOTE " just like most UNIX programs.\n\n"
		"Options:\n"
		"    d, D, -d, -D, --d, --D, doc, -doc, --doc, documentation, -documentation,\n"
		"    --documentation, language-documentation, -language-documentation\n"
		"    --language-documentation:\n"
		"        display the " LANGUAGE_NAME " documentation\n\n"
	);
	
	output_nullt_string(
		"    h, -h, --h, help, -help, --help:\n"
		"        display this help and exit\n\n"
		"    v, -v, --v, version, -version, --version:\n"
		"        display version information and exit\n\n"
		"    --optimize-for=what\n"
		"            What to optimize for. This options has two possible values:\n"
		"        " OPENING_QUOTE "speed" CLOSING_QUOTE ", which optimizes for speed, but uses more memory;\n"
		"        and " OPENING_QUOTE "memory" CLOSING_QUOTE ", which optimizes for memory usage, but is slower.\n"
		"        --optimize-for=speed is default.\n"
	);
}

void process_command_line_options(char** argv) {
	const char* program_name;
	
	assert_comparison(argv, !=, NULL, char**, void*);
	
	source_filename = NULL;
	program_name = DEREF(argv);
	
	while (*++argv) {
		const char* argument = DEREF(argv);
		
		if (
			strequal(argument, "h") || strequal(argument, "-h") || strequal(argument, "--h") ||
			strequal(argument, "help") || strequal(argument, "-help") || strequal(argument, "--help")
		) {
			show_help(program_name);
			exit_program();
		}
		
		/*
		It's not very likely that a user would directly run this
		program, much less use it to look at the documentation.
		That's why this if statement has an 'unlikely' in it.
		*/
		if (unlikely(
			strequal(argument, "d") ||
			strequal(argument, "D") ||
			strequal(argument, "-d") ||
			strequal(argument, "-D") ||
			strequal(argument, "--d") ||
			strequal(argument, "--D") ||
			strequal(argument, "doc") ||
			strequal(argument, "-doc") ||
			strequal(argument, "--doc") ||
			strequal(argument, "documentation") ||
			strequal(argument, "-documentation") ||
			strequal(argument, "--documentation") ||
			strequal(argument, "language-documentation") ||
			strequal(argument, "-language-documentation") ||
			strequal(argument, "--language-documentation")
		)) {
			#if UNIX
				/*
				"less language_documentation" sounds funny ;)
				It makes me imagine someone shouting "LESS LANGUAGE DOCUMENTATION!!! TOO MUCH DOCUMENTATION!!!! AAAA!!!"
				
				Also, execl() is really awesome. It allows very simple and concise code like this.
				If the first execl() fails then the next one is tried, and if that fails the one after that is tried, etc.
				*/
				execl("/bin/less", "-~", "-x4", "--prompt=" LANGUAGE_NAME " Documentation", "language_documentation", NULL);
				execl("/bin/more", "language_documentation", NULL);
				execl("/bin/cat", "language_documentation", NULL);
				
				/* Yes, this actually works! */
				execl("/bin/cp", "language_documentation", "/dev/stdout", NULL);
				
				output_nullt_string("For some strange reason " PROGRAM_NAME
					" was unable to display the documentation with any of commands "
					"less, more and cat and cp.\n");
				
				exit_program();
			#elif WINDOWS
				{
					int return_value = system("list language_documentation");
					if (unlikely(return_value == -1)) {
						system_failed();
					}
					
					if (likely(!return_value)) exit_program();
					
					/* "more language_documentation" also sounds funny. MORE LANGUAGE DOCUMENTATION!!! I NEED MORE!!! MORE!!!! */
					return_value = system("more language_documentation");
					if (unlikely(return_value == -1)) {
						system_failed();
					}
					
					if (likely(!return_value)) exit_program();
					
					output_nullt_string(
						"For some strange reason " PROGRAM_NAME " was unable to display the "
						"documentation with both more and type.\n");
				}
			#else
				#error "FIXME"
			#endif
		}
		
		if (
			strequal(argument, "v") ||
			strequal(argument, "-v") ||
			strequal(argument, "--v") ||
			strequal(argument, "version") ||
			strequal(argument, "-version") ||
			strequal(argument, "--version")
		) {
			output_nullt_string(
				"This program is " PROGRAM_NAME " version " VERSION " and it can compile "
				"and execute code in " LANGUAGE_NAME " " LANGUAGE_VERSION ".\n"
			);
			
			exit_program();
		}
		
		if (strequal(argument, "--optimize-for=speed")) {
			what_to_optimize_for = OPTIMIZE_FOR_SPEED;
			continue;
		}
		
		if (strequal(argument, "--optimize-for=memory")) {
			what_to_optimize_for = OPTIMIZE_FOR_MEMORY;
			continue;
		}
		
		if (strequal(argument, "--language=english")) {
			language = LANGUAGE_ENGLISH;
			continue;
		}
		
		if (strequal(argument, "--language=french")) {
			language = LANGUAGE_FRENCH;
			continue;
		}
		
		if (strequal(argument, "--language=german")) {
			language = LANGUAGE_GERMAN;
			continue;
		}
		
		if (strequal(argument, "--language=italian")) {
			language = LANGUAGE_ITALIAN;
			continue;
		}
		
		if (strequal(argument, "--language=korean")) {
			language = LANGUAGE_KOREAN;
			continue;
		}
		
		if (strequal(argument, "--language=portuguese")) {
			language = LANGUAGE_PORTUGUESE;
			continue;
		}
		
		if (strequal(argument, "--language=spanish")) {
			language = LANGUAGE_SPANISH;
			continue;
		}
		
		if (strequal(argument, "--language=russian")) {
			language = LANGUAGE_RUSSIAN;
			continue;
		}
		
		if (unlikely(source_filename)) {
			output_nullt_string("The file to open was already specified as " OPENING_QUOTE);
			output_nullt_string(source_filename);
			output_nullt_string(CLOSING_QUOTE ".\n");
			exit_program();
		}
		
		source_filename = argument;
	}
	
	if (unlikely(!source_filename)) {
		show_help(program_name);
		exit_program();
	}
	
	assert_comparison(source_filename, !=, NULL, const char*, void*);
}
