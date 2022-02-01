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





# 39 of 16 source files
module GObject
require_relative "string__"
    enum Type : UInt64
    INVALID   = 0 << 2
    NONE      = 1 << 2
    INTERFACE = 2 << 2
    CHAR      = 3 << 2
    UCHAR     = 4 << 2
    BOOLEAN   = 5 << 2
    INT       = 6 << 2
    UINT      = 7 << 2
    LONG      = 8 << 2
    ULONG     = 9 << 2
    INT64     = 10 << 2
    UINT64    = 11 << 2
    ENUM      = 12 << 2
    FLAGS     = 13 << 2
    FLOAT     = 14 << 2
    DOUBLE    = 15 << 2
    UTF8      = 16 << 2
    POINTER   = 17 << 2
    BOXED     = 18 << 2
    PARAM     = 19 << 2
    OBJECT    = 20 << 2
    VARIANT   = 21 << 2

    def self.gtype
      new LibGObject.gtype_get_type
    end

    def gtype?
      value == LibGObject.gtype_get_type
    end

    def fundamental
      Type.new GObject.type_fundamental(value)
    end

    def fundamental?
      fundamental == value
    end
  end
end








# Feedback and donation to gregorycohen2@gmail.com (money transfer or Paypal)