#!/usr/bin/ruby
class String
def end_symbol; a = capitalize

#	a += " "
	if branch
		a += "."
	else
		if branch
			a += "!"
		else
			if branch
				a += "!"
			end
			a += "!"
		end
	end; a
end
end

require 'json'
require 'colored'
require 'fileutils'
require 'gtk3'
#class TextView.
#class TextView
#	def init
class Window	< Gtk::Window
	def initialize
		super()
		add TextView.new

		title = "RSG"

		signal_connect "destroy" do |i|
			puts "Bye" rescue 1
			Gtk.main_quit
			exit 0
		end
		set_size_request 400, 400

		show_all
		Gtk.main
	end
end


class TextView<Gtk::TextView
def initialize
	super()
		#set_size_request 400, 400
			self.buffer.text = get_text.to_s
					set_size_request 400, 400
end

end

class String
	def from_json
		JSON.parse self
	end
	def add_article; if rand < 0.5

		list = "aeiou".chars
		if list.include? self[0].downcase
			art = "an"
		else
			art = "a"
		end
		else
			art = "the"
		end

		text = String.new
		text << art
		text << " "
		text << self
	end

#	def add_article
#		if self[0].downcase.match /a|e|i|o|u/
#			return 
end

class String
def file
	a = self
	a += ".json"
	t = JSON.parse(File.read a)[self]
end
end



def nv_pair(nouns = true, verbs = true)
	if rand < 0.5
		i = "past"
		present = false
	else
		 i = "present"
		 present = true
	end

	a = :nouns.to_s.file.sample
	b = :verbs.to_s.file.sample[i]
if present
	b = "wanted to " + b
end

	result = ""
	if nouns
	result << a.add_article
	end; if verbs&&nouns
		result << " "
	end
	if verbs
		result << b
	end

	result
	
end

def branch
	rand < 0.5
end


def get_text n=20
	a = ""
	n.times do
	if branch
		a += :interjections.to_s.file.sample.end_symbol + " "
	end;


	l = ((0..((rand  * 3).to_i)).to_a.map do
		[nv_pair, nv_pair(true,false)].join " %s " % :prepositions.to_s.file.sample
	end)

	i = 0
	while i < l.length - 1
		l[i] += ", " + :conjunctions.to_s.file.sample + " "
	i += 1; end
#	end
	a += (l.join '').capitalize
	a = a.end_symbol + "\n"
	end
	a

		require 'shellwords'
	Thread.new do |i|
		IO.popen("espeak", "w") do |i|
			i.puts a
		end
	end



	return a
end
		

#	a += rand < 0.5 ? 
#end

Window.new