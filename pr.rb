
#!/usr/bin/ruby
b=Dir.glob '*'#0
def out(*a)
	print a.join(" ")
	puts
end

a=nil
b.each do |i|
	sum=0.0
	b = File.open i
	b.readline rescue next
	b.readline rescue next
	lines = 0
	b.readlines.last(1000).each do |i|
		lines += 1
		a = i.split[3..4].reverse
		c = a[0].to_f - a[1].to_f
		sum += c.abs
	end


	if sum/lines < 5 || false#sum/lines > 99
		next
	end



#	out a, ""
	out a, lines, sum/lines, " ", i #, i / lines
	#puts b
	b.close
end
