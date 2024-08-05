def add_zero(a : String) : String
  if a.size == 1
    "0" + a
  else
    a
  end
end

DATE  = Time.local
DATE2 = "#{DATE.year}-#{add_zero(DATE.month.to_s)}-#{add_zero(DATE.day.to_s)}"
puts "<style>* { font-family: Monospace;}   </style>"
puts "<h1>Generated on #{DATE2}</h1>"
DAYS = [10, 5, 3, 20]

def put(a)
  puts a.gsub("\n", "<br>") + "<br><br>"
end

class Array
  def find_files
    map do |i|
      "us_stock_list/#{i}.csv"
    end.to_a
  end

  def average
    sum.to_f64 / size
  end
end

if ARGV[0] == "0"
  ARGV.shift
  ARGV.push "-1"
end

lists = [Dir.glob("us_stock_list/*.csv").sort[0..ARGV[0].to_i]] + [["GOOG", "MSFT",
                                                                    "TSLA",
                                                                    "NVDA",
                                                                    "META", "AMZN", "AAPL"].find_files] * 3 + [["NVDA"].find_files]*6
desc = ["All stocks", "Hot stocks", "Hot stocks - last 30 Market Days", "Hot stocks - last 120 Market days", "NVIDIA - 7 Market days", "NVIDIA - 3 Market days", "NVIDIA - 21 Market days", "NVIDIA - 4 Market days", "NVIDIA - 5 Market days", "NVIDIA - 700 Market days"]
durations = [-1, -1, 30, 120, 7, 3, 21, 4, 5, 700]

