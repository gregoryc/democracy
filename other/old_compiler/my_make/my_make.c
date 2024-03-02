/*
A small make-like utility for Shiltiumcomp.

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

#define _GNU_SOURCE
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#define NOTIFICATION_STRING_SUCCESS(x) "notify-send -t 6000 --icon='/usr/share/icons/oxygen/64x64/actions/dialog-ok-apply.png' '" x
#define NOTIFICATION_STRING_FAILURE(x) "notify-send -t 6000 --icon='/usr/share/icons/oxygen/64x64/status/dialog-error.png' '" x
#define HAPPY_FACE " :)'"
#define SAD_FACE " :('"

#define PROGRAM_NAME "Shiltiumcomp"
#define PROGRAM_NAME_LOWERCASE "shiltiumcomp"

#if 1
	#define restrict __restrict__
#endif

#define GCC_COMMAND "gcc"
#define MINGW32_COMMAND "i586-mingw32msvc-gcc"
#define GCC_RELEASE_COMMAND "gcc  "
#define GPLUSPLUS_COMMAND "g++"

static FILE* file;
static ssize_t bytes_read;
char* line;
char* object_string;
struct stat stat_buffer_for_c_file, stat_buffer_for_o_file;
bool sound = false;
time_t starting_time;

enum { C_32_BIT, C_64_BIT, C_64_BIT_DIETLIBC, C_WINDOWS_32_BIT, C_PLUS_PLUS_64_BIT } compilation_mode;

static void interrupt_handler(const int signal_number __attribute__((unused)));
static bool equals(const char* restrict string1, const char* restrict string2);
static bool should_recompile(void);
static void output_size_of_executable(void);
static void compile(void);
static void cleanup(void);
static void exit_program(int exit_status, const char* const restrict string) __attribute__((noreturn));

int main(int argc, char** argv) {
	struct stat objects_directory_stat_buffer;
	size_t allocated_length;
	size_t number_of_recompilations = 0;
	int exit_status;
	bool executable_file_exists;
	struct sigaction sa;
	
	starting_time = time(NULL);
	
	if (starting_time == (time_t)-1) {
		perror("time() failed");
		return 1;
	}
	
	sa.sa_flags = 0;
	sa.sa_handler = interrupt_handler;
	sigemptyset(&sa.sa_mask);
	
	if (sigaction(SIGINT, &sa, NULL) == -1) {
		fputs("sigaction() failed\n", stderr);
		return 1;
	}
	
	fputs("\33c\x1b[?25l", stdout);
	
	if (fflush(stdout) == -1) {
		perror("fflush() failed");
		return 1;
	}
	
	if (argc > 1 && (equals(argv[1], "-h") || equals(argv[1], "--help"))) {
		printf("Options for %s: -h, --help, clean, cpp, 32bit, sound, backup, disasm, memcheck, rel, lines, chars\n", *argv);
		exit_program(0, NULL);
	}
	
	if (argc > 1 && equals(argv[1], "sound")) {
		sound = true;
		++argv;
		--argc;
	}
	
	if (argc > 1 && equals(argv[1], "clean")) {
		if (stat("objects", &objects_directory_stat_buffer) == -1) {
			if (errno == ENOENT) {
				puts("Nothing to clean.");
				exit_program(0, NULL);
			} else {
				perror("Cannot stat() 'objects'");
				exit_program(1, NOTIFICATION_STRING_FAILURE("Failed to remove object files in 'objects' directory" SAD_FACE));
			}
		}
		
		exit_status = system("rm -rf objects");
		if (exit_status == -1) {
			perror("system() failed");
			exit_program(1, NOTIFICATION_STRING_FAILURE("Failed to remove object files in 'objects' directory" SAD_FACE));
		}
		
		if (exit_status) {
			exit_program(1, NOTIFICATION_STRING_FAILURE("Failed to remove object files in 'objects' directory" SAD_FACE));
		}
		
		puts("Successfully cleaned.");
		exit_program(0, NULL);
	}
	
	if (argc > 1 && equals(argv[1], "backup")) {
		puts("Backing up " PROGRAM_NAME "...");
		
		exit_status = system(
			"cd ~/Dropbox && f=`ls -l | grep ^- | awk '{print $8}' | python -c \"from os import sys; print '"
			PROGRAM_NAME_LOWERCASE "' + str(max([int(i.replace('" PROGRAM_NAME_LOWERCASE "', '').replace('.tar.bz2', '')) "
			"for i in sys.stdin.read().split('\\n') if i.startswith('" PROGRAM_NAME_LOWERCASE "') and i.endswith("
			"'.tar.bz2')] or [0]) + 1)\" || exit`; dropbox stop > /dev/null && cp -r ~/Desktop/" PROGRAM_NAME_LOWERCASE
			" ${f} && rm -rf ${f}/objects ${f}/__GENERATED_CODE__.bin ${f}/global_headers/debug.h ${f}/"
			PROGRAM_NAME_LOWERCASE " && tar cjf ${f}.tar.bz2 ${f} && rm -rf ${f} && dropbox start > /dev/null && echo "
			"Successfully backed up " PROGRAM_NAME " as ‘$f’ in ~/Dropbox."
		);
		
		if (exit_status == -1) {
			perror("system() failed");
			exit_program(1, NOTIFICATION_STRING_FAILURE("Failed to backup " PROGRAM_NAME SAD_FACE));
		}
		
		exit_program(exit_status, NOTIFICATION_STRING_SUCCESS("Successfully backed up " PROGRAM_NAME HAPPY_FACE));
	}
	
	if (argc > 1 && equals(argv[1], "disasm64")) {
		fputs("\x1b[?12l\x1b[?25h", stdout);
		fflush(stdout);
		
		/*
		This command can be used instead:
		ndisasm -b 64 __GENERATED_CODE__.bin
		
		ndisasm has a nicer assembly syntax but it thinks the syscall instruction is loadall286, which is obviously incorrect.
		Objdump understands the "syscall" instruction properly though.
		*/
		execlp("objdump", "objdump", "-D", "-b", "binary", "-mi386", "-Mx86-64,intel", "__GENERATED_CODE__.bin", NULL);
		
		perror("execlp() failed");
		return 1;
	}
	
	if (argc > 1 && equals(argv[1], "disasm32")) {
		fputs("\x1b[?12l\x1b[?25h", stdout);
		fflush(stdout);
		
		execlp("objdump", "objdump", "-D", "-b", "binary", "-mi386", "-Mintel", "__GENERATED_CODE__.bin", NULL);
		
		perror("execlp() failed");
		return 1;
	}
	
	if (argc > 1 && equals(argv[1], "memcheck")) {
		exit_status = system(
			"valgrind --show-reachable=yes --track-origins=yes --leak-check=full --track-origins=yes --freelist-vol=20000000 ./"
			PROGRAM_NAME_LOWERCASE " program.sl"
		);
		
		if (exit_status == -1) {
			perror("system() failed");
			exit_program(1, NOTIFICATION_STRING_FAILURE("Failed to run valgrind" SAD_FACE));
		}
		
		exit_program(0, NULL);
	}
	
	if (argc > 1 && equals(argv[1], "rel")) {
		exit_status = system(
			"gcc"
			" `find -regex '.*\\.c$' -type f` `find -regex './objects/*\\.o$' -type f` -o " PROGRAM_NAME_LOWERCASE
			" && printf 'Done\\n\\n' && echo Stripping... && strip --remove-section=.comment --remove-section=.note " PROGRAM_NAME_LOWERCASE
			" && printf 'Done\\n\\n'"
		);
		
		if (exit_status == -1) {
			perror("system() failed");
			exit_program(1, NOTIFICATION_STRING_FAILURE("Failed to compile " PROGRAM_NAME " in release mode" SAD_FACE));
		}
		
		if (exit_status) {
			exit_program(1, NOTIFICATION_STRING_FAILURE("Failed to compile " PROGRAM_NAME " in release mode" SAD_FACE));
		} else {
			fputs("Successfully compiled " PROGRAM_NAME " in release mode; size = ", stdout);
			output_size_of_executable();
		}
		exit_program(0, NOTIFICATION_STRING_SUCCESS("Successfully compiled " PROGRAM_NAME " in release mode" HAPPY_FACE));
	}
	
	if (argc > 1 && equals(argv[1], "32bit")) {
		compilation_mode = C_32_BIT;
	} else if (argc > 1 && equals(argv[1], "cpp")) {
		compilation_mode = C_PLUS_PLUS_64_BIT;
	} else if (argc > 1 && equals(argv[1], "dietlibc")) {
		compilation_mode = C_64_BIT_DIETLIBC;
	} else if (argc > 1 && equals(argv[1], "w32bit")) {
		compilation_mode = C_WINDOWS_32_BIT;
	} else {
		compilation_mode = C_64_BIT;
	}
	
	if (argc > 1 && equals(argv[1], "lines")) {
		exit_status = system("wc -l `find -type f -regextype posix-extended -regex '^.+\\.[ch](pp)?$'`");
		if (exit_status == -1) {
			perror("system() failed");
			exit_program(1, NULL);
		}
		exit_program(0, NULL);
	}
	
	if (argc > 1 && equals(argv[1], "chars")) {
		exit_status = system("wc -c `find -type f -regextype posix-extended -regex '^.+\\.[ch](pp)?$'`");
		if (exit_status == -1) {
			perror("system() failed");
			exit_program(1, NULL);
		}
		exit_program(0, NULL);
	}
	
	if (stat("objects", &objects_directory_stat_buffer) == -1) {
		if (errno == ENOENT) {
			if (mkdir("objects", S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) == -1) {
				perror("mkdir() failed");
				exit_program(1, NOTIFICATION_STRING_FAILURE(PROGRAM_NAME " compilation failed" SAD_FACE));
			}
		} else {
			perror("Cannot stat() 'objects'");
			exit_program(1, NOTIFICATION_STRING_FAILURE(PROGRAM_NAME " compilation failed" SAD_FACE));
		}
	} else if (!S_ISDIR(objects_directory_stat_buffer.st_mode)) {
		fputs("File 'objects' is not a directory.\n", stderr);
		exit_program(1, NOTIFICATION_STRING_FAILURE(PROGRAM_NAME " compilation failed" SAD_FACE));
	}
	
	{
		FILE* file = fopen("global_headers/debug.h", "w");
		if (!file) {
			perror("Cannot open file 'global_headers/debug.h'");
			exit_program(1, NOTIFICATION_STRING_FAILURE(PROGRAM_NAME " compilation failed" SAD_FACE));
		}
		
		if (fputs("#define DEBUG 1\n", file) == EOF) {
			fputs("Cannot write to file 'global_headers/debug.h'\n", stderr);
			exit_program(1, NOTIFICATION_STRING_FAILURE(PROGRAM_NAME " compilation failed" SAD_FACE));
		}
		
		if (fclose(file)) {
			perror("Unable to close file 'global_headers/debug.h'");
			exit_program(1, NOTIFICATION_STRING_FAILURE(PROGRAM_NAME " compilation failed" SAD_FACE));
		}
	}
	
	file = popen("find -type f | grep -E '.+\\.c$' | sort", "r");
	if (!file) {
		exit_program(1, NOTIFICATION_STRING_FAILURE(PROGRAM_NAME " compilation failed" SAD_FACE));
		fputs("Cannot popen()", stderr);
		exit_program(1, NOTIFICATION_STRING_FAILURE(PROGRAM_NAME " compilation failed" SAD_FACE));
	}
	
	while ((bytes_read = getline(&line, &allocated_length, file)) != -1) {
		char* char_;
		
		line[bytes_read - 1] = '\0';
		
		if (stat(line, &stat_buffer_for_c_file) == -1) {
			fputs("Can't stat() ", stderr);
			perror(line);
			cleanup();
			exit_program(1, NOTIFICATION_STRING_FAILURE(PROGRAM_NAME " compilation failed" SAD_FACE));
		}
		
		line[bytes_read - 2] = 'o';
		object_string = (char*)malloc(strlen(line + 2) + 9);
		if (!object_string) {
			fputs("malloc() failed", stderr);
			cleanup();
			exit_program(1, NOTIFICATION_STRING_FAILURE(PROGRAM_NAME " compilation failed" SAD_FACE));
		}
		
		memcpy(object_string, "objects/", 8);
		memcpy(object_string + 8, line + 2, strlen(line + 2));
		object_string[8 + strlen(line + 2)] = '\0';
		line[bytes_read - 2] = 'c';
		
		for (char_ = object_string + 8; *char_; ++char_) {
			if (*char_ == '/') {
				*char_ = '$';
			}
		}
		
		if (stat(object_string, &stat_buffer_for_o_file) == -1) {
			if (errno == ENOENT) {
				goto recompile;
			}
			
			fputs("Can't stat() ", stderr);
			perror(object_string);
			cleanup();
			exit_program(1, NOTIFICATION_STRING_FAILURE(PROGRAM_NAME " compilation failed" SAD_FACE));
		}
		
		if (should_recompile()) {
		recompile:
			++number_of_recompilations;
			compile();
		}
		
		free(object_string);
		object_string = NULL;
	}
	
	cleanup();
	
	{
		struct stat stat_buffer;
		
		if (stat(PROGRAM_NAME_LOWERCASE, &stat_buffer) == -1) {
			if (errno == ENOENT) {
				executable_file_exists = false;
			} else {
				perror("Cannot stat '" PROGRAM_NAME_LOWERCASE "'");
				exit_program(1, NOTIFICATION_STRING_FAILURE(PROGRAM_NAME " compilation failed" SAD_FACE));
			}
		} else {
			executable_file_exists = true;
		}
	}
	
	if (number_of_recompilations || !executable_file_exists) {
		if (number_of_recompilations) {
			putchar('\n');
		}
		
		puts("Linking...");
		
		switch (compilation_mode) {
			case C_32_BIT:
				exit_status = system(GCC_COMMAND " -g -m32 -lm -Wall -Wextra `find -regex '^\\./objects/.*\\.o$'` -o " PROGRAM_NAME_LOWERCASE);
				break;
			case C_64_BIT:
				exit_status = system(GCC_COMMAND " -g -lm -Wall -Wextra `find -regex '^\\./objects/.*\\.o$'` -o " PROGRAM_NAME_LOWERCASE);
				break;
			case C_64_BIT_DIETLIBC:
				exit_status = system("diet " GCC_COMMAND " -g -lm -Wall -Wextra `find -regex '^\\./objects/.*\\.o$'` -o " PROGRAM_NAME_LOWERCASE);
				break;
			case C_WINDOWS_32_BIT:
				exit_status = system(MINGW32_COMMAND " -g -Wall -Wextra `find -regex '^\\./objects/.*\\.o$'` -o " PROGRAM_NAME_LOWERCASE);
				break;
			case C_PLUS_PLUS_64_BIT:
				exit_status = system(GPLUSPLUS_COMMAND " -g -Wall -Wextra `find -regex '^\\./objects/.*\\.o$'` -o " PROGRAM_NAME_LOWERCASE);
				break;
			
			/* GCC warns if this isn't here. */
			default:
				/* This will never be reached, and I don't wan't to add an assert macro call here. */
				return 1;
		}
		
		if (exit_status == -1) {
			perror("system() failed");
			exit_program(1, NOTIFICATION_STRING_FAILURE(PROGRAM_NAME "compilation failed when trying to link" SAD_FACE));
		}
		
		if (exit_status) {
			exit_program(1, NOTIFICATION_STRING_FAILURE(PROGRAM_NAME " linking failed " SAD_FACE));
		} else {
			switch (compilation_mode) {
				case C_32_BIT:
					fputs("\33c\x1b[?25lSuccessfully compiled " PROGRAM_NAME " in 32-bit mode; size = ", stdout);
					output_size_of_executable();
					exit_program(0, NOTIFICATION_STRING_SUCCESS("Successfully compiled " PROGRAM_NAME " in 32-bit mode" HAPPY_FACE));
					break;
				case C_64_BIT:
					fputs("\33c\x1b[?25lSuccessfully compiled " PROGRAM_NAME "; size = ", stdout);
					output_size_of_executable();
					exit_program(0, NOTIFICATION_STRING_SUCCESS("Successfully compiled " PROGRAM_NAME HAPPY_FACE));
					break;
				case C_64_BIT_DIETLIBC:
					fputs("\33c\x1b[?25lSuccessfully compiled " PROGRAM_NAME " in 64-bit dietlibc mode; size = ", stdout);
					output_size_of_executable();
					exit_program(0, NOTIFICATION_STRING_SUCCESS("Successfully compiled " PROGRAM_NAME " in 64-bit dietlibc mode." HAPPY_FACE));
					break;
				case C_WINDOWS_32_BIT:
					fputs("\33c\x1b[?25lSuccessfully compiled " PROGRAM_NAME " in 32-bit Windows mode; size = ", stdout);
					output_size_of_executable();
					exit_program(0, NOTIFICATION_STRING_SUCCESS("Successfully compiled " PROGRAM_NAME " in 32-bit Windows mode" HAPPY_FACE));
					break;
				case C_PLUS_PLUS_64_BIT:
					fputs("\33c\x1b[?25lSuccessfully compiled " PROGRAM_NAME " in C++ mode; size = ", stdout);
					output_size_of_executable();
					puts(
						"\nNOTE: This should only be done for testing purposes, to see if the program\n"
						"compiles or to valgrind it with glibc. The " PROGRAM_NAME " executable is smaller and\n"
						"probably faster when compiled in C mode (the default)."
					);
					exit_program(0, NOTIFICATION_STRING_SUCCESS("Successfully compiled " PROGRAM_NAME " in C++ mode" HAPPY_FACE));
					break;
			}
		}
	} else {
		puts("\33c\x1b[?25l" PROGRAM_NAME " does not need to be recompiled or relinked.");
		exit_program(0, NULL);
	}
	
	/* This will never be reached, and I don't wan't to add an assert macro call here. */
	return 0;
}

