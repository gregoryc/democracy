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





# 51 of 16 source files
module GIRepository
require_relative "string__"
    module RegisteredTypeInfo
    def type_lib_definition
      "Lib#{namespace}::#{name}*"
    end

    def type_name
      ptr = LibGIRepository.registered_type_info_get_type_name(self)
      String.new ptr if ptr
    end

    def type_init_symbol
      ptr = LibGIRepository.registered_type_info_get_type_init(self)
      symbol = String.new ptr if ptr
      symbol == "intern" ? nil : symbol
    end

    def type_init
      symbol = type_init_symbol
      "_#{type_init_symbol}" if type_init_symbol
    end

    def type_init_lib_definition(builder)
      type_init = self.type_init
      builder.fun_binding type_init_symbol, name: type_init, return_type: "UInt64" if type_init
    end

    def wrapper_gtype(builder, libname)
      if type_init = self.type_init
        builder.call(type_init, receiver: libname)
      elsif type_name = self.type_name
        builder.call("type_from_name", builder.literal(type_name), receiver: "GObject")
      end
    end
  end
end








# Feedback and donation to gregorycohen2@gmail.com (money transfer or Paypal)