# frozen_string_literal: true
# BSD 2-Clause License
# 
# Copyright (c) 2021, Gregory Cohen
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 
# 1. Redistributions of source code must retain the above copyright notice, this
   # list of conditions and the following disclaimer.
# 
# 2. Redistributions in binary form must reproduce the above copyright notice,
   # this list of conditions and the following disclaimer in the documentation
   # and/or other materials provided with the distribution.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# Get rid of the "ca"s if you want to make it work for the US.
# This program is for personal, non-commerical use.
# Any sites this program downloads from are being used for personal, non-commerical use.
# If any legal problems do arise, feel free to contact me at my
# email and I can modify this program to use some ohter site.

require 'json'
a = File.read ARGV[0]
q = JSON.parse(a)
require 'readline'

def read
  $stdin.readline.strip
end

tot_index = 0
A_SIZE = a.split("\n").size
q.values[0].each do |key|
  key.each do |_q, a|
    index = 0
    while index < a.size
      puts `tput reset`
      puts format('%s %s %s %s %s', tot_index, '/', 'lines', A_SIZE - 2, a[index].inspect)
      i = a[index]
      puts (q.values[0].map(&:keys).to_a).join('|')

      tot_index += 1
      i[1].each_with_index do |a, b|
        warn "#{b + 1} #{a.inspect}"
      end

      print "#{a[0][0]} > #{i[0]}            "
      text = read
      if /back\s+[0-9]+/.match?(text)
        index -= begin
          text.split[1].to_i
        rescue StandardError
          1
        end
      end
      a[index][1] += text.split('|').map(&:strip).reject do |q|
        q == ''
      end.uniq.to_a

      index += 1
    end
  end
end
puts q.to_json
File.open '4.json', 'w' do |f|
  f.puts q.to_json
end
