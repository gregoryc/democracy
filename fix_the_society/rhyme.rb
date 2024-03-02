#!/usr/bin/ruby
# frozen_string_literal: false
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

NUM = 100_000
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
  "31\tWorship of a glowing light (psychotic)"
].map do |i|
  i.gsub "\t", ' -- '
end.reverse

LIST2 = LIST.dup.reverse

# !/usr/bin/ruby
require 'rhymes'

str = <<~EOF
  1 Abuse of Philosophical universalism

  Kid

  Assert

  Behave

  Retarded

  Retard

  Intention

  Neglect

  Remiss

  Viewpoint

  Regard

  Everyone



  2 Assuming enemies

  Paranoid

  Fed

  Opp

  Delusion

  Demon

  Monster

  Opponent

  Adversary


  3 Religious books over observation

  Blind Faith

  Submission

  Lack of Thought

  Conspiracy Mind

  Non-observation


  4 ........


  Submission

  Being well-adjusted

  Being conditioned

  Being trained

  Brainwashed

  Entranced

  Hypnotized



  5 Forming groups as escapism


  Escape

  Resource

  Resort

  Evasion

  Hide-away

  Getaway

  Run

  Cry

  Fear

  Problem

  Goal

  Purpose


  6 Limited range of discussion


  Talk

  Discuss

  Run away

  Conversation

  Conversation be insular

  Hive-mind

  Bubble

  Tunnel vision

  Vision

  Curious





  7 High regard group consensus

  Hive-mind

  Religion

  Collective thought agreement

  Agreement

  Humility

  Arrogance

  Arrogant

  Faith

  Science

  Church

  Government


  8 Ignoring discussion of things wrong by default

  How it is

  Improve

  Improvement

  Genetic

  Eugenic

  Biological

  From the start

  Vitamins

  Pain

  Unconsciousness

  Muscle

  Default

  Wrong

  Bottom

  Ascend

  Fucked up from the start


  9 ..............


  January Sixth

  Constitution

  America

  France

  Enlightenment

  Government

  Ignore

  Stupid

  Retard

  Idiot

  Whine

  Moron

  Attack

  Fight

  Argue

  Punch

  Battle

  War

  Hypocrite

  Liar

  Disingenuous

  Insincere

  Violent

  Monster

  Values


  10 Intentional negligence of field or domain

  Knowledge

  Liar

  Insularity

  Insular

  Tunnel vision

  Vision

  Science

  Not testing

  Curious

  Bubble



  11

  Holy

  Interpretation

  The way

  The path

  Only

  The only way

  God=E2=80=99s word


  12 Isolationism

  Island

  Separate

  Loner

  Away

  Enemy

  Angry

  Upset




  13


  Invalidate

  Invalidation

  Degrade

  Evaluate

  How it is

  Judge

  Judgment

  Degradation

  Insult

  Worsen

  Humility

  Modesty




  14     Lack of respect=E2=80=A6. Viewpoints=E2=80=A6 etc

  Respect

  Humility

  Modesty

  Arrogance

  Degradation

  Rude

  Person

  Guy

  Idea

  Thought

  Other


  15 Making enemies

  Enemy

  Opponent

  Opp

  Aggression

  Angry

  upset

  Menace


  16

  Physical

  Hopeless

  Material

  Mechanical

  Non-spiritual

  Not spiritual

  No soul

  No heart

  No love



  17

  Normalization

  Arbitrary

  Kid

  Develop

  action

  Introspection

  Protest

  Generalization

  Generalize

  Fighting

  Maldeveloped

  Child

  Behave




  18 Not believe future

  Future

  Hope

  Dream

  Dreams

  Hopes

  Optimist

  Optimism

  Pessimist

  Pessimism

  Aspire

  Aspiration

  Desire


  19 Not believe ideals

  Ideal

  Perfect

  What you want

  Correct

  Right

  Proper

  Nice

  Good

  Dream

  Fantasy


  20


  Power

  Ability

  Strength

  Might

  Responsibility

  Group-think

  Oppress

  Tax

  Degrade

  Entrap

  21 Not communicating

  Dumb

  Stupid

  Communicate

  Talk

  Protest

  Fight


  22

  Phenomena

  Stupidity

  Europe

  observe

  Higher

  Aspiration

  Observation


  23

  Common

  Typical

  Atypical

  Erratic

  Overwhelmed

  Overcome

  Tripping

  24 Religious backoff

  Politically correct

  Mind

  Thoughts

  Religious

  Religion

  Fear

  Afraid

  Faith


  25

  Retardation

  Retarded

  Pain

  Perks

  Killer

  Painkiller

  Psychiatrist

  Psychologist

  Perco-set

  Drugs

  Mask

  Cover

  Human mind

  Incident

  Remember

  Memory

  Thought

  Recall

  Unconsciousness


  26

  Separate

  Distant

  Away

  Cut off

  Rival

  Distance


  27 ...........

  Spinoza

  Mechanical

  Material

  God-obsessed

  Negative

  Contrary

  28 ..........

  Weak

  Overly tolerant

  Idealistic

  different

  Compromising

  Perfectionist


  29 .........

  Tolerating imperfection

  Tolerating stupidity

  Illogical

  Diverge

  Divergence



  30 .............

  The group

  Our group

  Insular

  Get people talking

  Egotism

  Insularity


  31 ...........

  Light

  Delusion

  Confusion

  Must be god

  Deluded

  Confused

  Worship

  Prayer

  Glow

  Glowing

  Praying



