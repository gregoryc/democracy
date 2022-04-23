#!/usr/bin/crystal
if ARGV.size==0
abort
#	return
end
#	puts "Needs
def watch
while true
#puts 5
args = ARGV

a = "https://query1.finance.yahoo.com/v7/finance/quote?lang=en-CA&region=CA&corsDomain=ca.finance.yahoo.com&fields=symbol,marketState,regularMarketPrice,regularMarketChange,regularMarketChangePercent,preMarketPrice,preMarketChange,preMarketChangePercent,postMarketPrice,postMarketChange,postMarketChangePercent&symbols=" + args.join(",")
#puts a
#puts a
#a =  Process.popen("curl -s '#{a}'").read
#Process.run
Process.run "curl", args: {"-s", a} do |p|
	a = p.output.gets_to_end
#end

end

puts a
	if Time.local.hour >= 17
		system "espeak Market close"
	end
#end
#end
end

end

def main
watch
end

require "json"
main