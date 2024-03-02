/*
 * BSD 2-Clause License
 * 
 * Copyright (c) 2023, Gregory Cohen
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <string.h>

#ifdef __linux__
  #define fputs fputs_unlocked
  #define getchar getchar_unlocked
  #define putchar putchar_unlocked
  int fputs_unlocked(const char *s, FILE *stream);
#endif


#define HELP_STR_NO_COLOR "blue_bold_underline help\n\nTurns text into blue, bolded and underlined\n"
#define HELP_STR "\e[4m\e[1m\e[34m" HELP_STR_NO_COLOR "\e[0m\e[0m\e[0m"
#define FIRST_COLOR "\e[4m\e[1m\e[34m"
#define SECOND_COLOR "\e[0m\e[0m\e[0m"


int main(int argc, char** argv) {
  if (argc != 1) {
    fputs(FIRST_COLOR, stdout);
    
    for (int i = 1; i < argc; ++i) {
      if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
        fputs(HELP_STR, stdout);
      } else {
        fputs(argv[i], stdout);
        if (i != argc - 1) putchar(' ');
      }
    }

    putchar('\n');
    
    fputs(SECOND_COLOR, stdout);
    return 0;
  }
  
  fputs(FIRST_COLOR, stdout);

  for (;;) {
    const int c = getchar();

    if (c == EOF) { fputs(SECOND_COLOR, stdout); return 0; }
    putchar(c);
  }
}


