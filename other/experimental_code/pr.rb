# !/usr/bin/ruby
b = Dir.glob "*" # 0

def out(*a)
  print a.join(" ")
  puts
end

a = nil
b.each do |i|
  sum = 0.0
  b = File.open i
  b.readline rescue next
  b.readline rescue next
  lines = 0; list = b.readlines.last(1000) # if(list[0].split[2].to_f) < 5
  #		next
  #	end

  b.each do |i|
    lines += 1
    a = i.split[3..4].reverse
    #		if i.split[2].t

    c = a[0].to_f - a[1].to_f
    sum += c
  end

  if sum < 0 || false # sum/lines > 99
    next
  end; puts i
  b.close
end
