#!/usr/bin/env ruby
#
# Democracy software compilation VERSION 0.0., created on 2021-11-14 17:41:34 -0500
# Get all of this code, and all of these files, with
# gem install democracy
# Linux ruby 2.7.0p0 (2019-12-25 revision 647ee6f091) [x86_64-linux-gnu]
#
# Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.





# 78 of 16 source files
module Negative
require_relative "string__"
 require_relative "./item"

  class Negative < Item::Item
    def initialize(*args)
    assert @@list.size==4
      b = %i[however personally disagree because evidence]
      super(b, args)
    end

    def however(argument); end

    def disagree(argument)
      @sentiment = "#{@@list.sample}#{argument}"
    end
    @@list = ['disagree with', "don't agree with", 'do not agree with', 'do not agree with the statement that']
  end; end









# Feedback and donation to gregorycohen2@gmail.com (money transfer or Paypal)