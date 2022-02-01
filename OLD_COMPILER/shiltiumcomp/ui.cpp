/*
Shiltium's user interface.

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

#include <iostream>
#include <stdio.h>
#include <sstream>
#include <cstdlib>

#if DEBUG
	static char sizeof_unsigned_short_equals_2_array[sizeof(unsigned short) == 2 ? 1 : -1] __attribute__((unused));
#endif

#define PROGRAM_NAME "Shiltium UI"

#define SHOW_MESSAGE_CODES  0
#define QUIET  0

static const enum {
	OUTPUT_TYPE_NORMAL,
	OUTPUT_TYPE_DIALOG,
	OUTPUT_TYPE_SPEAK_OUTPUT_WITH_ESPEAK,
	OUTPUT_TYPE_SPEAK_OUTPUT_WITH_FESTIVAL_BAD_VOICE,
	OUTPUT_TYPE_SPEAK_OUTPUT_WITH_FESTIVAL_GOOD_VOICE,
} output_type = OUTPUT_TYPE_NORMAL;

using std::cin;
using std::cout;
using std::cerr;

enum {
	MESSAGE_CODE_HELLO_WORLD,
	MESSAGE_CODE_OUTPUT_UINT,
	MESSAGE_CODE_OUTPUT_FLOAT,
	MESSAGE_CODE_EXIT
};

static void output_hello_world(void) {
	switch (output_type) {
		case OUTPUT_TYPE_NORMAL:
			cout << "Hello world!\n";
			break;
		case OUTPUT_TYPE_DIALOG: {
			int exit_status = system("zenity --info --text 'Hello world!'");
			
			if (exit_status == -1) {
				cout << "system() failed\n";
				exit(1);
			}
			
			exit_status = WEXITSTATUS(exit_status);
			
			if (exit_status) {
				cout << PROGRAM_NAME ": zenity exited with an exit status of " << exit_status << ", which means it failed\n";
				exit(exit_status);
			}
			
			break;
		}
		case OUTPUT_TYPE_SPEAK_OUTPUT_WITH_ESPEAK: {
			int exit_status;
			int tmp_fd;
			
			tmp_fd = dup(STDERR_FILENO);
			if (!tmp_fd) {
				perror(PROGRAM_NAME ": dup() failed");
				exit(1);
			}
			
			close(STDERR_FILENO);
			
			exit_status = system("espeak 'Hello world!'");
			
			if (exit_status == -1) {
				cout << "system() failed\n";
				exit(1);
			}
			
			if (dup2(tmp_fd, STDERR_FILENO) == -1) {
				perror(PROGRAM_NAME ": dup2() failed");
				exit(1);
			}
			
			exit_status = WEXITSTATUS(exit_status);
			
			if (exit_status) {
				cout << PROGRAM_NAME ": espeak exited with an exit status of " << exit_status << ", which means it failed\n";
				exit(exit_status);
			}
			
			break;
		}
		case OUTPUT_TYPE_SPEAK_OUTPUT_WITH_FESTIVAL_BAD_VOICE: {
			int exit_status;
			int tmp_fd;
			
			tmp_fd = dup(STDERR_FILENO);
			if (!tmp_fd) {
				perror(PROGRAM_NAME ": dup() failed");
				exit(1);
			}
			
			close(STDERR_FILENO);
			
			exit_status = system("echo 'Hello world!' | esddsp festival --tts");
			
			if (exit_status == -1) {
				cout << "system() failed\n";
				exit(1);
			}
			
			if (dup2(tmp_fd, STDERR_FILENO) == -1) {
				perror(PROGRAM_NAME ": dup2() failed");
				exit(1);
			}
			
			exit_status = WEXITSTATUS(exit_status);
			
			if (exit_status) {
				cout << PROGRAM_NAME ": espeak exited with an exit status of " << exit_status << ", which means it failed\n";
				exit(exit_status);
			}
			
			break;
		}
		case OUTPUT_TYPE_SPEAK_OUTPUT_WITH_FESTIVAL_GOOD_VOICE: {
			int exit_status;
			int tmp_fd;
			
			tmp_fd = dup(STDERR_FILENO);
			if (!tmp_fd) {
				perror(PROGRAM_NAME ": dup() failed");
				exit(1);
			}
			
			close(STDERR_FILENO);
			
			exit_status = system("printf '(voice_ked_diphone)\n(SayText \"Hello world!\")\n' | esddsp festival");
			
			if (exit_status == -1) {
				cout << "system() failed\n";
				exit(1);
			}
			
			if (dup2(tmp_fd, STDERR_FILENO) == -1) {
				perror(PROGRAM_NAME ": dup2() failed");
				exit(1);
			}
			
			exit_status = WEXITSTATUS(exit_status);
			
			if (exit_status) {
				cout << PROGRAM_NAME ": espeak exited with an exit status of " << exit_status << ", which means it failed\n";
				exit(exit_status);
			}
			
			break;
		}
	}
}

static void output_unsigned_integer(const unsigned long number) {
	std::stringstream stream(std::stringstream::out);
	
	switch (output_type) {
		case OUTPUT_TYPE_NORMAL:
			cout << number << '\n';
			break;
		case OUTPUT_TYPE_DIALOG:
			int exit_status;
			
			stream << "zenity --info --text '" << number << "'\n";
			exit_status = system(stream.str().c_str());
			
			if (exit_status == -1) {
				cout << "system() failed\n";
				exit(1);
			}
			
			exit_status = WEXITSTATUS(exit_status);
			
			if (exit_status) {
				cout << PROGRAM_NAME ": zenity exited with an exit status of " << exit_status << ", which means it failed\n";
				exit(exit_status);
			}
			
			break;
		case OUTPUT_TYPE_SPEAK_OUTPUT_WITH_ESPEAK: {
			int exit_status;
			int tmp_fd;
			
			tmp_fd = dup(STDERR_FILENO);
			if (!tmp_fd) {
				perror(PROGRAM_NAME ": dup() failed");
				exit(1);
			}
			
			close(STDERR_FILENO);
			
			stream << "espeak ' " << number << "'";
			exit_status = system(stream.str().c_str());
			
			if (exit_status == -1) {
				cout << "system() failed\n";
				exit(1);
			}
			
			if (dup2(tmp_fd, STDERR_FILENO) == -1) {
				perror(PROGRAM_NAME ": dup2() failed");
				exit(1);
			}
			
			exit_status = WEXITSTATUS(exit_status);
			
			if (exit_status) {
				cout << PROGRAM_NAME ": espeak exited with an exit status of " << exit_status << ", which means it failed\n";
				exit(exit_status);
			}
			
			break;
		}
		case OUTPUT_TYPE_SPEAK_OUTPUT_WITH_FESTIVAL_BAD_VOICE: {
			int exit_status;
			int tmp_fd;
			
			tmp_fd = dup(STDERR_FILENO);
			if (!tmp_fd) {
				perror(PROGRAM_NAME ": dup() failed");
				exit(1);
			}
			
			close(STDERR_FILENO);
			
			stream << "echo ' " << number << "' | esddsp festival --tts\n";
			exit_status = system(stream.str().c_str());
			
			if (exit_status == -1) {
				cout << "system() failed\n";
				exit(1);
			}
			
			if (dup2(tmp_fd, STDERR_FILENO) == -1) {
				perror(PROGRAM_NAME ": dup2() failed");
				exit(1);
			}
			
			exit_status = WEXITSTATUS(exit_status);
			
			if (exit_status) {
				cout << PROGRAM_NAME ": espeak exited with an exit status of " << exit_status << ", which means it failed\n";
				exit(exit_status);
			}
			
			break;
		}
		case OUTPUT_TYPE_SPEAK_OUTPUT_WITH_FESTIVAL_GOOD_VOICE: {
			int exit_status;
			int tmp_fd;
			
			tmp_fd = dup(STDERR_FILENO);
			if (!tmp_fd) {
				perror(PROGRAM_NAME ": dup() failed");
				exit(1);
			}
			
			close(STDERR_FILENO);
			
			stream << "printf '(voice_ked_diphone)\n(SayText \"" << number << "\")\n' | esddsp festival";
			exit_status = system(stream.str().c_str());
			
			if (exit_status == -1) {
				cout << "system() failed\n";
				exit(1);
			}
			
			if (dup2(tmp_fd, STDERR_FILENO) == -1) {
				perror(PROGRAM_NAME ": dup2() failed");
				exit(1);
			}
			
			exit_status = WEXITSTATUS(exit_status);
			
			if (exit_status) {
				cout << PROGRAM_NAME ": espeak exited with an exit status of " << exit_status << ", which means it failed\n";
				exit(exit_status);
			}
			
			break;
		}
	}
}

static void output_floating_point_number(const double number) {
	std::stringstream stream(std::stringstream::out);
	
	switch (output_type) {
		case OUTPUT_TYPE_NORMAL:
			cout << number << '\n';
			break;
		case OUTPUT_TYPE_DIALOG:
			int exit_status;
			
			stream << "zenity --info --text '" << number << "'\n";
			exit_status = system(stream.str().c_str());
			
			if (exit_status == -1) {
				cout << "system() failed\n";
				exit(1);
			}
			
			exit_status = WEXITSTATUS(exit_status);
			
			if (exit_status) {
				cout << PROGRAM_NAME ": zenity exited with an exit status of " << exit_status << ", which means it failed\n";
				exit(exit_status);
			}
			
			break;
		case OUTPUT_TYPE_SPEAK_OUTPUT_WITH_ESPEAK: {
			int exit_status;
			int tmp_fd;
			
			tmp_fd = dup(STDERR_FILENO);
			if (!tmp_fd) {
				perror(PROGRAM_NAME ": dup() failed");
				exit(1);
			}
			
			close(STDERR_FILENO);
			
			stream << "espeak ' " << number << "'";
			exit_status = system(stream.str().c_str());
			
			if (exit_status == -1) {
				cout << "system() failed\n";
				exit(1);
			}
			
			if (dup2(tmp_fd, STDERR_FILENO) == -1) {
				perror(PROGRAM_NAME ": dup2() failed");
				exit(1);
			}
			
			exit_status = WEXITSTATUS(exit_status);
			
			if (exit_status) {
				cout << PROGRAM_NAME ": espeak exited with an exit status of " << exit_status << ", which means it failed\n";
				exit(exit_status);
			}
			
			break;
		}
		case OUTPUT_TYPE_SPEAK_OUTPUT_WITH_FESTIVAL_BAD_VOICE: {
			int exit_status;
			int tmp_fd;
			
			tmp_fd = dup(STDERR_FILENO);
			if (!tmp_fd) {
				perror(PROGRAM_NAME ": dup() failed");
				exit(1);
			}
			
			close(STDERR_FILENO);
			
			stream << "echo ' " << number << "' | esddsp festival --tts\n";
			exit_status = system(stream.str().c_str());
			
			if (exit_status == -1) {
				cout << "system() failed\n";
				exit(1);
			}
			
			if (dup2(tmp_fd, STDERR_FILENO) == -1) {
				perror(PROGRAM_NAME ": dup2() failed");
				exit(1);
			}
			
			exit_status = WEXITSTATUS(exit_status);
			
			if (exit_status) {
				cout << PROGRAM_NAME ": espeak exited with an exit status of " << exit_status << ", which means it failed\n";
				exit(exit_status);
			}
			
			break;
		}
		case OUTPUT_TYPE_SPEAK_OUTPUT_WITH_FESTIVAL_GOOD_VOICE: {
			int exit_status;
			int tmp_fd;
			
			tmp_fd = dup(STDERR_FILENO);
			if (!tmp_fd) {
				perror(PROGRAM_NAME ": dup() failed");
				exit(1);
			}
			
			close(STDERR_FILENO);
			
			stream << "printf '(voice_ked_diphone)\n(SayText \"" << number << "\")\n' | esddsp festival";
			exit_status = system(stream.str().c_str());
			
			if (exit_status == -1) {
				cout << "system() failed\n";
				exit(1);
			}
			
			if (dup2(tmp_fd, STDERR_FILENO) == -1) {
				perror(PROGRAM_NAME ": dup2() failed");
				exit(1);
			}
			
			exit_status = WEXITSTATUS(exit_status);
			
			if (exit_status) {
				cout << PROGRAM_NAME ": espeak exited with an exit status of " << exit_status << ", which means it failed\n";
				exit(exit_status);
			}
			
			break;
		}
	}
}

static void read_from_shiltiumcomp(void) {
	int fds[2];
	int fork_return_value;
	
	if (pipe(fds) == -1) {
		perror(PROGRAM_NAME ": pipe() failed");
		exit(1);
	}
	
	fork_return_value = fork();
	
	if (fork_return_value == -1) {
		perror(PROGRAM_NAME ": fork() failed");
		exit(1);
	}
	
	if (fork_return_value) {
		/* Parent */
		unsigned short message_code;
		double floating_point_number;
		unsigned long unsigned_integer;
		
		if (dup2(fds[0], STDIN_FILENO) == -1) {
			perror(PROGRAM_NAME ": dup2() failed in parent");
		}
		
		cout.precision(10);
		
		while (cin.read((char*)&message_code, sizeof(message_code))) {
			switch (message_code) {
				case MESSAGE_CODE_HELLO_WORLD:
					if (SHOW_MESSAGE_CODES) {
						puts("Message code #0 -- Hello world");
					}
					
					output_hello_world();
					break;
				case MESSAGE_CODE_OUTPUT_UINT:
					if (SHOW_MESSAGE_CODES) {
						puts("Message code #1 -- Output UInt");
					}
					
					if (!cin.read((char*)&unsigned_integer, sizeof(unsigned_integer))) {
						cerr << PROGRAM_NAME ": error: could not read float\n";
						exit(1);
					}
					
					output_unsigned_integer(unsigned_integer);
					break;
				case MESSAGE_CODE_OUTPUT_FLOAT:
					if (SHOW_MESSAGE_CODES) {
						puts("Message code #1 -- Output Float");
					}
					
					if (!cin.read((char*)&floating_point_number, sizeof(floating_point_number))) {
						cerr << PROGRAM_NAME ": error: could not read float\n";
						exit(1);
					}
					
					output_floating_point_number(floating_point_number);
					break;
				case MESSAGE_CODE_EXIT:
					if (SHOW_MESSAGE_CODES) {
						puts("Message code #2 -- Exit");
					}
					
					if (output_type == OUTPUT_TYPE_SPEAK_OUTPUT_WITH_ESPEAK) {
						int exit_status;
						int tmp_fd;
						
						tmp_fd = dup(STDERR_FILENO);
						if (!tmp_fd) {
							perror(PROGRAM_NAME ": dup() failed");
							exit(1);
						}
						
						close(STDERR_FILENO);
						
						exit_status = system("espeak 'Shiltium exited.'");
						
						if (exit_status == -1) {
							cout << "system() failed\n";
							exit(1);
						}
						
						if (dup2(tmp_fd, STDERR_FILENO) == -1) {
							perror(PROGRAM_NAME ": dup2() failed");
							exit(1);
						}
						
						exit_status = WEXITSTATUS(exit_status);
						
						if (exit_status) {
							cout << PROGRAM_NAME ": espeak exited with an exit status of " << exit_status << ", which means it failed\n";
							exit(exit_status);
						}
					}
					
					exit(0);
				default:
					cerr << PROGRAM_NAME ": error: invalid message code " << message_code << '\n';
					exit(1);
			}
		}
	} else {
		/* Child */
		
		if (dup2(fds[1], STDOUT_FILENO) == -1) {
			perror(PROGRAM_NAME ": dup2() failed in child");
		}
		
		if (QUIET) {
			fclose(stderr);
		}
		
		execl("./shiltiumcomp", "./shiltiumcomp", "program.sl", NULL);
		perror(PROGRAM_NAME ": execl() failed");
	}
}

int main() {
	read_from_shiltiumcomp();
	return 0;
}
