#!/usr/bin/crystal

bads = ["BBC World News TV",
        "BBC World Service Radio",
        "Most read", "Top Stories", "Must see", "Full Story", "Most watched", "Breaking news", "Related content"]
lines = [] of String

a = `lynx -dump bbc.com/news`

a = a.gsub /Breaking(.+)\nAround the BBC/m do |i|
  $1.each_line do |line|
    line = line.chomp
    if line.empty?
      next
    end

    if line[0].whitespace?
    elsif bads.includes? line
    elsif line.match /^[Ww]atch/
    elsif line.match /^[Cc]ould/
    else
      lines << line
    end
  end
end

lines -= bads
lines.uniq!
if ARGV.size.zero?
  ARGV.push ""
end
require "colorize"

class String
  @@tty : Bool

  @@tty = STDOUT.tty?

  def to_color
    if @@tty #		if
      colorize(:red).to_s
    else
      self
    end
  end
end

lines.each_with_index(1) do |line, index|
  print [index, ". ", line, "\n"].join.to_color
  if ARGV[0] == "speak"
  	ln = line.gsub "'", "\\'"
    system "google_speak #{ln} 2>/dev/null"
  end
end
