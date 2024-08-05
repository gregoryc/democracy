#!/usr/bin/ruby
a=STDIN.read.strip


a=a.split("/").last.split(".")[0].gsub("_", " ")
print a
