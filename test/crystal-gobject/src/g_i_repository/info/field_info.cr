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





# 47 of 16 source files
module GIRepository
require_relative "string__"
    class FieldInfo < BaseInfo
    KEYWORDS = ArgInfo::KEYWORDS

    def type
      BaseInfo.wrap(GIRepository.field_info_get_type(self)).as(TypeInfo)
    end

    def name(keyword_safe = true)
      name = super()
      if name && name[0].uppercase?
        normalized_name = "#{name[0].downcase}#{name[1..-1]}"
        neighbour_fields =  case container = self.container
          when ObjectInfo, StructInfo, UnionInfo
            container.fields
          else
            nil
          end
        if neighbour_fields && neighbour_fields.any? &.raw_name.==(normalized_name)
          name = "_#{name}"
        else
          name = normalized_name
        end
      end
      name = "_#{name}" if keyword_safe && KEYWORDS.includes? name if name
      name.not_nil!
    end

    def raw_name
      ptr = LibGIRepository.base_info_get_name(self)
      String.new(ptr) if ptr
    end

    def wrapper_name
      name = name(false)
      name = "_#{name}" if name == "initialize" || name == "finalize"
      name
    end

    def flags
      GIRepository.field_info_get_flags(self)
    end

    def readable?
      flags.readable?
    end

    def writable?
      flags.writable?
    end

    def size
      GIRepository.field_info_get_size(self)
    end

    def offset
      GIRepository.field_info_get_offset(self)
    end

    def lib_definition(builder)
      builder.field_binding name, type.lib_definition(builder)
    end

    Dumper.def do
      dumper.puts "* flags = #{flags}"
      dumper.puts "* size = #{size}"
      dumper.puts "* offset = #{offset}"
      Dumper.dump_child type
    end
  end
end








# Feedback and donation to gregorycohen2@gmail.com (money transfer or Paypal)