lists.each do |list|
  count = 0
  didntGoBackDownTimes, didGoBackDownTimes = 0, 0
  currentGreaterThanOldCentre, currentSmallerThanOrEqualToOldCentre = 0, 0
  currentLowSmallerThanOrEqualToOldLow, currentLowGreaterThanOldLow = 0, 0
  currentHighSmallerThanOldLow, currentHighNotSmallerThanOldLow = 0, 0
  currentHighSmallerThanOldLow2, currentHighNotSmallerThanOldLow2 = 0, 0
  currentHighSmallerThanOldLow3, currentHighNotSmallerThanOldLow3 = 0, 0
  currentHighSmallerThanOldLow4, currentHighNotSmallerThanOldLow4 = 0, 0
  stocksize = durations.shift
  average_swing_ratio = [] of Float64
  open_larger_than_low = [] of Float64
  high_low_pairs = [] of Array(Float64)
  first_dates = [] of Array(String)
  old_lows = [] of Float64
  find_times = [] of Float64
  find_times2 = [] of Float64
  find_times3 = [] of Float64
  find_times4 = [] of Float64

  list.each_with_index do |q, list_index|
    first_date = nil
    STDERR << "\r#{q} #{list_index + 1}                         "
    STDERR.flush
    current = q
    olddate, oldopen, oldhigh, oldlow, oldclose, oldadjClose, oldvolume = nil, nil, nil, nil, nil, nil, nil
    l = File.read(current).strip.split("\n")[1..].reverse[0..stocksize - 1].reverse

    raise [l.size, stocksize].to_s unless l.size == stocksize || stocksize == -1
    if l.size == 0
      next
    end
    count += 1
    l.each do |i|
      commas = i.split(',')
      begin
        date, open, high, low, close, adjClose, volume = commas
      rescue
      end
      if first_date.nil?
        first_date = date
      end

      unless date && open && high && low && close && adjClose && volume
        raise l.inspect
      end
      if open.to_f == 0.0 || high.to_f == 0.0 || low.to_f == 0.0
        next
      end

      open_div_low = open.to_f64 / low.to_f64
      high_low_pairs.push [low.to_f64, high.to_f64]
      if open_div_low > 1.0
        open_larger_than_low.push open_div_low
      end

      average_swing_ratio.push(val = (high.to_f64 - low.to_f64) / high.to_f64) # If low is 0, then 1.0. If high is low, then 1.0 / high (near zero)

      if oldlow && oldhigh
        diff = low.to_f64 - oldhigh.to_f64
        if diff > 0
          didntGoBackDownTimes += 1
          # Is the current day's low greater than the old day's high?
        else
          didGoBackDownTimes += 1
          # Is the current day's low less than the old day's high? (It went back down?)
        end

        diffSecond1 = [low.to_f64 - (oldhigh.to_f64 + oldlow.to_f64)/2, low.to_f64 - oldlow.to_f64].min # Is today's low lower than the centre point of yesterday's range?
        #     puts diffSecond1
        #        diffSecond2 = high.to_f64 - (oldhigh.to_f64 + oldlow.to_f64)/2 # Is today's high lower than the centre point of yesterday's range?
        if count == 30
          #        puts [diffSecond1, low.to_f64, oldhigh.to_f64, oldlow.to_f64, (oldhigh.to_f64 + oldlow.to_f64)/2].inspect
          # Example [0.02500152587890625, 24.950000762939453, 25.049999237060547, 24.799999237060547, 24.924999237060547]
          #       exit
        end
        raise "WTF" if low.to_f64 == 0.0
        # Low is greater than 0 meaning low is higher than center
        if diffSecond1 > 0 # || diffSecond2 < 0
          currentGreaterThanOldCentre += 1
        else
          currentSmallerThanOrEqualToOldCentre += 1
        end

        diffSecond2 = low.to_f64 - (oldlow.to_f64) # Is today's low lower than the old low of yesterday?
        #        diffSecond2 = high.to_f64 - (oldhigh.to_f64 + oldlow.to_f64)/2 # Is today's high lower than the centre point of yesterday's range?
        #        puts [diffSecond, low.to_f64, oldhigh.to_f64, oldlow.to_f64, (oldhigh.to_f64 + oldlow.to_f64)/2].inspect
        #       exit
        if diffSecond2 > 0 # || diffSecond2 < 0
          currentLowGreaterThanOldLow += 1
        else
          currentLowSmallerThanOrEqualToOldLow += 1
        end

        #        diffSecond2 = high.to_f64 - (oldhigh.to_f64 + oldlow.to_f64)/2 # Is today's high lower than the centre point of yesterday's range?
        #        puts [diffSecond, low.to_f64, oldhigh.to_f64, oldlow.to_f64, (oldhigh.to_f64 + oldlow.to_f64)/2].inspect
        #       exit

        # Is today's low purchasable X days in the past
        found = false
        if old_lows.size >= DAYS[0]
          old_lows[-DAYS[0]..-1].reverse_each.each_with_index do |i, index|
            index += 1
            diffSecond3 = i - (high.to_f64 + low.to_f64) / 2 # Is the old low of yesterday greater than today's high?
            if diffSecond3 > 0                               # || diffSecond2 < 0
            else
              currentHighSmallerThanOldLow += 1
              found = true
              find_times << index
              break
            end

            diffSecond3 = nil
          end

          if !found
            currentHighNotSmallerThanOldLow += 1
          end
        end

        found = false
        if old_lows.size >= DAYS[1]
          old_lows[-DAYS[1]..-1].reverse_each.each_with_index do |i, index|
            index += 1
            diffSecond3 = i - (high.to_f64 + low.to_f64) / 2 # Is the old low of yesterday greater than today's high?
            if diffSecond3 > 0                               # || diffSecond2 < 0
            else
              currentHighSmallerThanOldLow2 += 1
              found = true
              find_times2 << index
              break
            end
            diffSecond3 = nil
          end

          if !found
            currentHighNotSmallerThanOldLow2 += 1
          end
        end

        found = false
        if old_lows.size >= DAYS[2]
          old_lows[-DAYS[2]..-1].reverse_each.each_with_index do |i, index|
            index += 1
            diffSecond3 = i - (high.to_f64 + low.to_f64) / 2 # Is the old low of yesterday greater than today's high?
            if diffSecond3 > 0                               # || diffSecond2 < 0
            else
              currentHighSmallerThanOldLow3 += 1
              found = true
              find_times3 << index
              break
            end
            diffSecond3 = nil
          end

          if !found
            currentHighNotSmallerThanOldLow3 += 1
          end
        end

        found = false
        if old_lows.size >= DAYS[3]
          old_lows[-DAYS[3]..-1].reverse_each.each_with_index do |i, index|
            index += 1
            diffSecond3 = i - (high.to_f64 + low.to_f64) / 2 # Is the old low of yesterday greater than today's high?
            if diffSecond3 > 0                               # || diffSecond2 < 0
            else
              currentHighSmallerThanOldLow4 += 1
              found = true
              find_times4 << index
              break
            end
            diffSecond3 = nil
          end

          if !found
            currentHighNotSmallerThanOldLow4 += 1
          end
        end

        #     ..
      end

      olddate, oldopen, oldhigh, oldlow, oldclose, oldadjClose, oldvolume = commas
      old_lows.push oldlow.to_f64
    end

    first_dates << [current, first_date.to_s]
    GC.collect
  end

  # Multi
  current_desc = desc.shift
  frac1 = average_swing_ratio.average * 100.0
  frac2 = open_larger_than_low.average * 100.0 - 100.0
  frac3 = (didGoBackDownTimes/(didGoBackDownTimes + didntGoBackDownTimes))*100.0
  frac4 = currentSmallerThanOrEqualToOldCentre.to_f64 / (currentSmallerThanOrEqualToOldCentre.to_f64 + currentGreaterThanOldCentre.to_f64) * 100.0
  frac5 = currentLowSmallerThanOrEqualToOldLow.to_f64 / (currentLowSmallerThanOrEqualToOldLow.to_f64 + currentLowGreaterThanOldLow.to_f64) * 100.0
  frac6 = currentHighSmallerThanOldLow.to_f64 / (currentHighSmallerThanOldLow.to_f64 + currentHighNotSmallerThanOldLow.to_f64) * 100.0
  frac7 = currentHighSmallerThanOldLow2.to_f64 / (currentHighSmallerThanOldLow2.to_f64 + currentHighNotSmallerThanOldLow2.to_f64) * 100.0
  frac8 = currentHighSmallerThanOldLow3.to_f64 / (currentHighSmallerThanOldLow3.to_f64 + currentHighNotSmallerThanOldLow3.to_f64) * 100.0
  frac9 = currentHighSmallerThanOldLow4.to_f64 / (currentHighSmallerThanOldLow4.to_f64 + currentHighNotSmallerThanOldLow4.to_f64) * 100.0

  put "<h1>#{current_desc} - #{count} stocks on US Market.\n\n</h1>"
  dates = String::Builder.new
  first_dates.each do |i|
    dates << i.first.split('/')[1].sub(/\.csv/, "") << ' ' << i.last << " - " << DATE2.to_s << "<br/>"
  end
  dates = "<code>#{dates.to_s}</code>"
  if first_dates.size < 20
    put dates
  end
  put "Average Swing percentage of #{count} stocks for like 20-40 years (day-range) or less if specified is " \
      "#{frac1}%, random sample is #{high_low_pairs.sample.inspect}"
  put "Open is #{frac2}% larger than day's low on average."
  put "Last day's high is strictly bigger than current day's low - #{didntGoBackDownTimes} count | Last days " \
      "low is strictly bigger than current day's low #{didGoBackDownTimes} count"
  put "<b>#{frac3}% of the time it dips the next day to the old high or less.</b> (<= yesterday's range center point again? #{currentSmallerThanOrEqualToOldCentre} count | Not <= than yesterday's range center point again #{currentGreaterThanOldCentre} count.)"
  put "<b>#{frac4}% of the time it reaches to the center point of yesterday again or lower.</b> (<= yesterdays low again #{currentLowSmallerThanOrEqualToOldLow} count | Not <= yesterday's low again   #{currentLowGreaterThanOldLow} count.)"
  put "<b>#{frac5}%</b> of the time it reaches yesterday's low again."

  put "All throughout the range of the stock, For #{current_desc}, over a #{DAYS[0]}-day-range-backward of market days period back to find current-day's average price ([high + low] / 2) being <= those #{DAYS[0]} day respectives lows, (#{currentHighSmallerThanOldLow} found lower vs. " \
      "#{currentHighNotSmallerThanOldLow} not-found-lower (<b>#{frac6}%</b>)) and it took <b>#{find_times.sum.to_f64 / find_times.size} days.</b> Note that this backtracking is applied for every day throughout the timespan, so it is averaged out to give some kind of idea of the ability to purchase in the market (how stable/flat is it if you go back #{DAYS[0]} days every day in the time period and see if it reaches lows. Find times data is sum (#{find_times.sum} times #{find_times.size})."

  put "All throughout the range of the stock, For #{current_desc}, over a #{DAYS[1]}-day-range-backward of market days period back to find current-day's average price ([high + low] / 2) being <= those #{DAYS[1]} day respectives lows, (#{currentHighSmallerThanOldLow2} found lower vs. " \
      "#{currentHighNotSmallerThanOldLow2} not-found-lower (<b>#{frac7}%</b>)) and it took <b>#{find_times2.sum.to_f64 / find_times2.size} days.</b> Note that this backtracking is applied for every day throughout the timespan, so it is averaged out to give some kind of idea of the ability to purchase in the market (how stable/flat is it if you go back #{DAYS[1]} days every day in the time period and see if it reaches lows. Find times data is sum (#{find_times2.sum} times #{find_times2.size})."

  put "All throughout the range of the stock, For #{current_desc}, over a #{DAYS[2]}-day-range-backward of market days period back to find current-day's average price ([high + low] / 2) being <= those #{DAYS[2]} day respectives lows, (#{currentHighSmallerThanOldLow3} found lower vs. " \
      "#{currentHighNotSmallerThanOldLow3} not-found-lower (<b>#{frac8}%</b>)) and it took <b>#{find_times3.sum.to_f64 / find_times3.size} days.</b> Note that this backtracking is applied for every day throughout the timespan, so it is averaged out to give some kind of idea of the ability to purchase in the market (how stable/flat is it if you go back #{DAYS[2]} days every day in the time period and see if it reaches lows. Find times data is sum (#{find_times3.sum} times #{find_times3.size})."

  put "All throughout the range of the stock, For #{current_desc}, over a #{DAYS[3]}-day-range-backward of market days period back to find current-day's average price ([high + low] / 2) being <= those #{DAYS[3]} day respectives lows, (#{currentHighSmallerThanOldLow4} found lower vs. " \
      "#{currentHighNotSmallerThanOldLow4} not-found-lower (<b>#{frac9}%</b>)) and it took <b>#{find_times4.sum.to_f64 / find_times4.size} days.</b> Note that this backtracking is applied for every day throughout the timespan, so it is averaged out to give some kind of idea of the ability to purchase in the market (how stable/flat is it if you go back #{DAYS[3]} days every day in the time period and see if it reaches lows. Find times data is sum (#{find_times4.sum} times #{find_times4.size})."
end

#
# NEXT - FOR THE MAGNIFICENT SEVEN ["GOOG", "MSFT","TSLA","NVDA", "META", "AMZN", "AAPL"]
