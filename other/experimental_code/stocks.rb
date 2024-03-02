#!/usr/bin/crystal
class String
  @@n = 5

  def clarity
    a = strip
    if @@n == 5
      @@n += 1

      #		system "google_speak " + a[0..98].dump
      #		system "espeak " + a.du
    end
    #	system "espeak
    # system "espeak" + " " + a.dump + "&"
    puts a.strip.colorize(:blue).bold
    #	system "espeak " + a.dump
  end
end

# end

def clarity(s)
  s.to_s.clarity
  # Best function name             ever...pure...ocean
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

def check_if_low(sp)
#	if ARGV.si
  sp[1..-3].each do |i|
    if i.to_f <= 2.0
    	if ARGV.size == 1
      raise BadStock.new
      end
    end
  end
end

#	end; end

def get_diff(i)
  i.split[3..4].reverse.diff
end

def process(n)
#if ARGV.size   != 1
if ARGV.size != 1
return ""
end


  a = n.strip.split("\n")      #.reverse
  #	pu,ts a
  #	a = a
  count = 0
  sp = a[-1].split
#  percent_sum = percent_sum2 = percent_sum3 = percent_sum4 = percent_sum5 = percent_sum6 =     0.0
  str = ""
  a.each do |i|
  
  
    check_if_low(sp)
    #			v=get_diff#).to_s + " "
#    v = get_diff i
#    percent_sum += v
    #		end
    #		end
  end

  if a[-1].split[1..4].uniq.size==1
  raise BadStock.new
 end

 begin

if sp[1].to_f < a[0].split[1].to_f
#if sp[1].to_f<a[0].split[1].to_ # < a[0]
	raise BadStock.new
end
rescue IndexError
	raise BadStock.new
end

if 0
end

# 

[300, 90, 150, 500, 1000, 400, 60].each do |days|
 begin

if sp[1].to_f<a[              -days].split[1].to_f # < a[0]
	raise BadStock.new
end
rescue IndexError
	raise BadStock.new
end
end

#(1..30).each do |day|
(1..50).each do |day|


begin
if sp[1].to_f   < a[-day].split[1].to_f
#if sp[1]<a[      -day].split[1].to # < a[0]
	raise BadStock.new
end
rescue IndexError
	raise BadStock.new

end
end



#return


  c = :cyan
  str = "overall 50 50 10 5 90"
  str += "%"
  str += " percent sum"
  str += [count, "had lower market close than open", count, "of", a.size].join " "
  #	enbd
  #	end
  str
end

class Array
  def prices # args=0
    # puts a
    count = 0
    args = [] of typeof("")

    each do |i|
    	count += 1
    	puts count
      begin
        if (!i.nil?)
          [i].old_prices

          args.push i # rescue : String  next
        end
      rescue e : BadStock
        #	puts "Not good stock"
        next
      end
    end

    while true
      a = "https://query1.finance.yahoo.com/v7/finance/quote?lang=en-CA&region=CA&corsDomain=ca.finance.yahoo.com&fields=symbol,marketState,regularMarketPrice,regularMarketChange,regularMarketChangePercent,preMarketPrice,preMarketChange,preMarketChangePercent,postMarketPrice,postMarketChange,postMarketChangePercent&symbols=" + args.join(',')
      puts args.size + "stocks being got"
      Process.run "curl", args: {"-s", a} do |p|
        a = p.output.gets_to_end
        # end
      end

      a = JSON.parse(a)
      # list = {} of typeof("") typeof(3.2)
      list = [] of typeof("")

      a = a["quoteResponse"]["result"].as_a

      a = a.sort do |a, b|
        k = "regularMarketChangePercent"
        begin
          a[k].as_f <=> b[k].as_f
        rescue
          a.to_s <=> b.to_s
        end
        #	a[k] <=> b[
      end
      rank = 0
      # puts "Most failing good stocks".upcase
      a.each do |i|
        rank += 1
        if (!i.as_h.has_key? "regularMarketPrice")
          next
        end
        r = rank.to_s
        r += "."
        s = i["symbol"] # r = rank.to_s

        list.push([
          [r, s, (i["regularMarketPrice"].as_f.to_s || (0.0/0).to_s)].join(" "), " ",
        ].join.colorize(:green).bold.to_s + # " failing " + \
                  i["regularMarketChangePercent"].to_s.colorize(:red).to_s)
        #	p = i["regularMarketPrice"]
      end

      # system "tput reset"
      print "\ec\e]104\a\e[!p\e[?3;4l\e[4l\e>"
      clarity "

The opening 9:30 a.m. to 10:30 a.m. Eastern time (ET) period is often one of the best hours of the day for day trading, offering the biggest moves in the shortest amount of time. A lot of professional day traders stop trading around 11:30 a.m. because that is when volatility and volume tend to taper off. Once that happens, trades take longer and moves are smaller with less volume. " # .colorize(:blue)

clarity "-" * 75

      clarity "Think of your spiritual freedom.... It's within your grasp...."
clarity "-" * 75
      clarity "


In the last hours of the trading day, volatility and volume increase again. In fact, common intraday stock market patterns show the last hour can be like the first—sharp reversals and big moves, especially in the last several minutes of trading. From 3 p.m. to 4 p.m. ET, day traders are often trying to close out their positions, or they may be attempting to join a late-day rally in the hope that the momentum will carry forward into the next trading day.

--Investopedia."

      puts list.join("\n")
    end
  end

  def old_prices
    # each do
    lines = [] of typeof("")

    to_a.each do |i|
    if ARGV.size == 1
    	if true
	      lines.push process(`tail /home/a/#{ARGV[0].to_s}/#{i} -n +3`)
	     end
      else
      
      lines.push process(`tail /home/a/stocks/#{i} -n +3`)
      end
    end

    return lines
    # end; l
  end
end

require "file_utils"

# include FileUtils

def main
if ARGV.size == 1
  FileUtils.cd ARGV[0]
  end
  l = %w[K
  BANFP
  DKL
  FOE
  ESQ
  ]
if ARGV.size == 1
  
  glob = Dir.glob("*").map do |i|
  if l.includes? i
  	nil
  else
  info = File.info i
  if info.size < 29
      nil
    else
      i
    end
    end
  end
  glob.compact!
  else
  	glob = ARGV
  end
  puts "Found all non-empty"

  # puts "Read globs"

  # if glob.nil?
  #	return
  # end
  glob.prices
  #		puts(k, v*1.05); end
  # end
  # exit
  # puts `grep -Fv Fewest $HOME/D | grep -F '1, 1, 1' | t i.split[0]`.split.prices
  # puts `grep -Fv Fewest $HOME/D | grep -F '1, 1, 1' | t i.split[0]`.split.old_prices
end

require "json"
require "colorize"
puts "Running"
main
