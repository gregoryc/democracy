#!/usr/bin/ruby
require 'colored'

ARGV.each do |file|
  lines = File.read(file)

  lines.chomp!

  warn file.to_s.red

  (lines = lines.gsub(/^(?<=[^\*\/]){6}\n\s*class ([A-Z_][a-zA-Z0-9_]+)/) do |f|
    "\n/**" + ("\nThe " + $1).gsub("\n", "\n * ") + " class.\n */\n#{f.strip}"
  end)

  (lines = lines.gsub(/(?<=[^\*\/]){6}\n\s*((?:public|private|protected|function|static|final)\s+)+([a-zA-Z0-9_]+)\s*\((.*?)\)/m) do |f|
  	warn f.to_s.red
    parts = $3.split(",").map do |i|
      i.strip!
      "\n * @param #{i} The #{(i.split[1]&.reverse&.chomp("$")&.reverse) || i.to_s}"
    end.to_a
    ("\n\n/**" + ("\nThe " + $2).gsub("\n", "\n * ") + " method.#{parts.join}\n * \n * @return void\n */#{f}").gsub("\n", "\n    ")
  end)

  abort 'ZERO' if lines.size.zero?
  warn lines.size.to_s.red

  lines.gsub! "\t", "    "

  puts lines
#  abort

  File.write file, lines

  system "phpcbf #{file}"

  lines = File.read(file)

  lines = lines.gsub(/\*\/[ \t]*\n\n+/, "*/\n")

  
  File.write file, lines
end