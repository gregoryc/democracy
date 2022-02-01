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





# 11 of 16 source files
module GIRepository
require_relative "string__"
    class Typelib
    include GObject::WrappedType

    @pointer : Void*

    def initialize(pointer : LibGIRepository::Typelib*)
      @pointer = pointer.as(Void*)
    end

    def to_unsafe
      @pointer.not_nil!.as(LibGIRepository::Typelib*)
    end

    def to_unsafe_girepository_typelib
      @pointer.not_nil!.as(LibGIRepository::Typelib*)
    end

    def free
      LibGIRepository.typelib_free(@pointer.as(LibGIRepository::Typelib*))
      nil
    end

    def namespace
      __var0 = LibGIRepository.typelib_get_namespace(@pointer.as(LibGIRepository::Typelib*))
      GObject.raise_unexpected_null("g_typelib_get_namespace") if __var0.null?
      ::String.new(__var0)
    end

    def symbol(symbol_name : ::String, symbol : Void*?)
      __var0 = LibGIRepository.typelib_symbol(@pointer.as(LibGIRepository::Typelib*), symbol_name.to_unsafe, symbol ? symbol : Pointer(Void).null)
      (__var0 == 1)
    end
  end
end








# Feedback and donation to gregorycohen2@gmail.com (money transfer or Paypal)