EOF
$str = +''
def html(a)
  # puts a
  print "<#{a}>"
  yield
  print "</#{a}>"
end

def print(val)
  $str << String.new(val)
end

html 'head' do
  html 'style' do
    print <<~EOF
      textarea, body {
      color: green;
      width: 100%;
      height: 100%;
      margin: 0;
      padding: 0;
      border: 0;
      font-family: Monospace;
      font-size: 14px;
      font-weight: bold;
      }
      h1 {
      color: black;

      }



    EOF
  end
end

str = str.squeeze("\n")
str = str.split(/\n\d+.+?\n/)
r = Rhymes.new
# puts $str

map = { 'Maldeveloped' => 'Developed' }
def j(a)
  print a
  #  print a.gsub "i'm", "I'm"
  $json << a
end; $json = +''

html 'body' do
  j '{'
  str.each_with_index do |cat, index|
    print "</textarea></pre><h1>#{LIST.pop}</h1><pre><textarea>"
    index += 1
    j index.to_s.dump
    j ": [\n"
    # warn cat.join
    # A
    cat.split("\n") do |word|
      next if /[0-9]/.match?(word[0])

      #		print "\t"
      j '{'
      j word.downcase.inspect
      j ':'

      begin
        val = if map.include? word
                map[word]
              else
                word
              end
        res = r.rhyme(val.split(/\s|-/)[-1]).map do |i|
          [i.downcase, []]
        end; j "\n\t"
        #	puts res.size
        #	if res.size!=0
        j '[["'
        j word.upcase
        j '", []],'
        j '[]' if res.empty?

        j "\n\t"
        res2 = res.inspect.split(',').join(",\n\t").gsub(/\s+\[\]\]/m, '[]]')[1..-1]
        j res2
      # end

      #		print res.to_a.split(",").joni(
      rescue Exception
        raise
      end
      j '},'
      #		break
    end
    #	O
    #	PRINT
    # PRINT '
    $str[-1] = ']'
    $json[-1] = ']'
    j ','
    #		break
  end
  # print "}"
  $str = "#{$str[0..-3]}]}"
  $json = "#{$json[0..-3]}]}"

  # end
end
# end

require 'json'

def print_(*a)
  # $stdout.print(*a)
  $file.print(*a)
