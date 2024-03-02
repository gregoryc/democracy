#!/usr/bin/ruby
# frozen_string_literal: false
require "shellwords"
NAME = "DISCOURSE_GENERATOR"
WIN_NAME = NAME + ".exe"
pwd = Dir.pwd
Dir.chdir "/tmp"
raise "BAD " unless Dir.pwd.include? "/tmp"
file = "#{Dir.home.shellescape}/file" << " "

if ARGV[0] != "skip"
  _2_file = pwd + "/" + "a.cc"
  _2_file = _2_file.shellescape

  cmd = "cd #{pwd.shellescape}; ruby 'strip_comments.rb' './a.cc'   || exit 1
/usr/lib/mxe/usr/bin/x86_64-w64-mingw32.static-g++ -xc++ -O2 -s -fno-exceptions -fwhole-program -fno-rtti #{_2_file} -std=c++17 -xc++ -Wall -Werror -o #{WIN_NAME} || exit 1   &
g++ -O2 #{_2_file} -s `pkg-config --libs --cflags readline` -fwhole-program  -fno-exceptions -fno-rtti -std=gnu++17 -Wall -Wextra -Werror -Wfatal-errors -o #{NAME} || exit 1
wait
"
  puts cmd
  system cmd, exception: true unless ARGV[0] == "no_compile"

  BADS = ["  ", ",,", ";;", " ;", "..", " ."].freeze
  NUM = ARGV[0].to_i rescue 50
  NUM.times do |i|
    print "                #{i}\r" if (i % 1).zero?

    cmd = "#{pwd.shellescape}/#{NAME}  #{file * 2000} > /tmp/out 2>&1" #,exception: true
    #  puts cmd
    system cmd, exception: true
    a = File.read "/tmp/out"
    #  puts a[-10_000..-1]

    if a.size < 10_000
      raise a
    end
    includes_bads = false
    BADS.each do |i|
      includes_bads = a.include? i
      if includes_bads
        puts "Bad"
        exit 1
      end
    end
    # puts "Good"
  end
  puts "Succ #{NUM}, no #{BADS.inspect}"
end

system "rm *txt *valid 2>/dev/null"; cmd = "mv --verbose '#{pwd}/#{WIN_NAME}' '#{pwd}/../bin'"; puts cmd; system cmd
cmd2 = "mv --verbose '#{pwd}/#{NAME}' '#{pwd}/../bin'"; puts cmd2; system cmd2; if ARGV[0] != "skip"; 50.times { puts :Good }; end
