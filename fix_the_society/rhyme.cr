#!/usr/bin/crystal

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


require "file_utils"
require "json"
require "colorize"

include FileUtils


LIST = [
  "1\tAbuse of philosophical universalism (making broad claims while intentionally not trying to understand things or see any sides of anything whatsoever, childish behavior worse than a 3 year old, acting retarded)",
  "2\tAssuming enemies",
  "3\tBelieving religious books over observation",
  "4\tEnvironment over mind",
  "5\tForming groups or hanging out with others as a form of escapism instead of trying to solve actual problems or uphold actual, legitimate values",
  "6\tHaving a limited range of discussion",
  "7\tHigh regard for ANY group consensus, any lack of humility or modesty in group understanding (religion, science, etc.) ANY lack of modesty (e.g., THE faith, THE science, THE church, THE government )",
  "8\tIgnoring discussion if anything has to do with things being \"wrong by default\" while simultaneously having such \"not be a problem\" (for example, vitamins, incidents of pain and unconsciousness, muscle, etc.)",
  "9\tIgnoring founding principles of a constitution or belief system and whining about things",
  "10\tIntentional negligence of field or domain (e.g., physical science, psychiatry, etc.)",
  "11\tInterpretation of a situation as having ONE Interpretation",
  "12\tIsolationism  (I am separate from them or him)",
  "13\tJudgment, evaluation, invalidation",
  "14\tLack of respect of individuals who would state viewpoints not understood by someone else",
  "15\tMaking enemies",
  "16\tMaterialism",
  "17\tNormalization of the arbitrary, lack of introspection and protest of the arbitrary, excessive normalization or generalization of behavior or action (sign of maldevelopment, \"autism\", or aggression, childish behavior)",
  "18\tNot believing in futures",
  "19\tNot believing in ideals",
  "20\tNot believing in the individual",
  "21\tNot resorting to communication or protest to resolve these.",
  "22\tNot respecting Enlightenment Values",
  "23\tNot wanting to have an atypical conversation and becoming erratic or overwhelmed",
  "24\tReligious backoff (political correctness)",
  "25\tRetardation about handling incidents of pain",
  "26\tSeparatism (They are separate from me or us)",
  "27\tSpinozism",
  "28\tToleration of different viewpoints",
  "29\tToleration of intellectual divergence",
  "30\tValuing group action over general democracy",
  "31\tWorship of a glowing light (psychotic)",


 #There is JSON for this one but it is "too chaotic" for release
  "32\tHedonism",
].map do |i|
  i.gsub "\t", " -- "
end.reverse

LIST2 = LIST.dup.reverse

def e_get_d(val)
  #    raise if val[0].nil?

  data_ = [val[0][0][0], val[0][0][1]]
  data_ = [data_[1].as_a.sample.as_s.gsub(/  +/, ' '), data_[0]]

  data = data_.join(' ')
  data = data.gsub(/\(.+?\)/) do |e|
    e = e[1..-2]
    e.split('^').sample.to_s
  end
  val = "#{data[0].upcase + data[1..-1].gsub(" i ", " I ")}\n"
  val.gsub(/[a-zA-Z]+$/, &.downcase)
end

def get_d(val)
  data_ = val[0].as_a.sample

  while data_[1].as_a.size == 0
    data_ = val[0].as_a.sample
  end

  data_ = [data_[1].as_a.sample.as_s.gsub(/  +/, ' '), data_[0]]

  data = data_.join(' ')
  data = data.gsub(/\(.+?\)/) do |e|
    e = e[1..-2]
    e.split('^').sample.to_s
  end
  val = "#{data[0].upcase + data[1..-1].gsub(" i ", " I ")}\n"
  val.gsub(/[a-zA-Z]+$/, &.downcase)
end

SET = %w[smalltalk intention hardly]

def tst(l : Array(String)) : Bool
  SET.each do |i|
    if l.includes?(i)
      return true
    end
  end
  false
end

def prt(end_words_array_of_hash, type_)
  str = String::Builder.new
  str << "#{type_.upcase}\n\n"
  # used = []
  case type_
  when "chorus"
    #  	num = 4..6
    num = 3..5
  when "verse"
    num = 2..4
  end

  raise "" if num.nil?

  rand(num).times do
    #  puts end_words_array_of_hash
    #  abort
    val = end_words_array_of_hash.as_a.sample.as_h.values
    out_ = [e_get_d(val)]
    lasts = [out_[0].split[-1].downcase]
    n = rand 4..6
    times = 0

    until n == out_.size
      new_ = get_d(val)
      new_last = new_.split[-1].downcase
      if (lasts.size >= 4 && tst(lasts)) || !(lasts.includes? new_last)
        lasts.push new_last
        out_ << new_
      elsif times > 100_000
        abort [lasts, new_last]
      end
      times += 1
    end

    str << out_.uniq.shuffle.join
    str << '\n'
  end

  str.to_s
end
BASE=ARGV[0].strip rescue ""
NUM =13_000
BASE.empty? && begin
puts "Enter basename to make songs folder in (folder/songs, num songs=#{NUM})".colorize(:red)
abort
end



mkdir_p BASE + "/songs"

LST = [0, 2, 4, 6]
processes = [] of Process

LST_ = if File.exists?("#{BASE}/#{32}.json")
         ["32"]
       else
         %w[1 21]
       end

time = Time.local


LST_.each do |f|
  processes << Process.fork do
    dir = "#{BASE}/#{f}.json"
    # vals = JSON.parse($json)
    vals = nil

    begin
      vals = JSON.parse(File.read(dir))
    end
    NUM.times do |_song|
      if _song % 10000 == 0
        print f, ' ', _song, "\n"
      end
      File.open fil = "#{BASE}/songs/#{f}-#{_song}.txt", "w" do |file|
        vals && vals.as_h.each do |k, v|
          file.print "(NO COPYRIGHT RESTRICTIONS ON THIS CONTENT WHATSOEVER. FULLY PUBLIC DOMAIN)\nName the song or poem as you wish, resell it, but please positively influence culture\n\nSubject matter | Category #{LIST2[k.to_i - 1]}\n\n"
          # print m
          chorus_ = prt(v, "chorus")
          LST.each do |i|
            file.print prt(v, "verse")
            break if i >= 2 && rand < 0.5

            file.print chorus_

            # print chorus_
          end
        end
        str = "---\n"
        file.print str
      end
    end
  end
end

processes.each do |proc|
  proc.wait
end
puts Time.local - time
# processes.weac
