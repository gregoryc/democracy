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





# 49 of 16 source files
module GIRepository
require_relative "string__"
 require_relative "./object"
  class PropertyInfo < BaseInfo
    def crystal_name
      name = self.name
      name.tr("-", "_") if name
    end

    def arg_name
      name = crystal_name
      name = "_#{name}" if name[0].uppercase? if name
      name = "_#{name}" if ArgInfo::KEYWORDS.includes? name
      name.gsub(/_+$/, "") if name
      name
    end

    def type
      BaseInfo.wrap(GIRepository.property_info_get_type(self)).as(TypeInfo)
    end

    def flags
      GIRepository.property_info_get_flags self
    end

    def getter?
      flags.readable?
    end

    def setter?
      flags.writable?
    end

    def construct_only?
      flags.construct_only?
    end

    def private?
      flags.private?
    end

    def lib_definition(builder)
      builder.comment "Property #{name} : #{type.lib_definition(builder)}"
    end

    def wrapper_definition(builder, libname)
      ptr = builder.call("as", "LibGObject::Object*", receiver: "@pointer")
      if getter?
        builder.def_method(crystal_name, return_type: type.wrapper_definition(builder, libname)) do
          gvalue = line assign_var call("new", {type: type.gvalue_type}, receiver: "GObject::Value")
          line call("object_get_property", ptr, literal(name), gvalue, receiver: "LibGObject")
          line type.unwrap_gvalue(gvalue)
        end
      end

      if setter? && !construct_only?
        builder.def_method("#{crystal_name}=", builder.arg("value", type: type.wrapper_definition(builder, libname))) do
          line call("object_set_property", ptr, literal(name), call("to_gvalue", receiver: "value"), receiver: "LibGObject")
        end
      end
    end

    Dumper.def do
      dumper.puts "* flags = #{flags}"
      Dumper.dump_child type
    end
  end
end








# Feedback and donation to gregorycohen2@gmail.com (money transfer or Paypal)