#!/usr/bin/nohup ac
# frozen_string_literal: true

#system "killall pulseaudio clock   --quiet"

class Time_
  def am?
    return self.hour < 12
  end

  def pm?
    return !am?
  end
end

l = %w[8 14 0].reverse
args = ARGV
if args.size > 0 && args[0] == "stop"
  args = args[1..]
  while true
    system "killall #{PROGRAM_NAME} --quiet -9 >/dev/null 2>/dev/null && (echo Alarm clock stopped | blue)"
    sleep 1
  end
end
if args.size < 3
  (3 - args.size).times do |i|
    args.push l.pop
  end
end

def a
  Time.local.hour
end

def b
  Time.local.minute
end

# STDOUT.close
# STDERR.close
# STDIN.close

32.times do |i|
  Signal.new(i).trap() do |a|
  end
end
Process.fork do
  32.times do |i|
    Signal.new(i).trap() do |a|
    end
  end

  #	args = [
  voices = `flite -lv`.strip.split(": ")[1].split.shuffle.reject { |i| i == "awb_time" }
  100.times do
    #    	system "xdotool key XF86MonBrightnessDown"
  end

  until a == (args[0].to_i) &&
        b == (args[1].to_i)
    sleep 1
  end

  loop do
    now = Time.local
    m = now.to_s
    t = [(((now.hour - 1) % 12) + 1).to_s + ((now.minute.to_s[0] == "0" ? "0" : "") + ":" + now.minute.to_s), "on", m, now.day]
    # p t
    t = t.join(" ")
    system "echo #{t} | padsp flite -voice #{voices[0]}"
    sleep 1
    voices = voices.shuffle
  end
end
