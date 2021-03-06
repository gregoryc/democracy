#!/usr/bin/ruby
# frozen_string_literal: true
bg = "#EEEEEE"

num=`cd $HOME/democracy/bin/; lines`


def p a
end



class String
def generate_encoding_data(encoding)
require 'base64'
b = "data:#{encoding};base64,#{Base64.strict_encode64(File.read(self))}".dump
end

def generate_image_data
generate_encoding_data 'image/png'
#	generetae
end
end

system 'which convert >/dev/null || echo Need convert'
eval(%w[Dir.chdir Dir.glob File.write].map do |i|
<<~EOF
def #{i.split('.').pop}(*args)
#{i}(*args)
end
EOF
end.to_a.join)

require 'colored'
require "shellwords"
colors_list = ''.colors.reject { |i| i == "yellow" || i == 'white' } * 1000
a = ''
require 'fileutils'
# include FileUtils
system "(find | grep -E '(\.(html|png)$)|--help' | args rm) 2> /dev/null"
glob('*').sort.each do |i|
orig = i
next if test 'd', i

p i
i = i.capitalize.gsub(/-|_/, ' ').dump
color = colors_list.pop
f = "#{orig}.png"

p `convert -size 800x120  xc:#{bg} -font Ubuntu   \
-pointsize 50 -fill '#222222' -annotate +10+40 #{i} \
-fill #{color} -annotate +8+38 #{i} -trim \
+repage #{f}`
# p e
i = i.undump
<<EOF
puts i.send color
EOF
#orig = "echo #{i}" if /Open|Ac|Email|(exe$)/.match?(i)

image_data = f.generate_image_data
FileUtils.rm f

image = "<img src=#{image_data}></img>"
#{`(timeout 0.1s ./#{orig} --help --help --help) 2>/dev/null`.gsub("\n", '<br/>')}


a += "<hr style='border: 0px; padding: 0px; display:   block; width: 100%;background-color:black;height:1px;'>

<p>#{image}<br/>
#{`cat ../documentation/#{orig.downcase.shellescape}`.gsub(/\n\n+/, "\n\n").prepend("\n\n").gsub("\n", "<br>").strip}

</p>"
end

a = <<EOF
<html>
<head>
<title>Tools</title>
<script data-ad-client="ca-pub-1541978815872006" async src="https://pagead2.googlesyndication.com/pagead/js/adsbygoogle.js"></script>
</head>


<body style='padding:10;margin:0;width:100%;font-size: 90%; background-color: #{bg}'>
<h1 style='font-size: 50px; text-align:center;text-shadow: 3px 3px 3px black; color:gold;'>Practical self-empowerment utilities covering every facet of life</h1>
<h2 style='font-family:Sans;text-align:center;text-shadow: 3px 3px 3px white'>These tools are for everyone</h2>
<h2 style='font-family:Sans;text-align:center;text-shadow: 3px 3px 3px white'>It doesn't matter what political ideology you favor, these tools are all still very valuable. You can live in Canada, the US, Russia, China or Africa, it doesn't matter.  </h2>
<h2 style='font-family:Sans;text-align:center;text-shadow: 3px 3px 3px white'>I intend to make people more able.</h2>
<hr>

<pre>
<iframe width='100%' height='100%' src='https://www.youtube.com/embed/ojzPxEKqldk' title='YouTube video player' frameborder='0' allow='accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture' allowfullscreen></iframe>


This lists all of the programs. The most important might be tb, see <a href=\"https://www.youtube.com/GregoryCohen1\">Democracy YouTube Channel (youtube.com/gregorycohen1)</a> and <a href=https://facebook.com/democracygregoryc>facebook.com/democracygregoryc</a>

There is also my old semantic metadata project, which is a valid idea, and very relevantly, the discourse generator

(which is a work in progress at ethicify.online and is somewhat bugged right now)


There is also a desktop widget for that site, and old random sentence generator (useless) and an old compiler for my own programming language.

Those aside, there are these <span style=color:lightblue>#{$num}   programs</span>


Most important are <b>c_program.exe</b> (<b>do_work</b> is an alternate name) and
and the <b>work-in-progress discourse generator</b>, but that's stored in a separate folder.
</pre>

<h1> All of these programs are mostly self-contained. This is a really good thing.</h1>

<h1>Few or no dependencies!</h1>

<p>My email is <a href=mailto:gregorycohen@gmail.com>gregorycohen2@gmail.com</a> and I really would like to make open source software. Everything I have is BSD licensed.</p>

<b><i>
If this software gets you a job or makes you money or improves your life or makes things easier for you, please consider donating. I have released this software under the BSD license (a very liberal license) for everyone to use and modify. I would appreciate it greatly if some people could return the favor :)</i></b>

<div>
<ul>
<li style=font-family:Sans><a href=http://www.github.com/gregoryc/democracy>Democracy  gem github</a></li>  <br/>
<li style=font-family:Sans><a href=http://www.rubygems.org/gems/democracy>Gem with 12,000 downloads</a></li>
</ul>
</div>


<h1>Without further ado, here are the #{$num} documented programs.</h1>


#{a}</body></html>

EOF
open('index.html', 'w') do |d|
d.print a
end

# `mv a.html ../DOCUMENTATION.html


system "google-chrome index.html"