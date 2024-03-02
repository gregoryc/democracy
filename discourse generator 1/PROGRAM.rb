#!/usr/bin/env ruby
# frozen_string_literal: true

#
# Democracy software compilation VERSION 0.0., created on 2021-11-14 17:41:34 -0500
# Get all of this code, and all of these files, with
# gem install democracy
# Linux ruby 2.7.0p0 (2019-12-25 revision 647ee6f091) [x86_64-linux-gnu]
#
# Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

# 66 of 16 source files
module MainClass
  require_relative 'string__'
  require_relative './inputreceiver'
  require_relative './itemlist'
  require_relative './negative'
  require_relative './neutral'
  require_relative './outputter'
  require_relative './positive'
  class MainClass
    def initialize
      ARGV[0] || ARGV[0] = 1
      txt =String.new
      ARGV[0].to_i.times do |_i|
        item_list = ItemMaker.new
        txt += item_list.to_s + "\n"
        #      end
      end
        outputter = Outputter::Outputter.new('copy', txt)
    end

    class ItemMaker
      def initialize
        @list = []
        @list = [['Enter emotion of something that is bad',
                  'Enter the CAUSE of why that is bad',
                  'Enter the __proof__ of why that is bad'],
                 ['Enter emotion of something that is good',
                  'Enter the CAUSE of why that is good',
                  'Enter the __proof__ of why that is good'],
                 ['Enter emotion of something that is not bad or good']].flatten.reverse


        however1, however2 = ['', '']
        @neg = Negative::Negative.new('', however1, i, i, i)
        @pos = Positive::Positive.new('', however2, i, i, i)
        @neut = Neutral::Neutral.new('', '', '', i, '')
      end

      def filter(a)
        a = a.strip
        a = a.gsub(/i\s*don.?t\s*like/i, '')
        a = a.gsub(/^(because|that|since)/i, '')
        a = "#{a}  "
        a = a[0].downcase + a[1..-1]
        a = a.strip
      end

      def i; t = filter Inputreceiver::Inputreceiver.new(@list.pop).receive_input

        " #{t}" # .prepend(' ')
      end

      def to_s
        list = [@neg, @pos].shuffle.push(@neut)
        assert list.instance_of?(Array)
        assert list.length >= 2

        ItemList::ItemList.new(list).to_s # )])# + split + @neut.to_s)
      end
    end
  end

  MainClass.new
end



# Feedback and donation to gregorycohen2@gmail.com (money transfer or Paypal)