end
def e_get_d(val)
  #    raise if val[0].nil?

  data_ = [val[0][0][0], val[0][0][1]]
  #    abort val[0][0].to_s
  data_ = data_.reverse
  #  print_(data_)
  data_[0] = data_[0].sample.gsub(/  +/, ' ')
  data_ = data_.join(' ')
  data_ = data_.gsub(/\(.+?\)/) do |e|
    e = e[1..-2]
    e.split('^').sample.to_s
  end
  val = "#{data_[0].upcase + data_[1..-1].gsub(' i ', ' I ')}\n"
  val.gsub!(/[a-zA-Z]+$/, &:downcase)
end

def get_d(val)
  data_ = [[], []]
  data_ = val[0].sample while data_[1].empty?

  data_ = data_.reverse
  data_[0] = data_[0].sample.gsub(/  +/, ' ')
  data_ = data_.join(' ')
  data_ = data_.gsub(/\(.+?\)/) do |e|
    e = e[1..-2]
    e.split('^').sample.to_s
  end
  val = "#{data_[0].upcase + data_[1..-1].gsub(' i ', ' I ')}\n"
  val.gsub!(/[a-zA-Z]+$/, &:downcase)
end

def prt(end_words_array_of_hash, type)
  str = "#{type.upcase}\n\n"
  # used = []
  case type
  when 'verse'
    #  	num = 4..6
    num = 3..5
  when 'chorus'
    num = 2..4
  end

  rand(num).times do
    val = end_words_array_of_hash.sample.values
    out = [e_get_d(val)]
    n = rand 3..5
      until n == out.size
        out << get_d(val)
        out.uniq!
      end
    out = out.uniq.shuffle.join
    str << out
    str << "\n"
  end

  str
end
processes=Array.new

%w[1 21].each do |cat|
  processes << fork do
    warn cat

    dir = "#{Dir.home}/democracy/fix_the_society/#{cat}.json"
    # vals = JSON.parse($json)
    vals = nil

    begin
      vals = JSON.parse(File.read(dir))
      vals || raise
    rescue StandardError
      puts dir
    end

    require 'colored'

    unless $stdout.tty?
      class String
        def bold
          self
        end
      end
    end

    time1 = Time.now

    NUM.times do |_song|
      warn _song if (_song % 1000).zero?
      $file = File.open (Dir.home + "/democracy/fix_the_society/songs/#{cat}-#{_song}.txt"), 'w'
      vals.each do |k, v|
        m = "(NO COPYRIGHT RESTRICTIONS ON THIS CONTENT WHATSOEVER. FULLY PUBLIC DOMAIN)\nName the song or poem as you wish, resell it, but please positively influence culture\n\nSubject matter | Category #{LIST2[k.to_i - 1]}\n\n"
        print_ m
        chorus_ = prt(v, 'chorus')
        [0, 2, 4, 6].each do |i|
          print_ prt(v, 'verse')
          break if i >= 2 && rand < 0.5

          #	print_ chorus_, "chorus"
          print_ chorus_
        end
      end

      print_ "---\n"
      $file.close
    end

    time2 = Time.now

    # warn (NUM * 31 / (time2 - time1)).to_s + " songs per second, #{NUM * 31} songs in total "

    # exit

    require 'tempfile'
    require 'fileutils'

    # fork do
    begin
      FileUtils.cp '1.json', Dir.glob('/media/a/*/1.json').first
      FileUtils.cp '21.json', Dir.glob('/media/a/*/21.json').first
      FileUtils.cp 'easier.rb', Dir.glob('/media/a/*/easier.rb')[0]
      FileUtils.cp 'rhyme.rb', Dir.glob('/media/a/*/rhyme.rb')[0]
      warn 'COPIED files'
    rescue Exception
    end
    next

    Tempfile.open(['ruby', '.html']) do |fle|
      fle.puts $str

      puts `google-chrome #{fle.path} 2>/dev/null & cat #{fle.path}`

      FileUtils.cp __FILE__, '/home/a/F1.rb'
      FileUtils.cp fle.path, '/home/a/F2.htm'
      # require 'f
    end
  end
end

processes.each do |process|
	Process.wait process
end

puts "Done."