#!/usr/bin/env ruby
def html topic, num, link
a=<<EOF
<!DOCTYPE html>
<html><head><style type='text/css'> div {
display: inline-block;
left: 0px;
position: absolute;
	font-family: Sans;
	font-size: 300%;
	width: 100%;
	border: 3px;
	text-align: center;
	background-color: white;
	opacity: 0.5;
//	display: inlin
}
#text{top: 40%; }
#text2{top: 60%; }
img {
	width: 100%;
	height: 100%;
}
</style>
</head><body><img src=#{link.dump} alt='Not working'/><div id='text'>#{topic}</div><div id='text2'>By #{topic} person</div></body></html>
EOF
b = topic + "_" + num.to_s
b += ".html"
File.write(b, a.strip) rescue true
end


require 'shellwords'
#require                            ; require 'colored'
require 'json'
#require 'openuri'
#require 'puppeteer-ruby'




class Main
	def main
	end


def main
	list = {}
#	puts "Main"
	Dir.glob '*' do |i| case i; when /r\.rb/; next; end;; puts i; text = File.read i; list[i] = JSON.parse(text)
#		puts i
#		puts "done"
#		text = File.re
#break
#		File.read i     
	end
	$list = list
	File.open("../out.json", "w") do |i|
		i.puts list.to_json
	end
#	puts $list
	puts "PUBLIC DOMAIN REPL"
	while true
		begin
		puts eval(STDIN.read)
		rescue => e
			puts e
		end
	end
end
end


a = Main.new

class String
	def download n=1
	Dir.chdir Dir.home
	Dir.mkdir "IMAGE_DOWNLOADS" rescue puts "Image folder exists"
	Dir.chdir "IMAGE_DOWNLOADS"
	
		puts "Downloading"
		puts $list.values.length
#		puts $list
#		num = 0
num = 0
		$list[self].each do |i|; if num == n
			break
		end
			num += 1
#			of 
f = self + "_" + num.to_s
			system "wget #{i.shellescape} -O #{f.shellescape}"
			if (File.read f).length < 100
				File.unlink f rescue             true
				num -= 1
			end

			html self, num, f  

			<<EOF
					html self, f, 
EOF
		end
		
#			end		end
		puts "Done"
	end
end
a.main