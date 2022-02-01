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





# 31 of 16 source files
module GLib
require_relative "string__"
    struct ListIterator(T, L)
    include Iterator(T)

    @start = true
    @end = false

    def initialize(@list : List)
    end

    def next
      return stop if @end

      value = T.new @list.data.as(L)

      if has_next?
        @list = GLib::List.new((@list.to_unsafe.as(LibGLib::List*).value._next).as(LibGLib::List*))
        @start = false
      else
        @end = true
      end

      value
    end

    def prev
      return stop if @start

      value = T.new @list.data.as(L)

      if has_prev?
        @list = GLib::List.new((@list.to_unsafe.as(LibGLib::List*).value.prev).as(LibGLib::List*))
        @end = false
      else
        @start = true
      end

      value
    end

    def rewind
      while has_prev?
        @list = GLib::List.new((@list.to_unsafe.as(LibGLib::List*).value.prev).as(LibGLib::List*))
      end
    end

    def has_next?
      !@list.to_unsafe.as(LibGLib::List*).value._next.null?
    end

    def has_prev?
      !@list.to_unsafe.as(LibGLib::List*).value.prev.null?
    end
  end
end








# Feedback and donation to gregorycohen2@gmail.com (money transfer or Paypal)