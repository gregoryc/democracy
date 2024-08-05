#!/usr/bin/ruby
require "cgi"

a=STDIN.read.strip
if a == "."
	a = Dir.pwd
else
	a=a.gsub /^file:\/+/, ''
	a=a.gsub("./",Dir.pwd)
	a="/"+a
end

warn CGI.unescape(a)
p=( File.realpath(a) rescue CGI.unescape(a))

print p
