#!/usr/bin/ruby

def watch
Thread.new do |i|
args = ARGV.map do |i|
	i + ".TO" + "," + i + ".CN"
end

args = args.join(",")
puts args


while true
a = 'https://query1.finance.yahoo.com/v7/finance/quote?lang=en-CA&region=CA&corsDomain=ca.finance.yahoo.com&fields=symbol,marketState,regularMarketPrice,regularMarketChange,regularMarketChangePercent,preMarketPrice,preMarketChange,preMarketChangePercent,postMarketPrice,postMarketChange,postMarketChangePercent&symbols=' + args
#puts a
a =  `curl -s '#{a}'`

require "json"
#a =  JSON.parse(a)

#a["quote


a = JSON.parse(a)

a = a["quoteResponse"]
#puts a["result"]

#result = a[

result = a  ["result"]
	system "tput reset"
	puts Time.now
result.each do |i|
	#i = i.sortz
	if i["market"] != "us_market"
#	print i["market"], i["regularMarketPrice"]
	market = i["market"]
	p = i["regularMarketPrice"]
	puts "#{p} #{market}"
	puts i
	end
end

begin
end
	sleep 0.5
end
#end
end

end

watch
while true
sleep 2 ** 32
end