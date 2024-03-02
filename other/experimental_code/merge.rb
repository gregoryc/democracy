#!/usr/bin/ruby

hertz =48000
system "yes | ffmpeg -i #{ARGV.first} -ar #{hertz} one_temp.mp3"; puts "DONE"
system "yes | ffmpeg -i #{ARGV[1]} -ar #{hertz} -filter:a \"volume=0.1\" two_temp.mp3"; puts "DONE"

system "sox -m one_temp.mp3 two_temp.mp3 three_temp.mp3; mplayer three_temp.mp3";