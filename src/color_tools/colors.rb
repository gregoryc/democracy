#!/usr/bin/ruby
# frozen_string_literal: true

time = Time.new

LIST = %w[%s_underline %s_bold %s_bold_underline].freeze

if ARGV[0] == "clean"
  puts "Cleaning and exiting"
  Dir.glob("*") do |i|
    if i.end_with? ".rb"
      puts i
    else
      File.unlink i
    end
  end
  exit 0
end

ENDR = "  }
}


"

FIRST = "/*
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\"
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


#define HELP_STR_NO_COLOR \"%s help\\n\\nTurns text into %s\\n\"
#define HELP_STR %s HELP_STR_NO_COLOR %s
#define FIRST_COLOR %s
#define SECOND_COLOR %s


int main(int argc, char** argv) {
  if (argc != 1) {
    fputs(FIRST_COLOR, stdout);
    
    for (int i = 1; i < argc; ++i) {
      if (!strcmp(argv[i], \"-h\") || !strcmp(argv[i], \"--help\")) {
        fputs(HELP_STR, stdout);
      } else {
        fputs(argv[i], stdout);
        if (i != argc - 1) putchar(' ');
      }
    }

    putchar('\\n');
    
    fputs(SECOND_COLOR, stdout);
    return 0;
  }
  
  fputs(FIRST_COLOR, stdout);

  for (;;) {
    const int c = getchar();

    if (c == EOF) { fputs(SECOND_COLOR, stdout); return 0; }
"

N = 1

COLORS = %w[black blue cyan green magenta red white yellow].freeze

require "colored"

COLORS.each do |color|
  $file = File.open("#{color}.c", "w")

  list = "TXT".send(color).split("TXT")
  $file << (FIRST % ([color, color, list[0].dump, list[1].dump, list[0].dump, list[1].dump] * N))

  list.each_with_index do |c, index|
    $file << "    putchar(c);\n" if index == 1
  end

  $file << ENDR
  $file.close
end

COLORS.each do |color|
  list = "TXT".send(color).underline.split("TXT")
  c2 = "#{color}, underlined"
  $file = File.open((LIST[0] % (color)) + ".c", "w")
  $file << (FIRST % ([LIST[0] % (color), c2, list[0].dump, list[1].dump, list[0].dump, list[1].dump] * N))
  list.each_with_index do |c, index|
    $file << "    putchar(c);\n" if index == 1
  end

  $file << ENDR
  $file.close
end

COLORS.each do |color|
  c2 = "#{color}, bolded"
  list = "TXT".send(color).bold.split("TXT")
  $file = File.open((LIST[1] % (color)) + ".c", "w")
  $file << (FIRST % ([LIST[1] % (color), c2, list[0].dump, list[1].dump, list[0].dump, list[1].dump] * N))

  list.each_with_index do |c, q|
    $file << "    putchar(c);\n" if q == 1
  end

  $file << ENDR
  $file.close
end

COLORS.each do |color|
  color2 = "#{color}, bolded and underlined"
  list = "TXT".send(color).bold.underline.split("TXT")
  $file = File.open((LIST[2] % (color)) + ".c", "w")
  $file << (FIRST % ([LIST[2] % color, color2, list[0].dump, list[1].dump, list[0].dump, list[1].dump] * N))

  list.each_with_index do |c, index|
    $file << "    putchar(c);\n" if index == 1
  end

  $file << ENDR
  $file.close
end

%w[bold underline].each do |type|
  list = "TXT".send(type).split("TXT")
  $file = File.open("#{type}.c", "w")
  $file << (FIRST % ([type, type, list[0].dump, list[1].dump, list[0].dump, list[1].dump] * N))

  list.each_with_index do |c, index|
    $file << "    putchar(c);\n" if index == 1
  end

  $file << ENDR
  $file.close
end

txt = +""

glob = Dir.glob("*.c").sort

glob.each do |file|
  name = file.sub(/\.c$/, "")
#  txt << "(musl-gcc -O2 --static -s -fwhole-program -Wall -Wextra #{file} -o #{name} && cp -v #{name} ../../bin/#{name}) || "
  txt << "(gcc -O3 -s -fwhole-program -Wall -Wextra #{file} -o #{name} && cp -v #{name} ../../bin/#{name})\n"
end
#abort txt

IO.popen("sh", "w") do |i|
  i << txt
end

time2 = Time.new

puts "Took #{time2 - time} seconds for #{glob.size}"