static void interrupt_handler(const int signal_number __attribute__((unused))) {
	puts("\nCompilation aborted.\x1b[?12l\x1b[?25h");
	exit(1);
}

static bool equals(const char* restrict string1, const char* restrict string2) {
	size_t i;
	
	if (strlen(string1) != strlen(string2)) return false;
		
	for (i = 0 ;; ++i) {
		if (!string1[i]) return true;
		if (string1[i] != string2[i]) return false;
	}
}

static bool should_recompile(void) {
	return stat_buffer_for_c_file.st_mtim.tv_sec >= stat_buffer_for_o_file.st_mtim.tv_sec;
}

static void compile(void) {
	pid_t pid = fork();
	
	if (pid == -1) {
		perror("my_make: fork() failed");
		cleanup();
		exit_program(1, NOTIFICATION_STRING_FAILURE(PROGRAM_NAME " compilation failed" SAD_FACE));
	}
	
	if (pid) {
		int status;
		waitpid(pid, &status, 0);
		if (status) {
			cleanup();
			exit_program(1, NOTIFICATION_STRING_FAILURE(PROGRAM_NAME " compilation failed" SAD_FACE));
		}
	} else {
		time_t time_taken = time(NULL) - starting_time;
		
		printf("\33c\x1b[?25lCompilation has taken %ld second%s.\n\nCompling %s...\n\n", time_taken, (time_taken == 1) ? "" : "s", line + 2);
		
		switch (compilation_mode) {
			case C_32_BIT:
				execlp(GCC_COMMAND, GCC_COMMAND, "-c", "-pipe", "-m32", "-g", "-fwrapv", "-Wall", "-Wextra",  "-Wfatal-errors",
					"-pedantic", "-Winit-self", "-Wundef", "-Wshadow", "-Wunsafe-loop-optimizations",
					"-funsafe-loop-optimizations", "-Wcast-align", "-Wwrite-strings", "-Wlogical-op", "-Waggregate-return",
					"-Wbad-function-cast", "-Wdeclaration-after-statement", "-Wc++-compat", "-Wstrict-prototypes", "-Wold-style-definition",
					"-Wformat=2", line + 2, "-o", object_string, NULL);
					break;
			case C_64_BIT:
				execlp(GCC_COMMAND, GCC_COMMAND, "-c", "-pipe", "-g", "-fwrapv", "-Wall", "-Wextra", "-Wfatal-errors",
					"-pedantic", "-Winit-self", "-Wundef", "-Wshadow", "-Wunsafe-loop-optimizations",
					"-funsafe-loop-optimizations", "-Wcast-align", "-Wwrite-strings", "-Wlogical-op", "-Waggregate-return",
					"-Wbad-function-cast", "-Wdeclaration-after-statement", "-Wc++-compat", "-Wstrict-prototypes", "-Wold-style-definition",
					"-Wformat=2", line + 2, "-o", object_string, NULL);
					break;
			case C_64_BIT_DIETLIBC:
				execlp("diet", "diet", GCC_COMMAND, "-c", "-pipe", "-g", "-fwrapv", "-Wall", "-Wextra", "-Wfatal-errors",
					"-pedantic", "-Winit-self", "-Wundef", "-Wshadow", "-Wunsafe-loop-optimizations",
					"-funsafe-loop-optimizations", "-Wcast-align", "-Wwrite-strings", "-Wlogical-op", "-Waggregate-return",
					"-Wbad-function-cast", "-Wdeclaration-after-statement", "-Wc++-compat", "-Wstrict-prototypes", "-Wold-style-definition",
					"-Wformat=2", line + 2, "-o", object_string, NULL);
					break;
			case C_WINDOWS_32_BIT:
				execlp(MINGW32_COMMAND, MINGW32_COMMAND, "-c", "-pipe", "-g", "-fwrapv", "-Wall", "-Wextra", "-Wfatal-errors",
					"-pedantic", "-Winit-self", "-Wundef", "-Wshadow", "-Wunsafe-loop-optimizations",
					"-funsafe-loop-optimizations", "-Wcast-align", "-Wwrite-strings", "-Waggregate-return",
					"-Wbad-function-cast", "-Wdeclaration-after-statement", "-Wc++-compat", "-Wstrict-prototypes", "-Wold-style-definition",
					"-Wformat=2", line + 2, "-o", object_string, NULL);
					break;
			case C_PLUS_PLUS_64_BIT:
				execlp(GPLUSPLUS_COMMAND, GPLUSPLUS_COMMAND, "-c", "-pipe", "-g", "-fwrapv", "-Wall", "-Wextra", "-Wfatal-errors", "-pedantic",
					"-Winit-self", "-Wundef", "-Wshadow", "-Wunsafe-loop-optimizations",
					"-funsafe-loop-optimizations", "-Wcast-align", "-Wwrite-strings", "-Wlogical-op", "-Waggregate-return",
					"-Wformat=2", line + 2, "-o", object_string, NULL);
				break;
		}
		
		perror("my_make: execlp() failed");
		
		if (compilation_mode == C_PLUS_PLUS_64_BIT) {
			fputs(
				"\nThis could have happened because you don't have g++ installed. Check if you do \n"
				"and if you don't, install it and re-run this program.\n",
				stderr);
		}
		cleanup();
		exit_program(1, NOTIFICATION_STRING_FAILURE(PROGRAM_NAME " compilation failed" SAD_FACE));
	}
}

static void output_size_of_executable(void) {
	struct stat stat_buffer;
	
	if (stat(PROGRAM_NAME_LOWERCASE, &stat_buffer)) {
		perror("Cannot stat() '" PROGRAM_NAME_LOWERCASE "'");
		exit_program(1, NULL);
	}
	
	printf("%lu bytes\n", stat_buffer.st_size);
}

static inline void cleanup(void) {
	free(object_string);
	free(line);
	pclose(file);
}

static void exit_program(int exit_status, const char* const __restrict__ string) {
	fputs("\x1b[?12l\x1b[?25h", stdout);
	
	/* Ignore the return value, since it's not necessary for this command to succeed. */
	if (string && system(string)) {}
	
	if (sound) {
		if (exit_status) {
			if (system("mplayer /usr/share/sounds/k3b_error1.wav > /dev/null 2>/dev/null &")) {}
		} else {
			if (system("mplayer /usr/share/sounds/k3b_success1.wav > /dev/null 2>/dev/null &")) {}
		}
	}
	
	exit(exit_status);
}
