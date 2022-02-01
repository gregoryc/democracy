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





# 61 of 16 source files
module GIRepository
require_relative "string__"
 require_relative "./object"
  class SignalInfo < BaseInfo
    include CallableInfo

    def flags
      GIRepository.signal_info_get_flags(self)
    end

    def true_stops_emit?
      GIRepository.signal_info_true_stops_emit(self)
    end

    def lib_definition(builder)
      builder.comment "Signal #{name}"
    end

    def wrapper_definition(builder, libname)
      ret_type = return_type.wrapper_definition(libname)
      ret_type = "Nil" if ret_type == "Void*"
      proc = builder.proc_type(args.map { |arg|
        type = arg.type.wrapper_definition(builder, libname)
        arg.nilable? ? "#{type}?" : type
      }, return_type: ret_type)
      alias_name = "#{self.alias_name}Signal"
      builder.attach_to_next_section { builder.def_alias alias_name, proc }

      proc_arguments = args.each.with_index.map { |ai|
        arg, i = ai
        builder.arg("arg#{i}", type: arg.type.signal_lib_definition(builder))
      }.to_a
      proc_arguments << builder.arg("box", type: "Void*") # We pass the trampoline user data
      block_arguments = args.each.with_index.map { |ai|
        arg, i = ai
        value = arg.type.convert_to_crystal(builder, "arg#{i}")
        if arg.type.is_pointer_in_signal?
          null_check = builder.call("null?", receiver: "arg#{i}")
          unexpected_null = builder.call("raise_unexpected_null", builder.literal("#{arg.name} in #{identifier}"), receiver: "GObject")
          arg.nilable? ? builder.ternary(null_check, builder.literal(nil), value) : builder.ternary(null_check, unexpected_null, value)
        else
          value
        end
      }.to_a
      trampoline_args = Array.new(args.size) { |i| "arg#{i}" }
      builder.def_method(
        "on_#{identifier}",
        builder.arg(nil, prefix: :splat),
        builder.arg("after", default: "false"),
        builder.arg("block", type: alias_name, prefix: :block_capture)) do
        callback = line assign_var def_proc(proc_arguments) { |b|
          trampoline = b.call("unbox", "box", receiver: "::Box(#{alias_name})")
          return_value = b.call("call", block_arguments, receiver: trampoline)
          b.line return_type.convert_from_crystal(return_value)
        }

        trampoline = call("new", receiver: alias_name, block_args: trampoline_args) { |b|
          b.line b.call("call", trampoline_args, receiver: "block")
        }
        trampoline_box = line assign_var call("box", trampoline, receiver: "::Box")

        this = call("as", "LibGObject::Object*", receiver: "@pointer")
        callback_ptr = call("new", call("pointer", receiver: callback),
          call("null", receiver: "Pointer(Void)"), receiver: "LibGObject::Callback")
        register_box = call("register", trampoline_box, receiver: "GObject::ClosureDataManager")
        deregister_box = "->#{call("deregister", receiver: "GObject::ClosureDataManager")}"

        flags = ternary("after", "GObject::ConnectFlags::AFTER", "GObject::ConnectFlags::None")
        line call("signal_connect_data",
          this, literal(name), callback_ptr, register_box,
          deregister_box, flags, receiver: "LibGObject")
      end

      builder.def_method("after_#{identifier}", builder.arg("block", type: alias_name, prefix: :block_capture)) do
        line call("on_#{identifier}", {after: literal(true)}, "&block")
      end
    end

    private def alias_name
      name.try &.split("-").map(&.capitalize).join
    end

    private def identifier
      name = self.name.try &.tr("-", "_")
      name += "_" if name && {"begin", "end"}.includes? name
      name
    end

    Dumper.def do
      dumper.puts "* flags = #{flags}"
      dumper.puts "* true_stops_emit = #{true_stops_emit?}"
    end
  end
end








# Feedback and donation to gregorycohen2@gmail.com (money transfer or Paypal)