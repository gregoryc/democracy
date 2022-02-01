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





# 54 of 16 source files
module GIRepository
require_relative "string__"
    class TypeInfo < BaseInfo
    TAG_MAP = {
      TypeTag::VOID     => "Void",
      TypeTag::BOOLEAN  => "LibC::Int",
      TypeTag::INT8     => "Int8",
      TypeTag::UINT8    => "UInt8",
      TypeTag::INT16    => "Int16",
      TypeTag::UINT16   => "UInt16",
      TypeTag::INT32    => "Int32",
      TypeTag::UINT32   => "UInt32",
      TypeTag::INT64    => "Int64",
      TypeTag::UINT64   => "UInt64",
      TypeTag::FLOAT    => "Float32",
      TypeTag::DOUBLE   => "Float64",
      TypeTag::GTYPE    => "UInt64",
      TypeTag::UTF8     => "UInt8",
      TypeTag::FILENAME => "UInt8",
      TypeTag::GLIST    => "Void*",
      TypeTag::GSLIST   => "Void*",
      TypeTag::GHASH    => "Void*",
      TypeTag::ERROR    => "LibGLib::Error*",
      TypeTag::UNICHAR  => "UInt8",
    }

    BLACKLIST = {"VaClosureMarshal", "DBusProxyClass", "DBusInterfaceSkeletonClass"}

    def tag
      GIRepository.type_info_get_tag(self)
    end

    def pointer?
      GIRepository.type_info_is_pointer(self)
    end

    def void?
      tag.void?
    end

    def interface
      BaseInfo.wrap(GIRepository.type_info_get_interface(self)).not_nil!
    end

    def array_type
      GIRepository.type_info_get_array_type(self)
    end

    def array_length_param
      GIRepository.type_info_get_array_length(self)
    end

    def array_fixed_size
      GIRepository.type_info_get_array_fixed_size(self)
    end

    def param_type(n = 0)
      BaseInfo.wrap(GIRepository.type_info_get_param_type(self, n)).as(TypeInfo)
    end

    def zero_terminated?
      GIRepository.type_info_is_zero_terminated(self)
    end

    def primitive?
      {
        TypeTag::BOOLEAN,
        TypeTag::INT8,
        TypeTag::UINT8,
        TypeTag::INT16,
        TypeTag::UINT16,
        TypeTag::INT32,
        TypeTag::UINT32,
        TypeTag::INT64,
        TypeTag::UINT64,
        TypeTag::FLOAT,
        TypeTag::DOUBLE,
        TypeTag::GTYPE,
        TypeTag::UTF8,
        TypeTag::FILENAME,
        TypeTag::UNICHAR,
      }.includes?(tag)
    end

    def lib_definition(builder)
      base = case tag
             when .interface?
               interface = self.interface
               name = interface.name
               if interface.info_type.unresolved?
                 return "Void*"
               elsif name.nil? || BLACKLIST.includes?(name) || (name && 'a' <= name[0] <= 'z') # More weird stuff, also compiler could be smarter here
                 interface.is_a?(CallbackInfo) ? "-> Void" : "Void*"
               elsif interface.is_a?(CallbackInfo) && inline_type?
                 return Crout.build { |b| write interface.as(CallbackInfo).signature(b) }
               else
                 namespace = "Lib#{interface.namespace}::"
                 namespace = nil if namespace == self.namespace
                 "#{namespace}#{interface.name}"
               end
             when .array?
               case array_type
               when ArrayType::C
                 param = param_type.lib_definition(builder)
                 param += "[#{array_fixed_size}]" if array_fixed_size > 1
                 param
               else
                 "Void"
               end
             else
               TAG_MAP[tag]
             end
      base = "Pointer(#{base})" if pointer?
      base
    end

    def null_value_for_lib(builder)
      if pointer?
        "#{lib_definition(builder)}.null"
      else
        "nil"
      end
    end

    def signal_lib_definition(builder)
      base = lib_definition(builder)
      base += "*" if is_pointer_in_signal?
      base
    end

    def is_pointer_in_signal?
      tag.interface? && !interface.is_a?(EnumInfo)
    end

    def gvalue_type
      "GObject::Type::" + case tag
      when .int32?
        "INT"
      when .uint32?
        "UINT"
      when .interface?
        interface = self.interface
        case interface
        when ObjectInfo, StructInfo, UnionInfo
          "OBJECT"
        when InterfaceInfo
          "INTERFACE"
        when EnumInfo
          "ENUM"
        else
          "INTERFACE"
        end
      when .ghash?
        "POINTER"
      else
        tag.to_s
      end
    end

    def wrapper_definition(builder, libname = "")
      case tag
      when .interface?
        interface.full_constant
      when .array?
        "::Enumerable(#{param_type.wrapper_definition(builder, libname)})"
      when .void?
        pointer? ? "Void*" : "Nil"
      when .utf8?, .filename?
        "::String"
      when .boolean?
        "::Bool"
      else
        TAG_MAP[tag]
      end
    end

    def convert_to_crystal(builder, variable) : String | Crout::Expression
      case tag
      when .interface?
        interface = self.interface
        case interface
        when ObjectInfo, StructInfo, UnionInfo, EnumInfo, FlagsInfo
          builder.call "new", variable, receiver: interface.full_constant
        when InterfaceInfo
          builder.call "new", variable, receiver: "#{interface.full_constant}::Wrapper"
        else
          variable
        end
      when .array?
        case array_type
        when ArrayType::C
          item = builder.declare_var
          builder.call("new", variable, receiver: "GObject::PointerIterator", block_args: {item}) do |b|
            b.line param_type.convert_to_crystal(b, item)
          end
        else
          variable
        end
      when .boolean?
        "(#{variable} == 1)"
      when .utf8?, .filename?
        builder.call "new", variable, receiver: "::String"
      when .glist?
        "GLib::ListIterator(#{param_type.wrapper_definition(builder)}, #{param_type.lib_definition(builder)}*).new(GLib::List.new(#{variable}.as(LibGLib::List*)))"
      when .gslist?
        "GLib::SListIterator(#{param_type.wrapper_definition(builder)}, #{param_type.lib_definition(builder)}*).new(GLib::SList.new(#{variable}.as(LibGLib::SList*)))"
      else
        variable
      end
    end

    def convert_from_crystal(variable)
      case tag
      when .interface?
        interface = self.interface
        if interface.namespace == "GObject" && interface.name == "Value"
          "#{variable}.to_gvalue.to_unsafe#{".value" unless pointer?}"
        elsif interface.is_a?(ObjectInfo) || interface.is_a?(InterfaceInfo)
          converter = interface.to_unsafe_name if interface.responds_to?(:to_unsafe_name)
          pointer? && converter ? "#{variable}.#{converter}" : variable
        else
          pointer? ? "#{variable}.to_unsafe.as(Lib#{interface.full_constant}*)" : variable
        end
      when .array?
        if param_type.tag.uint8? # Assume UInt8* (gchar*) is a string for now
          "#{variable}.to_unsafe"
        else
          "(__#{variable}_ary = #{variable}.map { |__item| #{param_type.convert_from_crystal("__item")} }.to_a).to_unsafe"
        end
      when .boolean?
        "LibC::Int.new(#{variable} ? 1 : 0)"
      when .glist?,
           .gslist?,
           .ghash?,
           .error?,
           .void?
        variable
      when .utf8?, .filename?
        "#{variable}.to_unsafe"
      else
        if pointer?
          variable
        else
          "#{TAG_MAP[tag]}.new(#{variable})"
        end
      end
    end

    def unwrap_gvalue(variable)
      case tag
      when .interface?
        interface = self.interface
        case interface
        when ObjectInfo, StructInfo, UnionInfo
          "#{interface.full_constant}.cast(#{variable}.object)"
        when InterfaceInfo
          "#{interface.full_constant}::Wrapper.cast(#{variable}.instance)"
        when FlagsInfo
          "#{interface.full_constant}.new(#{interface.type}.new(#{variable}.flags))"
        when EnumInfo
          "#{interface.full_constant}.new(#{interface.type}.new(#{variable}.enum))"
        else
          "#{variable}"
        end
      when .boolean?
        "#{variable}.boolean"
      when .utf8?
        "#{variable}.string"
      when .int32?
        "#{variable}.int"
      when .uint32?
        "#{variable}.uint"
      when .float?
        "#{variable}.float"
      when .double?
        "#{variable}.double"
      when .ghash?
        "GLib::HashTable.new(#{variable}.pointer.as(LibGLib::HashTable*))"
      else
        variable
      end
    end

    def inline_type?
      tag.interface? && equal(interface)
    end

    Dumper.def do
      dumper.puts "* tag = #{tag}"
      dumper.puts "* pointer = #{pointer?}"
      dumper.puts "* is_inline_type = #{inline_type?}"
      dumper.puts "* array_type = #{array_type}" if tag.array?
      dumper.puts "* array_length_param = #{array_length_param}" if tag.array?
      dumper.puts "* array_fixed_size = #{array_fixed_size}" if tag.array?
      dumper.puts "* is_zero_terminated = #{zero_terminated?}" if tag.array?

      Dumper.dump_child interface if tag.interface?
      Dumper.dump_child param_type if tag.array?
    end
  end
end








# Feedback and donation to gregorycohen2@gmail.com (money transfer or Paypal)