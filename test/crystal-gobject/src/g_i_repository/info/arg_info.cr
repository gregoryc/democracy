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





# 57 of 16 source files
module GIRepository
require_relative "string__"
    class ArgInfo < BaseInfo
    KEYWORDS = {"abstract", "alias", "begin", "def", "end", "in", "module", "next", "out", "self"}

    def name
      name = super
      name = "_#{name}" if name[0].uppercase? if name
      name = "_#{name}" if KEYWORDS.includes? name
      name.not_nil!
    end

    def direction
      GIRepository.arg_info_get_direction(self)
    end

    def in?
      direction.in?
    end

    def out?
      direction.out?
    end

    def inout?
      direction.inout?
    end

    def optional?
      GIRepository.arg_info_is_optional self
    end

    def nullable?
      GIRepository.arg_info_may_be_null self
    end

    def nilable?
      nullable? || optional?
    end

    def gvalue_out?
      out? && type.tag.interface? && type.interface.namespace == "GObject" && type.interface.name == "Value"
    end

    def closure?
      in? && type.tag.interface? && type.interface.namespace == "GObject" && type.interface.name == "Closure"
    end

    def return_value?
      GIRepository.arg_info_is_return_value(self)
    end

    def skip?
      GIRepository.arg_info_is_skip(self)
    end

    def type
      BaseInfo.wrap(GIRepository.arg_info_get_type(self)).as(TypeInfo)
    end

    def ownership_transfer
      GIRepository.arg_info_get_ownership_transfer(self)
    end

    def caller_allocates?
      GIRepository.arg_info_is_caller_allocates(self)
    end

    def lib_definition(builder)
      builder.arg name, type: "#{type.lib_definition(builder)}#{'*' if out? || inout?}"
    end

    def for_wrapper_definition(builder, libname)
      case type.tag
      when .int8?, .uint8?, .int16?, .uint16?, .int32?, .uint32?, .int64?, .uint64?
        builder.arg name, type: "::Int"
      when .float?, .double?
        builder.arg name, type: "::Float"
      when .array?
        if type.param_type.tag.int8? # Assume Int8* (gchar*) is a string for now
          builder.arg name, type: "::String#{"?" if nilable?}"
        elsif type.param_type.tag.uint8?
          builder.arg name, type: "::Bytes#{"?" if nilable?}"
        else
          builder.arg name, type: "::Enumerable#{"?" if nilable?}"
        end
      when .interface?
        interface = type.interface
        if interface.namespace == "GObject" && interface.name == "Value"
          builder.arg name
        elsif interface.is_a?(UnionInfo)
          builder.arg name, type: "#{type.wrapper_definition(libname)}::Union#{"?" if nilable?}"
        else
          builder.arg name, type: "#{type.wrapper_definition(libname)}#{"?" if nilable?}"
        end
      else
        builder.arg name, type: "#{type.wrapper_definition(libname)}#{"?" if nilable?}"
      end
    end

    def for_wrapper_pass(builder, libname)
      if out? || inout?
        name
      else
        if nullable?
          "#{name} ? #{type.convert_from_crystal(name)} : #{type.null_value_for_lib(builder)}"
        else
          type.convert_from_crystal(name)
        end
      end
    end

    def from_wrapper_pass
      if out? || inout?
        name
      else
        "#{"#{name} && " if nullable?}#{type.convert_to_crystal(name)}"
      end
    end

    Dumper.def do
      dumper.puts "* direction = #{direction}"
      dumper.puts "* optional = #{optional?}"
      dumper.puts "* nullable = #{nullable?}"
      dumper.puts "* return_value = #{return_value?}"
      dumper.puts "* ownership = #{ownership_transfer}"
      dumper.puts "* caller_allocates = #{caller_allocates?}"
      dumper.puts "* skip = #{skip?}"
      Dumper.dump_child type
    end
  end
end








# Feedback and donation to gregorycohen2@gmail.com (money transfer or Paypal)