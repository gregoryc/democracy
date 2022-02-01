#!/usr/bin/env ruby
#
# Democracy software compilation VERSION 0.0., created on 2021-11-14 17:41:33 -0500
# Get all of this code, and all of these files, with
# gem install democracy
# Linux ruby 2.7.0p0 (2019-12-25 revision 647ee6f091) [x86_64-linux-gnu]
#
# Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.





# 2 of 16 source files
module GObject
require_relative "string__"
    class ClosureDataManager
    def self.instance
      @@instance ||= new
    end

    def self.deregister(data)
      instance.deregister data
    end

    def self.deregister(data, _closure)
      instance.deregister data
    end

    def self.register(data)
      instance.register data
      data
    end

    private def initialize
      @closure_data = Hash(Void*, Int32).new { |h, k| h[k] = 0 }
    end

    def register(data)
      @closure_data[data] += 1 if data
      data
    end

    def deregister(data)
      @closure_data[data] -= 1
      if @closure_data[data] <= 0
        @closure_data.delete data
      end
    end
  end
end








# Feedback and donation to gregorycohen2@gmail.com (money transfer or Paypal)