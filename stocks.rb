#!/usr/bin/crystal
#class Aray

class Array
	def diff
	a = self
		a[0].to_f - a[1].to_f
	end
end

def procedss(n)
	a =n.strip.split("\n").reverse
#	puts a
#	a = a
count=0
str=""
	a.each do |i|
		if i[0] >= '0' && i[0] <= '9'
		str+=(v=i.split[3..4].reverse.diff).to_s + " "
		if v.negative?
			count += 1
		end
		end
	end
	str += [count, "had lower market close than open", count,"of",a.size].join " "
	str
end

class Object
def prices#args=0
args = self
#puts 5
a = "https://query1.finance.yahoo.com/v7/finance/quote?lang=en-CA&region=CA&corsDomain=ca.finance.yahoo.com&fields=symbol,marketState,regularMarketPrice,regularMarketChange,regularMarketChangePercent,preMarketPrice,preMarketChange,preMarketChangePercent,postMarketPrice,postMarketChange,postMarketChangePercent&symbols=" + args.join(",")
#puts a
#puts a
#a =  Process.popen("curl -s '#{a}'").read
#Process.run
Process.run "curl", args: {"-s", a} do |p|
	a = p.output.gets_to_end
#end

end

#puts a


a = JSON.parse(a)
#list = {} of typeof("") typeof(3.2)
list=[] of typeof("")

a = a["quoteResponse"]["result"].as_a


a = a.sort do   |a, b|
	k = "regularMarketChangePercent"
	a[k].as_f <=> b[k].as_f
#	a[k] <=> b[
end
rank = 0
puts "Most failing good stocks".upcase
a.each do |i| rank += 1;	list.push  ([
		[rank,".",s=i["symbol"],  (i["regularMarketPrice"].as_f.to_s || (0.0/0).to_s)].join(""),              	"failing" ,       
	].join.colorize(:green).bold.to_s + #" failing " + \
	i["regularMarketChangePercent"].to_s.colorize(:red).to_s + [s].old_prices(20).join(" "))
#	p = i["regularMarketPrice"]
#	end
end

list
#puts list

#end
#end
#end
#end
end


def old_prices(n=5)
#each do
lines=[] of typeof("")

to_a.each do |i|

            lines.push       procedss(`tail /home/a/s/#{i} -n #{n}`)
end

return lines
#end; l    

end

end


def main
while (a = STDIN.read_line.split) != nil
	a.prices.each do |k|
	puts k;end
#		puts(k, v*1.05); end
end
puts `grep -Fv Fewest $HOME/D | grep -F '1, 1, 1' | t i.split[0]`.split.prices
puts `grep -Fv Fewest $HOME/D | grep -F '1, 1, 1' | t i.split[0]`.split.old_prices
end

require "json"
require "colorize"
main





