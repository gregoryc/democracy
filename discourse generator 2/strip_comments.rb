#!/usr/bin/ruby
# frozen_string_literal: true

ARGV[0] || abort('Need arg')
`clang-format -i #{ARGV[0]}`

a = File.read ARGV[0]
a = a.gsub(/(#include.+?\n)+/) do |i|
  i = i.split("\n").sort do |a, b|
    b.size <=> a.size
  end
  i = i.join "\n"
  i += "\n\n"
end

a.gsub!(%r{/\*.+?\*/}m, '')
a.gsub!(%r{//.*?\n}m, "\n")
a.strip!

File.write ARGV[0], a
