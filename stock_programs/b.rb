#!/usr/bin/ruby
Dir.chdir Dir.home
Dir.chdir "TEMP_FOLDER"
glob = '/home/a/democracy/s/*'

Dir.glob(glob) do |i_|
	i = File.basename i_
#Dir.glob glo do |i|
	if test "e", "#{i}"
		puts i
		next
	else
		puts "Not skipping"
	end
	puts i

	if File.read(i_).size   > 20
		system "python3 ../democracy/stock_programs/b.py #{i} > #{i}"
	end
end
