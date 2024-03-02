#!/usr/bin/ruby
module Upwards

class List
	def initialize
		setup_nodes

		return self
	end

	def get_file_text
		require "json"
		json = JSON.parse(open("json").read)

		@keys = json.keys
		@values = json.values
		@length = json.length
		@json = json

		return json
		end

	def setup_nodes
		@nodes = Node.new(nil)
		@first = @nodes
		n = -1
		puts "Setting up"
		get_file_text
		
		@keys.each do |word|
			n += 1
			p word
			@nodes.add_node(word)
			@nodes = @nodes.get_cdr
		end
		@first.traverse_puts
	end
end

class Main
def initialize
	List.new
end
end

class Node
	def traverse_puts
		p "TP"
		node = self
		while true
			puts "Linked list"
			puts node.get_car;
			node = node.get_cdr

			if node.get_car.nil?
				puts "End"
				return node
			end
		end
	end

	def get_car
		@car
	end

	def get_cdr
		@cdr
	end

def add_node(value)
	@cdr = Node.new(value)
end
	def to_s
		return {"key" => @key, "value" => @value, "upwards": @upwards}
	end
	
	def initialize value
		if value == nil
			@cdr = nil
		else
			@key = value[0]
			@value = value[1]
			@upwards = String.new
			
			@car = value
			@cdr = Node.new nil;
		end
	end
end

#Main.new

end
require "json"


#open("j").open("important_file

all_keys = {}
$global_counter = 0
$av_counter = 0

def express_natural_containment_for_english_word(name, av,english_words)
	#puts "Natural containment of #{name} from EW and having av"

	av.each do |number_of_instances, contained_words|
		#$av_counter += 1
		if number_of_instances == 0
			puts "Number of instances == 0"
			exit 1
		end
		contained_words.each do |word|
			if english_words.has_key? word
				#puts word, english_words[word], number_of_instances
				english_words[word] += number_of_instances.to_i
				#puts "haske"
				$global_counter += 1
				if $global_counter % 1000000 == 0
					puts "Mod 1 million"
				end
			end
		#end
		end
	end
end

JSON.parse(open("json").read).keys.each { |key| all_keys[key] = 0 }

if all_keys.length != 42195
	puts "Error"
	exit 1
end

'''
open("important_file").each_line do       |line|

json = JSON.parse(line)
$av_counter += 1
key, value = json["name"], json["data"]
#puts all_keys.include? key

express_natural_containment_for_english_word(key, value, all_keys)
#puts json
if $av_counter % 50 == 0
	puts "#{$av_counter}th is done"
end
end



File.open("amazing_output", "w") do |file|
file.puts all_keys
end

File.open("amazing_output_values", "w") do |file|
file.puts all_keys.values
end

File.open("amazing_output_keys", "w") do |file|
file.puts all_keys.keys
end

File.open("amazing_output_length_of_keys", "w") do |file|
file.puts all_keys.keys.length
end

File.open("amazing_output_p_not_puts", "w") do |file|
file.p all_keys.values
'''
e



class HTMLMaker
end

class CSSMaker
end

class AutoSorter
end

class IntegrityVerifier
end

class PDFMaker
end

class PDFCoverMaker
