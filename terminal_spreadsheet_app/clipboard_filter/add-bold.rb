#!/usr/bin/ruby

t=STDIN.read
IO.popen('espeak', 'w') do |q|
	q<<"You just copied " << t
end


puts "<b>"+t+"</b>"
