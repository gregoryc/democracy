#!/usr/bin/crystal
#class Aray
#Best function name ver...pure...ocean
#def clarity
def clarity(a)
	puts a.strip.colorize(:blue).bold
end


class Object
def +(obj : String)
	to_s + obj
end
end


class BadStock < Exception
end


class Array
	def diff
	a = self
		a[0].to_f - a[1].to_f
	end
end

def check_if_low(i)
i.split[1..-3].each do |i|
		if i.to_f <= 2.0
			raise BadStock.new
		end
	end
end


#	end; end

def get_diff(i)
i.split[3..4].reverse.diff
end


def process(n : String)
#def procedss(n : Strin)
	a =n.strip.split("\n").reverse
#	puts a
#	a = a
count=0
percent_sum = percent_sum2 = percent_sum3 = percent_sum4 = percent_sum5 = 0.0
str=""
	if ARGV.size > 0 && ARGV[0] == "q"
		q = true
	end
	a.each do |i|
		check_if_low(i)
#			v=get_diff#).to_s + " "
			v = get_diff i
						percent_sum += v
#		end
		if v.negative?
			count += 1
		end
#		end
	end

	last = a.last 50

#	last.each do 


	  last.each do |i|
	  	percent_sum2 +=    get_diff i
	  end

	last = a.last 5

#	last.each do 


	  last.each do |i|
	  	percent_sum3 +=    get_diff i
	  end

	last = a.last 10

#	last.each do 


	  last.each do |i|
	  	percent_sum4 +=    get_diff i
	  end

	last = a.last 90

#	last.each do 


	  last.each do |i|
	  	percent_sum5 +=    get_diff i
	  end

	  
	c = :cyan
	l = [percent_sum, percent_sum2, percent_sum3, percent_sum4, percent_sum5
	]

	l.each do |i|
		if i.negative?
raise BadStock.new #			raise BadStock
		end
	end
	str += "overall 50 50 10 5 90"
str += "%"
	str += " percent sum"
	str += [count, "had lower market close than open", count,"of",a.size].join " "
#	enbd
#	end
	str
end

class Array
def prices#args=0

#puts a
args = [] of typeof("")



each do |i|
begin
if(!i.nil?)
[i].old_prices

args.push i# rescue : String  next
end
rescue e : BadStock
#	puts "Not good stock"
	next
end

end


while true
a = "https://query1.finance.yahoo.com/v7/finance/quote?lang=en-CA&region=CA&corsDomain=ca.finance.yahoo.com&fields=symbol,marketState,regularMarketPrice,regularMarketChange,regularMarketChangePercent,preMarketPrice,preMarketChange,preMarketChangePercent,postMarketPrice,postMarketChange,postMarketChangePercent&symbols=" +               args.join(',')
puts args.size + "stocks being got"
Process.run "curl", args: {"-s", a} do |p|
	a = p.output.gets_to_end
#end

end


a = JSON.parse(a)
#list = {} of typeof("") typeof(3.2)
list=[] of typeof("")

a = a["quoteResponse"]["result"].as_a


a = a.sort do   |a, b|
	k = "regularMarketChangePercent"
	begin
	a[k].as_f <=> b[k].as_f
	rescue
		a.to_s <=>b.to_s
	end
#	a[k] <=> b[
end
rank = 0
puts "Most failing good stocks".upcase
a.each do |i|


rank += 1;	

if (!i.as_h.has_key? "regularMarketPrice"); next; end
s=i["symbol"]

list.push  ([

		[rank,".",s,  (i["regularMarketPrice"].as_f.to_s || (0.0/0).to_s)].join(""),              	"failing" ,       
	].join.colorize(:green).bold.to_s + #" failing " + \
	i["regularMarketChangePercent"].to_s.colorize(:red).to_s)
#	p = i["regularMarketPrice"]
end



#system "tput reset"
print "\ec\e]104\a\e[!p\e[?3;4l\e[4l\e>"
clarity "

The opening 9:30 a.m. to 10:30 a.m. Eastern time (ET) period is often one of the best hours of the day for day trading, offering the biggest moves in the shortest amount of time. A lot of professional day traders stop trading around 11:30 a.m. because that is when volatility and volume tend to taper off. Once that happens, trades take longer and moves are smaller with less volume. "#.colorize(:blue)

clarity "Think of your spiritual freedom.... It's within your grasp...."
clarity "


In the last hours of the trading day, volatility and volume increase again. In fact, common intraday stock market patterns show the last hour can be like the first—sharp reversals and big moves, especially in the last several minutes of trading. From 3 p.m. to 4 p.m. ET, day traders are often trying to close out their positions, or they may be attempting to join a late-day rally in the hope that the momentum will carry forward into the next trading day. 

"

puts list.join("\n")
end


end






def old_prices()
#each do
lines=[] of typeof("")

to_a.each do |i|

            lines.push       process(`tail /home/a/s/#{i} -n +3`)
end

return lines
#end; l    

end

end
require "file_utils"
#include FileUtils      

def main
	FileUtils.cd "/home/a/s/"
glob = Dir.glob("*").map do |i|
	if File.read(i).size < 29
		nil
	else
		i
	end
end
glob.compact!

puts "Read globs"

#if glob.nil?
#	return
#end
glob.prices
#		puts(k, v*1.05); end
#end
#exit
#puts `grep -Fv Fewest $HOME/D | grep -F '1, 1, 1' | t i.split[0]`.split.prices
#puts `grep -Fv Fewest $HOME/D | grep -F '1, 1, 1' | t i.split[0]`.split.old_prices
end

require "json"
require "colorize"
main

