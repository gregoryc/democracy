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





# 38 of 16 source files
module GObject
require_relative "string__"
 require_relative "./object"
  class Object
    def connect(signal, *, after = false, &callback)
      connect signal, callback, after: after
    end

    def connect(signal, callback, *, after = false)
      flags = GObject::ConnectFlags::SWAPPED
      flags |= GObject::ConnectFlags::AFTER if after
      LibGObject.signal_connect_data(@pointer.as(LibGObject::Object*),
        signal,
        LibGObject::Callback.new(callback.pointer, Pointer(Void).null),
        ClosureDataManager.register(callback.closure_data),
        ->ClosureDataManager.deregister,
        flags).tap do |handler_id|
        if handler_id == 0
          raise ArgumentError.new("Couldn't connect signal #{signal} to #{type_name} (#{self.class})")
        end
      end
    end

    def disconnect(id : UInt64)
      GObject.signal_handler_disconnect self, id
    end

    def disconnect(signal : String)
      loop do
        handler_id = first_handler_for signal
        break if handler_id == 0
        disconnect handler_id
      end
    end

    private def first_handler_for(signal)
      GObject.signal_handler_find self,
        SignalMatchType::ID,
        signal_lookup(signal),
        0,
        nil,
        nil,
        nil
    end

    def signal_lookup(name)
      GObject.signal_lookup(name, gtype).tap do |signal|
        raise ArgumentError.new "Unknown signal #{name} for #{type_name} (#{self.class})" if signal == 0
      end
    end

    def get_property(name : String, gtype : T.class) forall T
      value = GObject::Value.new(type: T::GTYPE)
      LibGObject.object_get_property(to_unsafe_gobject_object, name, value)
      T.new(value.object.to_unsafe.as(typeof(T.allocate.to_unsafe)))
    end

    def gtype?
      if @pointer
        @pointer.as(LibGObject::Object*).value.g_type_instance.g_class.value.g_type
      else
        nil
      end
    end

    def gtype
      gtype? || raise ArgumentError.new("No gtype for #{inspect}")
    end

    def type_name
      @pointer.null? ? "null pointer" : (gtype = gtype?) ? GObject.type_name(gtype) : "Unknown"
    end

    def inspect(io)
      gtype = gtype?
      io << "GObject(#{type_name}#{":#{gtype.to_s(16)}" if gtype}:0x#{@pointer.address.to_s(16)})"
    end
  end
end








# Feedback and donation to gregorycohen2@gmail.com (money transfer or Paypal)