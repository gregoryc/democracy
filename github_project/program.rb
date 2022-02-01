#!/usr/bin/ruby
require "json"

class String
def lines_of_words

	split(/\s+/)#.select {           |qual| qual          =~ /^[A-Za-z'üéèöáóâçêñ]+$/ }
end
end

def main
	a = `find WF_FOLDER -type f`.strip.split("\n")

#puts a



puts "Doing #{ARGV[0].to_i + 1} times"


(ARGV[1].to_i).times do |no|
	no+=ARGV[0].to_i
	file = File.open("ranges/1_through_#{no + 1}_inclusive_stripped", "w")
	a.each do |file_of_word|
		#STDERR.puts 
#		lin
		map = {} of String => UInt64
		map2 = {} of UInt64 => Array(String)
#		p file_of_word
		
		File.open(file_of_word).gets_to_end.lines_of_words.each do |word|
			#map[word] += 1 if !(map[word].nil?)

			if !map.has_key?(word)#.nil?
				map[word] = 1
			else
				map[word] += 1
			end
		end

#		p map.length


#		map.each do |keys,
		map.each do |key, value|
				if !map2.has_key?(value)
					map2[value] = [] of String
				end
				map2[value].push(key)
		end

		no.times do |___|
			map2.delete(___ + 1)
		end
#		p map2.length
		f = file_of_word.split("/")[-1]

		hash = {"name": f, "data": map2.to_a.sort.reverse.to_h}

		file.puts hash.to_json

		
		
#			ma
	end; file.close;
end
#end
end

main
