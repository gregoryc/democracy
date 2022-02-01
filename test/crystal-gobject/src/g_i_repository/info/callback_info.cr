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





# 50 of 16 source files
module GIRepository
require_relative "string__"
    class CallbackInfo < BaseInfo
    include CallableInfo

    def name
      name = super.not_nil!
      'A' <= name[0] <= 'Z' ? name : "#{name[0].upcase}#{name[1..]}"
    end

    def signature(builder)
      builder.proc_type(args.map(&.type.lib_definition(builder)), return_type: return_type.lib_definition(builder))
    end

    def lib_definition(builder)
      builder.def_alias name, signature(builder)
    end

    def wrapper_definition(builder, libname)
      builder.def_alias name, "#{libname}::#{name}"
    end

    def for_wrapper_definition(builder, libname)
      return_type = self.return_type.wrapper_definition(builder, libname)
      return_type = "" if return_type == "Void" || return_type == "Void*"
      builder.proc_type args.map(&.type.wrapper_definition(builder, libname)), return_type: return_type
    end
  end
end








# Feedback and donation to gregorycohen2@gmail.com (money transfer or Paypal)