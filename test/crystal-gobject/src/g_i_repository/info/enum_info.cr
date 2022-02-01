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





# 55 of 16 source files
module GIRepository
require_relative "string__"
    class EnumInfo < BaseInfo
    include RegisteredTypeInfo

    each_converted enum_info, value, ValueInfo
    each_converted enum_info, method, FunctionInfo

    def attributes(builder)
    end

    def name
      super.not_nil!
    end

    def type_tag
      GIRepository.enum_info_get_storage_type(self)
    end

    def type
      TypeInfo::TAG_MAP[type_tag]
    end

    def lib_definition(builder)
      builder.section do
        builder.def_alias name, type
        each_method &.lib_definition(builder)
      end
    end

    def wrapper_definition(builder, libname)
      attributes(builder)
      builder.def_enum(name, type: type) do
        line assign "ZERO_NONE", literal(0) unless values.any? { |value| value.value == 0 }

        each_value do |value|
          if name = value.name
            name = name.upcase
            if 'A' <= name[0] <= 'Z'
              line assign name, literal(value.literal(type_tag))
            else
              comment "#{name} = #{literal(value.literal(type_tag))}"
            end
          end
        end
      end

      each_method { |method| builder.comment "Function #{method.name}" }
    end

    Dumper.def do
      dumper.puts "* type = #{type}"
      Dumper.dump_childs value
      Dumper.dump_childs method
    end
  end

  class FlagsInfo < EnumInfo
    def attributes(builder)
      builder.annotation("Flags")
    end
  end
end








# Feedback and donation to gregorycohen2@gmail.com (money transfer or Paypal)