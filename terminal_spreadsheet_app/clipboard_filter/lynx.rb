#!/usr/bin/ruby




a = STDIN.read
IO.popen('lynx -dump -stdin|trim', 'w') do |q|
	q << a
end
