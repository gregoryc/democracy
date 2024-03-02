#!/usr/bin/env ruby
# frozen_string_literal: true

begin
 Dir.chdir '/home/a/democracy/bin'
rescue StandardError
 nil
end

def speak(a)
 puts a.green
 require 'shellwords'
 return unless false

 system “espeak #{a.shellescape}”
end

<<~EOF
 speak “Documenting”
EOF

puts Dir.pwd

Dir.glob('*') do |file|
 case file
 when /DOC|sh$/
 next
 when /YOUTUBE_|LINUX/
 next
 end
 next if File.exist?("../documentation/#{file}") || File.exist?("../documentation/#{file}.exe")

 program = file
 require 'FileUtils'.downcase
 include FileUtils

 mkdir_p '../documentation'
 cd '..'
 cd 'documentation'

 def get_line
 require 'readline'
 $line = ok_or_no = Readline.readline.strip
 end
 $file = file
 $n = 0

 require 'colored'

 include FileUtils
 # require "coloed
 loop do
 cd Dir.home
 cd 'democracy'
 cd 'bin'
 puts "Enter description of the practical utility of `#{file}'"

 speak "#{file} tool"
 $doc = $stdin.read
 speak $doc
 speak 'Is this good?'

 FileUtils.mkdir_p '../documentation'
 FileUtils.cd '../documentation'

 def g
 $n += 1
 return unless get_line.match?(/y|Y/); # speak “On to the #{$n}th one”

 # speak “yes”
 File.open($file, 'a') do |f|
 puts pwd
 puts $file
 f.puts $doc
 end

 raise 'End'
 end

 begin
 g
 rescue Object
 break
 end

 # puts program
 end
end